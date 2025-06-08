// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textcaretcache.h"

#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/views/textrenderer.h"
#include "edbee/models/textrange.h"

#include "edbee/debug.h"

//#define STRICT_CHECK_ON_CACHE

namespace edbee {

/// The text document cache
TextCaretCache::TextCaretCache(TextDocument* doc, TextRenderer* renderer)
    : textDocumentRef_(doc)
    , textRendererRef_(renderer)
{
}


void TextCaretCache::clear()
{
    xPosCache_.clear();
}

void TextCaretCache::fill(TextRangeSet& selection)
{
    clear();
    for (size_t i = 0, cnt = selection.rangeCount(); i<cnt; ++i) {
        TextRange& range = selection.range(i);
        add(range.caret());
        if (range.hasSelection()) {
            add(range.anchor());
        }
    }
//    dump();
}

/// Replaces all cached content with the one given
void TextCaretCache::replaceAll(TextCaretCache& cache)
{
    xPosCache_ = cache.xPosCache_;
}


size_t TextCaretCache::xpos(size_t offset)
{
#ifdef STRICT_CHECK_ON_CACHE
    Q_ASSERT(xPosCache_.contains(offset));
#endif

    // when not in debug-mode fallback to calculating the position!
    if (!xPosCache_.contains(offset)) {
        add(offset);
    }
    return xPosCache_.value(offset);
}


/// Adds an xposition for the given offset
void TextCaretCache::add(size_t offset, size_t xpos)
{
    xPosCache_.insert(offset,xpos);
}


/// Adds the given offset by calculating the position
void TextCaretCache::add(size_t offset)
{
    size_t line = textDocumentRef_->lineFromOffset(offset);
    size_t col  = textDocumentRef_->columnFromOffsetAndLine(offset, line);
    size_t xpos = textRendererRef_->xPosForColumn(line, col);
    add(offset, xpos);
}

/// Should be called if the caret moves
void TextCaretCache::caretMovedFromOldOffsetToNewOffset(size_t oldOffset, size_t newOffset)
{
    Q_ASSERT(xPosCache_.contains(oldOffset));
    size_t xpos = xPosCache_.take(oldOffset);
    xPosCache_.insert(newOffset, xpos);
}


/// Checks if the cache is filled
bool TextCaretCache::isFilled()
{
    return !xPosCache_.isEmpty();
}


void TextCaretCache::dump()
{
    qlog_info() << "DUMP CARET CACHE:" << xPosCache_.size();
    auto localKeys = xPosCache_.keys();
    foreach (size_t key, localKeys) {
        qlog_info() << " - " << key << ": " << xPosCache_.value(key);
    }
}


} // edbee
