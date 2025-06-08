// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textselection.h"

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textrenderer.h"
#include "edbee/views/textcaretcache.h"


#include "edbee/debug.h"

namespace edbee {

/// Constructs the textextselection object
/// @param controller the controller this selection is for
TextSelection::TextSelection(TextEditorController* controller)
    : TextRangeSet( controller->textDocument())
    , textControllerRef_( controller )
{
}


/// A copy constructor for copying a text-selection
TextSelection::TextSelection(const TextSelection& selection)
    : TextRangeSet(selection)
    , textControllerRef_( selection.textControllerRef_ )
{
}

/// The text selection destructor
TextSelection::~TextSelection()
{
}


/// This method is for moving the carets up or down
/// This method can be executed on (this) when a 0 pointer is given.
/// When another rangeSet is given this code is execute on that rangeset! (FIXME need a cleaner API for this)
///
/// @param controller the controller for this operation
/// @param rangeSet the rangeset to apply this operation on
/// @param amount the number of lines to move
void TextSelection::moveCaretsByLine(TextEditorController* controller, TextRangeSet* rangeSet, ptrdiff_t amount)
{
    TextDocument* doc      = controller->textDocument();
    TextRenderer* renderer = controller->textRenderer();

    TextCaretCache* cache = controller->textCaretCache();
    if (!cache->isFilled()) {
        cache->fill(*rangeSet);
    }

    // next move all lines
    TextCaretCache newCache(doc, renderer);
    for (size_t rangeIdx = rangeSet->rangeCount() - 1; rangeIdx != std::string::npos; --rangeIdx) {
        TextRange& range = rangeSet->range(rangeIdx);

        size_t caret = range.caret();
        int xpos  = cache->xpos(caret);   // the (original) caret x-position

        // change the line
        ptrdiff_t sline = static_cast<ptrdiff_t>(doc->lineFromOffset(caret)) + amount;
        if (sline < 0) { sline = 0; }
        size_t line = sline;
        //if( line >= doc->lineCount() ) { line = doc->lineCount()-1; }

        // calculate the correct column
        size_t col = renderer->columnIndexForXpos(line, xpos);
        size_t offset = doc->offsetFromLine(line);
        size_t offsetNextLine = doc->offsetFromLine(line + 1);
        size_t newLinesToRemove = line + 1 < doc->lineCount() ? 1 : 0;
        range.setCaretBounded(doc, qMin(offset + col, offsetNextLine - newLinesToRemove));
//        cache->caretMovedFromOldOffsetToNewOffset( caret, range.caret() );  // when having multiple caret this doesn't work !
        newCache.add(range.caret(), xpos);
    }
    cache->replaceAll(newCache);

//    rangeSet->processChangesIfRequiredKeepCaretCache();
    rangeSet->processChangesIfRequired();
}



/// Moves the carets by page
///
/// @param controller the controller for this operation
/// @param rangeSet the rangeset to apply this operation
/// @param amount the number of pages
void TextSelection::moveCaretsByPage(TextEditorController* controller, TextRangeSet* rangeSet, ptrdiff_t amount)
{
    TextRenderer* renderer = controller->textRenderer();
    TextDocument* doc      = controller->textDocument();
    int linesPerPage = renderer->viewHeightInLines();
    for (size_t rangeIdx = rangeSet->rangeCount() - 1; rangeIdx != std::string::npos; --rangeIdx) {
        TextRange& range = rangeSet->range(rangeIdx);
        ptrdiff_t line = doc->lineFromOffset(range.caret());
        line += linesPerPage * amount;
        if (line < 0) { line = 0; }
        size_t offset = doc->offsetFromLine(static_cast<size_t>(line));
        range.setCaretBounded(doc, offset);

    }
    rangeSet->processChangesIfRequired();
}


/// This method adds the ranges by line
void TextSelection::addRangesByLine(TextEditorController* controller, TextRangeSet* rangeSet, ptrdiff_t amount)
{
    TextDocument* doc      = controller->textDocument();
    TextRenderer* renderer = controller->textRenderer();


    TextCaretCache* cache = controller->textCaretCache();

    if (!cache->isFilled()) {
        cache->fill(*rangeSet);
    }

    // next move all lines
    rangeSet->beginChanges(); // prevent the clearing of the range cache
    for (size_t rangeIdx=rangeSet->rangeCount() - 1; rangeIdx != std::string::npos; --rangeIdx) {
        TextRange& range = rangeSet->range(rangeIdx);

        size_t offset = amount > 0 ? range.max() : range.min();
        size_t xpos = cache->xpos(offset);   // the (original) caret x-position

        // change the line
        ptrdiff_t line = static_cast<ptrdiff_t>(doc->lineFromOffset(offset)) + amount;
        if (line >= 0) {

            // calculate the correct column
            int col = renderer->columnIndexForXpos(line, xpos);
            size_t lineOffset = doc->offsetFromLine(line);
            size_t offsetNextLine = doc->offsetFromLine(line + 1);
            int newLinesToRemove = line + 1 < doc->lineCount() ? 1 : 0;

            offset = qMin(lineOffset + col, offsetNextLine - newLinesToRemove);
            if (offset >= 0 && offset <= doc->length()) {
                rangeSet->addRange(offset, offset);
                cache->add(offset, xpos); // add the original xpos
            }
        }
    }
    rangeSet->endChanges();
    rangeSet->processChangesIfRequired();
}


/// Returns the controller for this selection
TextEditorController* TextSelection::textEditorController() const
{
    return textControllerRef_;
}


/// This method process the changes if required
void TextSelection::processChangesIfRequired( bool joinBorders)
{
    TextRangeSet::processChangesIfRequired(joinBorders);
    if (!changing()) {
        textEditorController()->textCaretCache()->clear();
    }
}


/// This method process the changes if required
void TextSelection::processChangesIfRequiredKeepCaretCache(bool joinBorders)
{
    Q_UNUSED(joinBorders);
}

} // edbee
