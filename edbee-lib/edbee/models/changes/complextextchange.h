/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "edbee/models/textchange.h"

namespace edbee {

class LineDataListTextChange;
class SingleTextChange;
class TextRangeSet;

/// A special mergable group textchange. Used by the editor
/// to merge editing operation together..
/// I hate the name, but currently don't know a better name of this class :)
class ComplexTextChange : public TextChangeGroup
{
public:
    ComplexTextChange( TextEditorController* controller );
    virtual ~ComplexTextChange();

    // this change cannot be optimized away
    virtual bool isDiscardable() { return false; }

    virtual void groupClosed();

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* document);


    bool mergeTextChange(TextDocument* document, SingleTextChange *gTextChange, int& nextTextChangeMergeIndex );
    bool mergeLineDataTextChange( TextDocument* document, LineDataListTextChange* lineDataTextChange, int& nextLineChangeIndex );


    /// This method tries to merge the given change with the other change
    /// The textChange supplied with this method should NOT have been executed yet.
    /// It's the choice of this merge operation if the execution is required
    /// @param textChange the textchange to merge
    /// @return true if the merge has been successfull. False if nothing has been merged and executed
    virtual bool merge( TextDocument* document, TextChange* textChange );

    virtual QString toString();

private:

    TextRangeSet* previousSelection_;
    TextRangeSet* newSelection_;

};

} // edbee
