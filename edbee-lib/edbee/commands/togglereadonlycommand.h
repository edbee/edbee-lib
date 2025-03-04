// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once
#include "edbee/exports.h"
#include "edbee/texteditorcommand.h"


namespace edbee {

class ToggleReadonlyCommand : public TextEditorCommand
{
public:
    ToggleReadonlyCommand();

    virtual void execute(TextEditorController* controller) override;
    virtual QString toString() override;
    virtual bool readonly() override;

};

} // edbee

