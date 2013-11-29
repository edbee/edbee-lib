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

/// A special mergable group textchange. Used by the editor  to merge editing operation together..
/// I hate the name, but currently don't know a better name of this class :)
///
/// This is a complete rewrite of the earlier implementation. This implemenation
/// simply adds all changes to a group. It tries to compress the changes that are compressable
/// This group has 1 previous selection and 1 next selection. All selection changes are 'removed' and only the last and first state is stored.
class ComplexTextChange : public TextChangeGroup
{
public:
    ComplexTextChange( TextEditorController* controller );
    virtual ~ComplexTextChange();

    // this change cannot be optimized away
    virtual bool isDiscardable();
    virtual void groupClosed();

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* document);

private:
    void addOffsetDeltaToTextChanges( int fromIndex, int delta );
    int findTextChangeInsertIndexForOffset( int offset );
    int mergeTextChange(TextDocument* doc, SingleTextChange* newChange, int& delta );
    void inverseMergeRemainingOverlappingTextChanges(TextDocument* doc, int mergedAtIndex, int orgStartOffset, int orgEndOffset , int delta);

public:
    void giveSingleTextChange( TextDocument* doc, SingleTextChange* change );
    virtual void giveLineDataListTextChange( LineDataListTextChange* change );

    virtual void giveChange( TextDocument* doc, TextChange* change );
    virtual TextChange* at( int idx );
    virtual TextChange* take( int idx );
    virtual int size();
    virtual void clear(bool performDelete=true);

    /// This method tries to merge the given change with the other change
    /// The textChange supplied with this method should NOT have been executed yet.
    /// It's the choice of this merge operation if the execution is required
    /// @param textChange the textchange to merge
    /// @return true if the merge has been successfull. False if nothing has been merged and executed
    virtual bool giveAndMerge( TextDocument* document, TextChange* textChange );

    virtual QString toString();
    QString toSingleTextChangeTestString();

    void moveChangesFromGroup( TextDocument* doc, TextChangeGroup* group);

protected:

    bool mergeAsGroup( TextDocument* document, TextChange* textChange );
    bool mergeAsSelection( TextDocument* document, TextChange* textChange );

    void compressTextChanges( TextDocument* document );
    void compressChanges( TextDocument* document );

private:

    QList<SingleTextChange*> textChangeList_;                   ///< The list of textchanges
    QList<LineDataListTextChange*> lineDataTextChangeList_;     ///<The list with liendata text changes
    QList<TextChange*> miscChangeList_;                         ///< Other textchanges


    TextRangeSet* previousSelection_;
    TextRangeSet* newSelection_;

};

} // edbee
