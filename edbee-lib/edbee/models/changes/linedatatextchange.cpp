/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "linedatatextchange.h"

#include "edbee/models/textdocument.h"
#include "edbee/models/textlinedata.h"

#include "debug.h"

namespace edbee {

LineDataTextChange::LineDataTextChange( int line, int field )
    : line_(line)
    , field_(field)
    , lineData_( 0 )
{
}

LineDataTextChange::~LineDataTextChange()
{
    delete lineData_;
}

void LineDataTextChange::giveLineData(TextLineData *lineData)
{
    lineData_ = lineData;
}

void LineDataTextChange::execute(TextDocument* doc)
{
    changeLineData( doc );
}

void LineDataTextChange::revert(TextDocument* doc)
{
    changeLineData( doc );
}

/// merge is never a problem, simply
bool LineDataTextChange::giveAndMerge(TextDocument* document, TextChange* textChange)
{
    Q_UNUSED( document );
    Q_UNUSED( textChange );
    LineDataTextChange* change =  dynamic_cast<LineDataTextChange*>(lineData_);
    if( change ) {
        if( line_ == change->line_ && field_ == change->field_ ) {
            delete textChange;
            return true;
        }
    }
    return false;
}

/// line is moved with the given delta
void LineDataTextChange::applyLineDelta(int line, int length, int newLength)
{
    if( line <= line_ ) {
        line_ += newLength - length;
    }
}

QString LineDataTextChange::toString()
{
    return QString("LineDataTextChange(%1,%2)").arg(line_).arg(field_);
}

/// The change line data
void LineDataTextChange::changeLineData(TextDocument* doc)
{
    TextLineData* oldLineData = doc->lineDataManager()->take( line_, field_ );
    doc->lineDataManager()->give( line_, field_, lineData_ );
    lineData_ = oldLineData;
}


} // edbee
