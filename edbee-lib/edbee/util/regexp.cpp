// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

// This is required for windows, to prevent linkage errors (somehow the sources of oniguruma assumes we're linking with a dll)


#ifdef __clang__
  #pragma clang diagnostic push
#else
  #pragma GCC diagnostic push
#endif
#ifdef _MSC_VER
  #pragma warning( push )
#endif

#define ONIG_EXTERN extern
#include "oniguruma.h"

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

#ifdef __clang__
  #pragma clang diagnostic pop
#else
  #pragma GCC diagnostic pop
#endif

#include "regexp.h"

#include <QRegExp>

#include "edbee/debug.h"

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

    /// clears the error message
    void clearError() { error_.clear(); }

    /// fills the error with the given code. It retrives the onig_error_code if requried
    /// @param code the ONIG error code
    void fillError(int code)
    {
        if( code == ONIG_NORMAL ) {
            error_.clear();
        } else {
            unsigned char s[ONIG_MAX_ERROR_MESSAGE_LEN];
            onig_error_code_to_str( s, code, &einfo_ );
            error_ = QString::fromLatin1((char*)s);
        }
    }


    /// deletes the current regon
    void deleteRegion()
    {
        if(region_) {
            onig_region_free(region_, 1 ); // 1:free self, 0:free contents only);
        }
        region_ = nullptr;
    }


public:

    /// Constructs the OnigRegExp engine
    /// @param pattern the regular expression pattern
    /// @param caseSensitive is the regexp case senstitive
    /// @param syntax the syntax to use
    OnigRegExpEngine(const QString& pattern, bool caseSensitive, RegExp::Syntax syntax)
        : reg_(nullptr)
        , region_(nullptr)
        , pattern_(pattern)
        , lineRef_(nullptr)
    {
        const QChar* patternChars = pattern.constData();

        OnigSyntaxType* onigSyntax = &OnigSyntaxRuby; // ONIG_SYNTAX_DEFAULT
        if( syntax == RegExp::SyntaxFixedString ) { onigSyntax = &OnigSyntaxASIS; }

        OnigOptionType onigOptions = ONIG_OPTION_NONE|ONIG_OPTION_CAPTURE_GROUP;
        if( !caseSensitive ) { onigOptions = onigOptions | ONIG_OPTION_IGNORECASE;}

        int result = onig_new(
            &reg_,
            (OnigUChar*)patternChars,
            (OnigUChar*)(patternChars + pattern.length()),
            onigOptions,
            ONIG_ENCODING_UTF16_LE,
            onigSyntax,
            &einfo_);
        valid_ = result == ONIG_NORMAL;
        fillError( result );
    }


    /// destructs the regular expression engine
    virtual ~OnigRegExpEngine()
    {
        deleteRegion();
        onig_free(reg_);
    }


    /// returns the pattern used
    virtual QString pattern() override { return pattern_; }


    /// returns true if the supplied regular expression was valid
    virtual bool isValid() override { return valid_; }


    /// returns the error message
    virtual QString error() override { return error_; }


    /// returns the index of the given QChar array in the given text
    /// @param charPtr the pointer to the string data
    /// @param offset the offset to start searching
    /// @param length the length of the string data
    /// @param reverse should the search be reversed?
    /// @return the position or std::string::npos if no match
    virtual size_t indexIn(const QChar* charPtr, ptrdiff_t offset, size_t length, bool reverse)
    {
        // invalid reg-exp don't use it!
        if (!valid_) { return std::string::npos; }

        // delete old regenion an make a new one
        deleteRegion();
        region_ = onig_region_new();

        lineRef_ = charPtr;
        OnigUChar* stringStart  = (OnigUChar*)charPtr;
        OnigUChar* stringEnd    = (OnigUChar*)(charPtr+length);
        OnigUChar* stringOffset = (OnigUChar*)(charPtr+offset);
        OnigUChar* stringRange  = (OnigUChar*)stringEnd;
        if (reverse) {
            stringOffset = stringEnd; //==stringStart ? stringEnd : stringEnd-1;
            stringRange  = (OnigUChar*)(charPtr+offset);
        }

        clearError();

        int result = onig_search(reg_, stringStart, stringEnd, stringOffset, stringRange, region_, ONIG_OPTION_NONE);
        if (result >= 0) {
            Q_ASSERT(result % 2 == 0);
            return static_cast<size_t>(result) >> 1;

        } else if (result == ONIG_MISMATCH) {
            return std::string::npos;

        } else { // error
            fillError(result);
            return std::string::npos;

        }
    }


    /// returns the position of the given match or returns an error code
    /// @param str the string to the search in
    /// @param offset the offset in the string to start the search
    /// @return the position of the given match (std::string::npos if not found, or on error)
    virtual size_t indexIn(const QString& str, ptrdiff_t offset) override
    {
        line_ = str;
        size_t length = static_cast<size_t>(line_.length()); // very scary, calling line_.length() invalidates the line_.data() pointer :S
        lineRef_ = line_.data();

        return indexIn(lineRef_, offset, length, false);
    }


    /// returns the position of the given match or returns an error code
    /// @param str the pointer to the line
    /// @param offset the offset to start searching
    /// @param length the total length of the str pointer
    /// @return the position of the given match (std::string::npos if not found, or on error)
    virtual size_t indexIn(const QChar* charPtr, ptrdiff_t offset, size_t length) override
    {
        return indexIn(charPtr, offset, length, false);
    }


    /// returns the last index of the given string
    /// @param str the string to search in
    /// @param offset the offset of the start of the search
    /// @return the position of the given match (std::string::npos if not found, or on error)
    virtual size_t lastIndexIn(const QString& str, ptrdiff_t offset) override
    {
        line_ = str;
        size_t length = static_cast<size_t>(line_.length()); // very scary, calling line_.length() invalidates the line_.data() pointer :S
        lineRef_ = line_.data();
        return lastIndexIn(lineRef_, offset, length);
    }


    /// returns the last index of the given string
    /// @param str the string to search in
    /// @param offset the offset of the start of the search
    /// @param length the length of the supplied string data
    /// @return the position of the given match (std::string::npos if not found, or on error)
    virtual size_t lastIndexIn(const QChar* str, ptrdiff_t offset, size_t length) override
    {
        if (offset < 0) {
            offset = static_cast<ptrdiff_t>(length) + offset;
            if(offset < 0) offset = 0;
        }
        return indexIn(str, offset, length, true);
    }


    /// returns the offset of the given match
    /// @param nth the given match
    /// @retuns the position or std::string::npos on no match
    virtual size_t pos(size_t nth) const override
    {
        if (!region_) { return std::string::npos; } // no region
        if (static_cast<int>(nth) < region_->num_regs) {
            int result = region_->beg[nth];
            if (result < 0) { return std::string::npos; }

            if (result % 2 != 0) {
                qlog_warn() << "*** ERROR ***:" << nth;
                qlog_warn() << "line   :" << line_;
                qlog_warn() << "pattern:" << pattern_;
                qlog_warn() << "";
                 for (int i = 0; i < region_->num_regs; i++) {
                     qlog_info() << QStringLiteral(" - %1: (%2,%3)").arg(i).arg(region_->beg[i]).arg(region_->end[i]);
                 }
                // fprintf(stderr,
                Q_ASSERT(result%2 == 0);
            }
            return static_cast<size_t>(result) >> 1;
        }
        return std::string::npos;
    }


    /// returns the length of the nth match
    /// @param nth the match number or std::string::npos if no match is found at the index
    virtual size_t len(size_t nth) const override
    {
        if (!region_) { return std::string::npos; } // no region
        if (static_cast<ptrdiff_t>(nth) < region_->num_regs) {
            int result = region_->end[nth] - region_->beg[nth]; // end is the first character AFTER the match
            Q_ASSERT(result % 2 == 0);
            return static_cast<size_t>(result) >> 1;
        }
        return std::string::npos;
    }

    /// returns the capture at the given index
    /// @param nth the position of the match
    /// @return the match ath the given position
    virtual QString cap(size_t nth = 0) const override
    {
        size_t p = pos(nth);
        size_t l = len(nth);
        if( p == std::string::npos || l == std::string::npos ) return QString();
        return QString(lineRef_ + p, static_cast<qsizetype>(l));
    }
};


//====================================================================================================================


/// The Qt regexp-engine
/// A pretty dumb wrapper around the QRegExp Class
class QtRegExpEngine : public RegExpEngine
{
    QRegExp* reg_;      ///< The Qt QRegExp objet

public:

    /// constructs the QRegExp engine
    /// @param pattern the pattern to match
    /// @param caseSensitive is the match case sensitive
    /// @param syntax the used syntadx
    QtRegExpEngine(const QString& pattern, bool caseSensitive, RegExp::Syntax syntax)
        : reg_(nullptr)
    {
        QRegExp::PatternSyntax regExpSyntax= QRegExp::RegExp2;
        if (syntax == RegExp::SyntaxFixedString) { regExpSyntax = QRegExp::FixedString; }

        reg_= new QRegExp(pattern, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, regExpSyntax);
    }


    /// destructs the QtRegExpEngine
    virtual ~QtRegExpEngine()
    {
        delete reg_;
    }


    /// Returns the supplied pattern
    virtual QString pattern() override
    {
        return reg_->pattern();
    }

    /// Returns true if the given regular expression was valid
    virtual bool isValid() override
    {
        return reg_->isValid();
    }

    /// returns the error message
    virtual QString error() override
    {
        return reg_->errorString();
    }


    /// returns the index of the regexp in the given string
    /// @param str the string to search in
    /// @param offset the start offset of the search
    /// @return the index of the given match or std::string::npos if no match was found
    virtual size_t indexIn(const QString& str, ptrdiff_t offset) override
    {
        int result = reg_->indexIn(str, static_cast<int>(offset));
        if (result < 0) return std::string::npos;
        return static_cast<size_t>(result);
    }


    /// Warning index in with char pointers creates a string from the string
    /// @param str the pointer to the given string
    /// @param offset the offset to start searching
    /// @param length the length of the given string
    /// @return the index of the given match or std::string::npos if no match was found
    virtual size_t indexIn(const QChar* str, ptrdiff_t offset, size_t length) override
    {
        QString realString(str + offset, static_cast<qsizetype>(length));
        int result = reg_->indexIn(realString, static_cast<int>(offset));
        if (result < 0) return std::string::npos;
        return static_cast<size_t>(result);
    }


    /// Returns the last match of this regexp in the given string
    /// @param str the string to search in
    /// @param offset the offset to start searching from
    /// @return the index of the given match or std::string::npos if no match was found
    virtual size_t lastIndexIn(const QString& str, ptrdiff_t offset) override
    {
        int result = reg_->lastIndexIn(str, static_cast<int>(offset));
        if (result < 0) return std::string::npos;
        return static_cast<size_t>(result);
    }


    /// Returns the last match of this regexp in the given string
    /// @param str the string to search in
    /// @param offset the offset to start searching from
    /// @param length the length of the given string
    /// @return the index of the given match or std::string::npos if no match was found
    virtual size_t lastIndexIn(const QChar* str, ptrdiff_t offset, size_t length) override
    {
        QString realString(str+offset, static_cast<qsizetype>(length));
        int result = reg_->lastIndexIn(realString, static_cast<int>(offset));
        if (result < 0) return std::string::npos;
        return static_cast<size_t>(result);
    }


    /// returns the position of the nth group match
    /// @param nth the group number to return
    /// @return the position of the nth match or std::string::npos if not found
    virtual size_t pos(size_t nth = 0) const override
    {
        int result = reg_->pos(static_cast<int>(nth));
        if (result < 0) return std::string::npos;
        return static_cast<size_t>(result);
    }


    /// returns the length of the nth group match
    /// @param nth the group number to return
    /// @return the length of the nth match or std::string::npos if not found
    virtual size_t len(size_t nth = 0) const override
    {
        qsizetype result = reg_->cap(static_cast<int>(nth)).length();
        if (result < 0) return std::string::npos;
        return static_cast<size_t>(result);
    }


    /// returns the nth group
    /// @param nth the group number to return
    /// @return the content of the given match or std::string::npos if not found
    virtual QString cap(size_t nth = 0) const override
    {
        return reg_->cap(static_cast<int>(nth));
    }
};


//====================================================================================================================


/// Constructs the regular expression matcher
/// @param pattern the pattern of the regular expression
/// @param caseSensitive should the match be case sensitive
/// @param syntax the syntax of th given regular expression (SyntaxDefault(default) or SyntaxFixedString)
/// @param engine the engine to use (EngineOniguruma(default) or EngineQRegExp)
RegExp::RegExp( const QString& pattern, bool caseSensitive, Syntax syntax, Engine engine)
    : d_(nullptr)
{
    switch (engine) {
        case EngineQRegExp:
            d_ = new QtRegExpEngine(pattern, caseSensitive, syntax);
            break;
        case EngineOniguruma:
            d_ = new OnigRegExpEngine(pattern, caseSensitive, syntax);
            break;
        default:
            Q_ASSERT(false);
            qlog_warn() << "Invalid engine supplied to RegExp. Falling back to EngineOniguruma";
            d_ = new OnigRegExpEngine(pattern, caseSensitive, syntax);
            break;
    }
}


/// destructs the regular expression object
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


/// returns true if the supplied regular expression was valid
bool RegExp::isValid() const
{
    return d_->isValid();
}


/// returns the error message of the regular expression
QString RegExp::errorString() const
{
    return d_->error();
}


/// returns th supplied pattern
QString RegExp::pattern() const
{
    return d_->pattern();
}


/// Attempts to find a match in str from position offset (0 by default). If offset is -1, the search starts at the last character; if -2, at the next to last character; etc.
/// Returns the position of the first match, or -1 if there was no match.
/// The caretMode parameter can be used to instruct whether ^ should match at index 0 or at offset.
/// You might prefer to use QString::indexOf(), QString::contains(), or even QStringList::filter(). To replace matches use QString::replace().
/// @returns the index (returns std::string::npos if not found)

// size_t RegExp::indexIn(const QString& str, ptrdiff_t offset)
// {
//     return d_->indexIn(str, offset);
// }

size_t RegExp::indexIn(const QString &str, size_t offset)
{
    return d_->indexIn(str, static_cast<ptrdiff_t>(offset));
}


/// Searches for the regular expression in the given string
/// @param str the string to search in
/// @param offset the offset to start searching
/// @param length the length of the supplied string
/// @return the found index of the regular expression (returns std::string::npos if not found)

// size_t RegExp::indexIn(const QChar* str, ptrdiff_t offset, size_t length)
// {
//     return d_->indexIn(str, offset, length);
// }

size_t RegExp::indexIn(const QChar *str, size_t offset, size_t length)
{
    return d_->indexIn(str, static_cast<ptrdiff_t>(offset), length);
}


/// Searches for the last match of the regular expression in the given string
/// @param str the string to search in
/// @param offset the offset to start searching
/// @return the found index of the regular expression (returns std::string::npos if not found)

// size_t RegExp::lastIndexIn(const QString& str, ptrdiff_t offset)
// {
//     return d_->lastIndexIn(str, offset);
// }

size_t RegExp::lastIndexIn(const QString& str, size_t offset)
{
    return d_->lastIndexIn(str, static_cast<ptrdiff_t>(offset));
}


/// Searches for the last match of the regular expression in the given string
/// @param str the string to search in
/// @param offset the offset to start searching
/// @param length the length of the supplied string
/// @return the found index of the regular expression (returns std::string::npos if not found)

// size_t RegExp::lastIndexIn(const QChar* str, ptrdiff_t offset, size_t length)
// {
//     return d_->lastIndexIn(str, offset, length);
// }

size_t RegExp::lastIndexIn(const QChar* str, size_t offset, size_t length)
{
    return d_->lastIndexIn(str, static_cast<ptrdiff_t>(offset), length);
}


/// Returns the position of the nth captured text in the searched string. If nth is 0 (the default), pos() returns the position of the whole match.
/// (returns std::string::npos if not found)
size_t RegExp::pos(size_t nth) const
{
    return d_->pos(nth);
}


/// The length of nth element
/// (returns std::string::npos if not found)
size_t RegExp::len(size_t nth) const
{
    return d_->len(nth);
}


/// This method returns the given matched length
/// (returns std::string::npos if not found)
QString RegExp::cap(size_t nth) const
{
    return d_->cap(nth);
}


} // edbee
