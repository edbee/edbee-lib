// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QHash>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include "edbee/models/textgrammar.h"

class QFile;

struct TSLanguage;
struct TSQuery;
struct TSParser;
struct TSTree;

namespace edbee {

class Edbee;
class RegExp;
class TextDocumentScopes;

/// For parsing with regexp like grammars (TextMate Grammar formt)
class EDBEE_EXPORT TreeSitterTextGrammar : public TextGrammar {
public:
    TreeSitterTextGrammar(
        const QString& identifier,
        const QString& name,
        const QString& displayName,
        const QString& parserPath,
        const QString& scope,
        const QString& injectionRegExp
    );

    virtual ~TreeSitterTextGrammar();

    virtual QString defaultScopeName() const { return scope_; }
    virtual TextLexer* createTextLexer(class TextDocumentScopes *scopes);

    bool initialize();
    bool initialized() const  { return initialized_; }

    void registerQueryPath(const QString& name, const QString& path);
    bool loadQuery(const QString& name, const QString& path);

    const TSLanguage* language() const;
    TSQuery* query(const QString& name);



private:
    const TSLanguage* initializeLanguage();
    void initializeQueries();
    TSQuery* loadQuery(const QString& path);


private:

    const QString parserPath_;          ///< The absoslute path of the parser location
    const QString scope_;		        ///< The base scope
    const QString injectionRegExp_;		///< Injecetion regexep (not yet used)

    const TSLanguage* tsLanguage_;    	///< The Tree Sitter Language
    QMap<QString, QString> queryPaths_; ///< The list of QueryPath names
    QMap<QString, TSQuery*> queries_; 	///< The list of TSQuerie (highlight, local, tags )

    bool initialized_;					///< Has this parsere been initialized
};

} // edbee
