// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "texteditorcommand.h"

#include "edbee/debug.h"

namespace edbee {


/// The default constructor
TextEditorCommand::TextEditorCommand()
{
}


/// A blank virtual destructor
TextEditorCommand::~TextEditorCommand()
{

}

bool TextEditorCommand::readonly()
{
    return false;
}


} // edbee
