// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QVector>

#include "edbee/models/textrange.h"
#include "edbee/util/gapvector.h"
#include "edbee/models/textdocumentscopes.h"


namespace edbee {

class EDBEE_EXPORT TreeSitterTextDocumentScopes : public TextDocumentScopes
{
public:
    TreeSitterTextDocumentScopes(TextDocument* textDocument);
    virtual ~TreeSitterTextDocumentScopes();

    virtual ScopedTextRangeIterator scopedRangeAtLine(size_t line);
    virtual bool lineBasedOffsets() { return false; }

    // retreive interface (need rework)
    virtual size_t scopedLineCount() { return 0; }

    // used by actions:
    virtual TextScopeList scopesAtOffset(size_t offset, bool includeEnd = false);
    virtual QVector<ScopedTextRange*> createScopedRangesAtOffsetList(size_t offset);

    virtual QString toString();
    virtual QStringList scopesAsStringList();

    // treesiteer document scopes interface
    void giveScopedTextRange(ScopedTextRange* range);

    size_t length() { return static_cast<size_t>(scopedTextRangeList_.length()); }
    ScopedTextRange* at(size_t offset) { return scopedTextRangeList_.at(static_cast<qsizetype>(offset)); }
    void clear();
    void removeScopesWithinOffsets(size_t offsetBegin, size_t offsetEnd, size_t startIndex = 0);

    void notifyScopesChanges(size_t offset, size_t endOffset, size_t line, size_t endLine);

private:
    /// TODO: Replace this with a balanced interval tree (https://github.com/lodborg/interval-tree)
    /// An interval tree is special sort of Red Black Tree, with sorting bye rang [minoffset, maxoffset] (lower first, followed by max)
    /// For now quick naive list implemenation to make this work :-)
    //GapVector<ScopedTextRange*> scopedTextRangeList_;  ///< TODO: optimize implementation
    QList<ScopedTextRange*> scopedTextRangeList_;  ///< TODO: optimize implementation
};


} // edbee
