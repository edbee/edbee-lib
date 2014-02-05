/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "texttheme.h"

#include <QDir>
#include <QVector>
#include <QStack>
#include <QDateTime>

#include "edbee/io/tmthemeparser.h"
#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textdocumentscopes.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/edbee.h"

#include "debug.h"

namespace edbee {


TextThemeRule::TextThemeRule(const QString& name, const QString& selector, QColor foreground, QColor background, bool bold, bool italic, bool underline)
    : name_( name )
    , scopeSelector_(0)
    , foregroundColor_( foreground )
    , backgroundColor_( background )
    , bold_(bold)
    , italic_(italic)
    , underline_(underline)
{
    scopeSelector_ = new TextScopeSelector(selector);
}

TextThemeRule::~TextThemeRule()
{
    delete scopeSelector_;
}

/// This method checks if the given scopelist matches the scope selector
bool TextThemeRule::matchesScopeList(const TextScopeList* scopes)
{
    return ( scopeSelector_->calculateMatchScore( scopes ) >= 0 ) ;
}

void TextThemeRule::fillFormat(QTextCharFormat* format)
{
    if( foregroundColor_.isValid() ) { format->setForeground(foregroundColor_ ); }
    if( backgroundColor_.isValid() ) { format->setBackground(backgroundColor_ ); }
    if( bold_ ) { format->setFontWeight( QFont::Bold ); }  //QFont::Black
    if( italic_) { format->setFontItalic(true); }
    if( underline_ ) { format->setFontUnderline(true); }
}

//=================================================


TextTheme::TextTheme()
    : name_("Default Theme")
    , uuid_("")
    , backgroundColor_( 0xffeeeeee )
    , caretColor_( 0xff000000  )
    , foregroundColor_( 0xff222222 )
    , lineHighlightColor_(0xff999999 )
    , selectionColor_( 0xff9999ff)

    // thTheme settings
//    , backgroundColor_(0xff272822)
//    , caretColor_(0xffF8F8F0)
//    , foregroundColor_(0xffF8F8F2)
//    , invisiblesColor_(0xff3B3A32)
//    , lineHighlightColor_(0xff3E3D32)
//    , selectionColor_(0xff49483E)
//    , findHighlightBackgroundColor_(0xffFFE792)
//    , findHighlightForegroundColor_(0xff000000)
//    , selectionBorderColor_(0xff222218)
//    , activeGuideColor_(0x9D550FB0)

//    , bracketForegroundColor_(0xF8F8F2A5)
//    , bracketOptions_("underline")

//    , bracketContentsForegroundColor_(0xF8F8F2A5)
//    , bracketContentsOptions_("underline")

//    , tagsOptions_("stippled_underline")

{

//    giveThemeRule( new TextThemeRule("Comment","comment", QColor("#75715E") ));
//    giveThemeRule( new TextThemeRule("String","string", QColor("#E6DB74") ));
}

TextTheme::~TextTheme()
{
    qDeleteAll(themeRules_);
}

/// The text theme
void TextTheme::giveThemeRule(TextThemeRule* rule)
{
    themeRules_.append(rule);
}

void TextTheme::fillFormatForTextScopeList( const TextScopeList* scopeList, QTextCharFormat* format)
{
//    format->setForeground( foregroundColor() );
//    format->setBackground( backgroundColor() );

    foreach( TextThemeRule* rule, themeRules_ ) {
        if( rule->matchesScopeList( scopeList ) ) {
            rule->fillFormat(format);
        }
    }

}


//=================================================


TextThemeStyler::TextThemeStyler( TextEditorController* controller )
    : controllerRef_( controller )
{
    connect( controller, SIGNAL(textDocumentChanged(edbee::TextDocument*,edbee::TextDocument*)), SLOT(textDocumentChanged(edbee::TextDocument*,edbee::TextDocument*)) );
//    textDocumentChanged(0,controller->textDocument());
}

TextThemeStyler::~TextThemeStyler()
{
}



/// This method returns a reference to the given line format.
/// WARNING this reference is ONLY valid very shortly.. Another call to this
/// method can invalidates the previous result!!!
///
/// @param lineIdx the line index
/// @return the array of ranges
QList<QTextLayout::FormatRange> TextThemeStyler::getLineFormatRanges( int lineIdx )
{
    TextDocumentScopes* scopes = controller()->textDocument()->scopes();

    // check if the range is in the case. When it is, use it
    QList<QTextLayout::FormatRange> formatRangeList;

    // get all textranges on the given line
    ScopedTextRangeList* scopedRanges = scopes->scopedRangesAtLine(lineIdx);
    if( scopedRanges == 0 || scopedRanges->size() == 0 ) { return formatRangeList; }


    // build format ranges from these (nested) scope ranges
    //
    //  [ source.c                             ]
    //     [ string.c          ]   [keyword]
    //         [ escape. ]
    // =
    //  [ ][xx][#########][xxxx][ ][kkkkkkk][  ]
    //
    QStack<ScopedTextRange*> activeRanges;
    activeRanges.append( scopedRanges->at(0) );

    int lastOffset = 0; //lineStartOffset;
    for( int i=1, cnt=scopedRanges->size(); i<cnt; ++i ) {
        ScopedTextRange* range = scopedRanges->at(i);
        int min = range->min();  // find the minimum position

        // unwind the stack if required
        while( activeRanges.size() > 1 ) {
            ScopedTextRange* activeRange = activeRanges.last();
            int activeRangeMax = activeRange->max();

            // when the 'min' is behind the end of the textrange on the stack we need to pop the stack
            if( activeRangeMax <= min ) {
                appendFormatRange( formatRangeList, lastOffset, activeRangeMax-1, activeRanges );
                activeRanges.pop();
                lastOffset = activeRangeMax;
                Q_ASSERT( !activeRanges.empty() );
            } else {
                break;
            }
        }

        // add a new 'range' if a new one is started and there's a 'gap'
        if( lastOffset < min ) {
            appendFormatRange( formatRangeList, lastOffset, min-1, activeRanges );
            lastOffset = min;
        }

        // push the new range to the stack
        activeRanges.push_back( range );

    }

    // next we must unwind the stack
    while( !activeRanges.isEmpty() ) {
        ScopedTextRange* activeRange = activeRanges.last();
        int activeRangeMax = activeRange->max();
        if( lastOffset < activeRange->max() ) {
            appendFormatRange(formatRangeList, lastOffset, activeRangeMax-1, activeRanges );
            lastOffset = activeRange->max();
        }
        activeRanges.pop();
    }

    return formatRangeList;
}

void TextThemeStyler::setThemeName(const QString &themeName)
{
    themeName_ = themeName;
}

TextTheme *TextThemeStyler::theme()
{
    TextTheme* theme = Edbee::instance()->themeManager()->theme(themeName_);
    if( !theme ) {
        theme = Edbee::instance()->themeManager()->fallbackTheme();  }
    return theme;
}



/// This method returns the character format for the given text scope
QTextCharFormat TextThemeStyler::getTextScopeFormat( QVector<ScopedTextRange*>& activeRanges )
{
//    ScopedTextRange* range = activeRanges.last();
    QTextCharFormat format;

    TextScopeList scopeList(activeRanges);
    theme()->fillFormatForTextScopeList( &scopeList, &format );
    return format;
}


/// helper function to create a format range
void TextThemeStyler::appendFormatRange(QList<QTextLayout::FormatRange> &rangeList, int start, int end,  QVector<ScopedTextRange*>& activeRanges )
{
    // only append a format if the lexer style is different then default
    if( activeRanges.size() > 1  ) {
        QTextLayout::FormatRange formatRange;
        formatRange.start  = start;
        formatRange.length = end - start + 1;
        formatRange.format = getTextScopeFormat( activeRanges );
        rangeList.append( formatRange );
    }
}

void TextThemeStyler::textDocumentChanged(edbee::TextDocument *oldDocument, edbee::TextDocument *newDocument)
{
    Q_UNUSED(newDocument);
    Q_UNUSED(oldDocument);
}

/// invalidates all layouts
void TextThemeStyler::invalidateLayouts()
{
//    formateRangeListCache_.clear();
}


//=================================================


TextThemeManager::TextThemeManager()
    : fallbackTheme_(0)
{
    fallbackTheme_ = new TextTheme();
    fallbackTheme_->setForegroundColor(0xFF000000);
    fallbackTheme_->setBackgroundColor(0xFFFFFFFF);
}

TextThemeManager::~TextThemeManager()
{
    clear();
    delete fallbackTheme_;
}


/// clears all items
void TextThemeManager::clear()
{
    qDeleteAll(themeMap_);
    themeMap_.clear();
    themeNames_.clear();
}

/// This method loads all theme names from the given theme path (*.tmTheme files)
/// @param the new themePath. if the themepath is blank, the themes of the current path are reloaded
void TextThemeManager::listAllThemes(const QString& themePath)
{
    if( !themePath.isEmpty() ) { themePath_ = themePath; }
    if( !themePath_.isEmpty() ) {
        clear();
        QDir dir(themePath_);
        QFileInfoList fileInfoList = dir.entryInfoList(QStringList("*.tmTheme"),QDir::Files, QDir::Name);
        foreach( QFileInfo fi, fileInfoList ) {
            themeNames_.append( fi.baseName() );
        }
    }
}

/// Returns the theme name
QString TextThemeManager::themeName(int idx)
{
    return themeNames_.at(idx);
}

/// This method gets or loads the theme
TextTheme *TextThemeManager::theme(const QString& name)
{
    if( name.isEmpty() ) { return 0; }
    TextTheme* theme=themeMap_.value(name);
    if( !theme ) {

        QString fullPath = QString("%1/%2.tmTheme").arg(themePath_).arg(name);
        QFile file(fullPath);
        if( file.exists() && file.open(QIODevice::ReadOnly) ) {
            TmThemeParser parser;
            theme = parser.readContent(&file);
            if( theme ) {
                delete themeMap_.value(name);       // delete the old theme
            } else {
                qlog_warn() << "Error parsing theme" << file.fileName() << ":" << parser.lastErrorMessage();
                theme = new TextTheme();
            }
            file.close();
        } else {
            qlog_warn() << "Error theme not found" << file.fileName() << ".";
            theme = new TextTheme();
        }

        // add the theme to the map
        themeMap_.insert(name, theme);
    }
    return theme;
}


} // edbee
