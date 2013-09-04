/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "textdocument.h"

#include "edbee/models/changes/complextextchange.h"
#include "edbee/models/changes/linedatatextchange.h"
#include "edbee/models/changes/singletextchange.h"
#include "edbee/models/changes/singletextchangewithcaret.h"

#include "edbee/models/textlinedata.h"
#include "edbee/models/textdocumentfilter.h"
#include "edbee/models/textrange.h"
#include "edbee/models/textundostack.h"

#include "debug.h"

namespace edbee {


TextDocument::TextDocument( QObject* obj )
    : QObject(obj)
    , documentFilter_(0)
    , documentFilterRef_(0)
{
}

TextDocument::~TextDocument()
{
    delete documentFilter_;
}


/// This method can be used to change the number of reserved fields by the document
/// Increasing the amount will result in a realoc
/// Decreasting the fieldcount reults in the lost of the 'old' fields
/// At least the 'PredefinedFieldCount' amont of fields are required
/// This method EMPTIES the undo-stack. So after this call all undo history is gone!
void TextDocument::setLineDataFieldsPerLine( int count )
{
    Q_ASSERT( count >= PredefinedFieldCount );
    lineDataManager()->setFieldsPerLine( count );
    textUndoStack()->clear();
}


/// This method gives a given data item to a text line
void TextDocument::giveLineData(int line, int field, TextLineData* dataItem)
{
    Q_ASSERT(line < lineCount() );
    LineDataTextChange* change = new LineDataTextChange( line, field );
    change->giveLineData( dataItem );
    executeAndGiveChange( change, true );
}

TextLineData* TextDocument::getLineData(int line, int field)
{
    int len = lineDataManager()->length();
    Q_ASSERT( len == lineCount() );
    Q_ASSERT( line < len );
    return lineDataManager()->get( line, field );
}

void TextDocument::beginUndoGroup(TextChangeGroup* group)
{
//    if( documentFilter() ) {
//        documentFilter()->filterBeginGroup( this, group );
//    }
    textUndoStack()->beginUndoGroup(group);
}

void TextDocument::endUndoGroup( int coalesceId, bool flatten)
{
//    if( documentFilter() ) {
//        TextChangeGroup* group = textUndoStack()->currentGroup();
//        documentFilter()->filterEndGroup( this, group, coalesceId, flatten );
//    }
    textUndoStack()->endUndoGroup(coalesceId,flatten);
}

/// Ends the undo group and discards all recorded information
/// Warning it doesn NOT undo all made changes!!!
void TextDocument::endUndoGroupAndDiscard()
{
    textUndoStack()->endUndoGroupAndDiscard();
}

/// this method return true if the undo stack is enabled
bool TextDocument::isUndoCollectionEnabled()
{
    return textUndoStack()->isCollectionEnabled();
}

/// Sets the undo collection
void TextDocument::setUndoCollectionEnabled(bool enabled)
{
    textUndoStack()->setCollectionEnabled(enabled);
}

/// This method should return true if the current change is the cause of an undo operation
bool TextDocument::isUndoRunning()
{
    return textUndoStack()->isUndoRunning();
}

bool TextDocument::isRedoRunning()
{
    return textUndoStack()->isRedoRunning();
}

/// Is it an undo or redo (which means all commands area already available)
bool TextDocument::isUndoOrRedoRunning()
{
    return isUndoRunning() || isRedoRunning();
}

/// Checks if the document is in a persited state
bool TextDocument::isPersisted()
{
    return textUndoStack()->isPersisted();
}

/// Calc this method to mark current state as persisted
void TextDocument::setPersisted(bool enabled)
{
    textUndoStack()->setPersisted(enabled);
}

/// Sets the document filter without tranfering the ownership
void TextDocument::setDocumentFilter(TextDocumentFilter* filter)
{
    delete documentFilter_;
    documentFilter_  = 0;
    documentFilterRef_ = filter;
}

/// this method sets the document filter
/// You can give a 0 pointer to delte the old filter!
void TextDocument::giveDocumentFilter(TextDocumentFilter* filter)
{
    delete documentFilter_;
    documentFilter_ = filter;
    documentFilterRef_ = filter;
}

/// This method returns the document filter
TextDocumentFilter* TextDocument::documentFilter()
{
    return documentFilterRef_;
}

/// replaces the given range-set with the given string
/// Warning when a documentfilter is installed it is possible the rangeSet is modified!!
/// @param rangeSet the rangeSet to replace
/// @param text the text to place at the given ranges
/// @param coalesceId the coalesceid
void TextDocument::replaceRangeSet(TextRangeSet& rangeSet, const QString& textIn, int coalesceId, TextEditorController* controller)
{
    beginUndoGroup( new ComplexTextChange( controller) );

    QString text = textIn;
    if( documentFilter() ) {
        documentFilter()->filterReplaceRangeSet( this, rangeSet, text );
    }

    rangeSet.beginChanges();

    int idx = 0, oldRangeCount = 0;
    while( idx < (oldRangeCount = rangeSet.rangeCount())  ) {
        TextRange& range = rangeSet.range(idx);

        SingleTextChangeWithCaret* change = new SingleTextChangeWithCaret(range.min(),range.length(),text,-1);

        // this can be filtered
        executeAndGiveChange( change, false );

        // so we need to adjust the caret with the (possible) adjusted change
        if( change->caret() < 0 ) {
            range.setCaret( change->offset() + change->length() );
        } else {
            range.setCaret( change->caret() );
        }
        range.reset();

        // next range.
        if( rangeSet.rangeCount() < oldRangeCount ) {
qlog_info() <<  "TEST TO SEE IF THIS REALLY HAPPENS!! I think it cannot happen. (but I'm not sure)";
Q_ASSERT(false);
            // else we stay at the same location
        } else {
            ++idx;
        }
    }

    rangeSet.endChanges();

    endUndoGroup(coalesceId,true);
}


/* Temporary disabled
void TextDocument::replace(int offset, int length, const QString& newText, bool merge )
{
    textUndoStack()->beginUndoGroup(0);  // we need to start a new undo-group, because line-data-changes are added seperately

    TextChange* change = new SingleTextChange( offset, length, newText );
    executeAndGiveChange( change, merge );

    textUndoStack()->endUndoGroup(0,true);
}
*/

/// call this method to execute a change. The change is first passed to the filter
/// so the documentFilter can handle the processing of the change
/// When not filter is active the 'execute' method is called on the change
void TextDocument::executeAndGiveChange(TextChange* change, int coalesceId )
{
    if( documentFilter() ) {
        documentFilter()->filterChange( this, change, coalesceId );
    } else {
        change->execute( this );
        giveChangeWithoutFilter( change, coalesceId );
    }
}

/// Appends the given text to the document
/// @param text the text to append
/// @param coalesceId (default 0) the coalesceId to use. Whe using the same number changes could be merged to one change. CoalesceId of 0 means no merging
void TextDocument::append(const QString& text, int coalesceId )
{
    replace( this->length(), 0, text, coalesceId );
}

/// Appends the given text
/// @param text the text to append
/// @param coalesceId (default 0) the coalesceId to use. Whe using the same number changes could be merged to one change. CoalesceId of 0 means no merging
void TextDocument::replace( int offset, int length, const QString& text, int coalesceId )
{
    executeAndGiveChange( new SingleTextChange( offset, length, text ), coalesceId );
}

void TextDocument::setText(const QString& text)
{
    replace( 0, length(), text, 0 );
}


/// begins the raw append modes. In raw append mode data is directly streamed
/// to the textdocument-buffer. No undo-data is collected and no events are fired
void TextDocument::rawAppendBegin()
{
    setUndoCollectionEnabled(false); // no undo's
    buffer()->rawAppendBegin();
}

/// When then raw appending is done. The events are fired that the document has been changed
/// The undo-collection is enabled again
void TextDocument::rawAppendEnd()
{
    buffer()->rawAppendEnd();
    setUndoCollectionEnabled(true);
}

/// Appends a single char in raw append mode
void TextDocument::rawAppend(QChar c)
{
    buffer()->rawAppend(c);
}

/// Appends an array of characters
void TextDocument::rawAppend(const QChar* chars, int length)
{
    buffer()->rawAppend(chars,length);
}


/// This method executes the given 'multi-text-change'
//void TextDocument::giveChange(TextChange *change, bool merge)
//{
//    if( documentFilter() ) {
//        documentFilter()->filterChange( this, change, merge );
//    } else {
//        giveChangeWithoutFilter( change, merge );
//    }
//}

/// default implementation is to forward this call to the textbuffer
int TextDocument::length()
{
    return buffer()->length();
}

int TextDocument::lineCount()
{
    return buffer()->lineCount();
}

QChar TextDocument::charAt(int idx)
{
    return buffer()->charAt(idx);
}

int TextDocument::offsetFromLine(int line)
{
    return buffer()->offsetFromLine(line);
}

int TextDocument::lineFromOffset(int offset)
{
    return buffer()->lineFromOffset(offset);
}

int TextDocument::columnFromOffsetAndLine(int offset, int line)
{
    return buffer()->columnFromOffsetAndLine(offset,line);
}

int TextDocument::offsetFromLineAndColumn(int line, int column)
{
    return buffer()->offsetFromLineAndColumn(line,column);
}

int TextDocument::lineLength(int line)
{
    return buffer()->lineLength(line);
}

QString TextDocument::text()
{
    return buffer()->text();
}

/// Returns the given part of the text
QString TextDocument::textPart(int offset, int length)
{
    return buffer()->textPart( offset, length );
}

QString TextDocument::lineWithoutNewline(int line)
{
    return buffer()->lineWithoutNewline(line);
}


QString TextDocument::line(int line)
{
    return buffer()->line(line);
}


} // edbee
