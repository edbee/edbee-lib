// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QVector>

#include "edbee/models/changes/abstractrangedchange.h"

namespace edbee {

class TextLineDataManager;
class TextLineDataList;

/// A full line data text change. This means the growing or shrinking of the line data buffer
/// It stores the old-data list that needs to be remebered for undoing
class EDBEE_EXPORT LineDataListChange : public AbstractRangedChange
{
public:
    LineDataListChange(TextLineDataManager* manager, size_t offset, size_t lenght, size_t newLength);
    virtual ~LineDataListChange();

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* doc);

    virtual void mergeStoredData(AbstractRangedChange* change);
    virtual bool giveAndMerge(TextDocument* document, Change* textChange);

    virtual QString toString();

    size_t offset() const;
    void setOffset(size_t value);

    virtual size_t docLength() const;
    void setDocLength(size_t value);

    virtual size_t storedLength() const;

    TextLineDataList** oldListList();
    size_t oldListListLength();

private:

    TextLineDataManager* managerRef_;         ///< A reference to the manager
    size_t offset_;                           ///< The line number start
    size_t docLength_;                        ///< The number of new items (they all will be 0)

    TextLineDataList** oldListList_;          /// The lists of items (old items)
    size_t contentLength_;                    ///< The number of elements in the oldListList
};

} // edbee
