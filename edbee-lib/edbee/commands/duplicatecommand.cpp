// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "duplicatecommand.h"

#include "edbee/models/changes/mergablechangegroup.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"
#include "edbee/views/textselection.h"
#include "edbee/texteditorcontroller.h"

#include "edbee/debug.h"

namespace edbee {

/// Executes the duplication command
/// @param controller the active controller
void DuplicateCommand::execute(TextEditorController* controller)
{
    // create a new range set and the new texts
    TextRangeSet newRanges( controller->textSelection());
    TextRangeSet* newSelection = new TextRangeSet(controller->textSelection());
    QStringList newTexts;
    TextDocument* doc = controller->textDocument();

    // iterate over all range and build the new texts to insert
    size_t delta = 0;
    for (size_t i = 0, cnt = newRanges.rangeCount(); i < cnt; ++i) {
        TextRange& range = newRanges.range(i);

        // when the range is empty we need to use the complete line line (inclusive the newline)
        if (range.isEmpty()) {
            // get the complete line
            size_t line = doc->lineFromOffset(range.caret());
            range.setCaret(doc->offsetFromLine(line));
            range.setAnchor(range.caret());
            newTexts.append(QStringLiteral("%1\n").arg(doc->lineWithoutNewline(line)));

        } else {
            // append the new text, and change the insert position to the place before the caret :)
            newTexts.append(doc->textPart(range.min(), range.length()));
            range.maxVar() = range.min();
        }

        // change the spatial of the new selection
        // insert the spatial at the caret position.
        // we only insert text so the length = 0 and the new length is the length of the inserted text
        size_t length = static_cast<size_t>(newTexts.last().length());
        newSelection->changeSpatial(range.caret() + delta, 0, length);
        delta += length;
    }

    // replace the texts
    doc->beginChanges(controller);
    doc->replaceRangeSet(newRanges, newTexts);
    doc->giveSelection(controller,  newSelection);
    doc->endChanges(CoalesceId_Duplicate);
}


/// Returns the textual representation of this command
QString DuplicateCommand::toString()
{
    return "DuplicateCommand";
}

} // edbee
