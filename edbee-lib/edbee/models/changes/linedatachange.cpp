// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "linedatachange.h"

#include "edbee/models/textdocument.h"
#include "edbee/models/textlinedata.h"

#include "edbee/debug.h"

namespace edbee {

LineDataChange::LineDataChange(size_t line, size_t field)
    : line_(line)
    , field_(field)
    , lineData_(nullptr)
{
}


LineDataChange::~LineDataChange()
{
    delete lineData_;
}


void LineDataChange::giveLineData(TextLineData* lineData)
{
    lineData_ = lineData;
}


void LineDataChange::execute(TextDocument* doc)
{
    changeLineData(doc);
}


void LineDataChange::revert(TextDocument* doc)
{
    changeLineData(doc);
}


/// merge is never a problem, simply
bool LineDataChange::giveAndMerge(TextDocument* document, Change* textChange)
{
    Q_UNUSED(document);
    Q_UNUSED(textChange);
    LineDataChange* change = dynamic_cast<LineDataChange*>(lineData_);
    if (change) {
        if (line_ == change->line_ && field_ == change->field_) {
            delete textChange;
            return true;
        }
    }
    return false;
}


/// line is moved with the given delta
void LineDataChange::applyLineDelta(size_t line, size_t length, size_t newLength)
{
    if (line <= line_) {
        if (newLength > length) {
            line_ += newLength - length;
        } else {
            line_ -= length - newLength;
        }
    }
}


/// Returns the debug text
QString LineDataChange::toString()
{
    return QStringLiteral("LineDataChange(%1,%2)").arg(line_).arg(field_);
}


/// The change line data
void LineDataChange::changeLineData(TextDocument* doc)
{
    TextLineData* oldLineData = doc->lineDataManager()->take(line_, field_);
    doc->lineDataManager()->give(line_, field_, lineData_);
    lineData_ = oldLineData;
}


/// Returns the line index
size_t LineDataChange::line() const
{
    return line_;
}


/// Sets the line of this change
void LineDataChange::setLine(size_t line)
{
    line_ = line;
}


/// retursn the field index of this line-data item
size_t LineDataChange::field() const
{
    return field_;
}


/// sets the field position
void LineDataChange::setField(size_t field)
{
    field_ = field;
}

} // edbee
