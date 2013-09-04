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


/// A basic command
class TextChange
{
public:
    virtual ~TextChange() {}

    /// This method should execute the command
    virtual void execute( TextDocument* document ) = 0;
    virtual void revert( TextDocument* ) { Q_ASSERT(false); }

    /// This method tries to merge the given change with the other change
    /// The textChange supplied with this method should NOT have been executed yet.
    /// It's the choice of this merge operation if the execution is required
    /// @param textChange the textchange to merge
    /// @return true if the merge has been successfull. False if nothing has been merged and executed
    virtual bool merge( TextDocument* document, TextChange* textChange ) { Q_UNUSED(document); Q_UNUSED(textChange ); return false; }

    virtual bool canUndo() { return false; }

    /// This  flag is used to mark this stack item as non-persistence requirable
    /// The default behaviour is that every textchange requires persistence. It is also possible to
    /// have certain changes that do not require persitence but should be placed on the undo stack
    virtual bool isPersistenceRequired() { return true; }

    /// A text command can belong to a controller/view
    /// When it's a view only command. The undo only applies only to this view
    /// warning a DOCUMENT change may NEVER return a controllerContext!!
    virtual TextEditorController* controllerContext() { return 0; }

    /// this method can be used to check if the given change is a document change
    bool isDocumentChange() { return controllerContext() == 0; }

    /// A method to move the offset with the given delta
    /// @param delta the delta to move
    virtual void moveOffset( int delta ) { Q_UNUSED(delta); }

    /// This method can be overriden by a textchange to react when the line-delta is changed
    /// @param delta the delta to move the line
    virtual void moveLine( int delta ) { Q_UNUSED(delta); }


    virtual QString toString() = 0;
};


//---------------

/// A textdocument command
class DocumentTextChange : public TextChange
{
public:

};

//---------------

/// a document text-change that doesn't do anyhting :-)
class EmptyDocumentTextChange : public TextChange
{
public:
    /// Empty change doesn't do anything
    virtual bool isPersistenceRequired() { return false; }

    virtual void execute( TextDocument* )  {}
    virtual void revert( TextDocument*) {}
    virtual QString toString() { return "EmptyDocumentTextChange"; }

};


//---------------

/// a replace-text command
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

//---------------


/// A textcontroller command. This can ALSO be a document command
class ControllerTextChange : public TextChange
{
public:
    ControllerTextChange( TextEditorController* controller );
    ~ControllerTextChange();

    /// A text command can belong to a controller/view
    /// When it's a view only command. The undo only applies only to this view
    virtual TextEditorController* controllerContext() { return controllerRef_; }

    /// The controller
    virtual TextEditorController* controller() { return controllerRef_; }

private:
    TextEditorController* controllerRef_;  ///< the controller
};


//---------------


/// An undoable-command-group
class TextChangeGroup : public ControllerTextChange
{
public:
    TextChangeGroup( TextEditorController* controller );
    virtual ~TextChangeGroup();

    virtual void giveCommand( TextChange* act );
    virtual void giveCommandAtIndex( int idx, TextChange* act );

    /// This method is called it the group is discardable. A discardable group will be optimized away if
    /// the group is empty, or if there's a single item in the group. A none-discardable group will
    /// always remain
    virtual bool isDiscardable() { return true; }

    /// This method is called if the group is closed and is added to the stack
    virtual void groupClosed() {}

    virtual void execute( TextDocument* document);
    virtual void revert( TextDocument* document);

    /// This method tries to merge the given change with the other change
    /// The textChange supplied with this method should NOT have been executed yet.
    /// It's the choice of this merge operation if the execution is required
    /// @param textChange the textchange to merge
    /// @return true if the merge has been successfull. False if nothing has been merged and executed
    virtual bool merge( TextDocument* document, TextChange* textChange );

    virtual void flatten();


    /// This method returns the last change
    TextChange* at( int idx );
    TextChange* take( int idx );
    TextChange* last();
    TextChange* takeLast();
    int size();
    int recursiveSize();


    /// A text command can belong to a controller/view
    /// When it's a view only command. The undo only applies only to this view
    virtual TextEditorController* controllerContext();

    virtual QString toString();

private:
    QList<TextChange*> changeList_;     ///< A list of all actions
};


//---------------

/// An undoable-command-group
/// The normal text change group can be discarded for optimization. By using this group
/// the undo will not be discarded
class TextChangeGroupNonDiscardable : public TextChangeGroup
{
public:
    TextChangeGroupNonDiscardable ( TextEditorController* controller ) : TextChangeGroup(controller) {}
    virtual bool isDiscardable() { return false; }
};

} // edbee
