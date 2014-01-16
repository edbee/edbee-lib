/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "edbee.h"

#include <QApplication>

#include "edbee/models/dynamicvariables.h"
#include "edbee/models/textbuffer.h"
#include "edbee/models/texteditorcommandmap.h"
#include "edbee/models/texteditorkeymap.h"
#include "edbee/models/textdocumentscopes.h"
#include "edbee/models/textgrammar.h"
#include "edbee/util/textcodec.h"
#include "edbee/views/texttheme.h"


#include "debug.h"

namespace edbee {

/// The edbee instance singleton
static Edbee* theInstance=0;


/// The constructor
Edbee::Edbee()
    : inited_(false)
    , defaultCommandMap_(0)
    , codecManager_(0)
    , scopeManager_(0)
    , grammarManager_(0)
    , themeManager_(0)
    , keyMapManager_(0)
    , environmentVariables_(0)
{
}


/// The edbee destructors destroys the different managers
Edbee::~Edbee()
{
    delete environmentVariables_;
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


/// sets the path where to find the keymap files
/// @param keyMapPath the path with keymap files
void Edbee::setKeyMapPath( const QString& keyMapPath )
{
    keyMapPath_ = keyMapPath;
}


/// Sets the grammar path
/// @param grammarPath the path with the grammar files
void Edbee::setGrammarPath( const QString& grammarPath )
{
    grammarPath_ = grammarPath;
}


/// Setst the path where to find the theme files
/// @param themePath the path to find the themes
void Edbee::setThemePath( const QString& themePath )
{
    themePath_ = themePath;
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


/// TODO: We need a way to load the (scoped) environment variables
/// for now we just add some variables for some common languages
static void initHardCodedDynamicScopes( DynamicVariables* env )
{
    QString tcs("TM_COMMENT_START");
    QString tce("TM_COMMENT_END");
    QString tcs2("TM_COMMENT_START_2");
    QString tce2("TM_COMMENT_END_2");
    QString tcs3("TM_COMMENT_START_3");
    QString tce3("TM_COMMENT_END_3");
    env->setAndGiveScopedSelector( tcs, "# ", "source.yaml");

    env->setAndGiveScopedSelector( tcs, "// ", "source.c, source.c++, source.objc, source.objc++");
    env->setAndGiveScopedSelector( tcs2, "/*", "source.c, source.c++, source.objc, source.objc++");
    env->setAndGiveScopedSelector( tce2, "*/", "source.c, source.c++, source.objc, source.objc++");

    env->setAndGiveScopedSelector( tcs, "/*", "source.css");
    env->setAndGiveScopedSelector( tce, "*/", "source.css");

    env->setAndGiveScopedSelector( tcs, "; ", "source.clojure");

    env->setAndGiveScopedSelector( tcs, "# ", "source.coffee");
    env->setAndGiveScopedSelector( tcs2, "###", "source.coffee");
    env->setAndGiveScopedSelector( tce2, "###", "source.coffee");

    env->setAndGiveScopedSelector( tcs, "<!-- ", "text.html");
    env->setAndGiveScopedSelector( tce, " -->", "text.html");

    env->setAndGiveScopedSelector( tcs, "<!-- ", "text.xml");
    env->setAndGiveScopedSelector( tce, " -->", "text.xml");

    env->setAndGiveScopedSelector( tcs, "// ", "source.java");
    env->setAndGiveScopedSelector( tcs2, "/*", "source.java");
    env->setAndGiveScopedSelector( tce2, "*/", "source.java");

    env->setAndGiveScopedSelector( tcs, "// ", "source.js, source.json");
    env->setAndGiveScopedSelector( tcs2, "/*", "source.js, source.json");
    env->setAndGiveScopedSelector( tce2, "*/", "source.js, source.json");

    env->setAndGiveScopedSelector( tcs, "// ", "source.php");
    env->setAndGiveScopedSelector( tcs2, "# ", "source.php");
    env->setAndGiveScopedSelector( tcs3, "/*", "source.php");
    env->setAndGiveScopedSelector( tce3, "*/", "source.php");

    env->setAndGiveScopedSelector( tcs, "# ", "source.perl");

    env->setAndGiveScopedSelector( tcs, "-# ", "text.haml");    // I hate the default '/'

    env->setAndGiveScopedSelector( tcs, "# ", "source.js, source.ruby");
    env->setAndGiveScopedSelector( tcs2, "=begin", "source.js, source.ruby");
    env->setAndGiveScopedSelector( tce2, "=end", "source.js, source.ruby");

    env->setAndGiveScopedSelector( tcs, "// ", "source.scss");
    env->setAndGiveScopedSelector( tcs2, "/*", "source.scss");
    env->setAndGiveScopedSelector( tce2, "*/", "source.scss");

    env->setAndGiveScopedSelector( tcs, "-- ", "source.sql");
    env->setAndGiveScopedSelector( tcs2, "/*", "source.sql");
    env->setAndGiveScopedSelector( tce2, "*/", "source.sql");

    env->setAndGiveScopedSelector( tcs, "# ", "source.shell");

}



/// initializes the engine on startup
void Edbee::init()
{
    defaultCommandMap_    = new TextEditorCommandMap();
    codecManager_         = new TextCodecManager();
    scopeManager_         = new TextScopeManager();
    themeManager_         = new TextThemeManager();
    grammarManager_       = new TextGrammarManager();
    keyMapManager_        = new TextKeyMapManager();
    environmentVariables_ = new DynamicVariables();

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

    // for now initialize the dynamic variables
    initHardCodedDynamicScopes(environmentVariables_);
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


/// returns the default editor keymap
TextEditorKeyMap* Edbee::defaultKeyMap()
{
    return keyMapManager()->get("");
}


/// Returns the default editor command map. This is the default command map that's used
/// by the editor components
/// @return the default command map
TextEditorCommandMap* Edbee::defaultCommandMap()
{
    return defaultCommandMap_;
}


/// Returns the codec-manager for Edbee
TextCodecManager* Edbee::codecManager()
{
    Q_ASSERT(inited_);
    return codecManager_;
}


/// Returns the scope manager. The scope manager is a general class to share scope names and identifiers between different editors
TextScopeManager* Edbee::scopeManager()
{
    Q_ASSERT(inited_);
    return scopeManager_;
}


/// Returns the grammar manager used by all editors
/// The grammar manager is used for sharing the different grammar definitions between the different editors
TextGrammarManager* Edbee::grammarManager()
{
    Q_ASSERT(inited_);
    return grammarManager_;
}


/// Returns the theme manager
/// The theme manager is used to share themes between the different editors
TextThemeManager* Edbee::themeManager()
{
    Q_ASSERT(inited_);
    return themeManager_;
}


/// Returns the keymap manager
TextKeyMapManager* Edbee::keyMapManager()
{
    Q_ASSERT(inited_);
    return keyMapManager_;
}


/// Rreturns the dynamicvariables object
DynamicVariables* Edbee::environmentVariables()
{
    Q_ASSERT(inited_);
    return environmentVariables_;
}


} // edbee
