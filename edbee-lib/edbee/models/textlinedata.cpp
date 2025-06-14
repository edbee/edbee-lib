// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textlinedata.h"

#include "edbee/models/changes/linedatalistchange.h"
#include "edbee/models/textlinedata.h"

#include "edbee/debug.h"

namespace edbee {

//-------

/// construct the text line data item
TextLineDataList::TextLineDataList()
    : lineDataList_(nullptr)
{
}

TextLineDataList::~TextLineDataList()
{
    if (lineDataList_) {
        qlog_warn() << "** Warning TextLineDataList requires manual destruction! ** ";
    }
}


/// destroys the data items
void TextLineDataList::destroy(TextLineDataManager* manager)
{
    if (lineDataList_) {
        for (size_t i = 0, cnt = manager->fieldsPerLine(); i < cnt; ++i) {
            delete lineDataList_[i];
            lineDataList_[i] = nullptr;
        }
    }
    delete[] lineDataList_;
    lineDataList_ = nullptr;
}


/// give data item to the given field position
void TextLineDataList::give(TextLineDataManager* manager, size_t field, TextLineData* dataItem)
{
    Q_ASSERT(field < manager->fieldsPerLine());
    if (!lineDataList_) { lineDataList_ = new TextLineData*[manager->fieldsPerLine()](); }
    delete lineDataList_[field];    // delete the old item
    lineDataList_[field] = dataItem;
}


/// Returns the given data item and transfers the ownership. The item in the list is set to 0
/// @param manager the manager to use
/// @param field the field index to retrieve
/// @return the TextLineData item in the given field or 0
TextLineData* TextLineDataList::take(TextLineDataManager* manager, size_t field)
{
    Q_ASSERT(field < manager->fieldsPerLine());
    Q_UNUSED(manager);
    if (!lineDataList_) { return nullptr; }
    TextLineData* dataItem = lineDataList_[field];
    lineDataList_[field] = nullptr;
    return dataItem;
}


/// Returns the given data item
/// @param manager the manager to use
/// @param field the field index to retrieve
/// @return the TextLineData item in the given field or 0
TextLineData* TextLineDataList::at(TextLineDataManager* manager, size_t field)
{
    Q_ASSERT(field < manager->fieldsPerLine());
    Q_UNUSED(manager);
    if (!lineDataList_) { return nullptr; }
    return lineDataList_[field];
}


/// This method reallocates the number of fields
void TextLineDataList::realloc(TextLineDataManager* manager, size_t oldFieldsPerLine, size_t newFieldsPerLine)
{
    TextLineData** oldData = lineDataList_;

    // copy the data
    if (oldData) {
        lineDataList_ = new TextLineData*[manager->fieldsPerLine()]();
        size_t copyCount = qMin(oldFieldsPerLine, newFieldsPerLine);
        for (size_t i = 0; i < copyCount; ++i ) {
            lineDataList_[i] = oldData[i];
            oldData[i] = nullptr;
        }

        for (size_t i = copyCount; i < oldFieldsPerLine; ++i) {
            delete oldData[i];
            oldData[i] = nullptr;
        }
        delete[] oldData;
    }
}


//-------


TextLineDataManager::TextLineDataManager(size_t fieldsPerLine)
    : fieldsPerLine_(fieldsPerLine)
    , textLineDataList_(2)
{
    textLineDataList_.setGrowSize(2);
    // add a 0 element
    textLineDataList_.fill(0, 0, 0, 1);
}


TextLineDataManager::~TextLineDataManager()
{
    clear();
}


/// this method clears all field items
void TextLineDataManager::clear()
{
    for (size_t i=0, cnt = textLineDataList_.length(); i < cnt; ++i) {
        TextLineDataList* list = textLineDataList_.at(i);
        if (list) {
            list->destroy(this);
            delete list;
        }
    }
    textLineDataList_.clear();
    textLineDataList_.fill(0,0,0,1);
}


/// this method gives the given data at the given line and field
void TextLineDataManager::give(size_t line, size_t field, TextLineData* dataItem)
{
    Q_ASSERT(field < fieldsPerLine_);
    TextLineDataList* list = textLineDataList_.at(line);
    if (!list) { textLineDataList_.set(line, list = new TextLineDataList()); }
    list->give(this, field, dataItem);
    emit lineDataChanged(line, 1, 1);
}


TextLineData* TextLineDataManager::take(size_t line, size_t field)
{
    Q_ASSERT(field<fieldsPerLine_);
    TextLineDataList* list = textLineDataList_.at(line);
    if (list) { return list->take(this, field); }
    return nullptr;
}


TextLineData* TextLineDataManager::get(size_t line,size_t field)
{
    Q_ASSERT(field<fieldsPerLine_);
    TextLineDataList* list = textLineDataList_.at(line);
    if (list) { return list->at(this, field); }
    return nullptr;
}


/// Can be used to change the number of reserved fields by the document
/// Increasing the amount will result in a realoc
/// Decreasting the fieldcount reults in the lost of the 'old' fields
/// At least the 'PredefinedFieldCount' amont of fields are required
void TextLineDataManager::setFieldsPerLine(size_t count)
{
    Q_ASSERT(count >= PredefinedFieldCount);
    for (size_t i = 0, cnt = textLineDataList_.length(); i < cnt; ++i) {
        TextLineDataList* list = textLineDataList_.at(i);
        if (list) {
            list->realloc(this, fieldsPerLine_, count);
        }
    }
    fieldsPerLine_ = count;
}


/// Creates a new lines replace change
Change* TextLineDataManager::createLinesReplacedChange(size_t lineStart, size_t lineCount, size_t newLineCount )
{
    // no changes
    if(lineCount == 0 && lineCount == newLineCount) { return nullptr; }

    LineDataListChange* change = new LineDataListChange(this, lineStart, lineCount, newLineCount);
    return change;
}


/// Takes the given list (and replaces it with a wnullptr)
TextLineDataList* TextLineDataManager::takeList(size_t line)
{
    TextLineDataList* list = textLineDataList_[line];
    textLineDataList_[line] = nullptr;
    return list;
}


/// Gives a list to the given line
/// emits a lineDataChanged signal
void TextLineDataManager::giveList(size_t line, TextLineDataList* newList)
{
    // delete the old list if required
    TextLineDataList* list = textLineDataList_[line];
    if (list) {
        list->destroy(this);
        delete list;
    }
    textLineDataList_[line] = newList;
    emit lineDataChanged(line, 1, 1);
}


/// Replace the given area with no-data
/// emits a lineDataChanged signal
void TextLineDataManager::fillWithEmpty(size_t line, size_t length, size_t newLength)
{

    destroyRange(line, length);
    textLineDataList_.fill(line, length, 0, newLength);
    emit lineDataChanged(line, length, newLength);
}


/// Replace the given items with
/// emits a lineDataChanged signal
void TextLineDataManager::replace(size_t line, size_t length, TextLineDataList** items, size_t newLength)
{
    destroyRange(line, length);
    textLineDataList_.replace(line, length, items, newLength);
    emit lineDataChanged(line, length, newLength);
}


/// destroys all items in the given range
void TextLineDataManager::destroyRange(size_t line, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        TextLineDataList* list = takeList(line + i);
        if (list) {
            list->destroy(this);
            delete list;
        }
    }
}


} // edbee
