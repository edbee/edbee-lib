// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

namespace edbee {

class TextEditorKeyMap;

/**
 * This class can fill the texteditor keymap with the factory defaults
 */
class EDBEE_EXPORT FactoryKeyMap {
public:
    void fill(TextEditorKeyMap* km );
};


} // edbee
