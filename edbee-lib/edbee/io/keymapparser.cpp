/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "keymapparser.h"

#include <QFile>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "edbee/io/jsonparser.h"
#include "edbee/models/texteditorkeymap.h"

#include "debug.h"

namespace edbee {

KeyMapParser::KeyMapParser()
    : parser_(0)
{
    parser_ = new JsonParser();
}

KeyMapParser::~KeyMapParser()
{
    delete parser_;
}

/// loads the given keymap file returns true on  success
bool KeyMapParser::parse(const QString& filename , TextEditorKeyMap* keyMap)
{
    if( parser_->parse( filename ) ) {
        return parse( parser_->result(), keyMap );
    }
    buildErrorMessageFromParser();
    return false;
}

bool KeyMapParser::parse(QIODevice* device, TextEditorKeyMap* keyMap)
{
    if( parser_->parse( device ) ) {
        return parse( parser_->result(), keyMap );
    }
    buildErrorMessageFromParser();
    return false;
}

bool KeyMapParser::parse(const QByteArray& bytes, TextEditorKeyMap* keyMap)
{
    if( parser_->parse( bytes) ) {
        return parse( parser_->result(), keyMap );
    }
    buildErrorMessageFromParser();
    return false;
}

/// Parsers the variant map so the keymapManager is filled
/// @param variant the variant to parse
/// @param manager the manager to parse
/// @param manager the keymap manager
bool KeyMapParser::parse(const QVariant& variant, TextEditorKeyMap* keyMap)
{
    errorMessage_.clear();

    // next read all exteniosn
    QVariantList list = variant.toList();
    if( list.isEmpty() ) {
        errorMessage_ = QObject::tr("No data found!");
        return false;
    }

    foreach( QVariant var, list ) {
        QVariantMap obj = var.toMap();
        if( obj.isEmpty() ) {
            errorMessage_ = QObject::tr( "Expected object in keymap" );
        } else {
            parseBindingBlock(obj,keyMap);
        }
    }
    return true;

}



void KeyMapParser::buildErrorMessageFromParser()
{
    errorMessage_ = parser_->fullErrorMessage();
}


/// parses the given block
bool KeyMapParser::parseBindingBlock(const QVariantMap& valueObject, TextEditorKeyMap* keyMap )
{
    QVariantMap context = valueObject.value("context").toMap();
    QVariantList bindings = valueObject.value("bindings").toList();

    // add all bindings
    foreach( QVariant bindingItem, bindings ) {
        QVariantMap binding = bindingItem.toMap();
        QString keys = binding.value("keys").toString();
        QString command = binding.value("command").toString();
        if( keys.size() > 0 && command.size() > 0 ) {

            // when the given keys-string is a 'standard-key' name we use the standard key
            QKeySequence::StandardKey standardKey = TextEditorKeyMap::standardKeyFromString(keys);
            if( standardKey != QKeySequence::UnknownKey) {
                keyMap->set( command, new TextEditorKey( QKeySequence(standardKey) ) );
            } else {
                keyMap->set( command, new TextEditorKey( QKeySequence(keys) ) );
            }
        }
    }
    return true;
}


} // edbee
