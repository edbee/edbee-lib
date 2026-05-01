// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "tmlanguageparser.h"

#include <QDir>
#include <QFile>
#include <QList>
#include <QHash>
#include <QVariant>
#include <QXmlStreamReader>

#include "edbee/io/baseplistparser.h"
#include "edbee/io/jsonparser.h"
#include "edbee/models/grammars/regextextgrammar.h"

#include "edbee/debug.h"

namespace edbee {

/// Parses a textmate language
TmLanguageParser::TmLanguageParser()
{
}


/// returns the last error message
QString TmLanguageParser::lastErrorMessage() const
{
    return lastErrorMessage_;
}


/// Sets the last error message
void TmLanguageParser::setLastErrorMessage(const QString& str)
{
    lastErrorMessage_ = str;
}


/// Parses a PList  (XML Grammar file definition)
RegexTextGrammar* TmLanguageParser::parsePlist(QIODevice* device)
{
    RegexTextGrammar* result = nullptr;

    BasePListParser plistParser;
    if (plistParser.beginParsing(device)) {
        QVariant plist = plistParser.readNextPlistType();
        result = createLanguage(plist);
    }

    if (!plistParser.endParsing()) {
        delete result;
        result = nullptr;
    }

    // returns the language
    return result;
}


/// Parses a JSON grammar file definition
RegexTextGrammar *TmLanguageParser::parseJson(QIODevice* device)
{
    RegexTextGrammar* result = nullptr;

    JsonParser jsonParser;
    if (jsonParser.parse(device)) {
        QVariant parseResult = jsonParser.result();
        result = createLanguage(parseResult);
    }

    return result;
}


/// reads the content of a single file
/// @param device the device to read from. The device NEEDS to be open!!
/// @param json use the json parser
/// @return the language grammar or nullptr on error
RegexTextGrammar* TmLanguageParser::parse(QIODevice* device, bool json)
{
    if( json ) {
        return parseJson(device);
    } else {
        return parsePlist(device);
    }

}

/// Parses the given file
RegexTextGrammar *TmLanguageParser::parse(QFile &file)
{
    if (file.open(QIODevice::ReadOnly)) {
        RegexTextGrammar* result = parse(&file, file.fileName().endsWith(".json"));
        file.close();
        return result;
    } else {
        setLastErrorMessage(file.errorString());
        return nullptr;
    }

}


/// parses the given grammar file
/// @param file the file to read
/// @return the language grammar or 0 on error
RegexTextGrammar* TmLanguageParser::parse(const QString& fileName)
{
    QFile file(fileName);
    return parse(file);
}


/// sets the captures
void TmLanguageParser::addCapturesToGrammarRule(RegexTextGrammarRule* rule, QHash<QString, QVariant> captures, bool endCapture)
{
    if (captures.isEmpty()) { return; }

    QHashIterator<QString,QVariant> itr(captures);
    while (itr.hasNext()) {
        itr.next();
        QHash<QString,QVariant> fields = itr.value().toHash();
        ptrdiff_t sKeyIndex = itr.key().toInt();
        Q_ASSERT(sKeyIndex >= 0);
        size_t keyIndex = static_cast<size_t>(sKeyIndex);

        QString name = fields.value("name").toString();
        if (endCapture) {
            rule->setEndCapture(keyIndex, name);
        } else {
            rule->setCapture(keyIndex, name);
        }
    }
}


/// Adds all patterns to the grammar rules
void TmLanguageParser::addPatternsToGrammarRule(RegexTextGrammarRule* rule, QList<QVariant> patterns)
{
    foreach (QVariant pattern, patterns) {
        RegexTextGrammarRule* childRule = createGrammarRule(rule->grammar(), pattern);
        if (childRule) { rule->giveRule(childRule); }
    }
}


/// creates a grammar rue
RegexTextGrammarRule* TmLanguageParser::createGrammarRule(RegexTextGrammar* grammar, const QVariant& data)
{
    QHash<QString,QVariant> map = data.toHash();
    QString match = map.value("match").toString();
    QString include = map.value("include").toString();
    QString begin = map.value("begin").toString();
    QString name = map.value("name").toString();

    // match filled?
    if (!match.isEmpty()) {
        RegexTextGrammarRule* rule = RegexTextGrammarRule::createSingleLineRegExp(grammar, name, match);
        QHash<QString,QVariant> captures = map.value("captures").toHash();
        addCapturesToGrammarRule(rule, captures);
        return rule;

    } else if(!include.isEmpty()) {
        return RegexTextGrammarRule::createIncludeRule(grammar, include);

    } else if(!begin.isEmpty()) {
        QString end = map.value("end").toString();

        // TODO: contentScopeName
        QString contentScope = name;
        RegexTextGrammarRule* rule = RegexTextGrammarRule::createMultiLineRegExp(grammar, name, contentScope, begin, end);

        // add the patterns
        QList<QVariant> patterns = map.value("patterns").toList();
        addPatternsToGrammarRule(rule, patterns);

        if (map.contains("captures")) {
            QHash<QString,QVariant> captures = map.value("captures").toHash();
            addCapturesToGrammarRule(rule, captures);
            addCapturesToGrammarRule(rule, captures, true);
        }
        if (map.contains("beginCaptures")) {
            QHash<QString,QVariant> captures = map.value("beginCaptures").toHash();
            addCapturesToGrammarRule(rule, captures);
        }
        if (map.contains("endCaptures")) {
            QHash<QString,QVariant> endCaptures = map.value("endCaptures").toHash();
            addCapturesToGrammarRule(rule, endCaptures, true);
        }
        return rule;

    } else {
        RegexTextGrammarRule* rule = RegexTextGrammarRule::createRuleList(grammar);

        // add the patterns
        QList<QVariant> patterns = map.value("patterns").toList();
        addPatternsToGrammarRule(rule, patterns);

        return rule;
    }


//    <key>angle_brackets</key>
//    <dict>
//        <key>begin</key>
//        <string>&lt;</string>
//        <key>end</key>
//        <string>&gt;</string>
//        <key>name</key>
//        <string>meta.angle-brackets.c++</string>
//        <key>patterns</key>
//        <array>
//            <dict>
//                <key>include</key>
//                <string>#angle_brackets</string>
//            </dict>
//            <dict>
//                <key>include</key>
//                <string>$base</string>
//            </dict>
//        </array>
//    </dict>
}


// parses the given language
RegexTextGrammar* TmLanguageParser::createLanguage(QVariant& data)
{
    QHash<QString,QVariant> hashMap = data.toHash();
    QString name      = hashMap.value("name").toString();
    QString scopeName = hashMap.value("scopeName").toString();
    // QString uuid      = hashMap.value("uuid").toString(); // is in there but is unused for now

    if (name.isEmpty() || scopeName.isEmpty()) {
        setLastErrorMessage("Name or scope is empty. Cannot parse language!");
        return nullptr;
    }

    // construct the grammar
    RegexTextGrammar* grammar = new RegexTextGrammar(scopeName, name);

    // and get the main patterns
    // construct the main rule
    RegexTextGrammarRule* mainRule = RegexTextGrammarRule::createMainRule(grammar, scopeName);
    grammar->giveMainRule(mainRule);

    QList<QVariant> patterns = hashMap.value("patterns").toList();
    addPatternsToGrammarRule(mainRule, patterns);


    // get the repos
    QHash<QString,QVariant> repos  = hashMap.value("repository").toHash();
    QHashIterator<QString,QVariant> itr(repos);
    while (itr.hasNext()) {
        itr.next();
        RegexTextGrammarRule* rule = createGrammarRule(grammar, itr.value());
        if (rule) {
            grammar->giveToRepos(itr.key(), rule);
        } else {
            qlog_warn() << "Error create grammar rule!";
        }
    }

    // add the file types
    QStringList fileTypes = hashMap.value("fileTypes").toStringList();
    foreach (QString fileType, fileTypes) {
        grammar->addFileExtension(fileType);
    }

    return grammar;
}


} // edbee
