// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textparser.h"

#include <math.h>
#include <dlfcn.h> // for usin glSym

#include <QDir>


#include "edbee/models/textdocument.h"

#include "tree_sitter/api.h"

# include "edbee/edbee.h"

// Declare the `tree_sitter_json` function, which is
// implemented by the `tree-sitter-json` library.
//const TSLanguage *tree_sitter_json(void);

// #include "edbee/models/textbuffer.h"
// #include "edbee/models/textdocument.h"
// #include "edbee/edbee.h"
// #include "edbee/util/regexp.h"

#include "edbee/debug.h"

#include <edbee/io/jsonparser.h>
#include <qjsonobject.h>


namespace edbee {

/*

// class implementation ------------------------------

TextDocumentParser::TextDocumentParser(TextDocument *textDocument)
    : textDocumentRef_(textDocument)
    , textParserGrammarRef_(nullptr)
    , tsParser_(nullptr)
    , tsTree_(nullptr)
    , tsQuery_(nullptr)
{
    qDebug() << "TextDocumentParser: Figure out how to use this later";
// const char *source_code = R"RUBY(
// class Test
//   def foo
//     puts 'foo: ' + "bar"
//   end
// end
// )RUBY";
//     this->textDocumentRef_->setText(source_code);
//

    buildParser();

}

TextDocumentParser::~TextDocumentParser()
{
    if (tsTree_) {
        ts_tree_delete(tsTree_);
    }
    if (tsParser_) {
        ts_parser_delete(tsParser_);
    }
}

void TextDocumentParser::buildParser()
{

    //textParserRef_ = Edbee::instance()->parserManager()->:w
    textParserGrammarRef_ = Edbee::instance()->parserGrammarManager()->get("ruby");
    if (!textParserGrammarRef_) {
        qDebug() << "Couldn't find grammar!";
        return;
    }

    // create the parser
    tsParser_ = ts_parser_new();
    if (!ts_parser_set_language(tsParser_, textParserGrammarRef_->language())) {
        qDebug() << "Error settings parser language to " <<  textParserGrammarRef_->displayName();
    }

//     const char *source_code = R"RUBY(
// class Test
//   def foo
//      puts 'foo: ' + "bar"
//   end
// end
// )RUBY";


    // tsTree_ = parseString(source_code, strlen(source_code));

    QChar* chars = textDocumentRef_->buffer()->rawDataPointer();
    size_t length = textDocumentRef_->buffer()->length();
    tsTree_ = createTsTreeForQCharBuffer(tsParser_, chars, length);


    buildScopes();

    /// ---------------------------------------/

}

void TextDocumentParser::buildScopes()
{
    if (!textParserGrammarRef_ ) {
        return;
    }


    TSQuery* query = textParserGrammarRef_->query("highlights");
    if (!query) {
        qDebug() << "** WARNING Query highlight not found!! **";

        return;
    }

    // Print the syntax tree as an S-expression.

    // Get the root node of the syntax tree.
    TSNode root_node = ts_tree_root_node(tsTree_);
    char *string = ts_node_string(root_node);
    qDebug() << "Syntax tree: " <<  string;
    // Free all of the heap-allocated memory.
    free(string);

    // Execute it
    // ref: <https://blog.pulsar-edit.dev/tag/tree-sitter/>
    // ref: <https://www.perplexity.ai/search/i-m-using-trees-titter-to-exec-CDxmM65VTEWd_KTydkn_Qw>
    TSQueryCursor *cursor = ts_query_cursor_new();
    ts_query_cursor_exec(cursor, query, root_node);


    // the raw pointer to the text buffer
    QChar* source_code = textDocumentRef_->buffer()->rawDataPointer();

    TSQueryMatch match;
    uint32_t match_count = 0;
    while (ts_query_cursor_next_match(cursor, &match)) {
        qDebug() << "> Match";
        //qDebug() << "  * id: " << match.id;
        //qDebug() << "  * pattern_index: " << match.pattern_index;
        //qDebug() << "  * captures: ";

        for (uint32_t i = 0; i < match.capture_count; i++ ) {
            TSQueryCapture capture = match.captures[i];
            char* nodeString = ts_node_string(capture.node);

            QString text = extract_node_text(capture.node, source_code);

            uint32_t nameLength = 0;
            const char* name = ts_query_capture_name_for_id(query, capture.index, &nameLength);
            QString captureName = QString::fromUtf8(name, nameLength);


            qDebug() << "   - " << i << ": " << capture.index << ": " << nodeString << " => " << text << " : " << captureName;
            free(nodeString);
        }

        // for (uint32_t i = 0; i < ts_query_capture_count(query); ++i) {
        //     // TODO Capture it!
        //     // TSNode node = ts_node_make_factory(match.captures[i * 2], match.captures[i * 2 + 1]);  // Simplified; use proper indexing
        //     // const char *capture_name = ts_query_capture_name_for_id(query, i, NULL);
        //     // char *text = ts_node_text(node, source_code);  // Assumes source buffer available
        //     // qDebug() << QString("Capture '%1': %2").arg(capture_name, text);
        //     // free(text);

        //     qDebug() << "id:" << match.id << ", pattern_index: " << match.pattern_index << ", capture_acount: " << match.capture_count;
        // }
    }

    ts_query_cursor_delete(cursor);


}


TSTree* TextDocumentParser::parseString(const char* code, int length)
{
    return ts_parser_parse_string(tsParser_, NULL, code, length);
}

/*
void TextDocumentParser::buildParser()
{
    //textParserRef_ = Edbee::instance()->parserManager()->:w

    tsParser_ = ts_parser_new();

    // try to load the language (TODO: Move this tot the language manager)
    QString language = "ruby";
    qDebug() << "TODO: fix absolute path for  loading parser";
    QString path = QString("/Users/rick/bit/code/edbee-app/edbee-data/parsers/%1").arg(language);
    QString ext = "dylib";
    QString dylib = QString("%1/libtree-sitter-%2.%3").arg(path, language, ext);
    QString highlight_scm = QString("%1/queries/highlights.scm").arg(path);

    const TSLanguage *lang = load_language(qPrintable(dylib), qPrintable(QString("tree_sitter_%1").arg(language)));
    if (!lang) {
        qDebug() << "Couldn't load treestitter language(" << language << "): " << dylib;
        return;
    }

    // Set the parser's language (JSON in this case).
    ts_parser_set_language(tsParser_, lang);

    // Build a syntax tree based on source code stored in a string.
    // const char *source_code = "[1, null]";
    const char *source_code = R"RUBY(
class Test
  def foo
     puts 'foo: ' + "bar"
  end
end
)RUBY";

    tsTree_ = ts_parser_parse_string(
        tsParser_,
        NULL,
        source_code,
        strlen(source_code)


    // Get the root node of the syntax tree.
    TSNode root_node = ts_tree_root_node(tsTree_);


    // Print the syntax tree as an S-expression.
    char *string = ts_node_string(root_node);
    qDebug() << "Syntax tree: " <<  string;

    // ------------------------------------------------ QUERY

    // QUERY: Load the query
    QFile f(highlight_scm);
    f.open(QFile::ReadOnly);
    QString highlightQueryCode = QString::fromUtf8(f.readAll());

    uint32_t error_offset;
    TSQueryError error_type;
    TSQuery *query = ts_query_new(
        lang, // const TSLanguage *language,
        qPrintable(highlightQueryCode),
        highlightQueryCode.length(),
        &error_offset,
        &error_type
    );
    if (!query) {
        qDebug() <<  "Query parse error at: " << error_offset << ": " << error_type;
        return;
    }

    // Execute it
    // ref: <https://blog.pulsar-edit.dev/tag/tree-sitter/>
    // ref: <https://www.perplexity.ai/search/i-m-using-trees-titter-to-exec-CDxmM65VTEWd_KTydkn_Qw>
    TSQueryCursor *cursor = ts_query_cursor_new();
    ts_query_cursor_exec(cursor, query, root_node);

    TSQueryMatch match;
    uint32_t match_count = 0;
    while (ts_query_cursor_next_match(cursor, &match)) {
        qDebug() << "> Match";
        qDebug() << "  * id: " << match.id;
        qDebug() << "  * pattern_index: " << match.pattern_index;
        qDebug() << "  * captures: ";

        for (uint32_t i = 0; i < match.capture_count; i++ ) {
            TSQueryCapture capture = match.captures[i];
            char* nodeString = ts_node_string(capture.node);
            QString text = extract_node_text(capture.node, source_code);

            uint32_t nameLength = 0;
            const char* name = ts_query_capture_name_for_id(query, capture.index, &nameLength);
            QString captureName = QString::fromUtf8(name, nameLength);


            qDebug() << "   - " << i << ": " << capture.index << ": " << nodeString << " => " << text << " : " << captureName;
            free(nodeString);
        }

        // for (uint32_t i = 0; i < ts_query_capture_count(query); ++i) {
        //     // TODO Capture it!
        //     // TSNode node = ts_node_make_factory(match.captures[i * 2], match.captures[i * 2 + 1]);  // Simplified; use proper indexing
        //     // const char *capture_name = ts_query_capture_name_for_id(query, i, NULL);
        //     // char *text = ts_node_text(node, source_code);  // Assumes source buffer available
        //     // qDebug() << QString("Capture '%1': %2").arg(capture_name, text);
        //     // free(text);

        //     qDebug() << "id:" << match.id << ", pattern_index: " << match.pattern_index << ", capture_acount: " << match.capture_count;
        // }
    }

    ts_query_cursor_delete(cursor);
    ts_query_delete(query);



    /// ---------------------------------------/


    // Free all of the heap-allocated memory.
    free(string);


}
* /

//==========================


TextParserGrammar::TextParserGrammar(
    const QString& name,
    const QString& displayName,
    const QString& parserPath,
    const QStringList& extensions,
    const QString& scope,
    const QString& injectionRegExp)
    : name_(name)
    , displayName_(displayName)
    , parserPath_(parserPath)
    , fileExtensions_(extensions)
    , scope_(scope)
    , injectionRegExp_(injectionRegExp)
    , tsLanguage_(nullptr)
{

}

TextParserGrammar::~TextParserGrammar()
{
    // delete all queries
    for (auto it = queries_.begin(); it != queries_.end(); ++it) {
        QString name = it.key();
        TSQuery* query = it.value();
        ts_query_delete(query);
    }
    queries_.clear();

}

/// Loads the parser with it's queries
bool TextParserGrammar::initializeGrammar()
{
    if (initialized_) return true;

    qDebug() << " **** INITIALIZE PARSER GRAMMAR ****";
    qDebug() << name_;

    // load the language
    tsLanguage_ = loadLanguageParser();
    if (!tsLanguage_) {
        return false;
    }


    qDebug() << " >> TODO << Parse all queries";

    // add all queries
    for (auto it = queryPaths_.begin(); it != queryPaths_.end(); ++it) {
        const QString name = it.key();
        const QString path = it.value();
        loadQuery(name, path);
    }

    // QDir dir(QString("%1/queries").arg(parserPath_));
    // QStringList nameFilters{"*.scm"};
    // QList<QFileInfo> files = dir.entryInfoList(nameFilters, QDir::Files | QDir::NoDotAndDotDot);

    // foreach (const QFileInfo& fileInfo, files) {
    //     qDebug() <<  " -> load scm file: " << fileInfo;  // Lists matching filenames (e.g., "example.txt")

    //     TSQuery* query = loadQuery(fileInfo.absoluteFilePath());
    //     if (query) {
    //         qDebug() << " - added: " << fileInfo.baseName();
    //         queries_[fileInfo.baseName()] = query;
    //     }
    // }

    initialized_ = true;

    return true;
}

/// Registers a query path.
/// Thesee queries are loaded when initialize parser is called
void TextParserGrammar::registerQueryPath(const QString& name, const QString& path)
{
    queryPaths_[name] = path;
}

/// Loads a query and registers it for a given name
/// The path is also registered in queryPaths
bool TextParserGrammar::loadQuery(const QString& name, const QString& path)
{
    TSQuery* query = loadQuery(path);
    if (!query) return false;

    if (queries_[name]) {
        qDebug() << "** WARNING replaceing query " << name << " for language " << name_;
        ts_query_delete(queries_[name]);
    }

    qDebug() << " - loaded query " << name << ": " << query;
    queryPaths_[name] = path;
    queries_[name] = query;
    return true;
}

const TSLanguage* TextParserGrammar::language() const
{
    return tsLanguage_;
}

TSQuery *TextParserGrammar::query(const QString& name)
{
    return queries_[name];
}

/// Loads  the language parser.
/// The following structure is used:
/// -
const TSLanguage *TextParserGrammar::loadLanguageParser()
{
    QString ext = "dylib";
    QString dylib = QString("%1/libtree-sitter-%2.%3").arg(parserPath_, name_, ext);
    qDebug() << "Load dylib: " << dylib;
    const TSLanguage* lang = load_language(qPrintable(dylib), qPrintable(QString("tree_sitter_%1").arg(name_)));
    return lang;
}

TSQuery* TextParserGrammar::loadQuery(const QString& path)
{
    QFile f(path);
    f.open(QFile::ReadOnly);
    QString highlightQueryCode = QString::fromUtf8(f.readAll());

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

//==========================


TextParserGrammarManager::TextParserGrammarManager()
{

}

TextParserGrammarManager::~TextParserGrammarManager()
{
    qDeleteAll(grammarMap_);
}

/// Register all parsers in the subdirectory of the given path.
/// It is also possible to directly parse a tree-sitter.json file
///
/// It expects the items in the given root path to be folders, containing TreeSitter parsers
/// Every subfolder should be the language name. Example: "ruby", "json", "lua", etc..
///
/// @param path the to the root of te parsers path you want to register
/// @param eagerLoad should the parsers be initialized on load? (default = false)
void TextParserGrammarManager::registerGrammars(const QString& path, bool eagerLoad)
{
eagerLoad = true; qDebug() << "REMOVE ME!";
    QDir dir(path);

    qDebug() << "LIST ALL PARSERS: ";
    qDebug() << "================";

    // QStringList filters = { "*.tmLanguage", "*.tmLanguage.json" };
    foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name) ) {
        QString language = fileInfo.fileName();
        QString path = fileInfo.absoluteFilePath();
        QString jsonFile = QString("%1/tree-sitter.json").arg(path);

        qDebug() << "TOOD: register jsonFile: " << jsonFile;
        registerGrammarsWithTreeSitterJson(jsonFile, eagerLoad);
        // registerParser(language, path, eagerLoad);
    }
}

/// registers the parsers with the given treesitter.json

bool TextParserGrammarManager::registerGrammarsWithTreeSitterJson(const QString& jsonFileName, bool eagerLoad)
{
    qDebug() << "Move this to the text parser manager";
    qDebug() << " - load all tree-sitter.json files";
    qDebug() << " - extract parsers name from { grammars: { name: }} ";
    qDebug() << " - this way a single treee-sitter.json can contain multiple parser";
    qDebug() << " - still support directory traversal like implemented first";
    QFileInfo jsonFileInfo(jsonFileName);
    QDir basePath = jsonFileInfo.dir();

    JsonParser jsonParser;
    if (!jsonParser.parse(jsonFileName)) {
        qDebug() << "Error parsing" << jsonFileName << ":" << jsonParser.fullErrorMessage();
        return false;
    }

    QVariantMap obj = jsonParser.result().toMap();
    if (obj.isEmpty()) {
        qDebug() << "No valid content found for " << jsonFileName;
        return false;
    }

    qDebug() << "Grammars: ";
    foreach(auto grammarVariant, obj.value("grammars").toList()) {
        QVariantMap grammar = grammarVariant.toMap();
        if (grammar.isEmpty()) continue;

        QString name = grammar["name"].toString();
        QString displayName = grammar["camelcase"].toString();
        QString scope = grammar["scope"].toString();
        QString relativePath = grammar["path"].toString();
        QStringList fileTypes = grammar["file-types"].toStringList();
        QString injectionRegex = grammar["injection-regexp"].toString();
        // QString highlightsScmFile = grammar["highlights"].toString();
        // QString tagsScmFile = grammar["tags"].toString();

        QString parserPath = basePath.absoluteFilePath(relativePath);

        // create the parser
        TextParserGrammar* textParserGrammar = new TextParserGrammar(
            name,
            displayName,
            parserPath,
            fileTypes,
            scope,
            injectionRegex
        );

        // Add all TS Query definitions (.scm files)
        for (auto it = grammar.begin(); it != grammar.end(); ++it) {
            const QString name = it.value().toString();
            if (!name.endsWith(".scm")) continue;

            QString queryPath = basePath.absoluteFilePath(it.value().toString());
            textParserGrammar->registerQueryPath(it.key(), queryPath);
        }

        this->registerGrammar(textParserGrammar, eagerLoad);
    }

    qDebug() << "DONE!";

    return true;
}

/// Registers a treesitter parser.
bool TextParserGrammarManager::registerGrammar(TextParserGrammar* parser, bool eagerLoad)
{
    const QString& name = parser->name();
    if (eagerLoad) {
        parser->initializeGrammar();
    }

    // make  sure we only register a language only once
    if (grammarMap_[name]) {
        qDebug() << "Warning grammar for " << name << " is being replaced.";
        delete grammarMap_[name];
    }

    grammarMap_[name] = parser;
    return parser;
}

/// Return the given text parser explicitly without initialization
TextParserGrammar* TextParserGrammarManager::getWithoutInitialization(const QString& name) const
{
    return grammarMap_[name];
}

/// Returns the given text parsers and makes sure it gets initialized
TextParserGrammar* TextParserGrammarManager::get(const QString& name) const
{
    TextParserGrammar* parser = getWithoutInitialization(name);
    parser->initializeGrammar();
    return parser;
}

*/

} // edbee
