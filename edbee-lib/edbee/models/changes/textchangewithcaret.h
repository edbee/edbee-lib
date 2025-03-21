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
    TextChangeWithCaret( int offset, int length, const QString& text, int caret );

    int caret() const ;
    void setCaret( int caret );

private:
    int caret_;         ///< The new cret
};

} // edbee
