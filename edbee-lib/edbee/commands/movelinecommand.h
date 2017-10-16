#pragma once

#include "edbee/texteditorcommand.h"

namespace edbee {

/// moves
class MoveLineCommand : public TextEditorCommand
{
public:
    MoveLineCommand( int direction );
    virtual ~MoveLineCommand();

    virtual void execute( TextEditorController* controller );
    virtual QString toString();

private:
    int direction_;
};

}
