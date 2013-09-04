/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "singletextchange.h"

namespace edbee {

/// A single text-change with an extra Offset which represents the location
/// of the caret
class SingleTextChangeWithCaret : public SingleTextChange
{
public:
    SingleTextChangeWithCaret( int offset, int length, const QString& text, int caret );

    int caret() { return caret_; }
    void setCaret( int caret ) { caret_ = caret; }


private:
    int caret_;         ///< The new cret
};

} // edbee
