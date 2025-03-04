// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QHash>


namespace edbee {


class TextDocument;
class TextRenderer;
class TextRangeSet;

/// A special cache. For remembering the x-coordinates of the carets
class EDBEE_EXPORT TextCaretCache {
public:
    TextCaretCache( TextDocument* doc, TextRenderer* renderer );

    void clear();
    void fill( TextRangeSet& selection );
    void replaceAll( TextCaretCache& cache );

    int xpos( int offset );
    void add( int offset, int xpos );
    void add( int offset );

    void caretMovedFromOldOffsetToNewOffset( int oldOffset, int newOffset );

    bool isFilled();


    void dump();

private:

    TextDocument* textDocumentRef_; ///< A reference to the current buffer
    TextRenderer* textRendererRef_; ///< A reference to the renderer
//    QVector<int> xPosCache_;        ///< The xpos cache for the carets
    QHash<int,int> xPosCache_;      ///< The x-pos cache
};

} // edbee
