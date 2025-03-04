// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QScrollArea>

class QLinearGradient;
class QPainter;

namespace edbee {

class PrivateShadowWidget;

/// A special scrollarea that makes it possible to add components to the margins of it
class EDBEE_EXPORT TextEditorScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit TextEditorScrollArea(QWidget* parent = 0);
    virtual ~TextEditorScrollArea();

    void setLeftWidget(QWidget* widget);
    void setTopWidget(QWidget* widget);
    void setRightWidget(QWidget* widget);
    void setBottomWidget(QWidget* widget);

public:

    void layoutMarginWidgets();
    void enableShadowWidget(bool enabled);

protected:
    virtual void resizeEvent(QResizeEvent* event);

private:

    QWidget* leftWidgetRef_;               ///< The left widget
    QWidget* topWidgetRef_;                ///< The top widget
    QWidget* rightWidgetRef_;              ///< The right widget
    QWidget* bottomWidgetRef_;             ///< The bottom widget
    PrivateShadowWidget* shadowWidgetRef_; ///< The private shadow widget

};

} // edbee
