/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QList>
#include <QString>

namespace edbee {

class TextDocument;
class TextEditorController;


/// A basic change
class TextChange
{
public:

    virtual ~TextChange();

    /// This method should execute the command
    virtual void execute( TextDocument* document ) = 0;
    virtual void revert( TextDocument* );

    virtual bool giveAndMerge( TextDocument* document, TextChange* textChange );
    virtual bool canUndo();

    virtual bool isPersistenceRequired();
    virtual TextEditorController* controllerContext();
    bool isDocumentChange();
    virtual bool isGroup();

    virtual void applyLineDelta( int line, int length, int newLength);

    virtual QString toString() = 0;
};


//--------------------------------------------------------------


/// A textdocument command
class DocumentTextChange : public TextChange
{
public:
};


//--------------------------------------------------------------


/// a document text-change that doesn't do anyhting :-)
class EmptyDocumentTextChange : public TextChange
{
public:
    virtual bool isPersistenceRequired();
    virtual void execute( TextDocument* );
    virtual void revert( TextDocument*);
    virtual QString toString();
};


//--------------------------------------------------------------


/// a replace-text command. Replaces a single text in the document
class ReplaceDocumentTextChange : public DocumentTextChange
{
public:
    ReplaceDocumentTextChange( int offset, int length, const QString& newText );
    virtual void execute( TextDocument* document );
    virtual void revert( TextDocument* document );

private:
    int offset_;                ///< The offset of the text to replace
    int length_;                ///< The length of the text to replace
    QString text_;              ///< The new text
    QString oldText_;           ///< the replaced text
};


//--------------------------------------------------------------


/// A textcontroller command. This can ALSO be a document command
class ControllerTextChange : public TextChange
{
public:
    ControllerTextChange( TextEditorController* controller );
    virtual TextEditorController* controllerContext();
    virtual TextEditorController* controller();

private:
    TextEditorController* controllerRef_;  ///< the controller
};


//--------------------------------------------------------------


/// An undoable-command-group
class TextChangeGroup : public ControllerTextChange
{
public:
    TextChangeGroup( TextEditorController* controller );
    virtual ~TextChangeGroup();

    virtual bool isGroup();

//    virtual void giveChangeAtIndex( int idx, TextChange* change ) = 0;

    virtual bool isDiscardable();
    virtual void groupClosed();

    virtual void execute( TextDocument* document);
    virtual void revert( TextDocument* document);

//    virtual void moveChangeFromGroup( TextChangeGroup* group );

    virtual bool giveAndMerge( TextDocument* document, TextChange* textChange ) = 0;
    virtual void flatten();

    virtual void giveChange( TextDocument* doc, TextChange* change ) = 0;
    virtual TextChange* at( int idx ) = 0;
    virtual TextChange* take( int idx ) = 0;
    virtual int size() = 0;
    virtual void clear(bool performDelete=true) = 0;
    TextChange* last();
    TextChange* takeLast();
    int recursiveSize();

    virtual TextEditorController* controllerContext();

    virtual QString toString();

private:
    QList<TextChange*> changeList_;     ///< A list of all actions
};


//--------------------------------------------------------------


/// An undoable-command-group
/// The normal text change group can be discarded for optimization. By using this group
/// the undo will not be discarded
class TextChangeGroupNonDiscardable : public TextChangeGroup
{
public:
    TextChangeGroupNonDiscardable ( TextEditorController* controller );
    virtual bool isDiscardable();
};


} // edbee
