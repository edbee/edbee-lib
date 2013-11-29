/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "complextextchange.h"

#include "edbee/models/changes/selectiontextchange.h"
#include "edbee/models/changes/singletextchange.h"
#include "edbee/models/changes/linedatalisttextchange.h"
#include "edbee/models/textrange.h"
#include "edbee/views/textselection.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/util/util.h"

#include "debug.h"

namespace edbee {


/// The default complex textchange constructor
ComplexTextChange::ComplexTextChange(TextEditorController* controller)
    : TextChangeGroup(controller)
    , previousSelection_(0)
    , newSelection_(0)
{
    if( controller ) {
       previousSelection_ = new TextRangeSet( *( controller->textSelection() ) );
    }
}


/// The default destructor
ComplexTextChange::~ComplexTextChange()
{
    clear(true);
    delete newSelection_;
    delete previousSelection_;
}


/// default not discardable
bool ComplexTextChange::isDiscardable()
{
    return false;
}


/// the group is closed, we must 'store' the selection
void ComplexTextChange::groupClosed()
{
    if( controller() ) {
        delete newSelection_;
        newSelection_ = new TextRangeSet( *(controller()->textSelection() ) ) ;
    }
}


/// Executes this textchange
/// @param document the document to execute this operation for
void ComplexTextChange::execute(TextDocument* document)
{
    TextChangeGroup::execute( document );
    if( newSelection_ ) {
        TextRangeSet* currentSelection = dynamic_cast<TextRangeSet*>( controller()->textSelection() );
        *currentSelection = *newSelection_ ;
    }
}


/// this method is called to revert the operation
/// reverts the given operation
void ComplexTextChange::revert( TextDocument* document )
{
    TextChangeGroup::revert(document);
    if( previousSelection_) {
        TextRangeSet* currentSelection = dynamic_cast<TextRangeSet*>( controller()->textSelection() );
        *currentSelection = *previousSelection_;
    }
}


/// This method adds the given delta to the changes
void ComplexTextChange::addOffsetDeltaToTextChanges(int fromIndex, int delta)
{
    for(int i = fromIndex; i<textChangeList_.size(); ++i ) {
        SingleTextChange* s2 = textChangeList_.at(i);
        s2->addOffset( delta );
    }
}


/// gives the given textchange to the merge routine
void ComplexTextChange::giveSingleTextChange(TextDocument* doc, SingleTextChange* newChange)
{
    //qlog_info() << "giveSingleTextChange" << newChange->toString();
    // remember the orginal ranges so we know which changes are affected by this new change
    int orgStartOffset = newChange->offset();
    int orgEndOffset = newChange->offset() + newChange->newLength();

    // some variables to remebmer
    int mergedAtIndex = -1;                 // the index that a merge succeeded
    int addDeltaFromIndex = size();         // From which change index should we add delta?!
    int delta = 0;

    // First try to merge this new change
    for( int i=0,cnt=textChangeList_.size(); i<cnt; ++i ){
        SingleTextChange* change = textChangeList_.at(i);
//qlog_info() << " - " << i << change->toString();

        // we need the previous length and new-length to know how the delta is changed of the other items
        int prevLength = change->length();
        int prevNewLength = change->newLength();

        // try to merge it
        if( change->giveAndMerge( doc, newChange ) ) {

            // apply the delta (newLength and length is reversed when in undo state!)
            delta += (change->length()-prevLength) - (change->newLength()-prevNewLength);
//qlog_info() << "* merged: " << change->toString() << " (delta: " << delta << ")";
            mergedAtIndex = i;
            break;
        }
    }

    // when we could merge the change,
    // we need to try to merge it with earlier changes
    if( mergedAtIndex >= 0 ) {

        SingleTextChange* mergedChange = textChangeList_.at(mergedAtIndex);
        for( int i=mergedAtIndex+1; i<textChangeList_.size(); ++i ) {
            SingleTextChange* nextChange= textChangeList_.at(i);
//qlog_info() << "   - " << i << nextChange->toString();

            // only perform merging if the change overlapped a previous change
            if( nextChange->offset() < orgEndOffset && orgStartOffset < (nextChange->offset() + nextChange->length())   ) {

//                int prevLength = nextChange->length();
//                int prevNewLength = nextChange->newLength();

                // take the delta of the previous change before the merge
                int tmpDelta = mergedChange->newLength() - mergedChange->length() + delta;
//    qlog_info() << "       tmpDelta:" << tmpDelta;
                // alter the delta, so we find the correct merge index
                nextChange->addOffset(tmpDelta);

                // notice the 'inversion of the merge. We apply the merged change to the next change
                if( nextChange->giveAndMerge( doc, mergedChange) ) {
//    qlog_info() << "  ~ delta: " << delta << ", length: " << prevLength << ", newLength: " << prevNewLength;
                    //delta += (nextChange->length()-prevLength) - (nextChange->newLength()-prevNewLength);
                    mergedChange = nextChange;
//    qlog_info() << "  * merged2: " << mergedChange->toString() << " (delta: " << delta << ")";
                    textChangeList_.removeAt(mergedAtIndex);
                    --i;
                } else {
                    break;  // cannot merge, we're done

                    // remove the temporary delta
                    nextChange->addOffset(-tmpDelta);
                }
            } else {
                break;
            }
        }
//        qlog_info() << "Try to merge to morenodes, and adjust delta's on merge :D";
        addDeltaFromIndex = mergedAtIndex + 1;

    // not merged? then we need to add the change
    // ad the given index
    } else {

        // find the insert index
        int insertIndex = 0;
        for( int i=0,cnt=textChangeList_.size(); i<cnt; ++i ){
            SingleTextChange* change = textChangeList_.at(i);
            if( change->offset() < newChange->offset() ) {
                insertIndex = i+1;
            }
        }

        // just insert change
        textChangeList_.insert( insertIndex, newChange);
        addDeltaFromIndex = insertIndex+1;

        // apply the delta (newLength and length is reversed when in undo state!)
        delta += newChange->newLength() - newChange->length();

//        qlog_info() << "* insert: " << newChange->toString() << " (delta: " << delta << ")";

    }

    // next apply the delta to the following change
    addOffsetDeltaToTextChanges(addDeltaFromIndex,delta);
}


/// gives a line data list text change
void ComplexTextChange::giveLineDataListTextChange(LineDataListTextChange* change)
{
    lineDataTextChangeList_.append(change);
}


/// Gives the change
void ComplexTextChange::giveChange( TextDocument* doc, TextChange* change)
{
    // a single text change
    SingleTextChange* textChange = dynamic_cast<SingleTextChange*>(change);
    if( textChange ) {
        //textChangeList_.append(textChange);
        giveSingleTextChange( doc, textChange);
        return;
    }

    // a list text change
    LineDataListTextChange* lineDataTextChange = dynamic_cast<LineDataListTextChange*>(change);
    if( lineDataTextChange ) {
        giveLineDataListTextChange(lineDataTextChange);
        return;
    }

    // a selection change simply is moved to the new selection object
    SelectionTextChange* selectionChange = dynamic_cast<SelectionTextChange*>(change);
    if( selectionChange ) {
        delete newSelection_;
        newSelection_ = selectionChange->takeRangeSet();
        /// qlog_info() << "take new selection: " << QString::number( (quintptr)newSelection_,16);

        /// we can simply delete the change, the ComplexTextChange automaticly records the last change selection on the undoGroupEnd
        delete change;
        return;
    }

    // a group changes, just add all the groups
    TextChangeGroup* group = dynamic_cast<TextChangeGroup*>(change);
    if( group ) {
        moveChangesFromGroup(doc,group);
        delete group;
        return;
    }


    // other changes are (currently) added to a misch change list. And are pretty scary for now :)
    miscChangeList_.append(change);
}


///returns the textchange at the given index
TextChange* ComplexTextChange::at(int idx)
{
    // plain text changes
    if( idx < textChangeList_.size() ) {
        return textChangeList_.at(idx);
    }
    // line-data changes
    idx -= textChangeList_.size();
    if( idx < lineDataTextChangeList_.size() ) {
        return lineDataTextChangeList_.at(idx);
    }
    // other changes
    idx -= lineDataTextChangeList_.size();
    Q_ASSERT(idx < miscChangeList_.size() );
    return miscChangeList_.at(idx);
}


/// Takes the given item
TextChange*ComplexTextChange::take(int idx)
{
    // plain text changes
    if( idx < textChangeList_.size() ) {
        return textChangeList_.takeAt(idx);
    }
    // line-data changes
    idx -= textChangeList_.size();
    if( idx < lineDataTextChangeList_.size() ) {
        return lineDataTextChangeList_.takeAt(idx);
    }
    // other changes
    idx -= lineDataTextChangeList_.size();
    Q_ASSERT(idx < miscChangeList_.size() );
    return miscChangeList_.takeAt(idx);
}


/// returns the number of elements
int ComplexTextChange::size()
{
    return textChangeList_.size() + lineDataTextChangeList_.size() + miscChangeList_.size();
}


/// clears all items
void ComplexTextChange::clear(bool performDelete)
{
    // delete
    if( performDelete ) {
        qDeleteAll(textChangeList_);
        qDeleteAll(lineDataTextChangeList_);
        qDeleteAll(miscChangeList_);
    }

    // clear the changes
    textChangeList_.clear();
    lineDataTextChangeList_.clear();
    miscChangeList_.clear();
}


/// this method tries to merge the textchange with this text change
bool ComplexTextChange::giveAndMerge(TextDocument* document, TextChange* textChange)
{
    giveChange( document, textChange );
    return true;
}



/// Converts this textchange to a textual representation
QString ComplexTextChange::toString()
{
    return QString("Complex::%1").arg(TextChangeGroup::toString());
}


/// Converts the textchangeList as as string
/// The format is the following:
/// <offset>:<length>:<str>,...
///
/// sample:
/// 0:2:ABC,1:2:QW
QString ComplexTextChange::toSingleTextChangeTestString()
{
    QString result;
    foreach( SingleTextChange* change, textChangeList_ ) {
        if( !result.isEmpty() ) result.append(",");
        result.append( QString("%1:%2:%3").arg(change->offset()).arg(change->length()).arg(change->text()));
    }
    return result;
}


/// Moves all textchanges from the given group to this group
/// @param group the group to move the selection from
void ComplexTextChange::moveChangesFromGroup( TextDocument* doc, TextChangeGroup* group )
{
//qlog_info() << "moveChangeFromGroup **************** (MERGE)";
//qlog_info() << "A:" << this->toString();
//qlog_info() << "B:" << group->toString();

    // process all changes
    for( int i=0,cnt=group->size(); i<cnt; ++i ) {

        TextChange* change = group->at(i);

        // handle nested groups
        if( change->isGroup() ) {
            TextChangeGroup* childGroup = dynamic_cast<TextChangeGroup*>( change );
            if( childGroup ) {
                moveChangesFromGroup( doc, childGroup );
                delete childGroup;
                continue;
            }
        }

        // merge the the change
        giveChange( doc, change );
    }
    group->clear(false); // no delete we've taken the ownership

//qlog_info() << "=1>:" << this->toString();

    // next we need to compress the changes (merging changes that are next to eachother)
    compressChanges(doc);
//qlog_info() << "************************";
//qlog_info() << "=2>:" << this->toString();

    // when it's a complex text change, also move the selection to this group
    ComplexTextChange* complexGroup = dynamic_cast<ComplexTextChange*>(group);
    if( complexGroup ) {
        qSwap( newSelection_, complexGroup->newSelection_ );
    }
}



/// merges the given textchange as a group.
/// @param document the document this merge is for
/// @param textChange the textchange to merge
/// @return true if the textchange was merged as group.
bool ComplexTextChange::mergeAsGroup(TextDocument* document, TextChange* textChange )
{
    Q_UNUSED(document);

    // make sure it's a group
    if( !textChange->isGroup() ) { return false; }
    TextChangeGroup* group = dynamic_cast<TextChangeGroup*>(textChange);
    if( !group ) { return false; }

    // move all changes from the other group to this group
    moveChangesFromGroup(document, group);

    delete textChange;
    return true;
}


//// @param document the document this merge is for
/// @param textChange the textchange to merge
/// @return true if the textchange was merged as group.
// merge the text selection
/// @param document the document this merge is for
/// @param textChange the textchange to merge
/// @return true if the textchange was merged as group.
bool ComplexTextChange::mergeAsSelection(TextDocument* document, TextChange* textChange)
{
    Q_UNUSED(document);
    SelectionTextChange* selectionChange = dynamic_cast<SelectionTextChange*>(textChange);
    if( !selectionChange ) { return false; }

    // simply take the selection from the textchange and make this the new selection
    delete newSelection_;
    newSelection_ = selectionChange->takeRangeSet();
    delete textChange;
    return true;
}


/// Compresses the textchanges
/// @param document the document to merge the changes for
void ComplexTextChange::compressTextChanges(TextDocument* document)
{
    // compress single text changes
    for( int i=0; i<textChangeList_.size(); ++i ) {
        SingleTextChange* change1 = textChangeList_.at(i);

        // find the next text change
        for( int j=i+1; j<textChangeList_.size(); ++j ) {
            SingleTextChange* change2 = textChangeList_.at(j);
            if( change1->giveAndMerge( document, change2 ) ) {
                textChangeList_.takeAt(j);  // just take it, the item is already deleted by the merge
                --j;
            }
        } // for j
    } // for i
}


/// This is going to be the magic method that's going to merge all changes
/// @param document the document to merge the changes for
void ComplexTextChange::compressChanges(TextDocument* document)
{
    compressTextChanges(document);
}

} // edbee
