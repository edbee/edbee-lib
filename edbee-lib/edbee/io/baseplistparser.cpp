// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include <QXmlStreamReader>

#include "baseplistparser.h"

#include "edbee/debug.h"

namespace edbee {


BasePListParser::BasePListParser()
    : xml_(nullptr)
{
}


BasePListParser::~BasePListParser()
{
    delete xml_;
}


/// Returns the last error message of the parsed file
QString BasePListParser::lastErrorMessage() const
{
    return lastErrorMessage_;
}


/// Sets the last error message.
/// To correctly set the last error message while parsing please use raiseError
void BasePListParser::setLastErrorMessage(const QString& str)
{
    lastErrorMessage_ = str;
}


/// Start the parsing of the plist. If it isn't a valid plist this method return false.
/// (it only checks and reads the existence of <plist>)
bool BasePListParser::beginParsing(QIODevice* device)
{
    lastErrorMessage_.clear();
    elementStack_.clear();

    xml_ = new QXmlStreamReader(device);
    if (readNextElement("plist")) {
        return true;
    } else {
        raiseError(QObject::tr("Start element not found!"));
        return false;
    }
}


/// Closes the parsers
/// @return true if the parsing was succesful
bool BasePListParser::endParsing()
{
    bool result = !xml_->error();
    if (!result) {
        lastErrorMessage_ = QObject::tr("line %1: %2").arg(xml_->lineNumber()).arg(xml_->errorString());
        result = false;
    }
    delete xml_;
    xml_ = nullptr;
    return result;
}



/// Raise an error and stop the xml parsing
/// @param str the error to raise
void BasePListParser::raiseError(const QString& str)
{
    xml_->raiseError(str);
}


/// reads a list of qvariants
QList<QVariant> BasePListParser::readList()
{
    QList<QVariant> result;
    QVariant value;
    qsizetype level = currentStackLevel();
    while ((value = readNextPlistType(level)).isValid()) {
        result.append(value);
    }
    return result;
}


/// reads a dictionary
QHash<QString, QVariant> BasePListParser::readDict()
{
    QHash<QString, QVariant> result;
    qsizetype level = currentStackLevel();
    while (readNextElement("key", level) ) {
        QString key = readElementText();
        QVariant data = readNextPlistType();
        result.insert(key,data);
    }
    return result;
}


/// reads the next plist type
/// @param level the level we're currently parsing
QVariant BasePListParser::readNextPlistType(qsizetype level)
{
    if( readNextElement("",level) ) {
        // reads a dictionary
        if (xml_->name().compare(QLatin1String("dict"), Qt::CaseInsensitive) == 0) {
            return readDict();

        // reads an array
        } else if (xml_->name().compare(QLatin1String("array"), Qt::CaseInsensitive) == 0) {
            return readList( );

        // reads a string
        } else if (xml_->name().compare(QLatin1String("string"), Qt::CaseInsensitive) == 0) {
            return readElementText();
        }
    }
    return QVariant();
}


/// Reads the next element and optionally expects it to be the given name
/// @param name the name taht's expected. Use '' to not check the name
/// @param minimum level the level the element should be. -1 means ignore this argument
bool BasePListParser::readNextElement(const QString& name, qsizetype level)
{
    while (!xml_->atEnd()) {
        switch (xml_->readNext()) {
            case QXmlStreamReader::StartElement:
                elementStack_.push(xml_->name().toString());

                if (name.isEmpty()) return true; // name doesn't matter
                if (xml_->name().compare(name, Qt::CaseInsensitive) == 0 ) return true;
                xml_->raiseError(QObject::tr("Expected %1 while parsing").arg(name));
                return false;

            case QXmlStreamReader::EndElement:
                if (!elementStack_.isEmpty()) { elementStack_.pop(); }

                // end level detected
                if (level >=0 && elementStack_.size() < level) {
                    return false;
                }
                break;
            default:
                // ignore
                break;
        }
    }
    return false;
}


/// Reads the element text contents
QString BasePListParser::readElementText()
{
    QString result = xml_->readElementText();
    if (!elementStack_.isEmpty()) { elementStack_.pop(); }
    return result;
}


/// returns the current stack-level
qsizetype BasePListParser::currentStackLevel()
{
    return elementStack_.size();
}


} // edbee
