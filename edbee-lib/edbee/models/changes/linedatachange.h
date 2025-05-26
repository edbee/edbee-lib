// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include "edbee/models/change.h"

namespace edbee {

class TextDocument;
class TextLineData;

/// a class to handle line-data textchanges
class EDBEE_EXPORT LineDataChange : public Change
{
public:
    LineDataChange(size_t line, size_t field);
    virtual ~LineDataChange();

    void giveLineData(TextLineData* lineData);

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* doc);

    virtual bool giveAndMerge(TextDocument* document, Change* textChange );

    virtual void applyLineDelta(size_t line, size_t length, size_t newLength);

    virtual QString toString();

    size_t line() const;
    void setLine(size_t line);

    size_t field() const;
    void setField(size_t field);

private:

    void changeLineData(TextDocument* doc);

private:

    size_t line_;                   ///< The line number
    size_t field_;                  ///< The field index
    TextLineData* lineData_;        ///< The text-line data
};

} // edbee
