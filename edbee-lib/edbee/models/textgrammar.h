// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
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

class Edbee;
class RegExp;
class RegexTextGrammar;
class TextLexer;
class TextDocument;
class TextDocumentScopes;
class TreeSitterTextGrammar;

/// This class defines a single language grammar
class EDBEE_EXPORT TextGrammar {
public:

    TextGrammar(const QString& name, const QString& displayName);
    TextGrammar(const QString& identifier, const QString& name, const QString& displayName);
    virtual ~TextGrammar();

    /// Allows a JIT construction of grammars.
    /// Initialize is called if the grammar is used for the first time
    virtual bool initialize() { return true; }

    QString identifier() const;  /// The identifier of the language.
    QString name() const;
    QString displayName() const;
    QStringList fileExtensions() const;

    void addFileExtension(const QString& ext);

    virtual QString defaultScopeName() const = 0;
    virtual TextLexer* createTextLexer(TextDocument* document) = 0;

protected:
    QString identifier_;    					 ///< the grammar identifier (uniqely identify this between TextMate and TreeSitter Grammars)
    QString name_;                               ///< the name of this language (seems to be source.ruby, etc..)
    QString displayName_;                        ///< the name to display
    QStringList fileExtensions_;                 ///< A list with all file-extensions
};


//================================


/// This class is used to manage all 'grammers' used by the lexers
class EDBEE_EXPORT TextGrammarManager {
protected:
    TextGrammarManager();
    virtual ~TextGrammarManager();

public:
    Q_DECL_DEPRECATED_X("Use readRegexGrammarFile() instead")
    RegexTextGrammar* readGrammarFile(const QString& file);
    RegexTextGrammar* readRegexGrammarFile(const QString& file);

    Q_DECL_DEPRECATED_X("Use readAllRegexGrammarFilesInPath() instead")
    void readAllGrammarFilesInPath(const QString& path);
    void readAllRegexGrammarFilesInPath(const QString& path);

    bool readTreeSitterGrammarFile(const QString& file);
    void readAllTreeSitterGrammarFilesInPath(const QString& path);

    TextGrammar* get(const QString& name);
    RegexTextGrammar* getRegexGrammar(const QString& name); //< TODO remove this later

    void giveGrammar(TextGrammar* grammar);

    QList<QString> grammarNames();
    QList<TextGrammar*> grammars();
    QList<TextGrammar*> grammarsSortedByDisplayName();

    TextGrammar* defaultGrammar() { return defaultGrammarRef_; }
    TextGrammar* detectGrammarWithFilename(const QString& fileName);

    QString lastErrorMessage() const;

private:

    TextGrammar* defaultGrammarRef_;                   ///< A reference to the default grammar
    QMap<QString,TextGrammar*> grammarMap_;            ///< A map with all grammar definitions
    QString lastErrorMessage_;                             ///< Returns the error message

    friend class Edbee;
};

} // edbee
