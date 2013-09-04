/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "texteditorcontroller.h"

#include <QApplication>
#include <QThread>

#include "edbee/commands/selectioncommand.h"
#include "edbee/models/changes/complextextchange.h"
#include "edbee/models/changes/singletextchange.h"
#include "edbee/models/changes/singletextchangewithcaret.h"
#include "edbee/models/changes/selectiontextchange.h"
#include "edbee/models/chardocument/chartextdocument.h"
#include "edbee/models/textbuffer.h"
#include "edbee/models/textchange.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textdocumentscopes.h"
#include "edbee/models/texteditorcommandmap.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/models/texteditorkeymap.h"
#include "edbee/models/textrange.h"
#include "edbee/models/textsearcher.h"
#include "edbee/models/textundostack.h"
#include "edbee/texteditorcommand.h"
#include "edbee/edbee.h"
#include "edbee/texteditorwidget.h"
#include "edbee/views/components/texteditorcomponent.h"
#include "edbee/views/textrenderer.h"
#include "edbee/views/textcaretcache.h"
#include "edbee/views/textselection.h"

#include "debug.h"


namespace edbee {


TextEditorController::TextEditorController( TextEditorWidget* widget, QObject *parent)
    : QObject(parent)
    , widgetRef_(widget)
    , textDocument_(0)
    , textDocumentRef_(0)
    , textSelection_(0)
    , keyMap_(0)
    , keyMapRef_(0)
    , commandMap_(0)
    , commandMapRef_(0)
    , textRenderer_(0)
    , textCaretCache_(0)
    , textSearcher_(0)
    , autoScrollToCaret_(AutoScrollAlways)
{

    // create the keymap
    keyMapRef_ = Edbee::instance()->defaultKeyMap();
    commandMapRef_ = Edbee::instance()->defaultCommandMap();

    // create the text renderer
    textRenderer_ = new TextRenderer( this );

    // create a text document ( this sould happen AFTER the creation of the renderer)
    giveTextDocument( new CharTextDocument() );


    // Now all objects have been created we can init them
    textRenderer_->init();
    textRenderer_->setThemeName( textDocument()->config()->themeName() );

}


TextEditorController::~TextEditorController()
{
    delete textSearcher_;
    delete textRenderer_;
    delete textCaretCache_;
    delete textSelection_;
    delete textDocument_;
    delete commandMap_;
    delete keyMap_;
}



/// This method is called to reset the caret timer and update the ui
void TextEditorController::notifyStateChange()
{
    if( widgetRef_ ) {
        widgetRef_->resetCaretTime();

        // scrolling is only required when focused (When scrolling without focus the sync-editor goes wacko :P )
        if( autoScrollToCaret_ == AutoScrollAlways || (autoScrollToCaret_ == AutoScrollWhenFocus && hasFocus())   ) {
            scrollOffsetVisible( textSelection()->range(0).caret() );
        }

        widgetRef_->updateComponents();
    }
}

/// sets the document and tranfers the ownership of the textdocument to this class
void TextEditorController::giveTextDocument(TextDocument* doc)
{
    if( doc != textDocument_ ) {
        setTextDocument( doc );
        textDocument_ = doc;        // this is all that's required for getting the ownership
    }
}

/// Set the text document
void TextEditorController::setTextDocument(TextDocument* doc)
{
    Q_ASSERT_GUI_THREAD;

    if( doc != textDocumentRef_ ) {       
        // disconnect the old document
        TextDocument* oldDocumentRef = textDocument();
        if( oldDocumentRef ) {
            oldDocumentRef->textUndoStack()->unregisterController(this);
            disconnect( oldDocumentRef, SIGNAL(textChanged(edbee::TextBufferChange)), this, SLOT(onTextChanged(edbee::TextBufferChange)) );
            disconnect( textDocumentRef_->lineDataManager(), SIGNAL(lineDataChanged(int,int,int)), this, SLOT(onLineDataChanged(int,int,int)));
        }

        // delete some old and dependent objects
        delete textCaretCache_;
        delete textSelection_;

        // create the document
        textDocumentRef_ = doc;
        textCaretCache_ = new TextCaretCache( textDocumentRef_, textRenderer_ ); // warning the cache needs to be created BEFORE the selection!!!
        textSelection_ = new TextSelection( this );
        textSelection_->addRange(0,0);  // add at least one cursor :-)

        textDocumentRef_->textUndoStack()->registerContoller(this);

        connect( textDocumentRef_, SIGNAL(textChanged(edbee::TextBufferChange)), this, SLOT(onTextChanged(edbee::TextBufferChange)));
        connect( textDocumentRef_->lineDataManager(), SIGNAL(lineDataChanged(int,int,int)), this, SLOT(onLineDataChanged(int,int,int)) );

        // create the new document
        emit textDocumentChanged( oldDocumentRef, textDocumentRef_ );

        // delete the old stuff
        delete textDocument_;
        textDocument_ = 0;
    }
}

/// This method return true if the text-editor has focus
bool TextEditorController::hasFocus()
{
    return widget()->hasFocus();
}


/// sets a keymap
void TextEditorController::setKeyMap(TextEditorKeyMap* keyMap)
{
    delete keyMap_;
    keyMap_ = 0;
    keyMapRef_= keyMap;
}

/// gives a keymap to the editor
void TextEditorController::giveKeyMap(TextEditorKeyMap* keyMap)
{
    delete keyMap_;
    keyMap_ = keyMap;
    keyMapRef_= keyMap;
}

/// set a commandmap
void TextEditorController::setCommandMap(TextEditorCommandMap* commandMap)
{
    delete commandMap_;
    commandMap_ = 0;
    commandMapRef_ = commandMap;
}

/// gives a commandmap to the editor
void TextEditorController::giveCommandMap(TextEditorCommandMap* commandMap)
{
    delete commandMap_;
    commandMap_ = commandMap;
    commandMapRef_ = commandMap;
}

/// Gives the text-searcher
void TextEditorController::giveTextSearcher(TextSearcher* searcher)
{
    delete textSearcher_;
    textSearcher_ = searcher;
}

TextSearcher *TextEditorController::textSearcher()
{
    if( !textSearcher_ ) { textSearcher_ = new TextSearcher(); }
    return textSearcher_;
}


//==========================================================================================
// Slots
//==========================================================================================

/// This slot is placed if a piece of text is replaced
void TextEditorController::onTextChanged( edbee::TextBufferChange change )
{
    Q_UNUSED(change);

    /// update the selection
    textSelection()->changeSpatial( change.offset(), change.length(), change.newTextLength() );

    /// TODO: improve this:
    if( widgetRef_) {
        widget()->updateGeometryComponents();
        notifyStateChange();
    }
}

/// the old selection has been changed
void TextEditorController::onSelectionChanged(TextRangeSet* oldRangeSet)
{
    Q_UNUSED(oldRangeSet);

    /// TODO: improve this:
    if( widgetRef_) {
        notifyStateChange();
    }
}

/// The line-data is changed, we need to repaint the selected lines
void TextEditorController::onLineDataChanged(int line, int length, int newLength)
{
    if( this->widgetRef_ ) {
        widgetRef_->updateLine( line, qMax(length,newLength));
    }
}

/// This method updates the status text. This is the text as displayed in the lower status bar
void TextEditorController::updateStatusText( const QString& extraText )
{
    QString text;
    TextDocument* doc = textDocument();

    if( !doc->textUndoStack()->isPersisted() ) {
        text.append("[*] ");
    }

    // add the ranges
    if( textSelection_->rangeCount() > 1 ) {
        text.append( QString("%1 ranges").arg(textSelection_->rangeCount() ) );
    } else {
        TextRange& range = textSelection_->range(0);
        int caret = range.caret();
        int line = doc->lineFromOffset( caret ) ;
        int col  = doc->columnFromOffsetAndLine( caret, line ) + 1;
        text.append( QString("Line %1, Column %2").arg(line+1).arg(col) );

        if( textDocument()->config()->showCaretOffset() ) {
            text.append( QString(", Offset %1").arg(caret) );
        }


        if( range.length() > 0 ) {
            text.append( QString(" | %1 chars selected").arg(range.length()) );

        // add the current scopes
        } else {
            text.append(" | scope: " );

            QString str;
            QVector<TextScope*> scopes = textDocument()->scopes()->scopesAtOffset( caret ) ;
            for( int i=0,cnt=scopes.size(); i<cnt; ++i ) {
                TextScope* scope = scopes[i];
                str.append( scope->name() );
                str.append(" ");
            }
//qlog_info() << "scopes: " << caret << ": " << scopes.size() << ": " << str;
            text.append( str );
            text.append( QString(" (%1)").arg( textDocument()->scopes()->lastScopedOffset() ) );
        }
    }

    // add the extra text
    if( !extraText.isEmpty() ) {
        text.append(" | " );
        text.append(extraText);
    }   
    emit updateStatusTextSignal( text );
}



/// Asserts the view shows the given position
void TextEditorController::scrollPositionVisible(int xPos, int yPos)
{
    emit widgetRef_->scrollPositionVisible( xPos, yPos );
}

/// Assets the view shows the given offset
void TextEditorController::scrollOffsetVisible(int offset)
{
    TextRenderer* renderer = textRenderer();
    int xPos = renderer->xPosForOffset(offset);
    int yPos = renderer->yPosForOffset(offset);
    scrollPositionVisible( xPos, yPos );
}

/// This method makes sure caret 1 is vible
void TextEditorController::scrollCaretVisible()
{
    scrollOffsetVisible( textSelection()->range(0).caret() );
}

/// This method adds a textchange on the stack that simply stores the current text-selection
void TextEditorController::storeSelection(int coalesceId)
{
    SelectionTextChange* change = new SelectionTextChange(this);
    change->giveTextRangeSet( new TextRangeSet( *textSelection() ) );
    textDocument()->executeAndGiveChange( change, coalesceId );
}


/// This method executes the command
void TextEditorController::executeCommand( TextEditorCommand* textCommand )
{
    emit commandToBeExecuted( textCommand  );
    textCommand->execute( this );
    emit commandExecuted( textCommand );

    // TODO: move this to a nicer place!!
    updateStatusText( textCommand->toString() );
}

/// Executes a command with the given name
/// @param name of the command to execute
/// @return true if the command exists
bool TextEditorController::executeCommand(const QString& name)
{
    TextEditorCommand* command = commandMap()->get(name);
    if( command ) { executeCommand( command ); }
    return command != 0;
}

/// replaces the given text
void TextEditorController::replace(int offset, int length, const QString& text, int coalesceId)
{
//    SelectionTextChange* change = new SelectionTextChange(this);
    TextRangeSet ranges(textDocument());
    ranges.addRange(offset, offset+length);
    replaceRangeSet(ranges,text,coalesceId);
}



/// This method replaces the selection with the given text
void TextEditorController::replaceSelection(const QString& text, int coalesceId )
{
    replaceRangeSet( *dynamic_cast<TextRangeSet*>( textSelection() ), text, coalesceId );
}

void TextEditorController::replaceRangeSet(TextRangeSet& rangeSet, const QString& text, int coalesceId)
{
    textDocument()->replaceRangeSet(rangeSet, text, coalesceId, this );
}


/// This method creates a command that moves the caret to the given line/column position
/// A negative number means that we're counting from the end
/// This method assumes line 0 is the first line!
///
/// For example:
///     moveCaretTo( 2, 1 ) => Moves the caret to the 3rd line and 2nd column
///     moveCaretTo( -1, -2 ) => Moves the caret to the character before the last character
void TextEditorController::moveCaretTo(int line, int col, bool keepAnchors )
{
    if( line < 0) {
        line = textDocument()->lineCount() + line;
        if( line < 0 ) { line = 0; }
    }
    int offset = textDocument()->offsetFromLine(line);
    int lineLength = textDocument()->lineLength(line);
    if( col < 0 ){
        col = lineLength + col;
    }
    offset += qBound(0, col, lineLength-1 );

//textDocument()->offsetFromLineAndColumn(line,col)

    return moveCaretToOffset( offset , keepAnchors );
}

void TextEditorController::moveCaretToOffset(int offset, bool keepAnchors)
{
//    SelectionCommand* command = new SelectionCommand( SelectionCommand::MoveCaretToExactOffset, offset, keepAnchors );
    SelectionCommand command( SelectionCommand::MoveCaretToExactOffset, offset, keepAnchors );
    return executeCommand( &command );
}

void TextEditorController::addCaretAt(int line, int col)
{
    return addCaretAtOffset( textDocument()->offsetFromLineAndColumn(line,col) );
}

/// Adds a carert at the given offset
void TextEditorController::addCaretAtOffset(int offset)
{
    SelectionCommand command( SelectionCommand::AddCaretAtOffset, offset );
    return executeCommand( &command );
}

/// This method changes the text selection
void TextEditorController::changeAndGiveTextSelection(TextRangeSet* rangeSet, int coalesceId )
{
    SelectionTextChange* change = new SelectionTextChange(this);
    change->giveTextRangeSet( rangeSet );
    textDocument()->executeAndGiveChange( change, coalesceId );
}

/// This method performs an undo operation. By supplying soft only
/// controller based operations are undone. When suppplying false a Document operation is being undone
void TextEditorController::undo(bool soft)
{
    textDocument()->textUndoStack()->undo( soft ? this : 0, soft );
}

/// This method performs an redo operation. By supplying soft only controller based operations are redone.
/// When suppplying false a Document operation is being redone
void TextEditorController::redo(bool soft)
{
    textDocument()->textUndoStack()->redo( soft ? this : 0, soft );
}


void TextEditorController::beginUndoGroup( TextChangeGroup* group )
{
    textDocument()->beginUndoGroup(group);
}

/// Ends the undo-group.
/// @param coalesceId is used to decide if merging of groups is required.
///                  a value of 0 means NO merging
///                  and id > 0 means if the previous command had the same id, the command is merged
/// @param flatten when an undogroup is ended and flatten is set to true ALL sub-undo-groups are merged to this group
void TextEditorController::endUndoGroup(int coalesceId, bool flatten )
{    
    textDocument()->endUndoGroup(coalesceId,flatten);
}


} // edbee
