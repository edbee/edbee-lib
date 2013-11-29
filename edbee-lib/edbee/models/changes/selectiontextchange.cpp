/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "selectiontextchange.h"

#include "edbee/models/textbuffer.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/models/textchange.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"
#include "edbee/models/textundostack.h"
#include "edbee/views/textrenderer.h"
#include "edbee/views/textselection.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/texteditorwidget.h"

#include "debug.h"

namespace edbee {


/// The selection textchange constructor
/// @param controller the controler this selection change is for
SelectionTextChange::SelectionTextChange(TextEditorController* controller )
    : ControllerTextChange( controller )
    , rangeSet_(0)
{
}


/// destructs the textrange
SelectionTextChange::~SelectionTextChange()
{
    delete rangeSet_;
}


/// Gives the textrange to the textchange
void SelectionTextChange::giveTextRangeSet(TextRangeSet* rangeSet)
{
    delete rangeSet_;
    rangeSet_ = rangeSet;
}


/// Takes ownership of the rangeset and clears the clearset
/// @return the rangeset
TextRangeSet* SelectionTextChange::takeRangeSet()
{
    TextRangeSet* result = rangeSet_;
    rangeSet_ = 0;
    return result;
}


/// Executes the textchange
/// @param document the textdocument to execute this change for
void SelectionTextChange::execute( TextDocument* document )
{
    if( !rangeSet_ ) { return; }
    Q_UNUSED(document);
    TextRangeSet* currentSelection = dynamic_cast<TextRangeSet*>( controllerContext()->textSelection() );
    TextRangeSet oldSelection( *currentSelection );

    *currentSelection = *rangeSet_;
    *rangeSet_ = oldSelection;

    notifyChange();
}


/// Reverts the selection change
/// @param document the textdocument to revert this change for
void SelectionTextChange::revert(TextDocument* document)
{
    if( !rangeSet_ ) { return; }

    Q_UNUSED(document);
    TextRangeSet* currentSelection = dynamic_cast<TextRangeSet*>( controllerContext()->textSelection() );
    TextRangeSet newSelection( *currentSelection );

    *currentSelection = *rangeSet_;
    *rangeSet_ = newSelection;
    notifyChange();
}


/// This method tries to merge the given change with the other change
/// The textChange supplied with this method. Should NOT have been executed yet.
/// It's the choice of this merge operation if the execution is required
bool SelectionTextChange::giveAndMerge( TextDocument* document, TextChange* textChange)
{
    Q_UNUSED( document );
    SelectionTextChange* selectionChange = dynamic_cast<SelectionTextChange*>( textChange );
    if( selectionChange ) {
//        *rangeSet_ = *selectionChange->rangeSet_;
        delete textChange;
        return true;
    }
    return false;
}


/// Convert this change to a string
QString SelectionTextChange::toString()
{
    return QString("SelectionTextChange(%1)").arg(rangeSet_->rangesAsString());
}


/// This method is called internally for notifying the control the selection has been changed
/// Perhaps we should make e proper emit-signal for this purpose
void SelectionTextChange::notifyChange()
{
    /// TODO: make the controllerContext only repaint the affected areas via the TextRangeSets
    controllerContext()->onSelectionChanged( rangeSet_ );
}


} // edbee
