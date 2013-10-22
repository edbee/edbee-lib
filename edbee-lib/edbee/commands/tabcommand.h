/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "edbee/texteditorcommand.h"

namespace edbee {

/// For inserting/executing forward and backwards tabs
class TabCommand : public TextEditorCommand
{
public:
    /// The possible directions of the tab command
    enum Direction{
        Forward,
        Backward
    };

    TabCommand( Direction direction, bool insertTab );

    virtual void indent( TextEditorController* controller );
    virtual void execute( TextEditorController* controller );
    virtual QString toString();

private:
    Direction dir_;                 ///< The tab direction
    bool insertTab_;                ///< Should we insert a tab
};

} // edbee
