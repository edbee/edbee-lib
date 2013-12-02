/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "textchange.h"

#include "edbee/models/textdocument.h"
#include "edbee/models/changes/singletextchange.h"

#include "debug.h"

namespace edbee {

/// a virtual empty destructor
TextChange::~TextChange()
{
}


/// this method reverts the given operation
void TextChange::revert(TextDocument*)
{
    Q_ASSERT(false);
}


/// Gives the change and merges it if possible. This method should return false if the change couldn't be merged.
/// When the method returns true the ownership of the given textchange is transfered to this class.
/// @param document the document this change is for
/// @param textChange the textchange
/// @return true if the merge succeeded. The textChange ownership is only tranfered if true if returend
bool TextChange::giveAndMerge(TextDocument* document, TextChange* textChange)
{
    Q_UNUSED(document);
    Q_UNUSED(textChange );
    return false;
}


/// This method should return true if the change can be reverted
bool TextChange::canUndo()
{
    return false;
}


/// This  flag is used to mark this stack item as non-persistence requirable
/// The default behaviour is that every textchange requires persistence. It is also possible to
/// have certain changes that do not require persitence but should be placed on the undo stack
bool TextChange::isPersistenceRequired()
{
    return true;
}


/// A text command can belong to a controller/view
/// When it's a view only command. The undo only applies only to this view
/// warning a DOCUMENT change may NEVER return a controllerContext!!
TextEditorController*TextChange::controllerContext()
{
    return 0;
}


/// this method can be used to check if the given change is a document change
bool TextChange::isDocumentChange()
{
     return controllerContext() == 0;
}


/// This method returns true if this change is a group change. When an object is group change
/// it should be inherited by TextChangeGroup
bool TextChange::isGroup()
{
    return false;
}


/// This method can be overriden by a textchange to react when the line-delta is changed
/// @param line
/// @param delta the delta to move the line
void TextChange::applyLineDelta(int line, int length, int newLength)
{
    Q_UNUSED(line);
    Q_UNUSED(length);
    Q_UNUSED(newLength);
}


//--------------------------------------------------------------


/// Empty change doesn't do anything
bool EmptyDocumentTextChange::isPersistenceRequired()
{
    return false;
}


/// does nothing
void EmptyDocumentTextChange::execute(TextDocument*)
{
}

/// does nothing
void EmptyDocumentTextChange::revert(TextDocument*)
{
}

/// returns the name of the textchange
QString EmptyDocumentTextChange::toString()
{
     return "EmptyDocumentTextChange";
}


//--------------------------------------------------------------


/// A replace document text change. A text replacement in the document
/// @param offset the offset of the change
/// @param length the length of the replaced text
/// @param newText the newly placed text
ReplaceDocumentTextChange::ReplaceDocumentTextChange(int offset, int length, const QString& newText)
    : offset_( offset )
    , length_( length )
    , text_( newText )
    , oldText_()
{
}


/// executes the replacement of the text
/// @param document the document to execute this change for
void ReplaceDocumentTextChange::execute( TextDocument* document )
{
    // backup the old text
    oldText_ = document->buffer()->textPart( offset_, length_ );
    document->buffer()->replaceText( offset_, length_, text_ );
}


/// reverts the text replacement
/// @param document the document to execute this change for
void ReplaceDocumentTextChange::revert(TextDocument* document)
{
    document->buffer()->replaceText( offset_, text_.length(), oldText_ );
}


//--------------------------------------------------------------


/// A controller specific textcommand. Warning you should NOT modify the textdocument!
/// @param controller the controller this change is for
ControllerTextChange::ControllerTextChange(TextEditorController* controller)
    : controllerRef_( controller )
{
}


/// A text command can belong to a controller/view
/// When it's a view only command. The undo only applies only to this view
TextEditorController*ControllerTextChange::controllerContext()
{
    return controllerRef_;
}


/// returns the controller
TextEditorController*ControllerTextChange::controller()
{
    return controllerRef_;;
}


//--------------------------------------------------------------


/// default contructor
/// @param controller the controller this groups belongs to
TextChangeGroup::TextChangeGroup(TextEditorController* controller)
    : ControllerTextChange( controller )

{
}


/// The destructor deletes all added textchanges
TextChangeGroup::~TextChangeGroup()
{
//    qDeleteAll(changeList_);
//    changeList_.clear();
}


/// A group change is a group change, so this method returns true :)
bool TextChangeGroup::isGroup()
{
    return true;
}


/// Adds a change to this group
/// @param change the change to give
//void TextChangeGroup::giveChange(TextChange* change)
//{
//    changeList_.append(change);
//}


/// Adds a change to this group at the given index
/// @param idx the index to add the group to
/// @param change the change to give
//void TextChangeGroup::giveChangeAtIndex(int idx, TextChange* change)
//{
//    changeList_.insert(idx,change);
//}


/// This method is called it the group is discardable. A discardable group will be optimized away if
/// the group is empty, or if there's a single item in the group. A none-discardable group will
/// always remain
bool TextChangeGroup::isDiscardable()
{
    return true;
}

/// This method is called if the group is closed and is added to the stack
/// Default implementation is to do nothing
void TextChangeGroup::groupClosed()
{
}


/// executes this command group
/// @param document the document the document to execute this for
void TextChangeGroup::execute(TextDocument* document)
{
    Q_UNUSED(document);
    for( int i=0,cnt=size(); i<cnt; ++i ) {
        at(i)->execute(document);
    }
}


/// Reverts the command gorup
void TextChangeGroup::revert(TextDocument* document)
{
    Q_UNUSED(document);
    for( int i=size()-1; i>=0; --i ) {
        at(i)->revert(document);
    }
}


/// Moves all textchanges from the given group to this group
/*
void TextChangeGroup::moveChangeFromGroup(TextChangeGroup* group)
{
    for( int i=0,cnt=group->size(); i<cnt; ++i ) {
        TextChange* change = group->at(i);
        if( change->isGroup() ) {
            TextChangeGroup* childGroup = dynamic_cast<TextChangeGroup*>( change );
            if( childGroup ) {
                moveChangeFromGroup( childGroup );
                continue;
            }
        }
        giveChange( change );
    }
    group->clear(false); // no delete we've taken the ownership
}
*/

#if 0

/// This method tries to merge the given change with the other change
/// The textChange supplied with this method should NOT have been executed yet.
/// It's the choice of this merge operation if the execution is required
/// @param document the document scope this merge needs to be executed for
/// @param textChange the textchange to merge
/// @return true if the merge has been successfull. False if nothing has been merged and executed
bool TextChangeGroup::giveAndMerge(TextDocument* document, TextChange* textChange)
{
    Q_UNUSED( document );
    Q_UNUSED( textChange );

    // whent the given textchange is a group the change is simply merged
    if( textChange->isGroup() ) {
        TextChangeGroup* group= dynamic_cast<TextChangeGroup*>(textChange);
        if( group ) {
            moveChangeFromGroup(group);
            delete textChange;
            return true;
        }
    }


//qlog_info() << "DISABLED merge!";
//return false;

/// TODO: We probably should put this 'magic' merge in a special 'TextChangeGroup' !!!!
/*
    TextChangeGroup* groupChange = dynamic_cast<TextChangeGroup*>( textChange );
    if( groupChange) {

        int nextTextChangeMergeIndex = 0;
        int nextSelectionChangeMergeIndex = 0;

        // merge all changes to the current list. And join changes as good as possible
        for( int gIdx=0; gIdx<groupChange->size(); ++gIdx ){
            TextChange* gChange = groupChange->changeList_[gIdx];

            // MERGE text changes?
            SingleTextChange* gTextChange = dynamic_cast<SingleTextChange*>(gChange);
            if( gTextChange ) {

                // find the location to MERGE it with in the list
                for( int i=nextTextChangeMergeIndex, cnt=changeList_.size(); i<cnt; ++i ) {
                    TextChange* change = changeList_[i];
                    SingleTextChange* textChange = dynamic_cast<SingleTextChange*>(change);

                    // merge succeeded?
                    if( change->merge( document, gChange ) ) {
                        Q_ASSERT( textChange );
                        nextTextChangeMergeIndex = i+1;

                        // we need to get the delta of the merge change
                        int delta = gTextChange->length() - gTextChange->text().length();

                        // apply the delta of this merge to all NEXT items in the list
                        for(++i; i<cnt; ++i ) {
                            change = changeList_[i];
                            textChange = dynamic_cast<SingleTextChange*>(change);
                            if( textChange ) {
                                textChange->moveOffset(delta);
                            }
                        }
                    }
                }
            // cannot merge/  no text change? just append the change
            } else {
                groupChange->changeList_.removeAt(gIdx);        // remove the change and give it to the list
                --gIdx;                                         // make sure the next iteration takes the correct item
                changeList_.append(gChange);
          }
        } // for group
        return true;
    } // group change?
*/
    /* groups should not be merged
    TextChangeGroup* groupChange = dynamic_cast<TextChangeGroup*>( textChange );
    if( groupChange) {
        for( int i=0,cnt=groupChange->changeList_.size(); i <cnt; ++i ) {
            changeList_.append( groupChange->changeList_.at(i) );
        }
        groupChange->changeList_.clear();
        return true;
    }
    */
    return false;
}

#endif


/// This method flattens the undo-group by expanding all subgroups to local groups
void TextChangeGroup::flatten()
{
    for( int idx=0; idx < changeList_.size(); ++idx ) {
        TextChangeGroup* group = dynamic_cast<TextChangeGroup*>( changeList_[idx] );
        if( group ) {
            changeList_.removeAt(idx);
            for( int i=0; i<group->size(); ++i ) {
                TextChangeGroup* subGroup = dynamic_cast<TextChangeGroup*>( group->changeList_[i] );
                if( subGroup ) {
                    subGroup->flatten();   // flatten the group
                    delete subGroup;
                    if( i >= group->size() ) break; // just make sure empty groups go right (in theory empty groups aren't possible)
                }
                changeList_.insert( idx+i, group->changeList_[i] );
            }
            group->changeList_.clear();
            delete group;
        }
    }
}


/// This method returns the textchange at the given index
/// @param idx the index of the change
/// @return the textchange
//TextChange* TextChangeGroup::at(int idx)
//{
//    return changeList_.at(idx);
//}


/// This method takes the textchange from the group, taking the ownership
/// @param idx the index of the change
/// @return the textchange
//TextChange* TextChangeGroup::take(int idx)
//{
//    return changeList_.takeAt(idx);
//}


/// This method returns the last change in the change group
/// @return the last textchange
TextChange* TextChangeGroup::last()
{
    if( size() == 0 ) { return 0; }
    return at(size()-1);
}


/// Takes the ownership of the last element and removes it from the stack
/// @return the last textchange
TextChange* TextChangeGroup::takeLast()
{
    if( size() == 0 ) { return 0; }
    return take(size()-1);
}


/// This method return the number of items
/// @return the number of items in the changelist
//int TextChangeGroup::size()
//{
//    return changeList_.size();
//}


/// The total number of items in the list (excluding the group items)
/// @return the number of items recussive (iterating) all groups
int TextChangeGroup::recursiveSize()
{
    int itemCount = 0;
    for( int i=0,cnt=changeList_.size(); i<cnt; ++i ) {
        TextChangeGroup* group = dynamic_cast<TextChangeGroup*>(changeList_[i]);
        if( group ) {
            itemCount += group->size();
        } else {
            ++itemCount;
        }
    }
    return itemCount;
}


/// This method remove all items from the group
/// @param performDelete (defaults to true)
//void TextChangeGroup::clear(bool performDelete)
//{
//    if( performDelete ) { qDeleteAll(changeList_); }
//    changeList_.clear();
//}


/// if this commandgroup only contains commands for a single controller context
/// Then this context is returned else 0 is returned
TextEditorController* TextChangeGroup::controllerContext()
{
    TextEditorController* context = 0;
    for( int i=size()-1; i>=0; --i ) {
        TextEditorController* commandContext = at(i)->controllerContext();

        // multiple context in 1 group means it's a 'hard' undo
        if( commandContext == 0 ) return 0;         /// 0 is always 0!
        if( commandContext && context && commandContext != context ) { return 0; }
        if( !context && commandContext ) {
            context = commandContext;
        }
    }
    return context;
}


/// Converts this change group to a string
QString TextChangeGroup::toString()
{
    QString s;
    s = QString("%1/%2").arg(size()).arg(recursiveSize());
//    for( int i=0,cnt=changeList_.size(); i<cnt; ++i ) {
//        s.append( changeList_.at(i)->toString() );
//        s.append(",");
//    }
//    s.remove(s.length()-1);

    QString extra;
    for( int i=0,cnt=size(); i<cnt; ++i ) {
        extra.append( QString(" - %1: ").arg(i));
        extra.append( at(i)->toString() );
        extra.append("\n");
    }

    return QString("TextChangeGroup(%1)\n%2").arg(s).arg(extra);
}


//--------------------------------------------------------------


/// creates a non-discardable undo-group
TextChangeGroupNonDiscardable::TextChangeGroupNonDiscardable(TextEditorController* controller)
    : TextChangeGroup(controller)
{

}


/// this group may not be discarded
bool TextChangeGroupNonDiscardable::isDiscardable()
{
    return false;
}


} // edbee
