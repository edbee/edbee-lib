// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QList>

namespace edbee {

class TreeSitterTextGrammar;
class TextGrammarManager;

/// For parsing a Textmate Language
class EDBEE_EXPORT TreeSitterGrammarParser
{
public:
    TreeSitterGrammarParser();

    QList<TreeSitterTextGrammar*> parseTreeSitterJson(const QString& jsonFileName);

};

} // edbee
