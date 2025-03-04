// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QString>

#include "edbee/texteditorcommand.h"

namespace edbee {

class TextEditorController;


/// This command is used for copying data to the clipboard
class EDBEE_EXPORT CopyCommand : public TextEditorCommand
{
public:
    static const QString EDBEE_TEXT_TYPE;

public:
    virtual void execute( TextEditorController* controller ) override;
    virtual QString toString() override;
    virtual bool readonly() override;
};


} // edbee
