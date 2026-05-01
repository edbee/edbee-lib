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

class RegexTextGrammar;
class TextGrammarManager;
class RegexTextGrammarRule;

/// For parsing a Textmate Language
class EDBEE_EXPORT TmLanguageParser
{
public:
    TmLanguageParser();
    RegexTextGrammar* parsePlist(QIODevice* device);
    RegexTextGrammar* parseJson(QIODevice* device);

    RegexTextGrammar* parse(QIODevice* device, bool json = false);
    RegexTextGrammar* parse(QFile& file);
    RegexTextGrammar* parse(const QString& fileName);

    QString lastErrorMessage() const;

protected:
    void setLastErrorMessage(const QString& str);

    void addCapturesToGrammarRule(RegexTextGrammarRule* rule, QHash<QString,QVariant> captures, bool endCapture = false);
    void addPatternsToGrammarRule(RegexTextGrammarRule* rule, QList<QVariant> patterns);

    RegexTextGrammarRule* createGrammarRule(RegexTextGrammar* grammar, const QVariant &data);
    RegexTextGrammar* createLanguage(QVariant& data);

private:
    QString lastErrorMessage_;               ///< The last error message
};

} // edbee
