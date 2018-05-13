#include "texteditorautocompletecomponent.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QtGui>

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
    //this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    //addWidget(listWidgetRef_);

    //layout->setContentsMargins(0, 0, 0, 0);
    //layout->setStretch(1, 1);
    //listWidgetRef_->setSizeAdjustPolicy(QListWidget::AdjustToContents);
    //layout->->setSizeAdjustPolicy(QListWidget::AdjustToContents);
    //this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //this->resize(350,150); //this->resize(350,150);
    hide();

    QPalette p = listWidgetRef_->palette();
    p.setColor(QPalette::Highlight, p.color(QPalette::Highlight));  // prevents the non-focus gray color
    p.setColor(QPalette::HighlightedText, p.color(QPalette::HighlightedText));
    p.setColor(QPalette::Base, QColor(37, 37, 38));
    p.setColor(QPalette::Text, Qt::white);
    //listWidgetRef_->setPalette(p);
    //listWidgetRef_->setToolTip(listWidgetRef_->toolTip());

    editorComponentRef_->installEventFilter(this);

    // prevent the widgets from having foxus
    setFocusPolicy(Qt::NoFocus);
    listWidgetRef_->setFocusPolicy(Qt::NoFocus);
    listWidgetRef_->setAttribute(Qt::WA_NoMousePropagation,true);   // do not wheel
    listWidgetRef_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    listWidgetRef_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidgetRef_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    AutoCompleteDelegate *acDel = new AutoCompleteDelegate(this);
    listWidgetRef_->setItemDelegate(acDel);

    // make clicking in the list word
    listWidgetRef_->setMouseTracking(true);
    connect( listWidgetRef_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listItemClicked(QListWidgetItem*)));
    connect( listWidgetRef_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listItemDoubleClicked(QListWidgetItem*)));
    //connect( listWidgetRef_, SIGNAL(entered(QModelIndex)), this, SLOT(selectItemOnHover(QModelIndex)));
}


/// Returns the current text editor controller
TextEditorController *TextEditorAutoCompleteComponent::controller()
{
    return controllerRef_;
}

QSize TextEditorAutoCompleteComponent::sizeHint() const
{
    if(!listWidgetRef_) return QSize();
    const int itemHeight = 15;
    const int visibleItems = qMin(listWidgetRef_->count(), 10);
    const int height = visibleItems * itemHeight;
    return QSize(350, height);
    //return listWidgetRef_->sizeHint();
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
            QListWidgetItem *wItem = new QListWidgetItem();
            //wItem->setText(item->usage());
            wItem->setData(Qt::DisplayRole, item->label());
            wItem->setData(Qt::DecorationRole, item->type());
            wItem->setData(Qt::ToolTipRole, item->usage());
            //wItem->setToolTip(item->usage());
            listWidgetRef_->addItem(wItem);
            //(item->usage().length() > 0) ? listWidgetRef_->addItem(item->label() + " - " + item->usage()) : listWidgetRef_->addItem(item->label());
        }
        listWidgetRef_->setCurrentIndex(listWidgetRef_->model()->index(0,0) );
        //listWidgetRef_->currentItem()->setToolTip(listWidgetRef_->currentItem()->toolTip());
        //listWidgetRef_->setToolTip(listWidgetRef_->toolTip());
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
        if( !key->text().isEmpty() ) {
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

            case Qt::Key_Shift: //ignore shift, don't hide
                return false;

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


void TextEditorAutoCompleteComponent::listItemClicked(QListWidgetItem* item)
{
    item->setSelected(true);
    //item->setToolTip(item->toolTip());
}

void TextEditorAutoCompleteComponent::listItemDoubleClicked(QListWidgetItem* item)
{
    insertCurrentSelectedListItem();
    hide();
}

void TextEditorAutoCompleteComponent::selectItemOnHover(QModelIndex modelIndex)
{
    listWidgetRef_->selectionModel()->select(modelIndex,QItemSelectionModel::SelectCurrent);
}

AutoCompleteDelegate::AutoCompleteDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{
    pixelSize = 12;
}

void AutoCompleteDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    QFont font("Consolas", 9);
    QFontMetrics fm(font);
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, QColor(0, 122, 204));
    else
        painter->fillRect(option.rect, QColor(37, 37, 38));
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    //painter->setPen(Qt::P);
    if (option.state & QStyle::State_Selected)
        painter->setBrush(option.palette.highlightedText());
    else
        painter->setBrush(option.palette.text());
    //QString str = index.data(Qt::DisplayRole).toString();
    //QString sName = str.split("(").value(0);
    //QString sUsage = str;
    //QString sReturn = index;
    QString sName = index.data(Qt::DisplayRole).toString();
    QString sUsage = index.data(Qt::ToolTipRole).toString();
    QString sType = index.data(Qt::DecorationRole).toString();
    /*if (str.contains(" = ")) {
        sReturn = str.split(" = ").value(0);
        sUsage = str.split(" = ").value(1);
    }*/
    //font.setBold(true);
    QRect typeRect = option.rect;
    QRect hyphenRect = option.rect;
    QRect nameRect = option.rect;
    hyphenRect.setX(hyphenRect.x() + fm.width(sName));
    typeRect.setX(nameRect.x() + fm.width(sName + " : "));
    painter->setFont(font);
    QPen namePen = QPen(QColor(86, 156, 214));
    QPen textPen = QPen(QColor(241, 241, 241));
    QPen parentPen = QPen(QColor(78, 201, 176));
    painter->setPen(textPen);
    painter->drawText(nameRect, sName);
    //painter->drawText(option.rect, sUsage);
    if (sType != "void")
    {
        painter->drawText(hyphenRect, " - ");
        painter->setPen(parentPen);
        painter->drawText(typeRect, sType);
    }
    painter->restore();
}

QSize AutoCompleteDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex &  index ) const
{
    //return QSize(400, 15);
    return QSize(100, 15);
}

FakeToolTip::FakeToolTip(QWidget *parent) :
    QWidget(parent, Qt::ToolTip | Qt::WindowStaysOnTopHint)
{
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_DeleteOnClose);

    // Set the window and button text to the tooltip text color, since this
    // widget draws the background as a tooltip.
    QPalette p = palette();
    const QColor toolTipTextColor = p.color(QPalette::Inactive, QPalette::ToolTipText);
    p.setColor(QPalette::Inactive, QPalette::WindowText, toolTipTextColor);
    p.setColor(QPalette::Inactive, QPalette::ButtonText, toolTipTextColor);
    setPalette(p);

    const int margin = 1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this);
    setContentsMargins(margin + 1, margin, margin, margin);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / 255.0);
}

void FakeToolTip::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionFrame opt;
    opt.init(this);
    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    p.end();
}

void FakeToolTip::resizeEvent(QResizeEvent *)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.init(this);
    if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
        setMask(frameMask.region);
}

}// edbee
