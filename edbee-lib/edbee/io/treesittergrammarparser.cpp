// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "treesittergrammarparser.h"

#include <QDir>


#include "edbee/io/jsonparser.h"
#include "edbee/models/grammars/treesittertextgrammar.h"


#include "edbee/debug.h"

namespace edbee {

TreeSitterGrammarParser::TreeSitterGrammarParser()
{
}


QList<TreeSitterTextGrammar*> TreeSitterGrammarParser::parseTreeSitterJson(const QString& jsonFileName)
{
    QFileInfo jsonFileInfo(jsonFileName);
    QDir basePath = jsonFileInfo.dir();
    QList<TreeSitterTextGrammar*> result;

    JsonParser jsonParser;
    if (!jsonParser.parse(jsonFileName)) {
        qDebug() << "Error parsing" << jsonFileName << ":" << jsonParser.fullErrorMessage();
        return result;
    }

    QVariantMap obj = jsonParser.result().toMap();
    if (obj.isEmpty()) {
        qDebug() << "No valid content found for " << jsonFileName;
        return result;
    }

    foreach (auto grammarVariant, obj.value("grammars").toList()) {
        QVariantMap grammar = grammarVariant.toMap();
        if (grammar.isEmpty()) continue;

        QString name = grammar["name"].toString();
        QString displayName = grammar["camelcase"].toString();
        if (displayName.isEmpty()) { displayName = name; }
        QString scope = grammar["scope"].toString();
        QString relativePath = grammar["path"].toString();
        QStringList fileTypes = grammar["file-types"].toStringList();
        QString injectionRegex = grammar["injection-regexp"].toString();
        // QString highlightsScmFile = grammar["highlights"].toString();
        // QString tagsScmFile = grammar["tags"].toString();

        QString parserPath = basePath.absoluteFilePath(relativePath);

        // create the parser
        TreeSitterTextGrammar* treeSitterGrammar = new TreeSitterTextGrammar(
            QString("%1.ts").arg(name),
            name,
            displayName,
            parserPath,
            scope,
            injectionRegex
            );

        // add all file
        for (const QString& extension : std::as_const(fileTypes)) {
            treeSitterGrammar->addFileExtension(extension);
        }

        // Add all TS Query definitions (.scm files)
        for (auto it = grammar.begin(); it != grammar.end(); ++it) {
            const QString name = it.value().toString();
            if (!name.endsWith(".scm")) continue;

            QString queryPath = basePath.absoluteFilePath(it.value().toString());
            treeSitterGrammar->registerQueryPath(it.key(), queryPath);
        }

        result.append(treeSitterGrammar);
    }
    return result;
}

} // edbee
