// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "chartextdocument.h"

#include <QTextCodec>
#include <QApplication>
#include <QThread>

#include "chartextbuffer.h"
#include "edbee/lexers/grammartextlexer.h"
#include "edbee/models/changes/textchange.h"
#include "edbee/models/textautocompleteprovider.h"
#include "edbee/models/textgrammar.h"
#include "edbee/models/textdocumentscopes.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/models/textlinedata.h"
#include "edbee/models/textundostack.h"
#include "edbee/edbee.h"
#include "edbee/util/lineending.h"
#include "edbee/util/textcodec.h"

#include "edbee/debug.h"

namespace edbee {

/// The main contstructor of the chartext document
CharTextDocument::CharTextDocument(QObject* object)
    : edbee::CharTextDocument(new TextEditorConfig(), object)
{
}

CharTextDocument::CharTextDocument(TextEditorConfig* config, QObject* object)
    : TextDocument(object)
    , config_(config)
    , textBuffer_(nullptr)
    , textScopes_(nullptr)
    , textLexer_(nullptr)
    , textCodecRef_(nullptr)
    , lineEndingRef_(nullptr)
    , textUndoStack_(nullptr)
    , autoCompleteProviderList_(nullptr)
{
    Q_ASSERT_GUI_THREAD;

    // auto initialize edbee if this hasn't been done already
    Edbee::instance()->autoInit();

    textBuffer_ = new CharTextBuffer();

    textScopes_ = new TextDocumentScopes(this);

    textCodecRef_ = Edbee::instance()->codecManager()->codecForName("UTF-8");
    lineEndingRef_ = LineEnding::unixType();

    // create the text scopes and lexer
    textLexer_ = new GrammarTextLexer(textScopes_);

    // create the undo stack
    textUndoStack_ = new TextUndoStack(this);

    // create the autocomplete provider (with the global parent provider)
    autoCompleteProviderList_ = new TextAutoCompleteProviderList(Edbee::instance()->autoCompleteProviderList());

    // simply forward the about to change signal
    connect(textBuffer_, SIGNAL(textAboutToBeChanged(edbee::TextBufferChange)), SIGNAL(textAboutToBeChanged(edbee::TextBufferChange)), Qt::DirectConnection);
    connect(textBuffer_, SIGNAL(textChanged(edbee::TextBufferChange,QString)), SLOT(textBufferChanged(edbee::TextBufferChange,QString)), Qt::DirectConnection);

    // forward the persisted state changes
    connect(textUndoStack_, SIGNAL(persistedChanged(bool)), this,  SIGNAL(persistedChanged(bool)));

    connect(textScopes_, SIGNAL(lastScopedOffsetChanged(size_t,size_t)), this, SIGNAL(lastScopedOffsetChanged(size_t,size_t)));
}


/// The default constructor
CharTextDocument::~CharTextDocument()
{
    delete autoCompleteProviderList_;
    delete textUndoStack_;
    delete textLexer_;
    delete textScopes_;
    delete textBuffer_;
    delete config_;
}


/// Returns the active textbuffer
TextBuffer* CharTextDocument::buffer() const
{
    return textBuffer_;
}


/// returns the language grammar
TextGrammar* CharTextDocument::languageGrammar()
{
    return textLexer_->grammar();
}


/// Sets the language grammar
void CharTextDocument::setLanguageGrammar(TextGrammar* grammar)
{
    TextGrammar* oldGrammar = languageGrammar();
    textLexer_->setGrammar(grammar);
    if( oldGrammar != grammar ) {
        emit languageGrammarChanged();
    }
}


/// Returns the autocmoplete provider list
TextAutoCompleteProviderList* CharTextDocument::autoCompleteProviderList()
{
    return autoCompleteProviderList_;
}


/// This method returns the configuration
TextEditorConfig* CharTextDocument::config() const
{
    return config_;
}


// currently not implemented
//TextLineData* CharTextDocument::takeLineData(int line, int field)
//{
//    return textLineDataManager_->take( line, field );
//}


/// Gives a change to the undo stack without invoking the filter
/// @param change the change to execute
/// @param coalesceId the coalescing identifier
Change* CharTextDocument::giveChangeWithoutFilter(Change *change, int coalesceId)
{
    return textUndoStack()->giveChange(change, coalesceId);
}


/// This method replaces the given text via the undo-button
//void CharTextDocument::replaceTextWithoutFilter( int offset, int length, const QString& text )
//{
//    SingleTextChange *textChange = new SingleTextChange( offset, length, text);
//    textUndoStack()->giveAndExecuteChange( textChange, true );
//}


// the text is changed
void CharTextDocument::textBufferChanged(const TextBufferChange& change, QString oldText)
{
    if (textLexer_) {
        textLexer_->textChanged(change);
    }

    // execute the line change
    if (!isUndoOrRedoRunning()) {
        Change* lineDataChange = lineDataManager()->createLinesReplacedChange(change.line() + 1, change.lineCount(), change.newLineCount());
        if (lineDataChange) {
            executeAndGiveChange(lineDataChange, 0);
        }
    }

    // emit the textchanged singal
    emit textChanged(change, oldText);   // and notify the document listeners
}


} // edbee
