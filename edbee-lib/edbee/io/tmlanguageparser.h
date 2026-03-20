// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QList>
#include <QMap>
#include <QStack>
#include <QVariant>

class QFile;
class QIODevice;
class QXmlStreamReader;

namespace edbee {

class TextRegexGrammar;
class TextGrammarManager;
class TextRegexGrammarRule;

/// For parsing a Textmate Language
class EDBEE_EXPORT TmLanguageParser
{
public:
    TmLanguageParser();
    TextRegexGrammar* parsePlist(QIODevice* device);
    TextRegexGrammar* parseJson(QIODevice* device);

    TextRegexGrammar* parse(QIODevice* device, bool json = false);
    TextRegexGrammar* parse(QFile& file);
    TextRegexGrammar* parse(const QString& fileName);

    QString lastErrorMessage() const;

protected:
    void setLastErrorMessage(const QString& str);

    void addCapturesToGrammarRule(TextRegexGrammarRule* rule, QHash<QString,QVariant> captures, bool endCapture = false);
    void addPatternsToGrammarRule(TextRegexGrammarRule* rule, QList<QVariant> patterns);

    TextRegexGrammarRule* createGrammarRule(TextRegexGrammar* grammar, const QVariant &data);
    TextRegexGrammar* createLanguage(QVariant& data);

private:
    QString lastErrorMessage_;               ///< The last error message
};

} // edbee
