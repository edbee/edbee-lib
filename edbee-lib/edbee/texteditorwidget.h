// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

// #include <QAbstractScrollArea>
#include "models/texteditorconfig.h"

#include <QStringList>
#include <QWidget>

class QResizeEvent;
class QScrollBar;

namespace edbee {

class TextDocument;
class TextEditorAutoCompleteComponent;
class TextEditorCommandMap;
class TextEditorComponent;
class TextEditorConfig;
class TextEditorController;
class TextEditorKeyMap;
class TextEditorScrollArea;
class TextMarginComponent;
class TextRenderer;
class TextSelection;


/// This is the general edbee widget
/// The core functionality of this widget is divided in several separate
/// components. (TextEditorComponent: the main editor, TextMarginComponent: the sidebar with line numbers)
class EDBEE_EXPORT TextEditorWidget : public QWidget
{
    Q_OBJECT
public:

    explicit TextEditorWidget(TextEditorController *controller, QWidget *parent = nullptr);
    explicit TextEditorWidget(TextDocument *document, QWidget *parent = nullptr);
    explicit TextEditorWidget(TextEditorConfig *config, QWidget *parent = nullptr);
    explicit TextEditorWidget(QWidget *parent = nullptr);
    virtual ~TextEditorWidget();
    
    void scrollPositionVisible(int xPosIn, int yPosIn);

    // a whole bunch of getters
    TextEditorController* controller() const;
    TextEditorConfig* config() const;
    TextEditorCommandMap* commandMap() const;
    TextEditorKeyMap* keyMap() const;
    TextDocument* textDocument() const;
    TextRenderer* textRenderer() const;
    TextSelection* textSelection() const;
    TextEditorComponent* textEditorComponent() const;
    TextMarginComponent* textMarginComponent() const;
    TextEditorScrollArea* textScrollArea() const;
    TextEditorAutoCompleteComponent* autoCompleteComponent() const;

    void resetCaretTime();
    void fullUpdate();

    QScrollBar* horizontalScrollBar() const;
    QScrollBar* verticalScrollBar() const;
    void setVerticalScrollBar(QScrollBar* scrollBar);
    void setHorizontalScrollBar(QScrollBar* scrollBar);
	int autoScrollMargin() const;
    void setAutoScrollMargin(int amount = 50);
    void setPlaceholderText(const QString& text);

    virtual bool readonly() const;
    void setReadonly(bool readonly);


protected:

    virtual void resizeEvent(QResizeEvent* event);
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void focusIn(QWidget* event);
    void focusOut(QWidget* event);

    void verticalScrollBarChanged(QScrollBar* newScrollBar);
    void horizontalScrollBarChanged(QScrollBar* newScrollBar);

protected slots:

    void connectVerticalScrollBar();
    void connectHorizontalScrollBar();

public slots:

    void scrollTopToLine(size_t line);
    virtual void updateLineAtOffset(size_t offset);
    virtual void updateAreaAroundOffset(size_t offset, int width = 8);
    virtual void updateLine(size_t line, size_t length = 1);
    virtual void updateComponents();

    virtual void updateGeometryComponents();

    virtual void updateRendererViewport();

private:

    TextEditorController* controller_;                     ///< This controller of the widget
    TextEditorScrollArea* scrollAreaRef_;                  ///< The scrollarea of the widget
    TextEditorComponent* editCompRef_;                     ///< The editor ref
    TextMarginComponent* marginCompRef_;                   ///< The margin components
    TextEditorAutoCompleteComponent* autoCompleteCompRef_; ///< The autocomplete list widget

    int autoScrollMargin_;                                 ///< Customize the autoscroll margin
    bool readonly_;                                        ///< Readonly mode
};

} // edbee
