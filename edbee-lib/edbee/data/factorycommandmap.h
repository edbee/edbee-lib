// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

namespace edbee {

class TextEditorCommandMap;


/// This class contains the factory edbee command map
class EDBEE_EXPORT FactoryCommandMap {
public:
    void fill(TextEditorCommandMap* cm );
};

} // edbee
