/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QRegExp>

// This is required for windows, to prevent linkage errors (somehow the sources of oniguruma assumes we're linking with a dll)
#define ONIG_EXTERN extern
#include "oniguruma.h"
#include "regexp.h"
#include "debug.h"

namespace edbee {

/// The onig regexp-engine
class OnigRegExpEngine : public RegExpEngine
{
private:
    regex_t* reg_;              ///< The regExp pattern
    OnigErrorInfo einfo_;       ///< The error information
    bool valid_;                ///< Is the reg-exp valid?
    QString error_;             ///< The current error as a qstring
    OnigRegion *region_;        ///< The current found region
    QString pattern_;           ///< The original regexp-pattern
    QString line_;              ///< The current line
    const QChar* lineRef_;      ///< A reference to the given line

    void clearError() { error_.clear(); }
    void fillError( int code )
    {
        if( code == ONIG_NORMAL ) {
            error_.clear();
        } else {
            unsigned char s[ONIG_MAX_ERROR_MESSAGE_LEN];
            onig_error_code_to_str( s, code, &einfo_ );
            error_ = QString::fromLatin1((char*)s);
        }
    }

    void deleteRegion()
    {
        if( region_ ) { onig_region_free(region_, 1 ); }  // 1:free self, 0:free contents only);
        region_ = 0;
    }


public:

    OnigRegExpEngine( const QString& pattern, bool caseSensitive, RegExp::Syntax syntax )
        : reg_(0)
        , region_(0)
        , pattern_(pattern)
    {
        const QChar* patternChars = pattern.constData();

        OnigSyntaxType* onigSyntax = ONIG_SYNTAX_DEFAULT;
        if( syntax == RegExp::SyntaxFixedString ) { onigSyntax = ONIG_SYNTAX_ASIS; }


        OnigOptionType onigOptions = ONIG_OPTION_NONE|ONIG_OPTION_CAPTURE_GROUP;
        if( !caseSensitive ) { onigOptions = onigOptions | ONIG_OPTION_IGNORECASE;}

        int result = onig_new(&reg_, (OnigUChar*)patternChars, (OnigUChar*)(patternChars + pattern.length()), onigOptions, ONIG_ENCODING_UTF16_LE, ONIG_SYNTAX_DEFAULT, &einfo_);
        valid_ = result == ONIG_NORMAL;
        fillError( result );
    }

    virtual ~OnigRegExpEngine()
    {
        deleteRegion();
        onig_free(reg_);
    }

    virtual QString pattern() { return pattern_; }
    virtual bool isValid() { return valid_; }
    virtual QString error() { return error_; }


    int indexIn( const QChar* charPtr, int offset, int length, bool reverse )
    {
        // invalid reg-exp don't use it!
        if( !valid_ ) { return -2; }

        // delete old regenion an make a new one
        deleteRegion();
        region_ = onig_region_new();

        lineRef_ = charPtr;
        OnigUChar* stringStart  = (OnigUChar*)charPtr;
        OnigUChar* stringEnd    = (OnigUChar*)(charPtr+length);
        OnigUChar* stringOffset = (OnigUChar*)(charPtr+offset);
        OnigUChar* stringRange  = (OnigUChar*)stringEnd;
        if( reverse ) {
            stringOffset = stringEnd; //==stringStart ? stringEnd : stringEnd-1;
            stringRange  = (OnigUChar*)(charPtr+offset);
        }

        clearError();

        int result = onig_search(reg_, stringStart, stringEnd, stringOffset, stringRange, region_, ONIG_OPTION_NONE);
        if ( result >= 0) {
            Q_ASSERT(result%2==0);
            return result>>1;
        } else if (result == ONIG_MISMATCH) {
            return -1;
        } else { // error
            fillError(result);
            return -2;
        }
    }


    /// returns the position of the given match or -1 if not found. -2 on error
    virtual int indexIn( const QString& str, int offset )
    {
        // line_ = str;
        // lineRef_ = line_.data();
        // return indexIn( lineRef_, offset, line_.length() );

        line_ = str;
        int length = line_.length(); // very scary, calling line_.length() invalidates the line_.data() pointer :S
        lineRef_ = line_.data();

        return indexIn( lineRef_, offset, length, false );

        /*
        // invalid reg-exp don't use it!
        if( !valid_ ) { return -2; }

        // delete old regenion an make a new one
        deleteRegion();
        region_ = onig_region_new();

        const QChar* charPtr = lineRef_; //line_.data();
        OnigUChar* stringStart  = (OnigUChar*)charPtr;
        OnigUChar* stringEnd    = (OnigUChar*)(charPtr+str.length());
        OnigUChar* stringOffset = (OnigUChar*)(charPtr+offset);
        OnigUChar* stringRange  = (OnigUChar*)stringEnd;

        clearError();

        int result = onig_search(reg_, stringStart, stringEnd, stringOffset, stringRange, region_, ONIG_OPTION_NONE);
        if ( result >= 0) {
            Q_ASSERT(result%2==0);
            // int i;
            // fprintf(stderr, "match at %d\n", r);
            // for (i = 0; i < region->num_regs; i++) {
            // fprintf(stderr, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
            return result>>1;

        } else if (result == ONIG_MISMATCH) {
            return -1;
        } else { // error
            fillError(result);
            return -2;
        }
        */
    }

    /// returns the position of the given match or -1 if not found. -2 on error
    /// @param str the pointer to the line
    /// @param offset the offset to start searching
    /// @param length the total length of the str pointer
    virtual int indexIn( const QChar* charPtr, int offset, int length )
    {
        return indexIn( charPtr, offset, length, false );
    }

    virtual int lastIndexIn( const QString& str, int offset )
    {
        line_ = str;
        int length = line_.length(); // very scary, calling line_.length() invalidates the line_.data() pointer :S
        lineRef_ = line_.data();
        return lastIndexIn( lineRef_, offset, length);
    }

    virtual int lastIndexIn( const QChar* str, int offset, int length )
    {
        if( offset < 0 ) {
            offset = length + offset;
            if( offset < 0 ) offset = 0;
        }
        return indexIn( str, offset, length, true);
    }


    /// returns the offset
    virtual int pos( int nth ) const
    {
        if( !region_ ) { return -1; } // no region
        if( nth < region_->num_regs ) {
            int result = region_->beg[nth];
            if( result < 0 ) { return -1; }

            if( result%2 != 0 ) {
                qlog_warn()<< "*** ERROR ***:" << nth;
                qlog_warn()<< "line   :" << line_;
                qlog_warn()<< "pattern:" << pattern_;
                qlog_warn()<< "";
                 for (int i = 0; i < region_->num_regs; i++) {
                     qlog_info() << QString(" - %1: (%2,%3)").arg(i).arg(region_->beg[i]).arg(region_->end[i]);
                 }
                // fprintf(stderr,
                Q_ASSERT(result%2==0);
            }
            return result >> 1;
        }
        return -1;
    }


    /// returns the length of the given position.
    virtual int len( int nth ) const
    {
        if( !region_ ) { return -1; } // no region
        if( nth < region_->num_regs ) {
            int result = region_->end[nth] - region_->beg[nth]; // end is the first character AFTER the match
            Q_ASSERT(result%2==0);
            return result >> 1;
        }
        return -1;
//       for (int i = 0; i < region_->num_regs; i++) {
//         qlog_info() << QString("%1: (%2-%3)").arg(i).arg(region_->beg[i]).arg(region_->end[i] );
//       }
    }


    /// returns the captureat the given index
    virtual QString cap( int nth = 0 ) const
    {
        int p = pos(nth);
        int l = len(nth);
        if( p < 0 || l < 0 ) return QString();
        return QString( lineRef_+p,l);
//        return  line_.mid(p,l);
    }


//    virtual QString cap( int nth= 0 ) const
//    {
//int lengte = len(nth);
//if( lengte <= 0 ) return QString();
//        qlog_warn() << "TODO: Implement cap !!";
//        return QString("a").repeated(lengte);
//    }

};


//====================================================================================================================


/// The Qt regexp-engine
class QtRegExpEngine : public RegExpEngine
{
    QRegExp* reg_;

public:
    QtRegExpEngine( const QString& pattern, bool caseSensitive, RegExp::Syntax syntax )
        : reg_(0)
    {
        QRegExp::PatternSyntax regExpSyntax= QRegExp::RegExp2;
        if( syntax == RegExp::SyntaxFixedString ) { regExpSyntax = QRegExp::FixedString; }

        reg_= new QRegExp( pattern, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, regExpSyntax);
    }
    virtual ~QtRegExpEngine()
    {
        delete reg_;
    }

    virtual QString pattern() { return reg_->pattern(); }

    virtual bool isValid() { return reg_->isValid(); }

    virtual QString error() { return reg_->errorString(); }

    virtual int indexIn( const QString& str, int offset )
    {
        return reg_->indexIn( str, offset );
    }

    /// Warning index in with char pointers creates a string from the string
    virtual int indexIn( const QChar* str, int offset, int length )
    {
        QString realString( str+offset, length );
        return reg_->indexIn( realString, offset );
    }


    virtual int lastIndexIn( const QString& str, int offset )
    {
        return reg_->lastIndexIn( str, offset );
    }

    virtual int lastIndexIn( const QChar* str, int offset, int length )
    {
        QString realString( str+offset, length );
        return reg_->lastIndexIn( realString, offset );
    }


    virtual int pos( int nth = 0 ) const { return reg_->pos(nth); }
    virtual int len( int nth = 0 ) const { return reg_->cap(nth).length(); }
    virtual QString cap( int nth= 0 ) const { return reg_->cap(nth); }

};


//====================================================================================================================


RegExp::RegExp(  const QString& pattern, bool caseSensitive, Syntax syntax, Engine engine)
    : d_(0)
{
    switch( engine ) {
        case EngineQRegExp:
            d_ = new QtRegExpEngine(pattern, caseSensitive, syntax);
            break;
        default:
            Q_ASSERT(false);
            qlog_warn() << "Invalid engine supplied to RegExp";
        case EngineOniguruma:
            d_ = new OnigRegExpEngine(pattern, caseSensitive, syntax);
    }
}

RegExp::~RegExp()
{
    delete d_;
}

/// escapes a string with every regexp special character escaped
/// we currently always use QRegExp::escape.. For the future we added an engine parameter
/// which is currently ignored
QString RegExp::escape(const QString& str, RegExp::Engine engine)
{
    Q_UNUSED(engine);
    return QRegExp::escape(str);
}

bool RegExp::isValid() const
{
    return d_->isValid();
}

QString RegExp::errorString() const
{
    return d_->error();
}

QString RegExp::pattern() const
{
    return d_->pattern();
}

/// matches th
/// Attempts to find a match in str from position offset (0 by default). If offset is -1, the search starts at the last character; if -2, at the next to last character; etc.
/// Returns the position of the first match, or -1 if there was no match.
/// The caretMode parameter can be used to instruct whether ^ should match at index 0 or at offset.
/// You might prefer to use QString::indexOf(), QString::contains(), or even QStringList::filter(). To replace matches use QString::replace().
///
/// Although const, this function sets matchedLength(), capturedTexts() and pos().
/// If the QRegExp is a wildcard expression (see setPatternSyntax()) and want to test a string against the whole wildcard expression, use exactMatch() instead of this function.
///
/// Currently we ignore the caretMode
int RegExp::indexIn(const QString& str, int offset)  // const
{
    return d_->indexIn( str, offset );
}

int RegExp::indexIn(const QChar* str, int offset, int length)
{
    return d_->indexIn( str, offset, length );
}

int RegExp::lastIndexIn(const QString &str, int offset)
{
    return d_->lastIndexIn( str, offset );
}


int RegExp::lastIndexIn(const QChar *str, int offset, int length)
{
    return d_->lastIndexIn( str, offset, length );
}

/// Returns the position of the nth captured text in the searched string. If nth is 0 (the default), pos() returns the position of the whole match.
/// For zero-length matches, pos() always returns -1. (For example, if cap(4) would return an empty string, pos(4) returns -1.) This is a feature of the implementation.
int RegExp::pos(int nth) const
{
    return d_->pos(nth);
}

/// The length of nth element
int RegExp::len(int nth) const
{
    return d_->len(nth);
}

/// This method returns the given matched length
QString RegExp::cap(int nth) const
{
    return d_->cap(nth);
}


} // edbee
