/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QHash>
#include <QString>
#include <QVariant>

class QIODevice;

namespace edbee {

class JsonParser;
class TextEditorKeyMap;
class TextKeyMapManager;

/// This emthod method can be used to load text-editor keymaps
///
/// The json file has got the following format
///
/// [
///   {
///      "context": {
///         "name": "texteditor",           // the part of the appliction these bindigns used for.
///         ...
///      },
///      "defaults": {
///         "context":  [ {                 // the context defined here is coppied to alle keybindings
///             {"key": "scope", "value": "source.php"},
///             ...
///         }]
///      },
///      "bindings": [
///         { "keys": "Ctrl+M,U", "command": "display_undo_stack" },        // todo, add scopes to keybindings
///         ....
///      ]
///   },
///   ...
/// ]
///
/// Context is still not completely thought over but it is used to make certain keybindings
/// context specific. Think for example of keybindings only used for given language
///
class KeyMapParser
{
public:
    KeyMapParser();
    virtual ~KeyMapParser();

    bool parse(const QString& filename, TextKeyMapManager* manager );
    bool parse( QIODevice* device, TextKeyMapManager* manager);
    bool parse( const QByteArray& bytes, TextKeyMapManager* manager );
    bool parse( const QVariant& variant, TextKeyMapManager* manager );

    QString errorMessage() { return errorMessage_; }


private:

    void buildErrorMessageFromParser();

    bool parseBindingBlock(const QVariantMap &valueObject, TextKeyMapManager* manager );

    QString errorMessage_;                         ///< The current error message
    JsonParser* parser_;                           ///< The json parser
};

} // edbee
