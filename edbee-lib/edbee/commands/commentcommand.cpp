/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "commentcommand.h"

#include "edbee/models/textdocument.h"
#include "edbee/views/textselection.h"
#include "edbee/texteditorcontroller.h"

#include "debug.h"

namespace edbee {

// TODO, we also need to support, block-comments

/// Executes the given comment
/// @param controller the controller this command is executed for
void CommentCommand::execute(TextEditorController* controller)
{
    QString commentStart = "//";    // TODO, we need to get this value from the current context (scope)
    TextDocument* doc = controller->textDocument();

    /// Iterate over the lines (TODO, implement this)!
}


/// converts this command to a string
QString CommentCommand::toString()
{
    return "CommentCommand";
}



} // edbee
