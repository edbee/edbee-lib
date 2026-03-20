// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QHash>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

class QFile;

namespace edbee {

class RegExp;
class TextRegexGrammar;
class Edbee;


/// defines a single grammar rule
class EDBEE_EXPORT TextRegexGrammarRule {
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

    TextRegexGrammarRule(TextRegexGrammar* grammar, Instruction instruction);
    ~TextRegexGrammarRule();

    static TextRegexGrammarRule* createMainRule(TextRegexGrammar* grammar, const QString& scopeName);
    static TextRegexGrammarRule* createRuleList(TextRegexGrammar* grammar );
    static TextRegexGrammarRule* createSingleLineRegExp(TextRegexGrammar* grammar, const QString& scopeName, const QString& regExp);
    static TextRegexGrammarRule* createMultiLineRegExp(TextRegexGrammar* grammar, const QString& scopeName, const QString& contentScopeName, const QString& beginRegExp, const QString& endRegExp);
    static TextRegexGrammarRule* createIncludeRule(TextRegexGrammar* grammar, const QString& includeName);

    inline bool isMainRule() { return instruction_ == MainRule; }
    inline bool isRuleList() { return instruction_ == RuleList; }
    inline bool isMultiLineRegExp() { return instruction_ == MultiLineRegExp; }
    inline bool isSingleLineRegExp() { return instruction_ == SingleLineRegExp; }
    inline bool isIncludeCall() { return instruction_ == IncludeCall; }

    qsizetype ruleCount() const { return ruleList_.size(); }
    TextRegexGrammarRule* rule(qsizetype idx) const;
    void giveRule(TextRegexGrammarRule* rule);

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
        Iterator(const TextRegexGrammarRule* rule) : index_(0), ruleRef_(rule) {}
        bool hasNext() { return index_ < ruleRef_->ruleCount(); }
        TextRegexGrammarRule* next() { return ruleRef_->rule(index_++); }
    private:
        int index_;
        const TextRegexGrammarRule* ruleRef_;
    };

    Iterator* createIterator() { return new Iterator(this); }
    TextRegexGrammar* grammar() { return grammarRef_; }

private:

    static RegExp* createRegExp(const QString& regexp);


private:
    TextRegexGrammar* grammarRef_;        ///< The grammar this rule belongs to
    Instruction instruction_;             ///< The instruction to execute
    QString scopeName_;                   ///< The scope name of this grammar

    RegExp* matchRegExp_;                 ///< The begin-matcher (or simple matcher)
    //RegExp* endRegExp_;                 ///< The end regular expression matcher
    QString endRegExpString_;             ///< The end regexp is a string

    QMap<size_t, QString> matchCaptures_; ///< all captures that need to be performed
    QMap<size_t, QString> endCaptures_;   ///< all end captures that need to be performed

    QString contentScopeName_;            ///< The content scopename

    QList<TextRegexGrammarRule*> ruleList_;    ///< Sub-rules to execute
};


//================================


/// This class defines a single language grammar
class EDBEE_EXPORT TextGrammar {
public:

    TextGrammar(const QString& name, const QString& displayName);
    virtual ~TextGrammar();

    QString name() const;
    QString displayName() const;
    QStringList fileExtensions() const;

    void addFileExtension(const QString& ext);

    virtual QString defaultScopeName() const = 0;

private:
    QString name_;                               ///< the display name of this
    QString displayName_;                        ///< the name to display
    QStringList fileExtensions_;                 ///< A list with all file-extensions
};


//================================

/// For parsing with regexp like grammars (TextMate Grammar formt)
class EDBEE_EXPORT TextRegexGrammar : public TextGrammar {
public:
    TextRegexGrammar(const QString& name, const QString& displayName);
    virtual ~TextRegexGrammar();

    virtual QString defaultScopeName() const;

    void giveMainRule(TextRegexGrammarRule* mainRule);
    TextRegexGrammarRule* mainRule() const;

    void giveToRepos(const QString& name, TextRegexGrammarRule* rule);
    TextRegexGrammarRule* findFromRepos( const QString& name, TextRegexGrammarRule* defValue = nullptr);
private:
    TextRegexGrammarRule *mainRule_;                  ///< the 'main' rule of this grammar
    QMap<QString, TextRegexGrammarRule*> repository_; ///< A map with all named grammar rules
};


//================================


/// This class is used to manage all 'grammers' used by the lexers
class EDBEE_EXPORT TextGrammarManager {
protected:
    TextGrammarManager();
    virtual ~TextGrammarManager();

public:
    TextRegexGrammar* readGrammarFile(const QString& file);
    void readAllGrammarFilesInPath(const QString& path);

    TextGrammar* get(const QString& name);
    TextRegexGrammar* getRegexGrammar(const QString& name); //< TODO remove this later

    void giveGrammar(TextGrammar* grammar);

    QList<QString> grammarNames();
    QList<TextGrammar*> grammars();
    QList<TextGrammar*> grammarsSortedByDisplayName();

    TextGrammar* defaultGrammar() { return defaultGrammarRef_; }
    TextGrammar* detectGrammarWithFilename( const QString& fileName );

    QString lastErrorMessage() const;

private:

    TextGrammar* defaultGrammarRef_;                   ///< A reference to the default grammar
    QMap<QString,TextGrammar*> grammarMap_;            ///< A map with all grammar definitions
    QString lastErrorMessage_;                             ///< Returns the error message

    friend class Edbee;
};

} // edbee
