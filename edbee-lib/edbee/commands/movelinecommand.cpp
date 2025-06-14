// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "movelinecommand.h"

#include "edbee/models/change.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"

#include "edbee/debug.h"


namespace edbee {

MoveLineCommand::MoveLineCommand(int direction)
    : direction_(direction)
{

}

MoveLineCommand::~MoveLineCommand()
{
}


/// all carets in the newCaretSelection are changed in the movedRange
void updateNewCaretSelectionForMove(TextDocument* doc, TextRangeSet& newCaretSelection, TextRange& movedRange, int direction)
{
    for (size_t i = 0, cnt = newCaretSelection.rangeCount(); i < cnt; ++i) {
        TextRange& newRange = newCaretSelection.range(i);
        if (movedRange.min() <= newRange.min() && newRange.max() <= movedRange.max()) {
            size_t min = movedRange.min();
            size_t max = movedRange.max();

            ptrdiff_t delta = 0;
            if (direction < 0) {
                size_t line = doc->lineFromOffset(min);
                delta = - static_cast<ptrdiff_t>(doc->lineLength(line - 1));
            } else {
                size_t line = doc->lineFromOffset(max - 1); // -1 exclude the last newline
                delta = static_cast<ptrdiff_t>(doc->lineLength(line + 1));
            }

            newRange.setCaretBounded(doc, static_cast<ptrdiff_t>(newRange.caret()) + delta);
            newRange.setAnchorBounded(doc, static_cast<ptrdiff_t>(newRange.anchor()) + delta);
        }
    }
}


void MoveLineCommand::execute(TextEditorController *controller)
{
    TextRangeSet* sel = controller->textSelection();
    TextDocument* doc = controller->textDocument();

    // expand the selection in full-lines
    TextRangeSet moveRangeSet(*sel);
    moveRangeSet.expandToFullLines(1);
    moveRangeSet.mergeOverlappingRanges(true);

    // because of the full-line expansion, we can assume that 0 means at the first line
    // moving up on the first line is impossible
    size_t firstOffset = moveRangeSet.firstRange().minVar();
    if (static_cast<ptrdiff_t>(firstOffset) + direction_ <= 0) return;

    size_t lastOffset = moveRangeSet.lastRange().maxVar();
    if (static_cast<ptrdiff_t>(lastOffset) + direction_ > static_cast<ptrdiff_t>(doc->length())) return;

    // Calculate the new caret positions
    TextRangeSet newCaretSelection( *sel );
    for (size_t i = 0, cnt = moveRangeSet.rangeCount(); i < cnt; ++i) {
        TextRange& range = moveRangeSet.range(i);
        updateNewCaretSelectionForMove(doc, newCaretSelection, range, direction_);
    }

    // move the text in the correct direciton
    controller->beginUndoGroup();
    for (size_t i = 0, cnt = moveRangeSet.rangeCount(); i < cnt; ++i) {
        TextRange& range = moveRangeSet.range(i);

        size_t line = doc->lineFromOffset(range.min());
        QString text = doc->textPart(range.min(), range.length());

        controller->replace(range.min(), range.length(), QStringLiteral(), 0);               // remove the line

        ptrdiff_t replaceOffset = static_cast<ptrdiff_t>(line) + direction_;
        Q_ASSERT(replaceOffset >= 0);
        controller->replace(doc->offsetFromLine(static_cast<size_t>(replaceOffset)), 0, text, 0); // add the line back
    }

    *sel = newCaretSelection; // change the selection

    int coalesceID = CoalesceId_MoveLine + direction_ + 10;
    controller->endUndoGroup(coalesceID, false);
}


QString MoveLineCommand::toString()
{
    return "MoveLineCommand";
}


}
