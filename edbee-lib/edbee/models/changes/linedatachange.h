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
    LineDataChange(int line, int field);
    virtual ~LineDataChange();

    void giveLineData( TextLineData* lineData );

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* doc);

    virtual bool giveAndMerge(TextDocument* document, Change* textChange );

    virtual void applyLineDelta( int line, int length, int newLength );

    virtual QString toString();

    int line() const;
    void setLine( int line );

    int field() const;
    void setField( int field );

private:
    void changeLineData( TextDocument* doc );

private:

    int line_;                      ///< The line number
    int field_;                     ///< The field index
    TextLineData* lineData_;        ///< The text-line data

};

} // edbee
