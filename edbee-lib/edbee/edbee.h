// edbee - Copyright (c) 2012-2025 by Rick Blommers and contirbutors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"
#include "edbee/edbeeversion.h"

#include <QObject>

namespace edbee {

class DynamicVariables;
class TextAutoCompleteProviderList;
class TextCodecManager;
class TextEditorCommandMap;
class TextEditorKeyMap;
class TextGrammarManager;
class TextKeyMapManager;
class TextParserGrammarManager;
class TextScopeManager;
class TextThemeManager;

/// The texteditor manager,
/// It manages all singleton objects for the editor
/// It performs the initialization and shutdown code for all editors
class EDBEE_EXPORT Edbee : public QObject
{
    Q_OBJECT
private:
    Edbee();
    virtual ~Edbee();

public:
    static Edbee* instance();

    void setKeyMapPath( const QString& keyMapPath );
    void setGrammarPath( const QString& grammarPath );
    void setParserPath( const QString& parserPath );
    void setThemePath( const QString& themePath );

    void autoInit();
    const char* version() const;

public slots:
    void init();
    void shutdown();
    void autoShutDownOnAppExit();

public:

    TextEditorKeyMap* defaultKeyMap();
    TextEditorCommandMap* defaultCommandMap();

    TextCodecManager* codecManager();
    TextScopeManager* scopeManager();
    TextGrammarManager* grammarManager();
    TextParserGrammarManager* parserGrammarManager();
    TextThemeManager* themeManager();
    TextKeyMapManager* keyMapManager();
    DynamicVariables* environmentVariables();
    TextAutoCompleteProviderList* autoCompleteProviderList();


protected:
    bool inited_;                                    ///< This method is set to true if the manager is inited

    QString grammarPath_;                            ///< The path to load all grammars from
    QString parserPath_;				             ///< The path to l     oad the tree sitte grammars from
    QString themePath_;                              ///< The path to load all themes from
    QString keyMapPath_;                             ///< The path to load all keymaps

    TextEditorCommandMap* defaultCommandMap_;        ///< The default command map

    TextCodecManager* codecManager_;                 ///< The text-editor codec manager
    TextScopeManager* scopeManager_;                 ///< The text-scope manager
    TextGrammarManager* grammarManager_;             ///< The grammar manager
    TextParserGrammarManager* parserGrammarManager_; ///< The parser manager
    TextThemeManager* themeManager_;                 ///< The text theme manager
    TextKeyMapManager* keyMapManager_;               ///< The keymap manager
    DynamicVariables* environmentVariables_;         ///< The (dynamic) environment variables
    TextAutoCompleteProviderList* autoCompleteProviderList_;   ///< The global autocomplete providers
};



} // edbee
