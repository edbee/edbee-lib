/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "edbee.h"

#include <QApplication>

#include "edbee/models/textbuffer.h"
#include "edbee/models/texteditorcommandmap.h"
#include "edbee/models/texteditorkeymap.h"
#include "edbee/models/textdocumentscopes.h"
#include "edbee/models/textgrammar.h"
#include "edbee/util/textcodec.h"
#include "edbee/views/texttheme.h"


#include "debug.h"

namespace edbee {

static Edbee* theInstance=0;

Edbee::Edbee()
    : inited_(false)
    , defaultCommandMap_(0)
    , codecManager_(0)
    , scopeManager_(0)
    , grammarManager_(0)
    , themeManager_(0)
    , keyMapManager_(0)
{
}

Edbee::~Edbee()
{
    delete keyMapManager_;
    delete themeManager_;
    delete grammarManager_;
    delete scopeManager_;
    delete codecManager_;
    delete defaultCommandMap_;
}

/// The singleton instance getter
Edbee* Edbee::instance()
{
    if( !theInstance ) { theInstance = new Edbee(); }
    return theInstance;
}

/*
static void initKeyMap( TextEditorKeyMap& km )
{
    //TextCommandManager* man = TextCommandManager::defaultManager();
    // movement
    km.set( "goto_next_char", QKeySequence::MoveToNextChar );
    km.set( "goto_prev_char", QKeySequence::MoveToPreviousChar );
    km.set( "goto_next_word", QKeySequence::MoveToNextWord );
    km.set( "goto_prev_word", QKeySequence::MoveToPreviousWord );
    km.set( "goto_bol", QKeySequence::MoveToStartOfLine );
    km.set( "goto_eol", QKeySequence::MoveToEndOfLine );
    km.set( "goto_next_line", QKeySequence::MoveToNextLine  );
    km.set( "goto_prev_line", QKeySequence::MoveToPreviousLine );
    km.set( "goto_bof", QKeySequence::MoveToStartOfDocument );
    km.set( "goto_eof", QKeySequence::MoveToEndOfDocument );
    km.set( "goto_page_down", QKeySequence::MoveToNextPage );
    km.set( "goto_page_up", QKeySequence::MoveToPreviousPage );

    // selection
    km.set( "sel_next_char", QKeySequence::SelectNextChar );
    km.set( "sel_prev_char", QKeySequence::SelectPreviousChar );
    km.set( "sel_next_word", QKeySequence::SelectNextWord );
    km.set( "sel_prev_word", QKeySequence::SelectPreviousWord );
    km.set( "sel_to_bol", QKeySequence::SelectStartOfLine );
    km.set( "sel_to_eol", QKeySequence::SelectEndOfLine );
    km.set( "sel_to_next_line", QKeySequence::SelectNextLine );
    km.set( "sel_to_prev_Line", QKeySequence::SelectPreviousLine );
    km.set( "sel_to_bof", QKeySequence::SelectStartOfDocument );
    km.set( "sel_to_eof", QKeySequence::SelectEndOfDocument );
    km.set( "sel_page_down", QKeySequence::SelectNextPage );
    km.set( "sel_page_up", QKeySequence::SelectPreviousPage );

    km.set( "sel_all", QKeySequence::SelectAll );
    km.set( "sel_word", QKeySequence("Ctrl+D") );
    km.set( "sel_line", QKeySequence("Ctrl+L") );
    km.set( "sel_prev_line", QKeySequence("Ctrl+Shift+L") );
    km.set( "add_caret_prev_line", QKeySequence("Meta+shift+Up") );
    km.set( "add_caret_next_line", QKeySequence("Meta+shift+Down") );
    km.set( "sel_reset", QKeySequence("Escape") );

    // line entry
    km.set( "ins_newline", QKeySequence( Qt::Key_Enter ) );
    km.set( "ins_newline", QKeySequence( Qt::Key_Return ) );

    km.set( "del_left", QKeySequence( Qt::Key_Backspace ) );
    km.set( "del_left", QKeySequence( Qt::SHIFT+Qt::Key_Backspace) );
    km.set( "del_right", QKeySequence( Qt::Key_Delete ) );

    // tab entry
    km.set( "tab", QKeySequence( Qt::Key_Tab ) );
    /// TODO: add a backtab action here
    //set( QKeySequence( Qt::Key_BackTab ), new )

    // undo / redo comamnds
    km.set( "undo", QKeySequence::Undo );
    km.set( "redo", QKeySequence::Redo );
    km.set( "soft_undo", QKeySequence("Ctrl+U") );
    km.set( "soft_redo", QKeySequence("Ctrl+Shift+U") );

    // clipboard operations
    km.set( "copy", QKeySequence::Copy );
    km.set( "cut", QKeySequence::Cut );
    km.set( "paste", QKeySequence::Paste  );

    // debug commands
    km.set( "debug_dump_scopes", QKeySequence("Ctrl+Shift+X,S") );
    km.set( "debug_rebuild_scopes", QKeySequence("Ctrl+Shift+X,R") );
    km.set( "debug_dump_undo_stack", QKeySequence("Ctrl+Shift+X,U") );

    // find commands
    km.set( "find_use_sel", QKeySequence("Ctrl+E") );
    km.set( "find_next_match", QKeySequence::FindNext );
    km.set( "find_prev_match", QKeySequence::FindPrevious );
    km.set( "sel_next_match", QKeySequence("Meta+S") );
    km.set( "sel_prev_match", QKeySequence("Meta+Shift+S") );
    km.set( "sel_all_matches", QKeySequence("Ctrl+Shift+Meta+A") );
}
*/


/// initializes the engine on startup
void Edbee::init()
{
    defaultCommandMap_  = new TextEditorCommandMap();   
    codecManager_       = new TextCodecManager();
    scopeManager_       = new TextScopeManager();
    themeManager_       = new TextThemeManager();
    grammarManager_     = new TextGrammarManager();
    keyMapManager_      = new TextKeyMapManager();


    qRegisterMetaType<edbee::TextBufferChange>("edbee::TextBufferChange");


    if( !grammarPath_.isEmpty() ) {
        grammarManager_->readAllGrammarFilesInPath( grammarPath_ );
    }
    if( !themePath_.isEmpty() ) {
       themeManager_->listAllThemes( themePath_ );
    }
    if( !keyMapPath_.isEmpty() ) {
        keyMapManager_->loadAllKeyMaps( keyMapPath_ );
    } else {
        qlog_warn() << "No keymap file path supplied. The editor will NOT work without a descent keymap!";
    }

    inited_ = true;
}

/// destroys the texteditor manager and all related class
void Edbee::shutdown()
{
    delete theInstance;
    theInstance = 0;
}

/// Call this method to automaticly shutdown the texteditor manager on shutdown
/// (This method listens to the qApp::aboutToQuit signal
void Edbee::autoShutDownOnAppExit()
{
    connect( qApp, SIGNAL(aboutToQuit()),this,SLOT(shutdown()) );
}

/// returns the editor keymap
TextEditorKeyMap* Edbee::defaultKeyMap()
{
    return keyMapManager()->get("");
}

TextCodecManager *Edbee::codecManager()
{
    Q_ASSERT(inited_);
    return codecManager_;
}

TextScopeManager *Edbee::scopeManager()
{
    Q_ASSERT(inited_);
    return scopeManager_;
}

TextGrammarManager *Edbee::grammarManager()
{
    Q_ASSERT(inited_);
    return grammarManager_;
}

TextThemeManager *Edbee::themeManager()
{
    Q_ASSERT(inited_);
    return themeManager_;
}

TextKeyMapManager *Edbee::keyMapManager()
{
    Q_ASSERT(inited_);
    return keyMapManager_;
}



} // edbee
