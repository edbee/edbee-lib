/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include "findcommand.h"

#include "edbee/models/texteditorconfig.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textsearcher.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/views/textselection.h"

#include "debug.h"

namespace edbee {

FindCommand::FindCommand( FindType findType )
    : findType_( findType )
{
}

void FindCommand::execute(TextEditorController* controller)
{
    TextDocument* doc = controller->textDocument();
    TextSearcher* searcher = controller->textSearcher();
    switch( findType_ ) {
        case UseSelectionForFind:
        {
            TextRange range = controller->textSelection()->range(0);
            // when empty select the word
            if( range.isEmpty() ) {
                range.expandToWord( doc, doc->config()->whitespaces(), doc->config()->charGroups());
            }
            searcher->setSearchTerm( doc->textPart( range.min(), range.length() ) );
            break;
        }
        case FindNextMatch:
            searcher->setReverse(false);
            searcher->findNext( controller->widget() );
            break;
        case FindPreviousMatch:
            searcher->setReverse(true);
            searcher->findNext( controller->widget() );
            break;

        case SelectNextMatch:
            searcher->setReverse(false);
            searcher->selectNext( controller->widget() );
            break;

        case SelectPreviousMatch:
            searcher->setReverse(true);
            searcher->selectNext( controller->widget() );
            break;

        case SelectAllMatches:
            searcher->selectAll( controller->widget() );
            break;

        default:
            qlog_warn() << "Warning, invalid FindCommand supplied!";
            Q_ASSERT(false);
    }
}

QString FindCommand::toString()
{
    QString str;
    switch( findType_ ) {
        case UseSelectionForFind: str =  "UseSelectionForFind"; break;
        case FindNextMatch: str =  "FindNext"; break;
        case FindPreviousMatch: str = "FindPrevious"; break;
        case SelectNextMatch: str = "SelectNextMatch"; break;
        case SelectPreviousMatch: str = "SelectPreviousMatch"; break;
        case SelectAllMatches: str =  "SelectAllMatches"; break;
    }
    return QString("FindCommand(%1)").arg(str);
}

} // edbee
