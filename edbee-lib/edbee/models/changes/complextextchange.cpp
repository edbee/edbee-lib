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
    delete newSelection_;
    delete previousSelection_;
}


/// the group is closed, we must 'store' the selection
void ComplexTextChange::groupClosed()
{
    if( controller() ) {
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


/// Merges a single textchange
/// @param gTextChange the global textchange
/// @param nextTechangeMergeIndex (in/out) alters the nextTextChangeMergeIndex
/// @return true if the merge succeeded
bool ComplexTextChange::mergeTextChange( TextDocument* document, SingleTextChange* gTextChange, int& nextTextChangeMergeIndex )
{
    bool result = false;
//qlog_info() << "MERGE=============";
//qlog_info() << "A> " << toString();
//qlog_info() << "B> " << gTextChange->toString();

    // find the location to MERGE it with in the list
    for( int i=nextTextChangeMergeIndex, cnt=size(); i<cnt; ++i ) {
        TextChange* change = at(i);
        SingleTextChange* textChange = dynamic_cast<SingleTextChange*>(change);

        // merge succeeded?
        if( change->merge( document, gTextChange ) ) {
            Q_ASSERT( textChange );
            nextTextChangeMergeIndex = i+1;
            //appendChange = false;
            result = true;

            // we need to get the delta of the merge change
            int delta = gTextChange->length() - gTextChange->text().length();
            int lineDelta = gTextChange->docText( document ).count('\n') - gTextChange->text().count('\n');

    //qlog_info() << "DELTA: " << delta << " + " << lineDelta << "("<<change->toString()<<")";
    //qlog_info() << "WE SHOULD ALSO move the line DELTA for this change";

            // apply the delta of this merge to all NEXT items in the list
            bool nextBorderChangeFound = false;
            for(++i; i<cnt; ++i ) {
                change = at(i);


                // only increase the offsets AFTER the next textchange or linedata-change (dirty hack for line-data issue)
                // Very diry solution...(very bugprone !!! )
                if( !nextBorderChangeFound ) {
                   TextChange* borderChange = dynamic_cast<LineDataListTextChange*>(change);
                   if( !borderChange ) { borderChange = dynamic_cast<SingleTextChange*>(change); }
                   if( borderChange ) { nextBorderChangeFound = true; }
                }
                if( nextBorderChangeFound ) {
                    change->moveOffset(delta);
                    change->moveLine(lineDelta);
                }
    //qlog_info() << " TODO: we should add moveLine?";
            }

        // merge failed, we're in deep trouble
        }
    }

    if (!result ){
       qlog_info() << "The textmerge FAILED, we're in deep trouble. ";
       qlog_info() << "I'm affraid I need to build in support for a canMerge function in text-changes";
       qlog_info() << "... very difficult to solve this ... ";
       Q_ASSERT(false == "SingleTextChange in ComplexTextChange merge failed!!!");
    }
    return result;
}

/// merges a line data textchange
/// @param document the textdocument
/// @param lineDataTextChange the text change
/// @param nextLineChangeIndex the next line chane
bool ComplexTextChange::mergeLineDataTextChange( TextDocument* document, LineDataListTextChange* lineDataTextChange, int& nextLineChangeIndex )
{
    // find the location to MERGE it with in the list
    for( int i=nextLineChangeIndex, cnt=size(); i<cnt; ++i ) {
        TextChange* change = at(i);
        LineDataListTextChange* lineChange= dynamic_cast<LineDataListTextChange*>(change);

        // merge succeeded?
        if( change->merge( document, lineDataTextChange ) ) {
            Q_ASSERT( lineChange );
            nextLineChangeIndex = i+1;
            return true;
        }
    }
    return false;
}

/// a special (internal) TextChange for marking the end of a mergable-scope
/// We need this to 'kwow' where to start merging changes
class TextStartMergeMarker : public TextChange
{
public:
    virtual void execute( TextDocument*) {}
    virtual void revert( TextDocument*) {}
    virtual QString toString() { return "TextStartMergeMarker"; }

};


/// this method tries to merge the textchange with this text change
bool ComplexTextChange::merge(TextDocument* document, TextChange* textChange)
{
    // when the new change isn't a group change return
    ComplexTextChange* groupChange = dynamic_cast<ComplexTextChange*>( textChange );
    if( !groupChange ) return false;


    // when the previous change is an end marker, we must add all items
    if( size() > 0 && dynamic_cast<TextStartMergeMarker*>(at(size()-1))) {
        while( groupChange->size() ) {
            TextChange* gChange = groupChange->take(0);
            giveCommand( gChange);
        }
        qSwap( newSelection_, groupChange->newSelection_ );
        return true;

    }

    int nextTextChangeMergeIndex = 0;
    int nextLineChangeIndex = 0;

    /// find the start merge offset (This is the first change after the last Selection)
    for( int i=size()-1; i>= 0; --i ) {
        TextChange* gChange = at(i);
        TextStartMergeMarker* marker = dynamic_cast<TextStartMergeMarker*>(gChange);
        if( marker ) {
            nextTextChangeMergeIndex = i;
            nextLineChangeIndex = i;
            break;
        }
    }


    // (Hack) Different number of ranges, the merge is not possible
    bool appendMergeEndMarker = false;
    if( newSelection_ &&
        groupChange->newSelection_ &&
        newSelection_->rangeCount() != groupChange->newSelection_->rangeCount() ) {
        appendMergeEndMarker = true;
    }

    // merge all changes to the current list. And join changes as good as possible
    for( int gIdx=0; gIdx<groupChange->size(); ++gIdx ){
        TextChange* gChange = groupChange->at(gIdx);

        SingleTextChange* gTextChange = dynamic_cast<SingleTextChange*>(gChange);
        LineDataListTextChange* lineDataListTextChange = dynamic_cast<LineDataListTextChange*>(gChange);
        bool appendChange=true;

        // MERGE text changes?
        if( gTextChange ) {
            appendChange = !mergeTextChange( document, gTextChange, nextTextChangeMergeIndex );

        // cannot merge/  no text change? just append the change
        } else if( lineDataListTextChange  ) {
            appendChange = !mergeLineDataTextChange( document, lineDataListTextChange, nextLineChangeIndex );

        } else {

            // dirty hack to support RemoveSelectionCommand. This prevents the padding of infinite textchanges
            SelectionTextChange* gTextChange = dynamic_cast<SelectionTextChange*>(gChange);
            if( gTextChange ) {
                appendChange = false;
            }
        }


        // cannot merge, just append the change (before the next text/line-change 'border' )
        if( appendChange ) {
            groupChange->take(gIdx);        // remove the change and give it to the list
            --gIdx;                           // make sure the next iteration takes the correct item
            //giveCommand(gChange);
            giveCommandAtIndex( nextTextChangeMergeIndex+1, gChange);
        }


    } // for group


    // add a end marker
    if( appendMergeEndMarker ) {        
        giveCommand( new TextStartMergeMarker() );
    }


    // the new selection is the selection of the merged group
    qSwap( newSelection_, groupChange->newSelection_ );
    return true;
}

QString ComplexTextChange::toString()
{
    return QString("Complex::%1").arg(TextChangeGroup::toString());
}


} // edbee
