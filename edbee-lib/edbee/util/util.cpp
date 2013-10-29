/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "util.h"

#include <QString>

#include "debug.h"

namespace edbee {


/// Converst all tabs to sapces of the given string, using the current tab/indent settings
/// It converts "\t" to spaces.
///
/// @param str the string where to convert the tabs to space
/// @param tabSize the size of a single tab. This needs to be at least 1
/// @return A string with all tabs converted to spaces
QString Util::convertTabsToSpaces(const QString& str, int tabSize )
{
    Q_ASSERT(tabSize > 0);

    QString result;
    result.reserve( str.length() );

    // append all characters to the result
    for( int i=0,cnt=str.size(); i<cnt; ++i ) {
        QChar c = str.at(i);

        // when a tab character is used it is converted to the correct column
        if( c == '\t' ) {
            int amount = tabSize - result.length() % tabSize;
            result.append( QString(" ").repeated(amount) );
        } else {
            result.append(c);
        }
    }
    return result;
}



} // edbee
