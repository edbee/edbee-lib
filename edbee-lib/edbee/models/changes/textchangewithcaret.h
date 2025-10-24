// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "textchange.h"

namespace edbee {

/// A single text-change with an extra offset which represents the location of the caret.
/// Currently this class is passed a TextDocumentFilter which enables it to alter the caret position
/// I'm in doubt if we really need to implement it this way.
class EDBEE_EXPORT TextChangeWithCaret : public TextChange
{
public:
    TextChangeWithCaret(size_t offset, size_t length, const QString& text, size_t caret);

    size_t caret() const ;
    void setCaret(size_t caret);

private:
    size_t caret_;         ///< The new cret
};

} // edbee
