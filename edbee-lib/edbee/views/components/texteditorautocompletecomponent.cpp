#include "texteditorautocompletecomponent.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QtGui>
#include <QTime>

#include "edbee/edbee.h"
#include "edbee/models/textautocompleteprovider.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/texteditorconfig.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/components/texteditorcomponent.h"
#include "edbee/views/textrenderer.h"
#include "edbee/views/textselection.h"
#include "edbee/views/texttheme.h"

#include "edbee/debug.h"

namespace edbee {

TextEditorAutoCompleteComponent::TextEditorAutoCompleteComponent(TextEditorController *controller, TextEditorComponent *parent)
    : QWidget(parent, Qt::ToolTip | Qt::WindowStaysOnTopHint)
    , controllerRef_(controller)
    , editorComponentRef_(parent)
    , infoTipRef_(0)
    , eventBeingFiltered_(false)
{
    /// initialize the widget
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->setSpacing(0);
    layout->setMargin(0);
    listWidgetRef_ = new QListWidget();
    listWidgetRef_->setObjectName("listWidgetRef");
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

    QPalette p = listWidgetRef_->palette();// listWidgetRef_->
    p.setColor(QPalette::Highlight, p.color(QPalette::Highlight));  // prevents the non-focus gray color
    p.setColor(QPalette::HighlightedText, p.color(QPalette::HighlightedText));
    p.setColor(QPalette::Base, QColor(37, 37, 38));
    p.setColor(QPalette::Text, Qt::white);
    //listWidgetRef_->setPalette(p);
    //listWidgetRef_->setToolTip(listWidgetRef_->toolTip());

    editorComponentRef_->installEventFilter(this);

    // prevent the widgets from having foxus
    //setFocusProxy(editorComponentRef_);
    //setFocusPolicy(Qt::NoFocus);
    //SSRlistWidgetRef_->setParent(0);
    //listWidgetRef_->setWindowFlags(Qt::ToolTip);
    //listWidgetRef_->setFocusPolicy(Qt::NoFocus);
    listWidgetRef_->setFocusPolicy(Qt::ClickFocus);
    listWidgetRef_->setFocusProxy(editorComponentRef_);
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
    return QSize(350, ( qMin(listWidgetRef_->count(), 10) * 15 ) + 5);
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

void TextEditorAutoCompleteComponent::showInfoTip()
{
    if( !listWidgetRef_->isVisible() )
        return;

    const QModelIndex &current = listWidgetRef_->currentIndex();
    if( !current.isValid() )
        return;

    QString infoTip = current.data(Qt::UserRole).toString();//Qt::WhatsThisRole).toString();
    if( infoTip.isEmpty() ) {
        //delete infoTipRef_.data();
        infoTip = "No tooltip data found!";
        //return;
    }

    if ( infoTipRef_.isNull() )
        infoTipRef_ = new FakeToolTip(this);


    //infoTipRef_->move(listWidgetRef_->x() + listWidgetRef_->width() + 2, listWidgetRef_->y());// (m_completionListView->infoFramePos());
    infoTipRef_->setText(infoTip);
    infoTipRef_->update();

    QRect r = listWidgetRef_->visualItemRect(listWidgetRef_->currentItem());
    int xOffset;
    if( listWidgetRef_->count() > 10 )
        xOffset = 22;
    else
        xOffset = 5;

    QPoint p(listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).x() + xOffset, listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).y());

    infoTipRef_->move(p);

//    infoTipRef_->calculateMaximumWidth();
    //infoTipRef_->adjustSize();
    //infoTipRef_->resize(350, 19);
    QSize tipSize = infoTipRef_->tipText.documentLayout()->documentSize().toSize();
    //infoTipRef_->tipText.documentLayout()->documentSize()
    infoTipRef_->resize(tipSize.width(), tipSize.height() - 2);
    //infoTipRef_->resize();
    infoTipRef_->show();
    infoTipRef_->raise();

    //m_infoTimer.setInterval(0);
}

void TextEditorAutoCompleteComponent::hideInfoTip()
{
    if ( !infoTipRef_.isNull() )
        infoTipRef_->hide();
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
            wItem->setData(Qt::DecorationRole, item->kind());
            wItem->setData(Qt::UserRole, item->detail());
            wItem->setData(Qt::WhatsThisRole, item->documentation());
            //wItem->setToolTip(item->usage());
            listWidgetRef_->addItem(wItem);
            //(item->usage().length() > 0) ? listWidgetRef_->addItem(item->label() + " - " + item->usage()) : listWidgetRef_->addItem(item->label());
        }
        listWidgetRef_->setCurrentIndex(listWidgetRef_->model()->index(0,0) );
        //listWidgetRef_->currentItem()->setToolTip(listWidgetRef_->currentItem()->toolTip());
        //listWidgetRef_->setToolTip(listWidgetRef_->toolTip());
    }
    else
    {
        hide();
    }
    setFixedHeight( ( qMin(listWidgetRef_->count(), 10) * 15 ) + 4 );
    if( items.length() > 10 ) {
        listWidgetRef_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    } else {
        listWidgetRef_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
    QPoint newLoc = listWidgetRef_->parentWidget()->parentWidget()->mapToGlobal(QPoint(x, y));
    //QPoint p(listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).x() + xOffset, listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).y());

    // TODO: Better position it so it fits no screen
    move(newLoc.x(), newLoc.y());
}

/// intercepts hide() calls to inform the tooltip to hide as well
void TextEditorAutoCompleteComponent::hideEvent(QHideEvent *event)
{
//    listWidgetRef_->hide();
    infoTipRef_->hide();
    event->isAccepted();
}

/*void TextEditorAutoCompleteComponent::focusOutEvent(QFocusEvent *event)
{
//    listWidgetRef_->hide();
    qDebug() << "focusOutEvent:" << event->type();
    infoTipRef_->hide();
    event->isAccepted();
}*/

/*void TextEditorAutoCompleteComponent::moveEvent(QMoveEvent *event)
{
    TextDocument* doc = controller()->textDocument();
    TextRange range = controller()->textSelection()->range(0);

    // when the character after
    if(!shouldDisplayAutoComplete(range, currentWord_)) {
      hide();
      return;
    }

    // position the widget
    positionWidgetForCaretOffset( qMax(0,range.caret() - currentWord_.length()) )
    //infoTipRef_->hide();
    QRect r = listWidgetRef_->visualItemRect(listWidgetRef_->currentItem());
    int xOffset;
    if( listWidgetRef_->count() > 10 )
        xOffset = 22;
    else
        xOffset = 5;

    QPoint p(listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).x() + xOffset, listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).y());

    //if( !infoTipRef_.isNull() )
        //infoTipRef_->move(p);

    event->isAccepted();
}*/

/// we need to intercept keypresses if the widget is visible
bool TextEditorAutoCompleteComponent::eventFilter(QObject *obj, QEvent *event)
{
    //qDebug() << "event ->" << event;
    //if( QApplication::focusWidget() )
        //qDebug() << QApplication::focusWidget()->metaObject()->className();
    /*if( QApplication::focusWindow() )
    {
        qDebug() << " window:" << QApplication::focusWindow()->objectName();
        qDebug() << "Currentfocus is on obj:" << QApplication::focusObject()->objectName();
        qDebug() << " widget:" << QApplication::focusWidget()->metaObject()->className();
    }*/
    //QEvent::ApplicationDeactivated
    if( event->type() == QEvent::WindowDeactivate ) {
        qDebug() << "Window changed! obj:" << obj << " event:" << event;
        qDebug() << "new window:" << QApplication::focusWindow();
        if( QApplication::focusObject() == this->window() )
        {
            qDebug() << "Interrupting before autocomplete takes focus...";
            return true;
        }
    }

    if( obj == editorComponentRef_ && event->type()==QEvent::FocusOut && isVisible() ) {
        qDebug() << "FocusOut at" << QTime::currentTime().toString();
        qDebug() << "obj ->" << obj;
        //qDebug() << "new focus on:" << QApplication::focusWidget()->metaObject()->className();
        //qDebug() << "new obj ->" << QApplication::focusWidget();
        if( QApplication::focusWidget() == listWidgetRef_ ){
            qDebug() << "Changed obj to listWidget! Intercepting!";
            parentWidget()->setFocus();
            //editorComponentRef_->setFocus();
            //editorComponentRef_->upd
            //editorComponentRef_->keyboardGrabber()
            //editorComponentRef_->grabKeyboard();
            //event->isAccepted();
            return true;
        } else if( QApplication::focusWidget() == editorComponentRef_ ) {
            qDebug() << "Changing obj to editorComponentRef_!";
            //hide();
            //close();
            return true;
        } else {
            qDebug() << "Changing obj to something else!";
            if( QApplication::focusWidget() ){
                qDebug() << "focusWidget() exists!" << QApplication::focusWidget();
            }
            if( QApplication::focusWindow() ){
                qDebug() << "focusWindow() exists!" << QApplication::focusWindow();
            }
            if( QApplication::focusObject() ){
                qDebug() << "focusObject() exists!" << QApplication::focusObject();
            }
            if( QApplication::focusObject() ){
                if( QApplication::focusObject() == this ){
                    qDebug() << "clicked 'this'!";
                    editorComponentRef_->setFocus();
                    return true;
                } else {
                    qDebug() << "clicked something other than 'this'!";
                    hide();
                    return false;
                }
            }
            hide();
            //close();
            return false;
        }
        //qDebug() << "eventFilter fired QEvent::FocusOut. obj:" << obj->metaObject()->className();
        //abort();
        //if (d->m_infoFrame)
            //d->m_infoFrame->close();
        //close();
        //return true;
    }
    /*if (event->type() == QEvent::FocusOut) {
        //hide();
        //if (listWidgetRef_)
            //listWidgetRef_->hide();
        if (infoTipRef_)
            infoTipRef_->hide();
        return true;
    } else if if( obj == editorComponentRef_ && event->type()==QEvent::KeyPress && isVisible() ) {*/
    //if( obj == editorComponentRef_ && )
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
                if( currentWord_ == listWidgetRef_->currentItem()->text() ) { // sends normal enter/return/tab if you've typed a full word
                    hide();
                    return false;
                } else {
                    insertCurrentSelectedListItem();
                    hide();
                    return true;
                }

            case Qt::Key_Backspace:
                return QObject::eventFilter(obj, event);

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
                showInfoTip();
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

void TextEditorAutoCompleteComponent::updateList()
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
        showInfoTip();
    }
}

/// this event is called when a key pressed
void TextEditorAutoCompleteComponent::textKeyPressed()
{
    updateList();
}

/// processes backspaces
void TextEditorAutoCompleteComponent::backspacePressed()
{
    updateList();
}

void TextEditorAutoCompleteComponent::listItemClicked(QListWidgetItem* item)
{
    item->setSelected(true);
    qDebug() << "listItemClicked!";
    if( QApplication::focusWidget() ){
        qDebug() << "focusWidget() exists!" << QApplication::focusWidget();
    }
    if( QApplication::focusWindow() ){
        qDebug() << "focusWindow() exists!" << QApplication::focusWindow();
    }
    if( QApplication::focusObject() ){
        qDebug() << "focusObject() exists!" << QApplication::focusObject();
    }
    editorComponentRef_->window()->activateWindow();
    editorComponentRef_->setFocus();
    //updateList();
    showInfoTip();
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
    //TextRenderer* renderer = controller()->textRenderer();
    //TextRenderer* renderer = listWidget
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, QColor(0, 122, 204));
        //painter->fillRect(option.rect, renderer->theme()->lineHighlightColor());
    else
        painter->fillRect(option.rect, QColor(37, 37, 38));
        //painter->fillRect(option.rect, renderer->theme()->backgroundColor());
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

    QString sLabel = index.data(Qt::DisplayRole).toString();
    int sKind = index.data(Qt::DecorationRole).toInt();
    QString sDetail = index.data(Qt::UserRole).toString();
    QString sDocumentation = index.data(Qt::WhatsThisRole).toString();
    QString sType = "void";
    if (sDetail.contains(" = ")) {
        sType = sDetail.split(" = ").value(0);
        //sUsage = str.split(" = ").value(1);
    }
    //font.setBold(true);
    QRect typeRect = option.rect;
    QRect hyphenRect = option.rect;
    QRect nameRect = option.rect;
    hyphenRect.setX(hyphenRect.x() + fm.width(sLabel));
    //typeRect.setX(nameRect.x() + fm.width(sName + " : "));
    typeRect.setX(nameRect.x() + nameRect.width() - fm.width(sType) - 1);
    painter->setFont(font);
    QPen typePen = QPen(QColor(86, 156, 214));
    QPen textPen = QPen(QColor(241, 241, 241));
    QPen namePen = QPen(QColor(78, 201, 176));
    painter->setPen(textPen);
    painter->drawText(nameRect, sLabel);
    //painter->drawText(option.rect, sUsage);
    if (sType != "void")
    {
        //painter->drawText(hyphenRect, " - ");
        painter->setPen(typePen);
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
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    tipText.setHtml("");

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

void FakeToolTip::setText(const QString text)
{
    tipText.setDefaultStyleSheet("p {color:#ffffff}; h1 {color:#ff8888}");
    tipText.setHtml(QString("<p>%1</p>").arg(text));
    //tipText.setHtml("<font color=white>" + text + "</font>");
}

void FakeToolTip::paintEvent(QPaintEvent *e)
{
    QStyle *style = this->style();
    QPainter *p = new QPainter(this);
    QStyleOptionFrame *opt = new QStyleOptionFrame();
    QRect labelRect, textRect;
    opt->init(this);
    style->drawPrimitive(QStyle::PE_PanelTipLabel, opt, p);

    labelRect = this->rect();

    //labelRect.setX(labelRect.x() + 2);
    //labelRect.setY(labelRect.y() + 2);
    //labelRect.setWidth(labelRect.width() - 2 + 250);
    //labelRect.setHeight(labelRect.height() - 2 + 400);

    labelRect.setX(labelRect.x() + 2);
    labelRect.setY(labelRect.y() + 2);
    labelRect.setWidth(labelRect.width() - 2);
    labelRect.setHeight(labelRect.height() - 2);

    p->fillRect(labelRect, QColor(37, 37, 38));

    p->translate(-1, -1);
    tipText.drawContents(p, labelRect);

    p->end();
    delete p;
    delete opt;
}

void FakeToolTip::resizeEvent(QResizeEvent *)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.init(this);
    //if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
//        setMask(frameMask.region);
}

}// edbee
