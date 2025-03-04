// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/texteditorcommand.h"

namespace edbee {

/// For inserting/executing forward and backwards tabs
class EDBEE_EXPORT TabCommand : public TextEditorCommand
{
public:
    /// The possible directions of the tab command
    enum Direction{
        Forward,
        Backward
    };

    /// Extra possible coalesce (sub-ids)
    enum {
        SubCoalesceId_Indent_Forward = Forward,
        SubCoalesceId_Indent_Backward = Backward,
        SubCoalesceId_Indent_InsertTab,
        SubCoalesceId_Indent_InsertSpaces
    };


    TabCommand( Direction direction, bool insertTab );

    virtual void indent( TextEditorController* controller );
    virtual void execute( TextEditorController* controller ) override;
    virtual QString toString() override;

private:
    Direction dir_;                 ///< The tab direction
    bool insertTab_;                ///< Should we insert a tab
};

} // edbee
