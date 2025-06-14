// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/texteditorcommand.h"

namespace edbee {

class TextDocument;
class TextRangeSet;

/// A delete command. This is a backspace or a delete operation
///
/// The delete behaviour is pretty obvious, it just deletes :)
///
/// The behaviour of the RemoveCharLeft depends on several factors:
///
/// When the editor has enabled useTabChar_is enabled, backspace is very simple, it simply
/// deletes the previous character or the selection
///
/// When spaces are used for tabs the behaviour is different. If the caret is left of
/// the first non-space character, it will move 1 column to the left, depending on the tabsize
///
class EDBEE_EXPORT RemoveCommand : public TextEditorCommand
{
public:
    enum RemoveMode {
        RemoveChar,                     ///< Remove a single character
        RemoveWord,                     ///< Remove a single word
        RemoveLine                      ///< Remove a line
    };

    enum Direction {
        Left,                           ///< Remove the item to the left
        Right                           ///< Remove the item to ther right
    };

    RemoveCommand(RemoveMode removeMode, Direction direction);

    int coalesceId() const;
    size_t smartBackspace(TextDocument* doc, size_t caret);

    void rangesForRemoveChar(TextEditorController* controller, TextRangeSet* ranges);
    void rangesForRemoveWord(TextEditorController* controller, TextRangeSet* ranges);
    void rangesForRemoveLine(TextEditorController* controller, TextRangeSet* ranges);

    virtual void execute(TextEditorController* controller) override;
    virtual QString toString() override;

private:
    int directionSign() const;

    RemoveMode removeMode_;        ///< The remove mode
    Direction direction_;         ///< The remove direction
};

} // edbee
