// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/texteditorcommand.h"


namespace edbee {

/// The Duplicate command.
/// Duplicates the selected line or text
class EDBEE_EXPORT DuplicateCommand : public TextEditorCommand
{
public:
    virtual void execute(TextEditorController* controller) override;
    virtual QString toString() override;
};

} // edbee
