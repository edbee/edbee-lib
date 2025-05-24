// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QVector>

#include "gapvector.h"


namespace edbee {

class TextBufferChange;



/// Implements the vector for storing the line numbers at certain offsets
/// The class allows the 'gap' position to contain a delta offset. Which means that
///
/// all offsets after the gap are increased with the offsetDelta when searched. Inserting/deleting
/// text this way usually only results in the changine of the offset delta. Which means speeeed
///
/// The line offset pointed at by each index is the first character in the given line.
class EDBEE_EXPORT LineOffsetVector {
public:
/// a structure to describe the line change that happend
//    struct LineChange {
//        int line;                   ///< the first line that's going to be replaced
//        int lineCount;              ///< the number of lines that are replaced
//        int newLineCount;           ///< the number of lines added/removed
//        int offsetDelta;            ///< the change in offset
//        QVector<int> newOffsets;    ///< the new offsets
//    };


    LineOffsetVector();

    void applyChange( TextBufferChange change );

    size_t at(size_t idx) const;
    size_t length() const;

    size_t findLineFromOffset(size_t offset);

    ptrdiff_t offsetDelta() { return offsetDelta_; }
    size_t offsetDeltaIndex() { return offsetDeltaIndex_; }

    void appendOffset(size_t offset);

    /// TODO: temporary method (remove)
    GapVector<size_t> & offsetList() { return offsetList_; }

protected:
    size_t searchOffsetIgnoringOffsetDelta(size_t offset, size_t org_start, size_t org_end );

    void moveDeltaToIndex(size_t index);
    void changeOffsetDelta(size_t index, std::ptrdiff_t delta);

public:
    QString toUnitTestString();
    void initForUnitTesting(size_t offsetDelta, size_t offsetDeltaIndex, ...);

private:

    GapVector<size_t> offsetList_; ///< All offsets
    ptrdiff_t offsetDelta_;        ///< The offset delta at the given offset index
    size_t offsetDeltaIndex_;      ///< The index that contains the offset delta


friend class LineOffsetVectorTest;


};

} // edbee
