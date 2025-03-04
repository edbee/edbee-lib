// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/texteditorcommand.h"

namespace edbee {

/// moves
class EDBEE_EXPORT MoveLineCommand : public TextEditorCommand
{
public:
    MoveLineCommand( int direction );
    virtual ~MoveLineCommand();

    virtual void execute( TextEditorController* controller ) override;
    virtual QString toString() override;

private:
    int direction_;
};

}
