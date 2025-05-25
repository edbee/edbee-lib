// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QObject>
#include <QTextLayout>

#include "edbee/util/gapvector.h"

namespace edbee {

enum TextLineDataPredefinedFields {
    LineTextScopesField=0,
    // LineDataMarkers,      /// Bookmarks etc
    LineAppendTextLayoutFormatListField=1,
    PredefinedFieldCount=2
};


class Change;
class TextLineDataManager;


/// A text line item reference
class EDBEE_EXPORT TextLineData {
public:
    TextLineData() {}
    virtual ~TextLineData() {}
};

/// a simple class to store a QString in a line
template<typename T>
class EDBEE_EXPORT BasicTextLineData : public TextLineData
{
public:
    BasicTextLineData(const T& val) : value_(val) {}
    T value() { return value_; }
    void setValue(const T& value) { value_ = value; }

private:
    T value_;
};


typedef BasicTextLineData<QString> QStringTextLineData;
typedef BasicTextLineData<QList<QTextLayout::FormatRange>> LineAppendTextLayoutFormatListData;


//-------


/// the line data items
class EDBEE_EXPORT TextLineDataList {
public:
    TextLineDataList();
    virtual ~TextLineDataList();
    virtual void destroy(TextLineDataManager* manager);

    void give(TextLineDataManager* manager, size_t field, TextLineData *dataItem);
    TextLineData* take(TextLineDataManager* manager, size_t field);
    TextLineData* at(TextLineDataManager* manager, size_t field);

    void realloc(TextLineDataManager* manager, size_t oldFieldPerLine, size_t newFieldsPerLine);

private:
    TextLineData** lineDataList_;   ///< The text line data items
};


//-------


/// This manager manages all line definitions
class EDBEE_EXPORT TextLineDataManager : public QObject
{
Q_OBJECT

public:
    TextLineDataManager(size_t fieldsPerLine = PredefinedFieldCount);
    virtual ~TextLineDataManager();

    void clear();


    void give(size_t line, size_t field, TextLineData *dataItem);
    TextLineData* take(size_t line, size_t field);
    TextLineData* get(size_t line, size_t field);

    /// returns the number of items per line
    size_t fieldsPerLine() { return fieldsPerLine_; }
    void setFieldsPerLine(size_t count);

    /// returns the number of items
    size_t length() const { return textLineDataList_.length(); }
    /// returns the textline data list item
    TextLineDataList* at(size_t idx) const { return textLineDataList_.at(idx); }

    // internal functions
    Change* createLinesReplacedChange(size_t lineStart, size_t lineCount, size_t newLineCount);
    TextLineDataList* takeList(size_t line);
    void giveList(size_t line, TextLineDataList* list);

    void fillWithEmpty(size_t line, size_t length, size_t newLength);
    void replace(size_t line, size_t length, TextLineDataList** items, size_t newLength);

    /// internal method for direct accesss
    GapVector<TextLineDataList*>* textLineDataList() { return &textLineDataList_; }

protected:

    void destroyRange(size_t line, size_t length);


public slots:

//    void linesReplaced( int lineStart, int lineCount, int newLineCount );
//    void dumpGapvector();
signals:

    void lineDataChanged(size_t line, size_t length, size_t newLength);   ///< This signal is emitted if line-data is changed

private:

    size_t fieldsPerLine_;                              ///< The number of items per line
    GapVector<TextLineDataList*>  textLineDataList_;    ///< The textline data list
};


} // edbee
