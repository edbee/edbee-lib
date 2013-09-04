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

SelectionTextChange::SelectionTextChange(TextEditorController* controller )
    : ControllerTextChange( controller )
    , rangeSet_(0)
{
}

SelectionTextChange::~SelectionTextChange()
{
    delete rangeSet_;
}


void SelectionTextChange::giveTextRangeSet(TextRangeSet* rangeSet)
{
    rangeSet_ = rangeSet;
}


void SelectionTextChange::execute( TextDocument* document )
{
    Q_UNUSED(document);
    TextRangeSet* currentSelection = dynamic_cast<TextRangeSet*>( controllerContext()->textSelection() );
    TextRangeSet oldSelection( *currentSelection );

    *currentSelection = *rangeSet_;
    *rangeSet_ = oldSelection;

    notifyChange();
}

/// Reverts the selection change
void SelectionTextChange::revert(TextDocument* document)
{
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
bool SelectionTextChange::merge( TextDocument* document, TextChange* textChange)
{
    Q_UNUSED( document );
    SelectionTextChange* selectionChange = dynamic_cast<SelectionTextChange*>( textChange );
    if( selectionChange ) {
//        *rangeSet_ = *selectionChange->rangeSet_;
        return true;
    }
    return false;
}




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
