/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "textsearcher.h"

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/texteditorwidget.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/util/regexp.h"
#include "edbee/views/textselection.h"

#include "debug.h"

namespace edbee {

TextSearcher::TextSearcher( QObject* parent )
    : QObject(parent)
    , searchTerm_()
    , syntax_(SyntaxPlainString)
    , caseSensitive_(false)
    , wrapAround_(true)
    , reverse_(false)
    , regExp_(0)
{
}

TextSearcher::~TextSearcher()
{
    delete regExp_;
}

/// Sets the current search term
void TextSearcher::setSearchTerm(const QString &term)
{
    searchTerm_ = term;
    setDirty();
}

/// Sets the syntax mode of the searcher
/// @parm syntax the SyntaxType to use (SyntaxPlainString, SyntaxRegExp)
void TextSearcher::setSyntax(TextSearcher::SyntaxType syntax)
{
    syntax_ = syntax;
    setDirty();
}


/// Toggles the case sensitivity of the search operation
void TextSearcher::setCaseSensitive(bool sensitive)
{
    caseSensitive_ = sensitive;
    setDirty();
}


/// For changing the wrap around mode of the TextSearcher
void TextSearcher::setWrapAround(bool on)
{
    wrapAround_ = on;
    setDirty();
}

/// Finds the next matching textrange
/// @param widget the widget to search in
/// @return the textRange with the found text.  TextRange::isEmpty() can be called to check if nothing has been found
TextRange TextSearcher::findNextRange(TextEditorWidget* widget)
{
    TextDocument* document = widget->textDocument();
    QChar* buffer          = document->buffer()->rawDataPointer();


    if( !regExp_ ) { regExp_ = createRegExp(); }

    int caretPos= 0;
    if( widget->textSelection()->rangeCount() > 0 ) {
        if( isReverse() ) {
            caretPos = widget->textSelection()->firstRange().min();
        } else {
            caretPos = widget->textSelection()->lastRange().max();
        }
    }

    int idx = 0;
    if( isReverse() ) {
        idx = regExp_->lastIndexIn(buffer,0,caretPos);
    } else {
        idx = regExp_->indexIn(buffer,caretPos,document->length());
    }

    // wrapped around? Let's try it from the beginning
    if( idx < 0 && isWrapAroundEnabled() ) {
        if( isReverse() ) {
            idx = regExp_->lastIndexIn(buffer,0,document->length());
        } else {
            idx = regExp_->indexIn(buffer,0,document->length());
        }
    }
    if( idx >= 0 ) {
        int len = regExp_->len(0);
        return TextRange(idx,idx+len);
    }
    return TextRange();
}

/// Finds the next item based on the current caret position
void TextSearcher::findNext(TextEditorWidget* widget)
{
    TextRange range = findNextRange(widget);

    if( !range.isEmpty()) {
        TextDocument* document = widget->textDocument();
        TextRangeSet* ranges = new TextRangeSet( document );
        ranges->addRange(range.anchor(),range.caret());
        widget->controller()->changeAndGiveTextSelection( ranges );
    }
}

void TextSearcher::findPrev(TextEditorWidget* widget)
{
    reverse_ = !reverse_;
    findNext(widget);
    reverse_ = !reverse_;
}

/// Selects the next word that matches the given critaria (Adds an extra selection range )
void TextSearcher::selectNext(TextEditorWidget* widget)
{
    TextRange range = findNextRange(widget);

    if( !range.isEmpty()) {
        TextSelection* sel = widget->controller()->textSelection();
// TODO: make it undoable?!??
        sel->addRange( range.anchor(), range.caret() );
        widget->updateComponents();
    }
}

void TextSearcher::selectPrev(TextEditorWidget* widget)
{
    reverse_ = !reverse_;
    selectNext(widget);
    reverse_ = !reverse_;
}

/// Selects all matches
void TextSearcher::selectAll(TextEditorWidget *widget)
{
    TextSelection* sel = widget->controller()->textSelection();
    TextRange oldRange = sel->range(0);

    // clear the selection and add all matches
    bool oldReverse = reverse_;     // we must NOT reverse find
    bool oldWrapAround = wrapAround_;
    reverse_ = false;
    wrapAround_ = false;
    sel->clear();
    TextRange range = findNextRange(widget);
    while( !range.isEmpty() )
    {
        sel->addRange(range.anchor(), range.caret());
        range = findNextRange(widget);
    }
    wrapAround_ = oldWrapAround;
    reverse_ = oldReverse;
    // no selection, we MUST place a caret
    if( sel->rangeCount() == 0 ) {
        sel->addRange(oldRange.caret(), oldRange.caret());
    }
    widget->updateComponents();
}

/// Marks the regexp as dirty (and deletes it)
void TextSearcher::setDirty()
{
    delete regExp_;
    regExp_ = 0;
}

/// Creates a regular expression for the current opions
RegExp *TextSearcher::createRegExp()
{
    RegExp::Syntax regExpSyntax = RegExp::SyntaxFixedString;
    if( syntax() == SyntaxRegExp ) regExpSyntax = RegExp::SyntaxDefault;

    RegExp* regExp = new RegExp( searchTerm(), RegExp::EngineOniguruma, regExpSyntax );
    return regExp;
}


} // edbee
