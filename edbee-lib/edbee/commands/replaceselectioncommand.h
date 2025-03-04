// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/texteditorcommand.h"

namespace edbee {

/// This command can be used to replace the current selection with a given text
class EDBEE_EXPORT ReplaceSelectionCommand : public TextEditorCommand
{
public:
    ReplaceSelectionCommand( const QString& text, int caolesceId );

    virtual int commandId() { return coalesceId_; }

    virtual void execute( TextEditorController* controller ) override;
    virtual QString toString() override;

private:
    QString text_;          ///< The text to 'replace'
    int coalesceId_;        ///< The coalesce id to use for this command

};

} // edbee
