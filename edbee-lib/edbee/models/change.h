// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QList>
#include <QString>

namespace edbee {

class TextDocument;
class TextEditorController;


/// A basic change
class EDBEE_EXPORT Change {
public:

    virtual ~Change();

    /// This method should execute the command
    virtual void execute( TextDocument* document ) = 0;
    virtual void revert( TextDocument* );

    virtual bool giveAndMerge( TextDocument* document, Change* textChange );
    virtual bool canUndo();

    virtual bool isPersistenceRequired();
    virtual TextEditorController* controllerContext();
    bool isDocumentChange();
    virtual bool isGroup();

    virtual QString toString() = 0;
};


//--------------------------------------------------------------


/// A textdocument change
class EDBEE_EXPORT DocumentChange : public Change
{
public:
};


//--------------------------------------------------------------


/// a document text-change that doesn't do anyhting :-)
class EDBEE_EXPORT EmptyDocumentChange : public Change
{
public:
    virtual bool isPersistenceRequired();
    virtual void execute( TextDocument* );
    virtual void revert( TextDocument*);
    virtual QString toString();
};


//--------------------------------------------------------------


/// A textcontroller command. This can ALSO be a document command
class EDBEE_EXPORT ControllerChange : public Change
{
public:
    ControllerChange( TextEditorController* controller );
    virtual TextEditorController* controllerContext();
    virtual TextEditorController* controller();

private:
    TextEditorController* controllerRef_;  ///< the controller
};


//--------------------------------------------------------------


/// An undoable-command-group
class EDBEE_EXPORT ChangeGroup : public ControllerChange
{
public:
    ChangeGroup( TextEditorController* controller );
    virtual ~ChangeGroup();

    virtual bool isGroup();

    virtual bool isDiscardable();
    virtual void groupClosed();

    virtual void execute( TextDocument* document);
    virtual void revert( TextDocument* document);

    virtual bool giveAndMerge( TextDocument* document, Change* textChange );
    virtual void flatten();

    virtual void giveChange( TextDocument* doc, Change* change );
    virtual Change* at( int idx );
    virtual Change* take( int idx );
    virtual size_t size();
    virtual void clear(bool performDelete=true);
    Change* last();
    Change* takeLast();
    size_t recursiveSize();

    virtual TextEditorController* controllerContext();

    virtual QString toString();

private:
    QList<Change*> changeList_;     ///< A list of all actions
};




} // edbee
