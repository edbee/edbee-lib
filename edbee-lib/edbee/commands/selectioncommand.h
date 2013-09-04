/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "edbee/texteditorcommand.h"

namespace edbee {

/// The selection action
class SelectionCommand : public TextEditorCommand
{
public:
    enum SelectionType {

      // movement and selection
        MoveCaretByCharacter,
        // SubWord, // TODO, implement subword selectin
        MoveCaretByWord,
        MoveCaretByLine,
        MoveCaretByPage,
        MoveCaretToLineBoundary,
        MoveCaretToDocumentBegin,
        MoveCaretToDocumentEnd,
        MoveCaretToExactOffset,         // amount is the exact offset value

      // selection only
        SelectAll,                      // amount is not used
        SelectWord,                     // select a full word
        SelectFullLine,                 // select a full line

      // add an extra caret
        AddCaretAtOffset,                // amount is the caret offset
        AddCaretByLine,                 /// amount is the amount of lines and the direction to add
        ResetSelection
    };

public:
    explicit SelectionCommand( SelectionType unit, int amount=0, bool keepSelection=false );
    virtual ~SelectionCommand();

    virtual int commandId();


    virtual void execute( TextEditorController* controller );
    virtual QString toString();

    SelectionType unit() { return unit_; }
    int amount() { return amount_; }
    bool keepSelection() { return keepSelection_; }

private:

    SelectionType unit_;
    int amount_;
    bool keepSelection_;

    
};


} // edbee
