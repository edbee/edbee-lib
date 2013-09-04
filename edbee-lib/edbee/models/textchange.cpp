/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "textchange.h"

#include "edbee/models/textdocument.h"
#include "edbee/models/changes/singletextchange.h"

#include "debug.h"

namespace edbee {

/// This method replace the given text
ReplaceDocumentTextChange::ReplaceDocumentTextChange(int offset, int length, const QString& newText)
    : offset_( offset )
    , length_( length )
    , text_( newText )
    , oldText_()
{
}

/// replaces the text
void ReplaceDocumentTextChange::execute( TextDocument* document )
{
    // backup the old text
    oldText_ = document->buffer()->textPart( offset_, length_ );
    document->buffer()->replaceText( offset_, length_, text_ );
}

/// reverts the operation
void ReplaceDocumentTextChange::revert(TextDocument *document)
{
    document->buffer()->replaceText( offset_, text_.length(), oldText_ );
}

//---------------

/// a controller specific textcommand. Warning you should NOT modify the textdocument!
ControllerTextChange::ControllerTextChange(TextEditorController *controller)
    : controllerRef_( controller )
{
}

ControllerTextChange::~ControllerTextChange()
{
}


//---------------


TextChangeGroup::TextChangeGroup(TextEditorController* controller)
    : ControllerTextChange( controller )

{
}

TextChangeGroup::~TextChangeGroup()
{
    qDeleteAll(changeList_);
    changeList_.clear();
}

//TextCommand *UndoableTextCommandGroup::clone()
//{
//    Q_ASSERT(false); /// Cloning a commandgroup isn't requried
//    return 0;
//}



void TextChangeGroup::giveCommand(TextChange* act)
{
    changeList_.append(act);
}

void TextChangeGroup::giveCommandAtIndex(int idx, TextChange* act)
{
    changeList_.insert(idx,act);
}



void TextChangeGroup::execute(TextDocument* document)
{
    Q_UNUSED(document);
    for( int i=0,cnt=changeList_.size(); i<cnt; ++i ) {
        changeList_.at(i)->execute(document);
    }
}

void TextChangeGroup::revert(TextDocument* document)
{
    Q_UNUSED(document);
    for( int i=changeList_.size()-1; i>=0; --i ) {
        changeList_.at(i)->revert(document);
    }
}

/// groups can be merged :-)
bool TextChangeGroup::merge(TextDocument *document, TextChange* textChange)
{
    Q_UNUSED( document );
    Q_UNUSED( textChange );

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

TextChange* TextChangeGroup::at(int idx)
{
    return changeList_.at(idx);
}

TextChange *TextChangeGroup::take(int idx)
{
    return changeList_.takeAt(idx);
}

/// This method returns the last change in the change group
TextChange *TextChangeGroup::last()
{
    if( changeList_.isEmpty() ) { return 0; }
    return changeList_.last();
}

/// Takes the ownership of the last element and removes it from the stack
TextChange* TextChangeGroup::takeLast()
{
    return changeList_.takeLast();
}

/// This method return the number of items
int TextChangeGroup::size()
{
    return changeList_.size();
}

/// The total number of items in the list (excluding the group items)
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

/// if this commandgroup only contains commands for a single controller context
/// Then this context is returned else 0 is returned
TextEditorController* TextChangeGroup::controllerContext()
{
    TextEditorController* context = 0;
    for( int i=changeList_.size()-1; i>=0; --i ) {
        TextEditorController* commandContext = changeList_.at(i)->controllerContext();

        // multiple context in 1 group means it's a 'hard' undo
        if( commandContext == 0 ) return 0;         /// 0 is always 0!
        if( commandContext && context && commandContext != context ) { return 0; }
        if( !context && commandContext ) {
            context = commandContext;
        }
    }
    return context;
}

QString TextChangeGroup::toString()
{
    QString s;
    s = QString("%1/%2").arg(changeList_.size()).arg(recursiveSize());
//    for( int i=0,cnt=changeList_.size(); i<cnt; ++i ) {
//        s.append( changeList_.at(i)->toString() );
//        s.append(",");
//    }
//    s.remove(s.length()-1);

    QString extra;
    for( int i=0,cnt=changeList_.size(); i<cnt; ++i ) {
        extra.append( QString(" - %1: ").arg(i));
        extra.append( changeList_.at(i)->toString() );
        extra.append("\n");
    }


    return QString("TextChangeGroup(%1)\n%2").arg(s).arg(extra);
}



} // edbee
