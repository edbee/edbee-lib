// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/texteditorcommand.h"

namespace edbee {

class TextEditorController;


/// Performs a cut command. The cut command removes the selected content
/// and places onto the clipboard.
///
/// When pressing cut without selection, the full line is cut
class EDBEE_EXPORT CutCommand : public TextEditorCommand
{
public:
    virtual void execute( TextEditorController* controller ) override;
    virtual QString toString() override;
};


} // edbee
