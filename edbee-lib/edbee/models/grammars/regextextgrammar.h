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

namespace edbee {

class RegExp;
class RegexTextGrammar;
class Edbee;


/// defines a single grammar rule
class EDBEE_EXPORT RegexTextGrammarRule {
public:

    /// the instructions
    enum Instruction {
        MainRule,         ///< The main rule has no regexp matches
        RuleList,         ///< A list of rules (no regexp)
        SingleLineRegExp, ///< A single line regexp
        MultiLineRegExp,  ///< A multi-line regexp (begin end)
        IncludeCall,      ///< Includes another scope
        Parser            ///< A full parser (not yet supported, by added as idea for the future). But could be marked by multiple regexps
    };

    RegexTextGrammarRule(RegexTextGrammar* grammar, Instruction instruction);
    ~RegexTextGrammarRule();

    static RegexTextGrammarRule* createMainRule(RegexTextGrammar* grammar, const QString& scopeName);
    static RegexTextGrammarRule* createRuleList(RegexTextGrammar* grammar );
    static RegexTextGrammarRule* createSingleLineRegExp(RegexTextGrammar* grammar, const QString& scopeName, const QString& regExp);
    static RegexTextGrammarRule* createMultiLineRegExp(RegexTextGrammar* grammar, const QString& scopeName, const QString& contentScopeName, const QString& beginRegExp, const QString& endRegExp);
    static RegexTextGrammarRule* createIncludeRule(RegexTextGrammar* grammar, const QString& includeName);

    inline bool isMainRule() { return instruction_ == MainRule; }
    inline bool isRuleList() { return instruction_ == RuleList; }
    inline bool isMultiLineRegExp() { return instruction_ == MultiLineRegExp; }
    inline bool isSingleLineRegExp() { return instruction_ == SingleLineRegExp; }
    inline bool isIncludeCall() { return instruction_ == IncludeCall; }

    qsizetype ruleCount() const { return ruleList_.size(); }
    RegexTextGrammarRule* rule(qsizetype idx) const;
    void giveRule(RegexTextGrammarRule* rule);

    void giveMatchRegExp(RegExp* regExp);
    void setEndRegExpString(const QString& str);

    Instruction instruction() const { return instruction_; }
    void setInstruction(Instruction ins) { instruction_ = ins; }
    QString scopeName() const  { return scopeName_; }
    void setScopeName(const QString& scopeName) { scopeName_ = scopeName; }
    RegExp* matchRegExp() const { return matchRegExp_; }
    QString endRegExpString() const { return endRegExpString_; }
    const QMap<size_t, QString>& matchCaptures() { return matchCaptures_; }
    const QMap<size_t, QString>& endCaptures() { return endCaptures_; }
    QString contentScopeName() const { return contentScopeName_; }
    void setContentScopeName(const QString& name) { contentScopeName_ = name; }

    /// the include name is stored in the content-scopename
    QString includeName() { return contentScopeName_; }
    void setIncludeName(const QString& name) { contentScopeName_ = name; }

    void setCapture(size_t idx, const QString& name) { matchCaptures_.insert(idx, name); }
    void setEndCapture(size_t idx, const QString& name) { endCaptures_.insert(idx, name); }

    QString toString(bool includePatterns=true);


    // An itetor class for iterating over the ruleset (todo template this)
    class Iterator
    {
    public:
        Iterator(const RegexTextGrammarRule* rule) : index_(0), ruleRef_(rule) {}
        bool hasNext() { return index_ < ruleRef_->ruleCount(); }
        RegexTextGrammarRule* next() { return ruleRef_->rule(index_++); }
    private:
        int index_;
        const RegexTextGrammarRule* ruleRef_;
    };

    Iterator* createIterator() { return new Iterator(this); }
    RegexTextGrammar* grammar() { return grammarRef_; }

private:

    static RegExp* createRegExp(const QString& regexp);


private:
    RegexTextGrammar* grammarRef_;        ///< The grammar this rule belongs to
    Instruction instruction_;             ///< The instruction to execute
    QString scopeName_;                   ///< The scope name of this grammar

    RegExp* matchRegExp_;                 ///< The begin-matcher (or simple matcher)
    //RegExp* endRegExp_;                 ///< The end regular expression matcher
    QString endRegExpString_;             ///< The end regexp is a string

    QMap<size_t, QString> matchCaptures_; ///< all captures that need to be performed
    QMap<size_t, QString> endCaptures_;   ///< all end captures that need to be performed

    QString contentScopeName_;            ///< The content scopename

    QList<RegexTextGrammarRule*> ruleList_;    ///< Sub-rules to execute
};

//================================

/// For parsing with regexp like grammars (TextMate Grammar formt)
class EDBEE_EXPORT RegexTextGrammar : public TextGrammar {
public:
    RegexTextGrammar(const QString& name, const QString& displayName);
    virtual ~RegexTextGrammar();

    virtual QString defaultScopeName() const;
    TextLexer* createTextLexer(TextDocumentScopes* scopes);

    void giveMainRule(RegexTextGrammarRule* mainRule);
    RegexTextGrammarRule* mainRule() const;

    void giveToRepos(const QString& name, RegexTextGrammarRule* rule);
    RegexTextGrammarRule* findFromRepos( const QString& name, RegexTextGrammarRule* defValue = nullptr);

private:
    RegexTextGrammarRule *mainRule_;                  ///< the 'main' rule of this grammar
    QMap<QString, RegexTextGrammarRule*> repository_; ///< A map with all named grammar rules
};

} // edbee
