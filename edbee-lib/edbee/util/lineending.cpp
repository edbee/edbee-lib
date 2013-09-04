/**
 * Copyright 2011-2012 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QString>

#include "lineending.h"

#include "debug.h"

namespace edbee {

/// This method returns the line endings
/// @param type the type to retrieve
LineEnding *LineEnding::types()
{
    static LineEnding types[] = {
        LineEnding( LineEnding::Unix, "\n", "\\n", "Unix" ),
        LineEnding( LineEnding::Windows, "\r\n", "\\r\\n", "Windows" ),
        LineEnding( LineEnding::MacClassic, "\r" , "\\r", "Mac Classic"),
    };
    return types;
}

/// This methode returns the line ending from the given type
LineEnding *LineEnding::get(int idx )
{
    Q_ASSERT( 0 <= idx && idx < TypeCount );
    return types() + idx;
}


LineEnding::LineEnding(LineEnding::Type type, const char* chars, const char* escaped, const char* name )
    : type_( type )
    , charsRef_( chars )
    , escapedCharsRef_( escaped )
    , nameRef_( name )
{
}

LineEnding::~LineEnding()
{
}



/// This method detects the, line endings of a QString.
/// A byte array is tricky because the encoding could be multi-byte
///
/// @param str the string to find the line-endings.
/// @param unknown the type to return when the ending is not nown. (defaults to 0)
/// @return the found type or the unkown parameter if not found
///
LineEnding *LineEnding::detect(const QString& str, LineEnding *unkown )
{
    const int endLoopWhenCountReaches = 3;
    int macClassicCount = 0;
    int unixCount = 0;
    int winCount = 0;

    for( int i=0, cnt=str.length(); i<cnt; i++ ) {

        // retrieve the characters
        QChar c1 = str.at(i);
        QChar c2 = (i+1<cnt) ? str.at(i+1) : QChar(0);

        // detect the line-ending
        if( c1 == '\r' ) {
           if( c2 == '\n' ) {
               ++winCount;
               ++i;
               if( winCount >= endLoopWhenCountReaches ) break;
           } else  {
               ++macClassicCount;
               if( macClassicCount >= endLoopWhenCountReaches ) break;
           }
        }
        else if( c1 == '\n') {
            ++unixCount;
            if( unixCount >= endLoopWhenCountReaches ) break;
        }
    }
    if( macClassicCount > unixCount && macClassicCount > winCount ) return get( LineEnding::MacClassic );
    if( winCount > unixCount ) return get( LineEnding::Windows );
    if( unixCount > 0) return get( LineEnding::Unix );
    return unkown;
}


} // edbee
