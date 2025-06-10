// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QWidget>

class QEvent;
class QFont;
class QLinearGradient;
class QMouseEvent;
class QPainter;

namespace edbee {

class TextEditorWidget;
class TextMarginComponent;
class TextRenderer;
class TextTheme;

/// The textmargin component delegate
/// You can override the methods in the class for adding functionality to the text-margin component
class EDBEE_EXPORT TextMarginComponentDelegate {
public:
    TextMarginComponentDelegate();
    virtual ~TextMarginComponentDelegate() {}

    virtual QString lineText(size_t line);

    virtual int widthBeforeLineNumber();
    virtual void renderBefore(QPainter* painter, size_t startLine, size_t endLine, int width);
    virtual void renderAfter(QPainter* painter, size_t startLine, size_t endLine, int width);

    virtual bool requiresMouseTracking();
    virtual void mouseMoveEvent(size_t line, QMouseEvent* event);
    virtual void mousePressEvent(size_t line, QMouseEvent* event);
    virtual void leaveEvent( QEvent* event );

    TextMarginComponent* marginComponent() { return marginComponentRef_; }
    void setMarginCompenent(TextMarginComponent* comp) { marginComponentRef_ = comp; }

private:
    TextMarginComponent* marginComponentRef_;  ///< A reference to the margincomponent
    size_t startLine_;                         ///< The line which starts the selection
};


/// The margin/line-number component
/// This class is used for rendering line-numbers etc
class EDBEE_EXPORT TextMarginComponent : public QWidget
{
    Q_OBJECT

public:
    TextMarginComponent(TextEditorWidget* editorWidget, QWidget* parent);
    virtual ~TextMarginComponent();

    void init();
    void updateMarginFont();

    int widthHint() const;
    virtual QSize sizeHint() const;
    bool isGeometryChangeRequired();

    void fullUpdate();
    TextEditorWidget* editorWidget() const { return editorRef_;}
    TextRenderer* renderer() const;

    TextMarginComponentDelegate* delegate() const { return delegateRef_; }
    void setDelegate(TextMarginComponentDelegate* delegate);
    void giveDelegate(TextMarginComponentDelegate* delegate);

protected:

    virtual void paintEvent(QPaintEvent* event);
    virtual void renderCaretMarkers(QPainter* painter, size_t startLine, size_t endLine, int width);
    virtual void renderLineNumber(QPainter* painter, size_t startLine, size_t endLine, int width);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void leaveEvent(QEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

public:
    virtual void updateLineAtOffset(size_t offset);
    virtual void updateLine(size_t line, size_t length);

protected slots:

    virtual void topChanged(int value);
    virtual void connectScrollBar();
    virtual void updateFont();

private:


    /// the width of the sidebar
    int top_;                                   ///< The first pixel that needs to been shown
    mutable int width_;
    mutable size_t lastLineCount_;
    QFont* marginFont_;                         ///< the font used for the margin
    TextEditorWidget* editorRef_;               ///< The text-editor widget
    TextMarginComponentDelegate* delegate_;     ///< The 'owned' text delegate
    TextMarginComponentDelegate* delegateRef_;  ///< The delegate reference
};

} // edbee
