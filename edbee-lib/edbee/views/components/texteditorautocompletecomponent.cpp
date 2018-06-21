#include "texteditorautocompletecomponent.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLayout>
#include <QHBoxLayout>
#include <QWidgetAction>
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
    : QWidget(parent)//: QWidget(parent, Qt::ToolTip | Qt::WindowStaysOnTopHint)
    , controllerRef_(controller)
    , menuRef_(nullptr)
    , editorComponentRef_(parent)
    , eventBeingFiltered_(false)
    , infoTipRef_(nullptr)
{
    /// initialize the widget
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->setSpacing(0);
    layout->setMargin(0);

    menuRef_ = new QMenu();
    listWidgetRef_ = new QListWidget(menuRef_);
    listWidgetRef_->installEventFilter(this);
    menuRef_->installEventFilter(this);
    menuRef_->setStyleSheet("QMenu { border: 1px solid black; }");
    listWidgetRef_->setObjectName("listWidgetRef");
    //layout->addWidget(listWidgetRef_);
    setLayout(layout);
    resize(0, 0);

    QWidgetAction *wAction = new QWidgetAction(menuRef_);
    wAction->setDefaultWidget(listWidgetRef_);
    menuRef_->addAction(wAction);

    listWidgetRef_->setFocus();
    //menuRef_->setFocus();
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

    //editorComponentRef_->installEventFilter(this);

    // prevent the widgets from having foxus
    //setFocusProxy(editorComponentRef_);
    //setFocusPolicy(Qt::NoFocus);
    //SSRlistWidgetRef_->setParent(0);
    //listWidgetRef_->setWindowFlags(Qt::ToolTip);
    //listWidgetRef_->setFocusPolicy(Qt::NoFocus);

    //listWidgetRef_->setFocusPolicy(Qt::ClickFocus);
    //listWidgetRef_->setFocusProxy(editorComponentRef_);

    listWidgetRef_->setAttribute(Qt::WA_NoMousePropagation,true);   // do not wheel
    listWidgetRef_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    listWidgetRef_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidgetRef_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    AutoCompleteDelegate *acDel = new AutoCompleteDelegate(controllerRef_, this);
    listWidgetRef_->setItemDelegate(acDel);

    // make clicking in the list word
    listWidgetRef_->setMouseTracking(true);
    connect( listWidgetRef_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listItemClicked(QListWidgetItem*)));
    connect( listWidgetRef_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listItemDoubleClicked(QListWidgetItem*)));
    //connect( listWidgetRef_, SIGNAL(currentRowChanged(int)), this, SLOT(showToolTip()));
    //connect( listWidgetRef_, SIGNAL(itemSelectionChanged()), this, SLOT(showToolTip()));
    connect( listWidgetRef_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(showInfoTip()));
    //connect( listWidgetRef_, SIGNAL(entered(QModelIndex)), this, SLOT(selectItemOnHover(QModelIndex)));
}


/// Returns the current text editor controller
TextEditorController *TextEditorAutoCompleteComponent::controller() const
{
    return controllerRef_;
}

QSize TextEditorAutoCompleteComponent::sizeHint() const
{
    if(!listWidgetRef_) return QSize();
    //return QSize();
    //return QSize(350, ( qMin(listWidgetRef_->count(), 10) * 15 ) + 5);

    const QFont font = controller()->textDocument()->config()->font();
    QFontMetrics fm(font);
    return QSize(350, ( qMin(listWidgetRef_->count(), 10) * fm.height() ) + 4 );

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
        infoTipRef_ = new FakeToolTip(controller(), this);


    //infoTipRef_->move(listWidgetRef_->x() + listWidgetRef_->width() + 2, listWidgetRef_->y());// (m_completionListView->infoFramePos());
    infoTipRef_->setText(infoTip);
    infoTipRef_->update();

    const QFont font = controller()->textDocument()->config()->font();
    QFontMetrics fm(font);
    menuRef_->resize(listWidgetRef_->width(), listWidgetRef_->height());
    //menuRef_->resize(QSize(350, ( qMin(listWidgetRef_->count(), 10) * fm.height() ) + 4 ));

    QRect r = listWidgetRef_->visualItemRect(listWidgetRef_->currentItem());
    int xOffset;
    if( listWidgetRef_->count() > 10 )
        xOffset = 22+1;
    else
        xOffset = 5+1;

    QPoint p(listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).x() + xOffset, listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).y());

    infoTipRef_->move(p);

//    infoTipRef_->calculateMaximumWidth();
    //infoTipRef_->adjustSize();
    //infoTipRef_->resize(350, 19);
    QSize tipSize = infoTipRef_->tipText.documentLayout()->documentSize().toSize();
    //infoTipRef_->tipText.documentLayout()->documentSize()
    infoTipRef_->resize(tipSize.width(), tipSize.height() - 4);
    infoTipRef_->setContentsMargins(QMargins());
    //infoTipRef_->
    //infoTipRef_->setStyleSheet("border: 0px");
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
    QFont font = controller()->textDocument()->config()->font();
    QFontMetrics fm(font);
    setFixedHeight( ( qMin(listWidgetRef_->count(), 10) * fm.height() ) + 4 );
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
    //QPoint newLoc = listWidgetRef_->parentWidget()->parentWidget()->mapToGlobal(QPoint(x, y));
    QPoint newLoc = editorComponentRef_->parentWidget()->parentWidget()->mapToGlobal(QPoint(x, y));
    //QPoint p(listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).x() + xOffset, listWidgetRef_->parentWidget()->mapToGlobal(r.topRight()).y());

    // TODO: Better position it so it fits no screen
    menuRef_->move(newLoc.x(), newLoc.y());
}

/// intercepts hide() calls to inform the tooltip to hide as well
void TextEditorAutoCompleteComponent::hideEvent(QHideEvent *event)
{
//    listWidgetRef_->hide();
    qDebug() << "void TextEditorAutoCompleteComponent::hideEvent(QHideEvent *event)" << event;
    infoTipRef_->hide();
    //menuRef_->close();
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
    //qDebug() << "obj->" << obj << "event ->" << event;
    //if( QApplication::focusWidget() )
        //qDebug() << QApplication::focusWidget()->metaObject()->className();
    /*if( QApplication::focusWindow() )
    {
        qDebug() << " window:" << QApplication::focusWindow()->objectName();
        qDebug() << "Currentfocus is on obj:" << QApplication::focusObject()->objectName();
        qDebug() << " widget:" << QApplication::focusWidget()->metaObject()->className();
    }*/
    //QEvent::ApplicationDeactivated
    if( event->type() == QEvent::Close && obj == menuRef_) {
        //if( event->type() == QEvent::Close && obj == menuRef_) {
        //qDebug() << "if( event->type() == QEvent::Close && obj == menuRef_) {";
        hide();
        hideInfoTip();
        return QObject::eventFilter(obj, event);
    /*} else if( event->type() == QEvent::Close ) {
        qDebug() << "} else if( event->type() == QEvent::Close ) {";
        //hide();
        //return false;*/
    }

    /*if( event->type() == QEvent::WindowDeactivate ) {
        qDebug() << "Window changed! obj:" << obj << " event:" << event;
        qDebug() << "new window:" << QApplication::focusWindow();
        if( QApplication::focusObject() == this->window() )
        {
            qDebug() << "Interrupting before autocomplete takes focus...";
            return true;
        }
    }

    if( event->type() == QEvent::Move ) {
        qDebug() << "Window moved!";
        hide();
    }

    if( event->type() == QEvent::MouseButtonPress ) {
        qDebug() << "Pressed mouse OUTSIDE of list widget!";
        hide();
    }*/

    /*if( obj == editorComponentRef_ && event->type()==QEvent::FocusOut && isVisible() ) {
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
    }*/

    /*if (event->type() == QEvent::FocusOut) {
        //hide();
        //if (listWidgetRef_)
            //listWidgetRef_->hide();
        if (infoTipRef_)
            infoTipRef_->hide();
        return true;
    } else if if( obj == editorComponentRef_ && event->type()==QEvent::KeyPress && isVisible() ) {*/
    //if( obj == editorComponentRef_ && )
    //if( obj == editorComponentRef_ && event->type()==QEvent::KeyPress && isVisible() ) {
    //if( obj == menuRef_ && event->type()==QEvent::KeyPress && isVisible() ) {
    /*if( obj == listWidgetRef_ && event->type()==QEvent::KeyPress )
        qDebug() << "if( obj == listWidgetRef_ && event->type()==QEvent::KeyPress )";*/

    //if( obj == listWidgetRef_ && event->type()==QEvent::KeyPress && isVisible() ) {
    if( obj == listWidgetRef_ && event->type()==QEvent::KeyPress ) {
        //qDebug() << "if( obj == listWidgetRef_ && event->type()==QEvent::KeyPress && isVisible() ) {";
        QKeyEvent* key = static_cast<QKeyEvent*>(event);

        // text keys are allowed
        if( !key->text().isEmpty() ) {
            QChar nextChar = key->text().at(0);
            if( nextChar.isLetterOrNumber() ) {
              QApplication::sendEvent(editorComponentRef_, event);
              return true;
              //return QObject::eventFilter(obj, event);
            }
        }

        // escape key
        switch( key->key() ) {
            case Qt::Key_Escape:
                qDebug() << "Escape";
                menuRef_->close();//hide();
                return true; // stop event

            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Tab:
                qDebug() << "Ent/Ret/Tab";
                if( currentWord_ == listWidgetRef_->currentItem()->text() ) { // sends normal enter/return/tab if you've typed a full word
                    menuRef_->close();//hide();
                    QApplication::sendEvent(editorComponentRef_, event);
                    return true;
                    //return false;
                } else {
                    insertCurrentSelectedListItem();
                    menuRef_->close();//hide();
                    return true;
                }

            case Qt::Key_Backspace:
                qDebug() << "Backspace";
                QApplication::sendEvent(editorComponentRef_, event);
                return true;
                //return QObject::eventFilter(obj, event);

            case Qt::Key_Shift: //ignore shift, don't hide
                qDebug() << "Shift";
                QApplication::sendEvent(editorComponentRef_, event);
                return true;
                //return false;

            // forward special keys to list
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_PageDown:
            case Qt::Key_PageUp:
                //mousePressEvent(event);
                //listWidgetRef_->keyPressEvent(key);
                //showInfoTip();
                return false;
                //return QObject::eventFilter(obj, event);
                /*qDebug() << "Up/Down/PGUp/PGDown";
                if( !eventBeingFiltered_ ) {
                    qDebug() << "notbeingfiltered";
                    //return QObject::eventFilter(obj, event);
                    eventBeingFiltered_ = true;
                    //QApplication::sendEvent(listWidgetRef_, event);
                    return false;
                    return QObject::eventFilter(obj, event);
                }*/
                //eventBeingFiltered_ = false;
                //showInfoTip();
                //return false;
        }

        // default operation is to hide and continue the event
        menuRef_->close();//hide();
        QApplication::sendEvent(editorComponentRef_, event);
        return true;
        //return false; // continue event

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
      menuRef_->close();//hide();
      return;
    }

    // position the widget
    positionWidgetForCaretOffset( qMax(0,range.caret() - currentWord_.length()) );

    // fills the autocomplete list with the curent word
    if( fillAutoCompleteList(doc, range, currentWord_)) {
        /*int offset = qMax(0,range.caret() - currentWord_.length());
        TextRenderer* renderer = controller()->textRenderer();
        int y = renderer->yPosForOffset(offset) + renderer->lineHeight();
        int x = renderer->xPosForOffset(offset);
        QPoint newLoc = listWidgetRef_->parentWidget()->parentWidget()->mapToGlobal(QPoint(x, y));*/
        //menuRef_->popup(newLoc);
        menuRef_->popup(menuRef_->pos());
        //show();
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
    item->setSelected(true);/*
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
    //updateList();*/
    showInfoTip();
    //item->setToolTip(item->toolTip());
}

void TextEditorAutoCompleteComponent::listItemDoubleClicked(QListWidgetItem* item)
{
    insertCurrentSelectedListItem();
    menuRef_->close();//hide();
}

void TextEditorAutoCompleteComponent::selectItemOnHover(QModelIndex modelIndex)
{
    listWidgetRef_->selectionModel()->select(modelIndex,QItemSelectionModel::SelectCurrent);
}

AutoCompleteDelegate::AutoCompleteDelegate(TextEditorController *controller, QObject *parent) : QAbstractItemDelegate(parent)
{
    pixelSize = 12;
    controllerRef_ = controller;
}

TextEditorController *AutoCompleteDelegate::controller() const
{
    return controllerRef_;
}

void AutoCompleteDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    //Edbee::themeManager() edbee::Edbee::instance()->themeManager();
    //TextThemeManager* tm = edbee::Edbee::instance()->themeManager();
           //    themeManager = edbee->themeManager();
    //if controller()->textRenderer()->
    TextRenderer* renderer = controller()->textRenderer();
    TextTheme* themeRef_ = renderer->theme();
    //painter->setBackground( th
    //themeRef_->backgroundColor() );
    painter->setPen( themeRef_->caretColor() );//themeRef_->caretColor() );//themeRef_->foregroundColor() );
    //painter->fillRect( *renderer()->clipRect(), themeRef_->backgroundColor() );
    //QFont font("Consolas", 9);
     //QFont font = TextEditorConfig::font();
    QFont font = controller()->textDocument()->config()->font();
    QFontMetrics fm(font);
    //pHost->mDisplayFont;
    //TextRenderer* renderer = controller()->textRenderer();
    //TextRenderer* renderer = listWidget
    if (option.state & QStyle::State_Selected)
        //painter->fillRect(option.rect, QColor(0, 122, 204));
        painter->fillRect(option.rect, renderer->theme()->selectionColor());
    else
        //painter->fillRect(option.rect, QColor(37, 37, 38));
        painter->fillRect(option.rect, renderer->theme()->backgroundColor());
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    //painter->setPen(Qt::P);
    //if (option.state & QStyle::State_Selected)
        //painter->setBrush(option.palette.highlightedText());
        // //painter->setPen(themeRef_->findHighlightForegroundColor());
        //painter->setPen(themeRef_->findHighlightForegroundColor());
        //painter->setBrush(themeRef_->findHighlightForegroundColor());
        //painter->setBrush(themeRef_->caretColor());
    //else
        painter->setPen(themeRef_->foregroundColor());
        //painter->setBrush(option.palette.text());
        //painter->setBrush(themeRef_->foregroundColor());
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
    //QPen typePen = QPen(QColor(86, 156, 214));
    QPen typePen = QPen(themeRef_->findHighlightForegroundColor());
    //QPen textPen = QPen(themeRef_->findHighlightForegroundColor());
    //QPen textPen = QPen(QColor(241, 241, 241));
    //QPen namePen = QPen(QColor(78, 201, 176));
    QPen namePen = QPen(themeRef_->foregroundColor());
    //painter->setPen(namePen);
    painter->drawText(nameRect, sLabel);
    //painter->drawText(option.rect, sUsage);
    if (sType != "void")
    {
        //painter->drawText(hyphenRect, " - ");
        //painter->setPen(typePen);
        painter->drawText(typeRect, sType);
    }
    painter->restore();
}

QSize AutoCompleteDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                              const QModelIndex &  index ) const
{
    //return QSize(400, 15);
    //return QSize(100, 15);
    const QFont font = controller()->textDocument()->config()->font();
    QFontMetrics fm(font);
    return QSize(100, ( fm.height() ) );
}

FakeToolTip::FakeToolTip(TextEditorController *controller, QWidget *parent) :
    QWidget(parent, Qt::ToolTip | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint)
{
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    tipText.setHtml("");

    controllerRef_ = controller;

    // Set the window and button text to the tooltip text color, since this
    // widget draws the background as a tooltip.
    QPalette p = palette();
    const QColor toolTipTextColor = p.color(QPalette::Inactive, QPalette::ToolTipText);
    p.setColor(QPalette::Inactive, QPalette::WindowText, toolTipTextColor);
    p.setColor(QPalette::Inactive, QPalette::ButtonText, toolTipTextColor);
    setPalette(p);

    //const int margin = 1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this);
    //setContentsMargins(margin + 1, margin, margin, margin);
    setContentsMargins(0, 0, 0, 0);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / 255.0);
}

void FakeToolTip::setText(const QString text)
{
    TextRenderer* renderer = controller()->textRenderer();
    TextTheme* themeRef_ = renderer->theme();
    tipText.setDefaultStyleSheet("p {color:" + themeRef_->foregroundColor().name() + "}");
    tipText.setHtml(QString("<p>%1</p>").arg(text));
    //tipText.setHtml(QString("%1").arg(text));
    //tipText.setHtml("<font color=white>" + text + "</font>");
}

TextEditorController *FakeToolTip::controller()
{
    return controllerRef_;
}

void FakeToolTip::paintEvent(QPaintEvent *e)
{
    QStyle *style = this->style();
    QPainter *p = new QPainter(this);
    QStyleOptionFrame *opt = new QStyleOptionFrame();
    QRect labelRect, textRect;
    //controller()->textRenderer();
    TextRenderer* renderer = controller()->textRenderer();
    TextTheme* themeRef_ = renderer->theme();
    opt->init(this);
    style->drawPrimitive(QStyle::PE_PanelTipLabel, opt, p);

    tipText.setDefaultFont(controller()->textDocument()->config()->font());

    labelRect = this->rect();

    //labelRect.setX(labelRect.x() + 2);
    //labelRect.setY(labelRect.y() + 2);
    //labelRect.setWidth(labelRect.width() - 2 + 250);
    //labelRect.setHeight(labelRect.height() - 2 + 400);

    labelRect.setX(labelRect.x() + 2);
    labelRect.setY(labelRect.y() + 2);
    labelRect.setWidth(labelRect.width() - 2);
    labelRect.setHeight(labelRect.height() - 2);

    p->fillRect(labelRect, themeRef_->backgroundColor());//renderer->theme()->backgroundColor()//QColor(37, 37, 38));

    p->translate(-1, -1);

    p->setPen(themeRef_->foregroundColor());
    //tipText.setDefaultStyleSheet();
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
