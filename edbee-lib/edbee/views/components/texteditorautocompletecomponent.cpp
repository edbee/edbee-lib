#include "texteditorautocompletecomponent.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLayout>
#include <QListWidget>

#include "edbee/edbee.h"
#include "edbee/models/textautocompleteprovider.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/components/texteditorcomponent.h"
#include "edbee/views/textrenderer.h"
#include "edbee/views/textselection.h"

#include "edbee/debug.h"

namespace edbee {

TextEditorAutoCompleteComponent::TextEditorAutoCompleteComponent(TextEditorController *controller, TextEditorComponent *parent)
    : QWidget(parent)
    , controllerRef_(controller)
    , listWidgetRef_(0)
    , editorComponentRef_(parent)
    , eventBeingFiltered_(false)
{

    /// initialize the widget
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->setSpacing(0);
    layout->setMargin(0);
    listWidgetRef_ = new QListWidget();
    layout->addWidget(listWidgetRef_);
    setLayout(layout);

    this->resize(350,150);
    hide();

    QPalette p = listWidgetRef_->palette();
    p.setColor(QPalette::Highlight, p.color(QPalette::Highlight));  // prevents the non-focus gray color
    p.setColor(QPalette::HighlightedText, p.color(QPalette::HighlightedText));
    listWidgetRef_->setPalette(p);

    editorComponentRef_->installEventFilter(this);
}


/// Returns the current text editor controller
TextEditorController *TextEditorAutoCompleteComponent::controller()
{
    return controllerRef_;
}

QSize TextEditorAutoCompleteComponent::sizeHint() const
{
    if(!listWidgetRef_) return QSize();
    return listWidgetRef_->sizeHint();
}


/// This method check if the autocomplete should be shown
/// this method ALSO sets the word that's display
bool TextEditorAutoCompleteComponent::shouldDisplayAutoComplete(TextRange& range, QString& word )
{
    // when it's a selection (which shouldn't be posssible, but still check it)
    if( range.hasSelection()) return false;

    // when the character AFTER the current is a identifier we should NOT open it
    TextDocument* doc = controller()->textDocument();
    QChar nextChar = doc->charAtOrNull(range.caret());
    if( nextChar.isLetterOrNumber() ) return false;

    // expand the given range to word
    TextRange wordRange = range;
    wordRange.expandToWord(doc, doc->config()->whitespaces(), doc->config()->charGroups() );
    wordRange.maxVar() = range.max(); // next go past the right caret!
    word = doc->textPart(wordRange.min(), wordRange.length()).trimmed();      // workaround for space select bug! #61

    if(word.isEmpty()) return false;

    // else we can should
    return true;
}


/// Fills the autocomplete list
bool TextEditorAutoCompleteComponent::fillAutoCompleteList(TextDocument* document, const TextRange& range, const QString& word)
{
    listWidgetRef_->clear();

    QList<TextAutoCompleteItem*> items = document->autoCompleteProviderList()->findAutoCompleteItemsForRange(document,range,word);
    if( items.length() > 0 ) {
        foreach( TextAutoCompleteItem* item, items ) {
            listWidgetRef_->addItem(item->label());
        }
        listWidgetRef_->setCurrentIndex(listWidgetRef_->model()->index(0,0) );
    }
    return items.length() > 0;
}


/// positions the widget so it's visible.
/// This should be improved so border and screen positions are respected
void TextEditorAutoCompleteComponent::positionWidgetForCaretOffset(int offset)
{
    // find the caret position
    TextRenderer* renderer = controller()->textRenderer();
    int y = renderer->yPosForOffset(offset) + renderer->lineHeight();
    int x = renderer->xPosForOffset(offset);

    // TODO: Better position it so it fits no screen
    move(x,y);
}


/// we need to intercept keypresses if the widget is visible
bool TextEditorAutoCompleteComponent::eventFilter(QObject *obj, QEvent *event)
{
    if( obj == editorComponentRef_ && event->type()==QEvent::KeyPress && isVisible() ) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);

        // text keys are allowed
        if( !key->text().isEmpty()  ) {
            QChar nextChar = key->text().at(0);
            if( nextChar.isLetterOrNumber() ) {
              return QObject::eventFilter(obj, event);
            }
        }

        // escape key
        switch( key->key() ) {
            case Qt::Key_Escape:
                hide();
                return true; // stop event

            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Tab:
                insertCurrentSelectedListItem();
                hide();
                return true;

            // forward special keys to list
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_PageDown:
            case Qt::Key_PageUp:
                if( !eventBeingFiltered_ ) {
                    eventBeingFiltered_ = true;
                    QApplication::sendEvent(listWidgetRef_, event);
                }
                eventBeingFiltered_ = false;
                return true;
        }

        // default operation is to hide and continue the event
        hide();
        return false; // continue event

    }
    return QObject::eventFilter(obj, event);
}


/// inserts the currently selected list item
void TextEditorAutoCompleteComponent::insertCurrentSelectedListItem()
{
    TextSelection* sel = controller()->textSelection();
    sel->moveCarets(- currentWord_.length());
    if( listWidgetRef_->currentItem() ) {
        controller()->replaceSelection(listWidgetRef_->currentItem()->text());
    }
}


/// this event is called when a key pressed
void TextEditorAutoCompleteComponent::textKeyPressed()
{
    // fetch the current selection
    TextDocument* doc = controller()->textDocument();
    TextRange range = controller()->textSelection()->range(0);

    // when the character after
    if(!shouldDisplayAutoComplete(range, currentWord_)) {
      hide();
      return;
    }

    // position the widget
    positionWidgetForCaretOffset( qMax(0,range.caret() - currentWord_.length()) );

    // fills the autocomplete list with the curent word
    if( fillAutoCompleteList(doc, range, currentWord_)) {
        show();
    }
}

}// edbee
