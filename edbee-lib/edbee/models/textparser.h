// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

struct TSLanguage;
struct TSParser;
struct TSQuery;
struct TSTree;


namespace edbee {

/*
class TextDocument;
class TextParserGrammar;


class EDBEE_EXPORT TextDocumentParser {
public:
    TextDocumentParser(TextDocument* textDocument);
    virtual ~TextDocumentParser();

    void buildParser();
    void buildScopes();

    TSParser* tsParser() const { return tsParser_; }
    TSTree* parseString(const char* string, int length);


private:
    TextDocument* textDocumentRef_;
    TextParserGrammar* textParserGrammarRef_;
    TSParser* tsParser_;
    TSTree* tsTree_;
    TSQuery* tsQuery_;
};

//================================

/// A TextParser - a treesitter-based parser is .scm for highlighting etc
/// The expected structure for a parser directory is the following:
///
///  libtree-sitter-<language>.a
///  libtree-sitter-<lagnuage>.dylib
///  queries/highlights.scm
///  queries/*.scm
///  tree-sitter.json
///
class EDBEE_EXPORT TextParserGrammar {
public:
    TextParserGrammar(
        const QString& name,
        const QString& displayName,
        const QString& parserPath,
        const QStringList& extensions,
        const QString& scope,
        const QString& injectionRegexp
    );
    virtual ~TextParserGrammar();

    bool initializeGrammar();
    bool initialized() const  { return initialized_; }
    void registerQueryPath(const QString& name, const QString& path);
    bool loadQuery(const QString& name, const QString& path);

    QString name() const { return name_; }
    QString displayName() const { return displayName_; }

    const TSLanguage* language() const;
    TSQuery* query(const QString& name);

private:
    const TSLanguage* loadLanguageParser();
    TSQuery* loadQuery(const QString& path);


private:
    // const QString language_;
    // const QStringList extensions_;

    const QString name_;                ///< the display name of this
    const QString displayName_;         ///< the name to display
    const QString parserPath_;          ///< The absoslute path of the parser location
    const QStringList fileExtensions_;  ///< A list with all file-extensions
    const QString scope_;		        ///< The base scope
    const QString injectionRegExp_;		///< Injecetion regexep (not yet used)

    const TSLanguage* tsLanguage_;    	///< The Tree Sitter Language
    QMap<QString, QString> queryPaths_; ///< The list of QueryPath names
    QMap<QString, TSQuery*> queries_; 	///< The list of TSQuerie (highlight, local, tags )

    bool initialized_;					///< Has this parsere been initialized
};

//================================

/// This is the text parser manager.
/// It loads all available parsers with it's .scm queries to build hightlights
/// Default behaviour is JIT, so things only get loaded when required
class EDBEE_EXPORT TextParserGrammarManager {
protected:
     TextParserGrammarManager();
     virtual ~TextParserGrammarManager();

public:
    void registerGrammars(const QString& path, bool eagerLoad = false);
    bool registerGrammarsWithTreeSitterJson(const QString& jsonFileName, bool eagerLoad = false);
    bool registerGrammar(TextParserGrammar* parser, bool eagerLoad = false);

    TextParserGrammar* get(const QString& name) const;
    TextParserGrammar* getWithoutInitialization(const QString& name) const;

private:
    TextParserGrammar* buildParserFromPath(const QString& language, const QString& path);

//     TextGrammar* get(const QString& name);
//     void giveGrammar(TextGrammar* grammar);
//
//     QList<QString> grammarNames();
//     QList<TextGrammar*> grammars();
//     QList<TextGrammar*> grammarsSortedByDisplayName();
//
//     TextGrammar* defaultGrammar() { return defaultGrammarRef_; }
//     TextGrammar* detectGrammarWithFilename( const QString& fileName );
//
//     QString lastErrorMessage() const;
//
 private:

     TextParserGrammar* defaultParserRef_;            ///< A reference to the default grammar
     QMap<QString, TextParserGrammar*> grammarMap_;   ///< A map with all grammar definitions
     QString lastErrorMessage_;                       ///< Returns the error message

     friend class Edbee;

};

*/

}
