// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textgrammar.h"

#include <QDir>

#include "edbee/io/tmlanguageparser.h"
#include "edbee/util/regexp.h"

#include "edbee/debug.h"

namespace edbee {


/// The text grammar rule constructor
/// @param grammar the grammar this rule belongs to
/// @param instruction the type of instruction this is
TextRegexGrammarRule::TextRegexGrammarRule(TextRegexGrammar* grammar, Instruction instruction)
    : grammarRef_(grammar)
    , instruction_(instruction)
    , matchRegExp_(nullptr)
    , endRegExpString_()
{
}


/// The tex text grammar rule
TextRegexGrammarRule::~TextRegexGrammarRule()
{
    qDeleteAll(ruleList_);
    ruleList_.clear();
    delete matchRegExp_;
}


/// Creates a main grammar rule
/// @param grammar the grammar this main rule belongs to
/// @param scopeName the name of the scope
/// @return the main grammar rule
TextRegexGrammarRule* TextRegexGrammarRule::createMainRule(TextRegexGrammar* grammar, const QString& scopeName)
{
    TextRegexGrammarRule* rule = new TextRegexGrammarRule(grammar, MainRule);
    rule->setScopeName(scopeName);
    return rule;
}


/// Creates a grammar rule list
/// @param grammar the grammar this rule list belongs to
/// @return the TextGrammarRule that include the rule list
TextRegexGrammarRule* TextRegexGrammarRule::createRuleList(TextRegexGrammar* grammar)
{
    TextRegexGrammarRule* rule = new TextRegexGrammarRule(grammar, RuleList);
    return rule;
}


/// Creates an include rule
/// @param grammar the grammar this rules belongs to
/// @param includeName the name to include
/// @return an include grammar rule
TextRegexGrammarRule* TextRegexGrammarRule::createIncludeRule(TextRegexGrammar* grammar, const QString& includeName)
{
    TextRegexGrammarRule* rule = new TextRegexGrammarRule(grammar, IncludeCall);
    rule->setIncludeName(includeName);
    return rule;
}


/// Creates an single line regexp
/// @param grammar the grammar this rule belongs to
/// @param scopeName the scopename of this rule
/// @param regExp the regular expression used for this rule
/// @return the created grammar rule
TextRegexGrammarRule* TextRegexGrammarRule::createSingleLineRegExp(TextRegexGrammar* grammar, const QString& scopeName, const QString& regExp)
{
    TextRegexGrammarRule* rule = new TextRegexGrammarRule(grammar, SingleLineRegExp);
    rule->setScopeName(scopeName);
    rule->giveMatchRegExp(TextRegexGrammarRule::createRegExp(regExp));
    return rule;
}


/// Creates a multiline regexp
/// @param scopeName the name of the scope
/// @param contentScopeName the scope name of the content
/// @param beginRegExp the start regexp to use
/// @param endRegExp the end regular expression to use
/// @return TextGrammarRule the multiple grammar rule
TextRegexGrammarRule* TextRegexGrammarRule::createMultiLineRegExp(TextRegexGrammar* grammar, const QString& scopeName, const QString& contentScopeName, const QString& beginRegExp, const QString& endRegExp)
{
    TextRegexGrammarRule* rule = new TextRegexGrammarRule(grammar, MultiLineRegExp);
    rule->setScopeName(scopeName);
    rule->setContentScopeName(contentScopeName);
    rule->giveMatchRegExp(TextRegexGrammarRule::createRegExp(beginRegExp));
    rule->setEndRegExpString(endRegExp);
    return rule;
}


/// returns the child rule at the given index
/// @param idx the index of the grammar rule
TextRegexGrammarRule* TextRegexGrammarRule::rule(qsizetype idx) const
{
    Q_ASSERT( 0 <= idx && idx < ruleCount());
    return ruleList_.at(idx);
}


/// Gives the rulle to the rule-list of the grammar rule
/// @param the rule to give (ownership is transfered to this object)
void TextRegexGrammarRule::giveRule(TextRegexGrammarRule* rule)
{
    ruleList_.append(rule);
}


/// Gives the main regular expression
/// @param regExp the regular expression to give
void TextRegexGrammarRule::giveMatchRegExp(RegExp* regExp)
{
    matchRegExp_ = regExp;
}


/// Sets the ends regular expression(only for multi-line regexp rules
/// @param str the end regular expression
void TextRegexGrammarRule::setEndRegExpString(const QString& str)
{
    endRegExpString_ = str;
}


/// convers the curent object toString (for debugging purposes)
/// @param includePatters should the patterns be included
QString TextRegexGrammarRule::toString(bool includePatterns)
{
    QString r;
    switch (instruction_) {
        case MainRule: r.append("MainRule"); break;
        case RuleList: r.append("RuleList"); break;
        case SingleLineRegExp: r.append("SingleLineRegExp"); break;
        case MultiLineRegExp: r.append("MultiLineRegExp"); break;
        case IncludeCall: r.append("IncludeCall");break;
        case Parser: r.append("Parser"); break;
        default: r.append("Unkown");
    }

    r.append(":").append(scopeName_);
    if (instruction_ == IncludeCall) {
        r.append(" ");
        r.append(includeName());
    }

    if (includePatterns && matchRegExp_) { r.append(", begin: ").append(matchRegExp_->pattern()); }
    if (includePatterns && !endRegExpString_.isEmpty()) { r.append(", end: ").append(endRegExpString_); }
    r.append(QStringLiteral(", %1 subrules").arg(ruleCount()));
    r.append(QStringLiteral(", %1 captures").arg(matchCaptures().size()));
    return r;
}


/// parses the given string as a regexp
/// @param regexp the regular expression string to create a regexp from
/// @return the RegExp object
RegExp* TextRegexGrammarRule::createRegExp(const QString& regexp)
{
    RegExp* result = new RegExp(regexp, RegExp::EngineOniguruma); //, Qt::CaseSensitive, QRegExp::RegExp2 );
    if (!result->isValid()) {
        qlog_warn() << "Error in regexp: " << result->errorString() << "\n" << regexp;
    }
    return result;
}

//==========================

/// The default texgrammar constructor
/// @param name the name of the textgrammar
/// @param displayName th name to display
TextGrammar::TextGrammar(const QString& name, const QString& displayName)
    : name_(name)
    , displayName_(displayName)
{
}


TextGrammar::~TextGrammar()
{
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


/// The default texgrammar constructor
/// @param name the name of the textgrammar
/// @param displayName th name to display
TextRegexGrammar::TextRegexGrammar(const QString& name, const QString& displayName)
    : TextGrammar(name, displayName)
    , mainRule_(nullptr)
{
}


TextRegexGrammar::~TextRegexGrammar()
{
    qDeleteAll(repository_);
    repository_.clear();
    delete mainRule_;
}

/// The default scope to use for this lanuage
QString TextRegexGrammar::defaultScopeName() const
{
    return mainRule_->scopeName();
}


/// Gives the main rule the grammmar
void TextRegexGrammar::giveMainRule(TextRegexGrammarRule* mainRule)
{
    Q_ASSERT(!mainRule_);
    mainRule_ = mainRule;
}

/// Returns the main grammar rule for this textgrammar
TextRegexGrammarRule* TextRegexGrammar::mainRule() const
{
    return mainRule_;
}

/// Adds the given grammar rule to the repos
void TextRegexGrammar::giveToRepos(const QString& name, TextRegexGrammarRule* rule)
{
    repository_.insert(name, rule);
}


/// Finds the grammar rule from the respos
/// @param name the name of the rule
/// @param defValue the grammar rule to return if not found
/// @return the found grammar rule (or the defValue if not found)
TextRegexGrammarRule* TextRegexGrammar::findFromRepos(const QString& name, TextRegexGrammarRule* defValue)
{
    return repository_.value(name, defValue);
}


//==========================


/// The text grammar manager constructor
TextGrammarManager::TextGrammarManager()
    : defaultGrammarRef_(nullptr)
{

    // always make sure there's a default grammar
    TextRegexGrammar* grammar = new TextRegexGrammar("text.plain", "Plain Text");
    grammar->giveMainRule(TextRegexGrammarRule::createMainRule(grammar, "text.plain"));

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
TextRegexGrammar* TextGrammarManager::readGrammarFile(const QString& file)
{
    lastErrorMessage_.clear();

    TextRegexGrammar* grammar = nullptr;
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
void TextGrammarManager::readAllGrammarFilesInPath(const QString& path)
{
    QDir dir(path);
    QStringList filters = { "*.tmLanguage", "*.tmLanguage.json" };
    foreach (QFileInfo fileInfo, dir.entryInfoList( filters, QDir::Files, QDir::Name) ) {
        readGrammarFile(fileInfo.absoluteFilePath());
    }
}


/// This method returns the given language grammar
TextGrammar* TextGrammarManager::get(const QString& name)
{
    return grammarMap_.value(name, nullptr);
}

/// Returns te regexp variant of this language grammar
TextRegexGrammar* TextGrammarManager::getRegexGrammar(const QString& name)
{
    return dynamic_cast<TextRegexGrammar*>(get(name));
}


/// Gives a language grammar to the document
/// @param grammar the grammar to give
void TextGrammarManager::giveGrammar(TextGrammar* grammar)
{
    const QString name = grammar->name();

    // when the grammar already exists delete it
    if (grammarMap_.contains(name)) {
        TextGrammar* oldGrammar = grammarMap_.take(name);

        // when the old grammar was the default, replace the default (feels pretty dirty)
        if (defaultGrammarRef_ == oldGrammar) {
            defaultGrammarRef_ = grammar;
        }

        // clearup the old one
        delete oldGrammar;
    }
    grammarMap_.insert(name, grammar);
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
