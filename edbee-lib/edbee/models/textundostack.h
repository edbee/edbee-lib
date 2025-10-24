// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QObject>
#include <QMap>
#include <QStack>

namespace edbee {

class Change;
class TextDocument;
class TextEditorController;
class ChangeGroup;


/// This is the undo stack for the texteditor. This stack is SHARED by all views of the document
/// The stack also stores view-specific commands of all views
///
/// Every view has got it's own pointer on the stack. This pointer points at the index AFTER the last item on the stack
/// that's used for this view.
///
/// When a view-performs a view-specific undo (soft-undo) it's own pointer is stepped back. Only view-specific commands are undone
/// When a view performs a document undo. ALL view-pointers a are undone to the point of the document undo.
///
/// ~~~~
///
/// 7 |       |  <= controllerIndexMap_(v1)
///   +-------+
/// 6 |  v1   |  <= controllerIndexMap_(v2)
///   +-------+
/// 5 |  v2   |
///   +-------+
/// 4 |  v1   |  <= changeIndex_
///   +-------+
/// 3 |  DOC  |
///   +-------+
/// 2 |  v1   |
///   +-------+
/// 1 |  DOC  |
///   +-------+
/// 0 |  v2   |
///   +-------+
///
/// ~~~~

class EDBEE_EXPORT TextUndoStack : public QObject
{
    Q_OBJECT

public:

    /// This enumeration is signaled to the listeners to notify what happend when ending an undo group
    enum EndUndoGroupAction {
        ActionUngrouped,        ///< The group has been ungrouped, the single change has been added to the stack
        ActionFullDiscard,      ///< The group contained nothing usefull, nothing is added to the stack
        ActionEnd               ///< A normal group end
    };

public:
    explicit TextUndoStack(TextDocument* doc, QObject* parent = nullptr);
    virtual ~TextUndoStack();
    void clear();

    void registerContoller(TextEditorController* controller);
    void unregisterController(TextEditorController* controller);
    bool isControllerRegistered(TextEditorController* controller);

    void beginUndoGroup(ChangeGroup* group);
    ChangeGroup* currentGroup();
    void endUndoGroup(int coalesceId , bool flatten);
    void endUndoGroupAndDiscard();
    size_t undoGroupLevel();

    int lastCoalesceIdAtCurrentLevel();
    void setLastCoalesceIdAtCurrentLevel(int id);
    void resetAllLastCoalesceIds();

    Change* giveChange(Change* change, int coalesceId);

    bool canUndo(TextEditorController* controller = nullptr);
    bool canRedo(TextEditorController* controller = nullptr);

    void undo(TextEditorController* controller = nullptr, bool controllerOnly = false);
    void redo(TextEditorController* controller = nullptr, bool controllerOnly = false);

    bool isCollectionEnabled() { return collectionEnabled_; }
    void setCollectionEnabled(bool enabled) { collectionEnabled_ = enabled; }

    bool isUndoRunning() { return undoRunning_; }
    bool isRedoRunning() { return redoRunning_; }

    qsizetype size();
    Change* at(qsizetype idx);
    qsizetype currentIndex(TextEditorController* controller = nullptr);
    qsizetype lastIndex(TextEditorController* controller = nullptr);
    Change* last(TextEditorController* controller = nullptr);

    qsizetype sizeInDocChanges();
    qsizetype currentIndexInDocChanges();

    Change* findRedoChange(TextEditorController* controller = nullptr);
    Change* findUndoChange(TextEditorController* controller = nullptr);

    void setPersisted(bool enabled);
    bool isPersisted();
    qsizetype persistedIndex();

    TextDocument* document() { return documentRef_; }

    QString dumpStack();
    void dumpStackInternal();

protected:
    qsizetype findRedoIndex(qsizetype index, TextEditorController* controller = nullptr);
    qsizetype findUndoIndex(qsizetype index, TextEditorController* controller = nullptr);
    void clearRedo(TextEditorController* controller );
    bool undoControllerChange(TextEditorController* controller);
    void undoDocumentChange();
    bool redoControllerChange(TextEditorController* controller);
    void redoDocumentChange();

    void setPersistedIndex(qsizetype index);
    void setChangeIndex(qsizetype index);

signals:

    void undoGroupStarted(edbee::ChangeGroup* group);

    /// This signal is fired when the group is ended. Warning, when the group is merged
    /// the group pointer will be 0!!
    void undoGroupEnded(int coalesceId, bool merged, int action);
    void changeAdded(edbee::Change* change);
//    void changeMerged( edbee::TextChange* oldChange, edbee::TextChange* change );
    void undoExecuted(edbee::Change* change);
    void redoExecuted(edbee::Change* change);

    /// This signal is emitted if the persisted state is changed
    void persistedChanged(bool persisted);

private:
    void clearHistoryLists();

    TextDocument* documentRef_;                                 ///< A reference to the textdocument

    QList<Change*> changeList_;                                 ///< The list of stack commands
    qsizetype changeIndex_;                                     ///< The current command index (TextDocument/Global)
    QMap<TextEditorController*, qsizetype> controllerIndexMap_; ///< The current controller pointers (View specific)
    qsizetype persistedIndex_;                                  ///< The current persisted index. A persisted-index of -1, means it never is persisted

    QStack<ChangeGroup*> undoGroupStack_;                       ///< A stack of all undo items
    QStack<int> lastCoalesceIdStack_;                           ///< The last coalesceId

    bool collectionEnabled_;                                    ///< Is the undo-stack enabled?

    bool undoRunning_;                                          ///< This flag is set if undo is running
    bool redoRunning_;                                          ///< This flag is set if a redo is running
};

} // edbee
