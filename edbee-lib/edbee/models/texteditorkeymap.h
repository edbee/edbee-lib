/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QKeySequence>
#include <QHash>
#include <QStringList>

namespace edbee {

class TextEditorCommand;
class TextEditorController;


/// A text editor key map
/// This key map, maps key-sequences to action-names.
class TextEditorKeyMap
{
public:

    TextEditorKeyMap( TextEditorKeyMap* parentKeyMap=0 );
//    TextEditorKeyMap( const TextEditorKeyMap& keyMap );
    virtual ~TextEditorKeyMap();

    void copyKeysTo( TextEditorKeyMap* keyMap );

    QKeySequence get( const QString& name ) const;
    QList<QKeySequence> getAll( const QString& name  ) const;
    bool has( const QString& name ) const;

    void set( const QString& name, const QKeySequence& sequence );
    void set( const QString& name, const QKeySequence::StandardKey key );
    void replace( const QString& name, const QKeySequence& sequence );

    QString findBySequence( QKeySequence sequence, QKeySequence::SequenceMatch& match );

    static QKeySequence joinKeySequences( const QKeySequence seq1, const QKeySequence seq2 );
    static QKeySequence::StandardKey standardKeyFromString( const QString& str );

    QString toString() const;

    TextEditorKeyMap* parentMap() { return parentRef_; }

private:

    TextEditorKeyMap* parentRef_;           ///< a reference to a parent keymap
    QHash<QString,QKeySequence> keys_;      ///< a map to convert a name to a
};

//-------------------------------------------------------------------

/// There can be different keymaps for different file-types/
/// The keymap manager manages all the available keymaps
/// There's ALWAYS a keymap with the name ""
class TextKeyMapManager
{
public:
    TextKeyMapManager();
    ~TextKeyMapManager();

    void loadAllKeyMaps( const QString& path );

    TextEditorKeyMap* get( const QString& name="" );
    TextEditorKeyMap* findOrCreate( const QString& name );

private:

    QHash<QString,TextEditorKeyMap*> keyMapHash_;   ///< All loaded keymaps identified by name

};



} // edbee
