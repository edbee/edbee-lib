// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "regextextgrammar.h"

#include <QDir>

#include "edbee/lexers/regextextlexer.h"
#include "edbee/io/tmlanguageparser.h"
#include "edbee/util/regexp.h"

#include "edbee/debug.h"

namespace edbee {

/// The text grammar rule constructor
/// @param grammar the grammar this rule belongs to
/// @param instruction the type of instruction this is
RegexTextGrammarRule::RegexTextGrammarRule(RegexTextGrammar* grammar, Instruction instruction)
    : grammarRef_(grammar)
    , instruction_(instruction)
    , matchRegExp_(nullptr)
    , endRegExpString_()
{
}


/// The tex text grammar rule
RegexTextGrammarRule::~RegexTextGrammarRule()
{
    qDeleteAll(ruleList_);
    ruleList_.clear();
    delete matchRegExp_;
}


/// Creates a main grammar rule
/// @param grammar the grammar this main rule belongs to
/// @param scopeName the name of the scope
/// @return the main grammar rule
RegexTextGrammarRule* RegexTextGrammarRule::createMainRule(RegexTextGrammar* grammar, const QString& scopeName)
{
    RegexTextGrammarRule* rule = new RegexTextGrammarRule(grammar, MainRule);
    rule->setScopeName(scopeName);
    return rule;
}


/// Creates a grammar rule list
/// @param grammar the grammar this rule list belongs to
/// @return the TextGrammarRule that include the rule list
RegexTextGrammarRule* RegexTextGrammarRule::createRuleList(RegexTextGrammar* grammar)
{
    RegexTextGrammarRule* rule = new RegexTextGrammarRule(grammar, RuleList);
    return rule;
}


/// Creates an include rule
/// @param grammar the grammar this rules belongs to
/// @param includeName the name to include
/// @return an include grammar rule
RegexTextGrammarRule* RegexTextGrammarRule::createIncludeRule(RegexTextGrammar* grammar, const QString& includeName)
{
    RegexTextGrammarRule* rule = new RegexTextGrammarRule(grammar, IncludeCall);
    rule->setIncludeName(includeName);
    return rule;
}


/// Creates an single line regexp
/// @param grammar the grammar this rule belongs to
/// @param scopeName the scopename of this rule
/// @param regExp the regular expression used for this rule
/// @return the created grammar rule
RegexTextGrammarRule* RegexTextGrammarRule::createSingleLineRegExp(RegexTextGrammar* grammar, const QString& scopeName, const QString& regExp)
{
    RegexTextGrammarRule* rule = new RegexTextGrammarRule(grammar, SingleLineRegExp);
    rule->setScopeName(scopeName);
    rule->giveMatchRegExp(RegexTextGrammarRule::createRegExp(regExp));
    return rule;
}


/// Creates a multiline regexp
/// @param scopeName the name of the scope
/// @param contentScopeName the scope name of the content
/// @param beginRegExp the start regexp to use
/// @param endRegExp the end regular expression to use
/// @return TextGrammarRule the multiple grammar rule
RegexTextGrammarRule* RegexTextGrammarRule::createMultiLineRegExp(RegexTextGrammar* grammar, const QString& scopeName, const QString& contentScopeName, const QString& beginRegExp, const QString& endRegExp)
{
    RegexTextGrammarRule* rule = new RegexTextGrammarRule(grammar, MultiLineRegExp);
    rule->setScopeName(scopeName);
    rule->setContentScopeName(contentScopeName);
    rule->giveMatchRegExp(RegexTextGrammarRule::createRegExp(beginRegExp));
    rule->setEndRegExpString(endRegExp);
    return rule;
}


/// returns the child rule at the given index
/// @param idx the index of the grammar rule
RegexTextGrammarRule* RegexTextGrammarRule::rule(qsizetype idx) const
{
    Q_ASSERT( 0 <= idx && idx < ruleCount());
    return ruleList_.at(idx);
}


/// Gives the rulle to the rule-list of the grammar rule
/// @param the rule to give (ownership is transfered to this object)
void RegexTextGrammarRule::giveRule(RegexTextGrammarRule* rule)
{
    ruleList_.append(rule);
}


/// Gives the main regular expression
/// @param regExp the regular expression to give
void RegexTextGrammarRule::giveMatchRegExp(RegExp* regExp)
{
    matchRegExp_ = regExp;
}


/// Sets the ends regular expression(only for multi-line regexp rules
/// @param str the end regular expression
void RegexTextGrammarRule::setEndRegExpString(const QString& str)
{
    endRegExpString_ = str;
}


/// convers the curent object toString (for debugging purposes)
/// @param includePatters should the patterns be included
QString RegexTextGrammarRule::toString(bool includePatterns)
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
RegExp* RegexTextGrammarRule::createRegExp(const QString& regexp)
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
RegexTextGrammar::RegexTextGrammar(const QString& name, const QString& displayName)
    : TextGrammar(name, displayName)
    , mainRule_(nullptr)
{
}


RegexTextGrammar::~RegexTextGrammar()
{
    qDeleteAll(repository_);
    repository_.clear();
    delete mainRule_;
}

/// The default scope to use for this lanuage
QString RegexTextGrammar::defaultScopeName() const
{
    return mainRule_->scopeName();
}


/// Creates a text lexer for this grammar
TextLexer* RegexTextGrammar::createTextLexer(TextDocument* document)
{
    return new RegexTextLexer(this, document);
}


/// Gives the main rule the grammmar
void RegexTextGrammar::giveMainRule(RegexTextGrammarRule* mainRule)
{
    Q_ASSERT(!mainRule_);
    mainRule_ = mainRule;
}

/// Returns the main grammar rule for this textgrammar
RegexTextGrammarRule* RegexTextGrammar::mainRule() const
{
    return mainRule_;
}

/// Adds the given grammar rule to the repos
void RegexTextGrammar::giveToRepos(const QString& name, RegexTextGrammarRule* rule)
{
    repository_.insert(name, rule);
}


/// Finds the grammar rule from the respos
/// @param name the name of the rule
/// @param defValue the grammar rule to return if not found
/// @return the found grammar rule (or the defValue if not found)
RegexTextGrammarRule* RegexTextGrammar::findFromRepos(const QString& name, RegexTextGrammarRule* defValue)
{
    return repository_.value(name, defValue);
}


} // edbee
