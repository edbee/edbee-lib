// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QHash>
#include <QString>
#include <QVariant>

#include "baseplistparser.h"

namespace edbee {

class TextTheme;

class EDBEE_EXPORT TmThemeParser : public BasePListParser
{
public:
    TmThemeParser();

    QColor parseThemeColor(const QString& color) const;

    TextTheme* readContent(QIODevice* device);

protected:

    void fillRuleSettings(TextTheme* theme, const QHash<QString, QVariant> &settings );
    void parseRules(TextTheme* theme, const QList<QVariant> &settings );
    TextTheme* createTheme( QVariant& data );

};

} // edbee
