/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "edbee/texteditorcommand.h"

namespace edbee {

class FindCommand : public TextEditorCommand
{
public:

    enum FindType {
        UseSelectionForFind,
        FindNextMatch,
        FindPreviousMatch,
        SelectNextMatch,
        SelectPreviousMatch,
        SelectAllMatches
    };


    FindCommand( FindType findType );

    virtual void execute( TextEditorController* controller );
    virtual QString toString();

private:
    FindType findType_;     ///< the current find-type

};

} // edbee
