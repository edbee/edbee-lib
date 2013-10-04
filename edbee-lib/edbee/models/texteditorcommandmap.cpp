/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "texteditorcommandmap.h"

#include "edbee/commands/copycommand.h"
#include "edbee/commands/cutcommand.h"
#include "edbee/commands/debugcommand.h"
#include "edbee/commands/findcommand.h"
#include "edbee/commands/pastecommand.h"
#include "edbee/commands/redocommand.h"
#include "edbee/commands/removeselectioncommand.h"
#include "edbee/commands/replaceselectioncommand.h"
#include "edbee/commands/selectioncommand.h"
#include "edbee/commands/tabcommand.h"
#include "edbee/commands/undocommand.h"
#include "edbee/texteditorcommand.h"

#include "debug.h"

namespace edbee {

TextEditorCommandMap::TextEditorCommandMap(QObject *parent)
    : QObject(parent)
{

    give( "goto_next_char", new SelectionCommand( SelectionCommand::MoveCaretByCharacter, 1 ) );
    give( "goto_prev_char", new SelectionCommand( SelectionCommand::MoveCaretByCharacter, -1 ) );
    give( "goto_next_word", new SelectionCommand( SelectionCommand::MoveCaretByWord, 1 ) );
    give( "goto_prev_word", new SelectionCommand( SelectionCommand::MoveCaretByWord, -1 ) );
    give( "goto_bol", new SelectionCommand( SelectionCommand::MoveCaretToLineBoundary, -1 ) );
    give( "goto_eol", new SelectionCommand( SelectionCommand::MoveCaretToLineBoundary, 1 ) );
    give( "goto_next_line", new SelectionCommand( SelectionCommand::MoveCaretByLine, 1 ) );
    give( "goto_prev_line", new SelectionCommand( SelectionCommand::MoveCaretByLine, -1 ) );
    give( "goto_bof", new SelectionCommand( SelectionCommand::MoveCaretToDocumentBegin ) );
    give( "goto_eof", new SelectionCommand( SelectionCommand::MoveCaretToDocumentEnd ) );
    give( "goto_page_down", new SelectionCommand( SelectionCommand::MoveCaretByPage, 1 ));
    give( "goto_page_up", new SelectionCommand( SelectionCommand::MoveCaretByPage, -1 ));

    // selection
    give( "sel_next_char", new SelectionCommand( SelectionCommand::MoveCaretByCharacter, 1, true ) );
    give( "sel_prev_char", new SelectionCommand( SelectionCommand::MoveCaretByCharacter, -1, true ) );
    give( "sel_next_word", new SelectionCommand( SelectionCommand::MoveCaretByWord, 1, true ) );
    give( "sel_prev_word", new SelectionCommand( SelectionCommand::MoveCaretByWord, -1, true ) );
    give( "sel_to_bol", new SelectionCommand( SelectionCommand::MoveCaretToLineBoundary, -1, true) );
    give( "sel_to_eol", new SelectionCommand( SelectionCommand::MoveCaretToLineBoundary, 1, true ) );
    give( "sel_to_next_line", new SelectionCommand( SelectionCommand::MoveCaretByLine, 1, true ) );
    give( "sel_to_prev_Line", new SelectionCommand( SelectionCommand::MoveCaretByLine, -1, true ) );
    give( "sel_to_bof", new SelectionCommand( SelectionCommand::MoveCaretToDocumentBegin, -1, true ) );
    give( "sel_to_eof", new SelectionCommand( SelectionCommand::MoveCaretToDocumentEnd, 1, true ) );
    give( "sel_page_down", new SelectionCommand( SelectionCommand::MoveCaretByPage, 1, true ));
    give( "sel_page_up", new SelectionCommand( SelectionCommand::MoveCaretByPage, -1, true ));

    give( "sel_all", new SelectionCommand( SelectionCommand::SelectAll ) );
    give( "sel_word", new SelectionCommand( SelectionCommand::SelectWord ));
    give( "sel_line", new SelectionCommand( SelectionCommand::SelectFullLine, 1 ));
    give( "sel_prev_line", new SelectionCommand( SelectionCommand::SelectFullLine, -1 ));
    give( "add_caret_prev_line", new SelectionCommand( SelectionCommand::AddCaretByLine, -1));
    give( "add_caret_next_line", new SelectionCommand( SelectionCommand::AddCaretByLine, 1));
    give( "sel_reset", new SelectionCommand( SelectionCommand::ResetSelection ) );

    // line entry
    give( "ins_newline", new ReplaceSelectionCommand( "\n", CoalesceId_InsertNewLine ));

    give( "del_left", new RemoveSelectionCommand(-1) );
    give( "del_right", new RemoveSelectionCommand(1) );

    // tab entry
    //give( "tab", new ReplaceSelectionCommand( "\t", CoalesceId_InsertTab ));
    /// TODO: add a backtab action here
    //set( QKeySequence( Qt::Key_BackTab ), new )
    give( "tab", new TabCommand( TabCommand::Forward, true ));
    give( "tab_back", new TabCommand( TabCommand::Backward, true ));

    give( "indent", new TabCommand( TabCommand::Forward, false));
    give( "outdent", new TabCommand( TabCommand::Backward, false ));

    // undo / redo comamnds
    give( "undo", new UndoCommand() );
    give( "redo", new RedoCommand() );
    give( "soft_undo", new UndoCommand(true) );
    give( "soft_redo", new RedoCommand(true) );

    // clipboard operations
    give( "copy", new CopyCommand( ) );
    give( "cut", new CutCommand() );
    give( "paste", new PasteCommand() );

    // debug commands
    give( "debug_dump_scopes", new DebugCommand( DebugCommand::DumpScopes ) );
    give( "debug_rebuild_scopes", new DebugCommand( DebugCommand::RebuildScopes ) );
    give( "debug_dump_undo_stack", new DebugCommand( DebugCommand::DumpUndoStack ) );

    // find items
    give( "find_use_sel", new FindCommand( FindCommand::UseSelectionForFind ) );
    give( "find_next_match", new FindCommand( FindCommand::FindNextMatch) );
    give( "find_prev_match", new FindCommand( FindCommand::FindPreviousMatch) );
    give( "sel_next_match", new FindCommand( FindCommand::SelectNextMatch ));
    give( "sel_prev_match", new FindCommand( FindCommand::SelectPreviousMatch));
    give( "sel_all_matches", new FindCommand( FindCommand::SelectAllMatches ));

    give( "select_under_expand", new FindCommand( FindCommand::SelectUnderExpand ));
    give( "select_all_under", new FindCommand( FindCommand::SelectAllUnder ));
}

TextEditorCommandMap::~TextEditorCommandMap()
{
    qDeleteAll( commandMap_ );
    commandMap_.clear();
}

/// gives the command
void TextEditorCommandMap::give(const QString& key, TextEditorCommand* command)
{
    delete commandMap_.value(key);
    commandMap_.insert(key,command);
}

/// returns the given command or 0
TextEditorCommand *TextEditorCommandMap::get(const QString& key)
{
    return commandMap_.value(key);
}

} // edbee
