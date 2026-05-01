// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textgrammar.h"

#include <QDir>

#include "edbee/models/grammars/regextextgrammar.h"
#include "edbee/io/tmlanguageparser.h"
#include "edbee/io/treesittergrammarparser.h"
#include "edbee/models/grammars/treesittertextgrammar.h"

#include "edbee/debug.h"


namespace edbee {

/// The default texgrammar constructor
/// @param identifier the identifier of the grammar
/// @param name the name of the textgrammar
/// @param displayName th name to display
TextGrammar::TextGrammar(const QString& identifier, const QString &name, const QString& displayName)
    : identifier_(identifier)
    , name_(name)
    , displayName_(displayName)
{
}

/// The default texgrammar constructor
/// @param identifier the identifier of the grammar
/// @param name the name of the textgrammar
/// @param displayName th name to display
TextGrammar::TextGrammar(const QString& name, const QString& displayName)
    : identifier_(name)
    , name_(name)
    , displayName_(displayName)
{
}

TextGrammar::~TextGrammar()
{
}


/// returns the identifier of the grammar
QString TextGrammar::identifier() const
{
    return identifier_;
}


/// return the name of the textgrammar
QString TextGrammar::name() const
{
    return name_;
}


/// Returns the displayname of this grammar
QString TextGrammar::displayName() const
{
    return displayName_;
}

/// Returns all file extensions that are used by this grammar
/// @return a stringlist with all extension (without the '.')
QStringList TextGrammar::fileExtensions() const
{
    return fileExtensions_;
}

/// Adds a file extension
/// @param ext the extension to add.
void TextGrammar::addFileExtension(const QString& ext)
{
    fileExtensions_.append(ext);
}

//==========================


/// The text grammar manager constructor
TextGrammarManager::TextGrammarManager()
    : defaultGrammarRef_(nullptr)
{

    // always make sure there's a default grammar
    RegexTextGrammar* grammar = new RegexTextGrammar("text.plain", "Plain Text");
    grammar->giveMainRule(RegexTextGrammarRule::createMainRule(grammar, "text.plain"));

    defaultGrammarRef_ = grammar;
    giveGrammar(defaultGrammarRef_);
}


/// The detstructor (deletes all grammars)
TextGrammarManager::~TextGrammarManager()
{
    qDeleteAll(grammarMap_);
    grammarMap_.clear();
}


/// This method reads the given grammar file and adds it to the grammar manager.
/// The grammar manager stays the owner of the grammar file
///
/// @param filename the direct filename to read
/// @return the TextGrammar file. When an error happend, the errorMessage is set
RegexTextGrammar* TextGrammarManager::readGrammarFile(const QString& file)
{
    qDebug() << "DEPRECATED: readGrammarFile is deprecated, please use readRegexGrammarFile instead.";
    return readRegexGrammarFile(file);
}

/// This method reads the given regex grammar file and adds it to the grammar manager.
/// The grammar manager stays the owner of the grammar file
///
/// @param filename the direct filename to read
/// @return the TextGrammar file. When an error happend, the errorMessage is set
RegexTextGrammar* TextGrammarManager::readRegexGrammarFile(const QString& file)
{
    lastErrorMessage_.clear();

    RegexTextGrammar* grammar = nullptr;
    TmLanguageParser parser;

    grammar = parser.parse(file);
    if (grammar) {
        giveGrammar(grammar);
    } else {
        QFileInfo fileInfo(file);
        lastErrorMessage_ = QObject::tr("Error reading file %1:%2").arg(fileInfo.absoluteFilePath(), parser.lastErrorMessage());
        qlog_warn() << lastErrorMessage_;
    }
    return grammar;
}


/// reads all grammar files in the given path
/// @param path the path to read all grammar files from
/// @deprecated use readAllRegexGrammarFilesInPath
void TextGrammarManager::readAllGrammarFilesInPath(const QString& path)
{
    qDebug() << "DEPRECATED: readGrammarFilesInPath is deprecated, please use readAllRegexgrammarFilesInPath instead.";
    readAllRegexGrammarFilesInPath(path);
}

/// reads all regex grammar files in the given pathh
/// @param path the path to read all grammar files from
void TextGrammarManager::readAllRegexGrammarFilesInPath(const QString& path)
{
    QDir dir(path);
    qDebug() << "read all grammar files !!: " << path;
    QStringList filters = { "*.tmLanguage", "*.tmLanguage.json" };
    foreach (QFileInfo fileInfo, dir.entryInfoList( filters, QDir::Files, QDir::Name) ) {
        readRegexGrammarFile(fileInfo.absoluteFilePath());
    }
}

bool TextGrammarManager::readTreeSitterGrammarFile(const QString& file)
{
    TreeSitterGrammarParser grammarParser;

    qDebug() << " - read " << file;
    QList<TreeSitterTextGrammar*> grammars = grammarParser.parseTreeSitterJson(file);
    for(TreeSitterTextGrammar* grammar : std::as_const(grammars)) {
        qDebug() << "ADD GRAMMAR" << grammar->name();
        giveGrammar(grammar);
    }
    return grammars.length() > 0;
}

/// Reads all treesitter parser from a given directory
/// It readys every tree-sitter.json from its subfolders
void TextGrammarManager::readAllTreeSitterGrammarFilesInPath(const QString& path)
{
    QDir dir(path);

    qDebug() << "LIST ALL PARSERS: ";
    qDebug() << "================";

    // QStringList filters = { "*.tmLanguage", "*.tmLanguage.json" };
    foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name) ) {
        QString language = fileInfo.fileName();
        QString path = fileInfo.absoluteFilePath();
        QString jsonFile = QString("%1/tree-sitter.json").arg(path);

        readTreeSitterGrammarFile(jsonFile);
       //  registerParser(language, path, eagerLoad);
    }
}


/// This method returns the given language grammar
TextGrammar* TextGrammarManager::get(const QString& name)
{
    return grammarMap_.value(name, nullptr);
}

/// Returns te regexp variant of this language grammar
RegexTextGrammar* TextGrammarManager::getRegexGrammar(const QString& name)
{
    return dynamic_cast<RegexTextGrammar*>(get(name));
}


/// Gives a language grammar to the document
/// @param grammar the grammar to give
void TextGrammarManager::giveGrammar(TextGrammar* grammar)
{
    const QString identifier = grammar->identifier();

    // when the grammar already exists delete it
    if (grammarMap_.contains(identifier)) {
        TextGrammar* oldGrammar = grammarMap_.take(identifier);

        // when the old grammar was the default, replace the default (feels pretty dirty)
        if (defaultGrammarRef_ == oldGrammar) {
            defaultGrammarRef_ = grammar;
        }

        // clearup the old one
        delete oldGrammar;
    }
    grammarMap_.insert(identifier, grammar);
}


/// Returns all grammar names
QList<QString> TextGrammarManager::grammarNames()
{
    return grammarMap_.keys();
}


/// Returns the values
QList<TextGrammar*> TextGrammarManager::grammars()
{
    return grammarMap_.values();
}


/// ompares the grammarnames of textgrammars
/// @param g1 the first grammar
/// @param g2 the second grammar to compare
static bool grammarsDisplayNameSorterLessThen(const TextGrammar* g1, const TextGrammar* g2)
{
    return g1->displayName().toLower() < g2->displayName().toLower();
}


/// Returns all grammars sorted on displayname
/// Warning, this method sorts the grammars so calling this method multiple times is not what you want to do
/// @return the list of grammars sorted on displayname
QList<TextGrammar*> TextGrammarManager::grammarsSortedByDisplayName()
{
    QList<TextGrammar*> results = grammarMap_.values();
    std::sort(results.begin(), results.end(), grammarsDisplayNameSorterLessThen);
    return results;
}


/// this method detects the grammar with a given filename
/// @param fileName the filename to detect the grammar
/// @return the defaultGrammar if no grammar was found
TextGrammar* TextGrammarManager::detectGrammarWithFilename(const QString& fileName)
{
    auto grammarMapValues = grammarMap_.values();
    foreach (TextGrammar* grammar, grammarMapValues) {
        foreach (QString ext, grammar->fileExtensions()) {
            if (fileName.endsWith( QStringLiteral(".%1").arg(ext))) return grammar;
        }
    }
    return this->defaultGrammar();
}


/// returns the grammar manager
/// @return the last error message
QString TextGrammarManager::lastErrorMessage() const
{
    return lastErrorMessage_;
}


} // edbee
