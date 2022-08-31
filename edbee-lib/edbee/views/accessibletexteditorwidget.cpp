#include "accessibletexteditorwidget.h"

#include <QWidget>
#include <QAccessibleInterface>

#include "edbee/models/changes/textchange.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"
#include "edbee/texteditorwidget.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"
#include "edbee/views/textrenderer.h"
#include "edbee/views/components/texteditorcomponent.h"

#include "edbee/debug.h"

/// I've tried to use the TextEditorWidget direct.
/// This works for Windows Reader and Mac OS X Speaker, but doesn't seem to work for NVDA
/// The TexteditorComponent is the component that has the true focus
#define VIA_EDITOR_COMPONENT


namespace edbee {

AccessibleTextEditorWidget::AccessibleTextEditorWidget(TextEditorWidget* widget)
#ifdef VIA_EDITOR_COMPONENT
    : QAccessibleWidget(widget->textEditorComponent(), QAccessible::EditableText)
#else
    : QAccessibleWidget(widget, QAccessible::EditableText)
#endif
    , textWidgetRef_(widget)
{
//    addControllingSignal(QLatin1String("textChanged(const QString&)"));
//    addControllingSignal(QLatin1String("returnPressed()"));
//    addControllingSignal(QLatin1String("textKeyPressed()"));
}

AccessibleTextEditorWidget::~AccessibleTextEditorWidget()
{

}

/// Construct the AccessibleTextEditor interface for the given widget
QAccessibleInterface *AccessibleTextEditorWidget::factory(const QString &className, QObject *object)
{
    // edbee::TextMarginComponent, edbee::TextEditorScrollArea, edbee::TextEditorComponent
#ifdef VIA_EDITOR_COMPONENT
    if (className == QLatin1String("edbee::TextEditorComponent") && object && object->isWidgetType()) {
        return new AccessibleTextEditorWidget(static_cast<edbee::TextEditorComponent *>(object)->controller()->widget());
    }
#else
    if (className == QLatin1String("edbee::TextEditorWidget") && object && object->isWidgetType()) {
        qDebug() << " factory >> " << className;
        return new AccessibleTextEditorWidget(static_cast<edbee::TextEditorWidget *>(object));
    }

#endif

    return nullptr;
}

/// Returns the widget that should be used for accessibility events
QWidget* AccessibleTextEditorWidget::eventWidgetForTextEditor(TextEditorWidget* widget)
{
#ifdef VIA_EDITOR_COMPONENT
    return widget->textEditorComponent();
#else
    return widget;
#endif
}

/// Notifies a text-selection change event
void AccessibleTextEditorWidget::notifyTextSelectionEvent(TextEditorWidget *widget, TextSelection *selection)
{
    QWidget* eventWidget = eventWidgetForTextEditor(widget);
    for(int i=0, cnt = selection->rangeCount(); i < cnt; ++i) {
        TextRange range = selection->range(i);

        QAccessibleTextSelectionEvent ev(eventWidget, range.min(), range.max());
        ev.setCursorPosition(range.caret());
//        qDebug() << " !!updateAccessibility: QAccessibleTextSelectionEvent: " << range.min()<< ", " << range.max() << ", " << range.caret();
        QAccessible::updateAccessibility(&ev);
    }
}

/// Notifies a text change event happens
void AccessibleTextEditorWidget::notifyTextChangeEvent(TextEditorWidget *widget, TextBufferChange *change)
{
    QWidget* eventWidget = eventWidgetForTextEditor(widget);

    // this is a bit tricky, a textbuffer change uses the newtext-value
    // for storing the old text. The new text can be found in the document
    QString oldText(change->newText());
    QString newText = widget->textDocument()->textPart(change->offset(), change->newTextLength());

    QAccessibleTextUpdateEvent ev(eventWidget, change->offset(), oldText, newText);
    // TODO: When a caret is included, (Inherited change, use this caret position)

//    qDebug() << "!! updateAccessibility: QAccessibleTextUpdateEvent: " << change->offset()<< ", " << oldText << ", " << newText;
    QAccessible::updateAccessibility(&ev);
}



void *AccessibleTextEditorWidget::interface_cast(QAccessible::InterfaceType t)
{
    if (t == QAccessible::TextInterface) {
        return static_cast<QAccessibleTextInterface*>(this);
    }
    if (t == QAccessible::EditableTextInterface) {
        return static_cast<QAccessibleEditableTextInterface*>(this);
    }
    return QAccessibleWidget::interface_cast(t);
}


QAccessible::State AccessibleTextEditorWidget::state() const
{
    QAccessible::State s = QAccessibleWidget::state();
    s.selectableText = true;
//    s.multiSelectable = true;
//    s.extSelectable = true;
    s.multiLine = true;
    s.focusable = true;
//    s.marqueed = true; // The object displays scrolling contents, e.g. a log view.
    return s;
}


/// Returns a selection. The size of the selection is returned in startOffset and endOffset.
/// If there is no selection both startOffset and endOffset are nullptr.
///
/// The accessibility APIs support multiple selections. For most widgets though, only one selection
/// is supported with selectionIndex equal to 0.
void AccessibleTextEditorWidget::selection(int selectionIndex, int *startOffset, int *endOffset) const
{   
    if(selectionIndex >= textSelection()->rangeCount()) {
        *startOffset = 0;
        *endOffset = 0;
    }

    TextRange& range = textSelection()->range(selectionIndex);
    *startOffset = range.min();
    *endOffset = range.max();
//    qDebug() << " selection >> " << selectionIndex << ", " << *startOffset << ", " << *endOffset;
}

/// Returns the number of selections in this text.
int AccessibleTextEditorWidget::selectionCount() const
{
//    qDebug() << " selectionCount() >> " << textSelection()->rangeCount();
    return textSelection()->rangeCount();
}


/// Select the text from startOffset to endOffset. The startOffset is the first character that will be selected.
/// The endOffset is the first character that will not be selected.
///
/// When the object supports multiple selections (e.g. in a word processor), this adds a new selection,
/// otherwise it replaces the previous selection.
///
/// The selection will be endOffset - startOffset characters long.
void AccessibleTextEditorWidget::addSelection(int startOffset, int endOffset)
{
    TextSelection selection = *textSelection();
    selection.addRange(startOffset, endOffset);
// qDebug() << " addSelection() >> " << startOffset << ", " << endOffset;

    controller()->changeAndGiveTextSelection(&selection);
}

/// Clears the selection with index selectionIndex.
void AccessibleTextEditorWidget::removeSelection(int selectionIndex)
{
    TextSelection selection = *textSelection();
    selection.removeRange(selectionIndex);
//    qDebug() << " removeSelection() >> " << selectionIndex;
    controller()->changeAndGiveTextSelection(&selection);
}

/// Set the selection selectionIndex to the range from startOffset to endOffset.
void AccessibleTextEditorWidget::setSelection(int selectionIndex, int startOffset, int endOffset)
{
    TextSelection selection = *textSelection();
    selection.setRange(startOffset, endOffset, selectionIndex);
//    qDebug() << " setSelection() >> " << selectionIndex << ", " << startOffset << ", " << endOffset;
    controller()->changeAndGiveTextSelection(&selection);
}

/// Returns the current cursor position.
int AccessibleTextEditorWidget::cursorPosition() const
{
//    qDebug() << " cursorPosition() >> " << textSelection()->range(0).caret();
    return textSelection()->range(0).caret();
}

/// Move the cursor position
void AccessibleTextEditorWidget::setCursorPosition(int position)
{
//    qDebug() << " moveCaretToOffset() >> " << position;

    controller()->moveCaretToOffset(position, false);
}

QString AccessibleTextEditorWidget::text(QAccessible::Text t) const
{
//qDebug() << " text >> " << t;
    if (t != QAccessible::Value) {
        return QAccessibleWidget::text(t);
    }
//qDebug() << " text.B >> return full text";

    return textWidget()->textDocument()->text();
}

/// Returns the text from startOffset to endOffset. The startOffset is the first character that will be returned.
/// The endOffset is the first character that will not be returned.
QString AccessibleTextEditorWidget::text(int startOffset, int endOffset) const
{
    QString str = textWidget()->textDocument()->textPart(startOffset, endOffset-startOffset);
//    qDebug() << " text >> " << startOffset << ", " << endOffset << " : " << str;
    return str;
}

/// Returns the length of the text (total size including spaces).
int AccessibleTextEditorWidget::characterCount() const
{
//    qDebug() << " characterCount >>" << textWidget()->textDocument()->length();
    return textWidget()->textDocument()->length();
}


/// Returns the position and size of the character at position offset in screen coordinates.
QRect AccessibleTextEditorWidget::characterRect(int offset) const
{
    TextEditorComponent* comp = textWidget()->textEditorComponent();
    int xPos = this->renderer()->xPosForOffset(offset);
    int yPos = this->renderer()->yPosForOffset(offset);
    QPoint point(xPos, yPos);
    QPoint pointScreen = comp->mapToGlobal(point);
    //xPos = comp->mapToGlobal()
    //yPos = rect.y();

//    qDebug() << " characterCount >>" << offset << ": " << pointScreen;

    return QRect(pointScreen.x(), pointScreen.y(), renderer()->emWidth(), renderer()->lineHeight());
}


/// Returns the offset of the character at the point in screen coordinates.
int AccessibleTextEditorWidget::offsetAtPoint(const QPoint &point) const
{
    int line = renderer()->rawLineIndexForYpos(point.y());
    int col = renderer()->columnIndexForXpos(line, point.x());
//   qDebug() << " offsetAtPoint >>" << point << ": " << line << ", " << col;

   return textDocument()->offsetFromLineAndColumn(line, col);
}

/// Ensures that the text between startIndex and endIndex is visible.
void AccessibleTextEditorWidget::scrollToSubstring(int startIndex, int endIndex)
{
//    qDebug() << " scrollToSubstring >>" << startIndex << ", " << endIndex;
    controller()->scrollOffsetVisible(startIndex);
}


/// Returns the text attributes at the position offset.
/// In addition the range of the attributes is returned in startOffset and endOffset.
QString AccessibleTextEditorWidget::attributes(int offset, int *startOffset, int *endOffset) const
{
//    qDebug() << " attributes >>" << offset << ", " << *startOffset << ", " <<  *endOffset;
    return QString();
}


/// Deletes the text from startOffset to endOffset.
void AccessibleTextEditorWidget::deleteText(int startOffset, int endOffset)
{
//    qDebug() << " deleteText >>" << startOffset << ", " << endOffset;
    controller()->replace(startOffset, endOffset - startOffset, QString(), 0);
}

/// Inserts text at position offset.
void AccessibleTextEditorWidget::insertText(int offset, const QString &text)
{
//    qDebug() << " insertText >>" << offset << ", " << text;
    controller()->replace(offset, 0, text, 0);
}

/// Removes the text from startOffset to endOffset and instead inserts text.
void AccessibleTextEditorWidget::replaceText(int startOffset, int endOffset, const QString &text)
{
//    qDebug() << " replaceText >>" << startOffset << ", " << endOffset << ", " << text;
    controller()->replace(startOffset, endOffset - startOffset, text, 0);
}

QAccessibleInterface *AccessibleTextEditorWidget::focusChild() const
{
    QAccessibleInterface* child = QAccessibleWidget::focusChild();
    return child;
}

/// Returns the rectangle for the editor widget
/// It returns the location of the textWidget (even when the TextComponent has got focus)
QRect AccessibleTextEditorWidget::rect() const
{
    QRect widgetRect = textWidget()->rect();
    QRect focusRect(textWidget()->mapToGlobal(widgetRect.topLeft()), widgetRect.size());
    return focusRect;
}


TextDocument* AccessibleTextEditorWidget::textDocument() const
{
    return textWidget()->textDocument();
}

TextSelection *AccessibleTextEditorWidget::textSelection() const
{
    return textWidget()->textSelection();
}

TextEditorController* AccessibleTextEditorWidget::controller() const
{
    return textWidget()->controller();
}

TextRenderer *AccessibleTextEditorWidget::renderer() const
{
    return textWidget()->textRenderer();
}

TextEditorWidget *AccessibleTextEditorWidget::textWidget() const
{
    return textWidgetRef_;
}


} // namespace Edbee
