// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "treesittertextgrammar.h"

#include <math.h>
#include <dlfcn.h> // for usin glSym

#include <QDir>

#include "tree_sitter/api.h"

#include "edbee/lexers/treesittertextlexer.h"

#include "edbee/debug.h"

namespace edbee {

// experiment with dynmamic loading languages ----------
typedef const TSLanguage *(*LanguageFn)(void);

// Dirty Stuff: <https://www.perplexity.ai/search/i-m-using-tree-sitter-is-it-po-kdAyojGjQwuJBLO0ihxc2A>
const TSLanguage *load_language(const char *lib_path, const char *symbol_name) {
    void *handle = dlopen(lib_path, RTLD_NOW);
    if (!handle) return NULL;

    LanguageFn fn = (LanguageFn)dlsym(handle, symbol_name);
    if (!fn) return NULL;

    return fn();
}

// =====================


/// The default texgrammar constructor
/// @param name the name of the textgrammar
/// @param displayName th name to display
TreeSitterTextGrammar::TreeSitterTextGrammar(
    const QString& identifier,
    const QString& name,
    const QString& displayName,
    const QString& parserPath,
    const QString& scope,
    const QString& injectionRegExp)
    : TextGrammar(identifier, name, displayName)
    , parserPath_(parserPath)
    , scope_(scope)
    , injectionRegExp_(injectionRegExp)
    , tsLanguage_(nullptr)
    , initialized_(false)
{
}


TreeSitterTextGrammar::~TreeSitterTextGrammar()
{
    // delete all queries
    for (auto it = queries_.begin(); it != queries_.end(); ++it) {
        QString name = it.key();
        TSQuery* query = it.value();
        ts_query_delete(query);
    }
    queries_.clear();
}


/// Creates the TreeSitterTextLexer for this grammar
TextLexer* TreeSitterTextGrammar::createTextLexer(TextDocumentScopes* scopes)
{
    return new TreeSitterTextLexer(this, scopes);
}

/// Initializes the grammar
bool TreeSitterTextGrammar::initialize()
{
    if (initialized_) return true;

    // create the language
    tsLanguage_ = initializeLanguage();
    initializeQueries();

    initialized_ = true;
    return true;
}


/// Registers a query path.
/// Thesee queries are loaded when initialize parser is called
void TreeSitterTextGrammar::registerQueryPath(const QString& name, const QString& path)
{
    queryPaths_[name] = path;
}


/// Loads a query and registers it for a given name
/// The path is also registered in queryPaths
bool TreeSitterTextGrammar::loadQuery(const QString& name, const QString& path)
{
    TSQuery* query = loadQuery(path);
    if (!query) return false;

    if (queries_[name]) {
        qDebug() << "** WARNING replaceing query " << name << " for language " << this->name();
        ts_query_delete(queries_[name]);
    }

    qDebug() << " - loaded query " << name << ": " << query;
    queryPaths_[name] = path;
    queries_[name] = query;
    return true;
}


/// Returns the Tree Sitter language
const TSLanguage* TreeSitterTextGrammar::language() const
{
    return tsLanguage_;
}


/// Returns the given query
TSQuery* TreeSitterTextGrammar::query(const QString& name)
{
    return queries_[name];
}


/// Initializes the language parser
/// - Dynamically load a library and refer the parser function
const TSLanguage* TreeSitterTextGrammar::initializeLanguage()
{
    QString ext = "dylib";
    QString dylib = QString("%1/libtree-sitter-%2.%3").arg(parserPath_, name_, ext);
    qDebug() << "Load dylib: " << dylib;
    const TSLanguage* lang = load_language(qPrintable(dylib), qPrintable(QString("tree_sitter_%1").arg(name_)));
    return lang;
}


/// initializes all queres
void TreeSitterTextGrammar::initializeQueries()
{
    for (auto it = queryPaths_.begin(); it != queryPaths_.end(); ++it) {
        const QString name = it.key();
        const QString path = it.value();
        loadQuery(name, path);
    }
}


/// load the query
TSQuery* TreeSitterTextGrammar::loadQuery(const QString& path)
{
    QFile f(path);
    if (!f.open(QFile::ReadOnly)) {
        qDebug() << "Error opening file" << path;
        return nullptr;
    }
    QString highlightQueryCode = QString::fromUtf8(f.readAll());
    f.close();

    uint32_t error_offset;
    TSQueryError error_type;
    TSQuery *query = ts_query_new(
        tsLanguage_, // const TSLanguage *language,
        qPrintable(highlightQueryCode),
        static_cast<uint32_t>(highlightQueryCode.length()),
        &error_offset,
        &error_type
        );
    if (!query) {
        qDebug() <<  "Query parse error at: " << error_offset << ": " << error_type;
        return nullptr;
    }
    return query;
}


} // edbee
