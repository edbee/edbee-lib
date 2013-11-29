/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QVector>

#include "edbee/models/textchange.h"

namespace edbee {

class TextLineDataManager;
class TextLineDataList;

/// A full line data text change
class LineDataListTextChange : public TextChange
{
public:
    LineDataListTextChange( TextLineDataManager* manager, int line, int lenght, int newLength );
    virtual ~LineDataListTextChange();

    virtual void execute(TextDocument* document);
    virtual void revert(TextDocument* doc);

    virtual bool giveAndMerge(TextDocument* document, TextChange* textChange );
    virtual void applyLineDelta( int line, int length, int newLength );

    virtual QString toString();

    int line();
    int length();
    int newLength();

    TextLineDataList** oldListList();
    int oldListListLength();


private:

    TextLineDataManager* managerRef_;         ///< A reference to the manager
    int line_;                                ///< The line number start
    int length_;                              ///< The number of lines to "replace"
    int newLength_;                           ///< The number of new items (they all will be 0)

    TextLineDataList** oldListList_;          /// The lists of items (old items)
};

} // edbee
