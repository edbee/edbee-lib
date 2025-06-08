// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QCache>
#include <QObject>
#include <QHash>
#include <QRect>


#include "edbee/models/textbuffer.h"

class QPainter;
class QRect;

namespace edbee {

class AbstractComponent;
class TextDocument;
class TextEditorConfig;
class TextEditorController;
class TextEditorWidget;
class TextRangeSet;
class TextSelection;
class TextTheme;
class TextThemeStyler;
class TextLayout;

/// A class for rendering the text
/// TODO: Currently this class is also used for positioning text. This probably should be moved in a class of its own
class EDBEE_EXPORT TextRenderer : public QObject
{
Q_OBJECT

public:
    TextRenderer( TextEditorController* controller );
    virtual ~TextRenderer();
    virtual void init();
    virtual void reset();

// calculation functions
    size_t lineHeight();
    size_t rawLineIndexForYpos(size_t y);
    size_t lineIndexForYpos(size_t y);
    int totalWidth();
    int totalHeight();
    int emWidth();
    int nrWidth();
    int viewHeightInLines();
    int firstVisibleLine();

    int columnIndexForXpos( int line, int x );
    int xPosForColumn( int line, int column );
    size_t xPosForOffset(size_t offset);
    size_t yPosForLine(size_t line);
    size_t yPosForOffset(size_t offset);

// Document access functions (Document vs Placeholder text)
    int lineCount();
    QString getLine(int index);

// caching
    TextLayout* textLayoutForLine(size_t line);
    TextLayout* textLayoutForLineForPlaceholder(size_t line);
    TextLayout* textLayoutForLineNormal(size_t line);

// rendering
    void renderBegin(const QRect& rect );
    void renderEnd( const QRect& rect );

// getters / setters
    TextDocument* textDocument();
    TextDocument* placeholderTextDocument();
    TextSelection *textSelection();
    TextEditorConfig* config();
    TextEditorController* controller();
    TextEditorWidget* textWidget();

    void setViewport( const QRect& viewport );

    void resetCaretTime();
    bool shouldRenderCaret();
    bool isCaretVisible();
    void setCaretVisible(bool visible);

    QRect viewport() { return viewport_; }
    int viewportX() { return viewport_.x(); }
    int viewportY(){ return viewport_.y(); }
    int viewportWidth() { return viewport_.width(); }
    int viewportHeight() { return viewport_.height(); }

// theme support
    TextThemeStyler* themeStyler() { return textThemeStyler_; }

    QString themeName() const;
    TextTheme* theme();
    void setThemeByName( const QString& name );
    void setTheme( TextTheme* theme );

// temporary getters only valid while rendering!!
    const QRect* clipRect() { return clipRectRef_; } ///< This method is valid only while rendering!
    size_t startOffset() { return startOffset_; }  ///< This method is valid only while rendering!
    size_t endOffset() { return endOffset_; }      ///< This method is valid only while rendering!
    size_t startLine() { return startLine_; }      ///< This method is valid only while rendering!
    size_t endLine() { return endLine_; }          ///< This method is valid only while rendering!

private:
    void updateWidthCacheForRange(int offset, int length);

protected slots:

    void textDocumentChanged(edbee::TextDocument* oldDocument, edbee::TextDocument* newDocument);
    void textChanged(edbee::TextBufferChange change, QString oldText = QString());

    void lastScopedOffsetChanged(size_t previousOffset, size_t newOffset);

public slots:

    void invalidateTextLayoutCaches(size_t fromLine = 0);
    void invalidateCaches();

signals:
    void themeChanged( TextTheme* theme );


private:

    TextEditorController* controllerRef_;   ///< A reference to the widget it is rendering
    qint64 caretTime_;                      ///< The current time of the caret. -1 means that the caret is disabled
    qint64 caretBlinkRate_;                 ///< The caret blink rate

    QCache<size_t, TextLayout> cachedTextLayoutList_;   ///< A list of cached text layouts

    QRect viewport_;                    ///< The current (total) viewport. (This is updated from the window)
    int totalWidthCache_;               ///< The total width cache

    TextThemeStyler* textThemeStyler_;  ///< The current theme styler

    // temporary variables only valid the int the current context
    const QRect* clipRectRef_; ///< A reference to the clipping rectangle
    size_t startOffset_;       ///< The start offset that needs rendering
    size_t endOffset_;         ///< The end opffset that needs rendering
    size_t startLine_;         ///< The first line that needs rendering
    size_t endLine_;           ///< The last line that needs rendering

    TextDocument* placeHolderDocument_;
};

} // edbee
