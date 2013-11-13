/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include <QList>

class QString;

namespace edbee {

/// A global utiltity class.
/// The purpose of this class is to put 'global' function that don't quite fit on other places
///
/// You can use this class like this:   Util().converTabsToSpaces()
class Util
{
public:
    QString convertTabsToSpaces( const QString& str, int tabSize );
    QList<int> tabColumnOffsets( const QString& str, int tabSize );
};

} // edbee
