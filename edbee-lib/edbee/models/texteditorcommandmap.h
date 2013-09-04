/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QObject>
#include <QMap>
#include <QString>


namespace edbee {

class TextEditorCommand;

/// This is a texteditor map. This is used to map Command-Names to commands
class TextEditorCommandMap : public QObject
{
    Q_OBJECT
public:
    explicit TextEditorCommandMap(QObject *parent = 0);
    virtual ~TextEditorCommandMap();
    
    void give( const QString& key, TextEditorCommand* command );
    TextEditorCommand* get( const QString& key );

signals:
    
public slots:
    
private:
    QMap<QString,TextEditorCommand*> commandMap_;       ///< The command map
};

} // edbee
