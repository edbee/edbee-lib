// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "newlinecommand.h"

#include "edbee/models/texteditorconfig.h"
#include "edbee/models/textbuffer.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textrange.h"
#include "edbee/texteditorcontroller.h"
#include "edbee/util/util.h"
#include "edbee/views/textselection.h"

#include "edbee/debug.h"


namespace edbee {

/// Constructs the newline command
/// @param metod the method
NewlineCommand::NewlineCommand(NewlineCommand::NewLineType method)
    : newLineType_(method)
{
}

/// This method calculates the required 'smart' indent value at the given positon
/// @param controller the controller
/// @param range the range to calculate the smart tab for
QString NewlineCommand::calculateSmartIndent(TextEditorController* controller, TextRange& range)
{
    QString result;

    TextDocument* doc = controller->textDocument();
    TextBuffer* buf = doc->buffer();
    TextEditorConfig* config = doc->config();

    // find the line column position
    size_t caretOffset = range.min();
    size_t line = doc->lineFromOffset(range.min());
    size_t startOffset = doc->offsetFromLine(line);
    size_t endOffset = qMin( caretOffset ,doc->offsetFromLine(line + 1) - 1);

    // searches for the start of the current line
    size_t charPos = buf->findCharPosWithinRangeOrClamp(startOffset, 1, config->whitespaceWithoutNewline(), false, startOffset, endOffset);

    // when the start is found
    if (charPos > startOffset) {
        // we have found the column index. This isn't yet the column,
        // because when using tab-characters the column position can be different.
        // calculate the position by converting the tabs to spaces
        size_t column = static_cast<size_t>(Util().convertTabsToSpaces(doc->textPart(startOffset, charPos-startOffset), config->indentSize()).length());

        // when a tab character is used, we need to add the given number of tabs
        if (config->useTabChar()) {
            ptrdiff_t tabs   = static_cast<ptrdiff_t>(column) / config->indentSize();
            ptrdiff_t spaces = static_cast<ptrdiff_t>(column) % config->indentSize();
            result.append(QStringLiteral("\t").repeated(tabs));
            if (spaces > 0) {
                result.append(QStringLiteral(" ").repeated(spaces));
            }
        // else we can add spaces to the given column position
        } else {
            result.append(QStringLiteral(" ").repeated(static_cast<qsizetype>(column)));
        }
    }
    return result;
}


/// Executes a 'normal' newline operation
void NewlineCommand::executeNormalNewline(TextEditorController* controller)
{
    TextDocument* doc = controller->textDocument();
    TextEditorConfig* config = doc->config();

    // when no smarttab is enabled this function is simple
    // the current selection is simple replaced by a newline :)
    if (!config->smartTab()) {
        controller->replaceSelection("\n", CoalesceId_InsertNewLine+NormalNewline);
        return;
    }

    // when we're using smart tab. We're going to indent every next line
    // to the same level as the previous one
    QStringList texts;

    TextSelection* sel = controller->textSelection();
    for (size_t i = 0, cnt = sel->rangeCount(); i < cnt; ++i) {
        TextRange& range = sel->range(i);

        // add the string to insert
        texts.push_back(QStringLiteral("\n%1").arg( calculateSmartIndent(controller, range)));
    }

    // next execute the replace command
    controller->replaceSelection(texts, CoalesceId_InsertNewLine + NormalNewline);
}


/// Executes a special newline operation. (Inserts a newline before or the current line)
/// @param controller the controller to execute this operation for
/// @param nextLine a nextline operation
void NewlineCommand::executeSpecialNewline(TextEditorController* controller, bool nextLine)
{
    TextDocument* doc = controller->textDocument();
    TextEditorConfig* config = doc->config();

    // we need to insert a newline above every caret
    bool smart = config->smartTab();
    doc->beginChanges(controller);

    // iterate over the selection via a dynamic rangeset
    DynamicTextRangeSet sel(controller->textSelection());
    TextRangeSet* newSelection = new TextRangeSet(doc);
    size_t lastLine = std::string::npos;
    size_t lineDelta = nextLine ? 1 : 0;
    for (size_t i = 0; i < sel.rangeCount(); ++i) {

        // only handle the line if it's another line then the previous
        TextRange& range = sel.range(i);
        size_t line = doc->lineFromOffset(range.caret());
        if (line > lastLine || lastLine == std::string::npos) {
            // create a new range
            size_t pos = doc->offsetFromLine(line + lineDelta) - 1;
            TextRange newRange(pos, pos);

            QString text = QStringLiteral("\n%1").arg(smart ? calculateSmartIndent(controller, newRange) : "");

            // replaces the text
            doc->replace(qMax(0u, newRange.caret()), 0, text);
            newRange.setCaret(pos + static_cast<size_t>(text.length()));
            newRange.reset();

            // add the range to the selection
            newSelection->addRange(newRange);
            lastLine = line;
        }
    }

    // give the selection and end the changes
    doc->giveSelection( controller, newSelection);
    doc->endChanges(CoalesceId_InsertNewLine + (nextLine ? AddLineAfter : AddLineBefore) );

    // the following call shouldn't be necessary
    controller->update();
}


/// Executes the insert newline command
/// @param controller the controller to execute this command for
void NewlineCommand::execute(TextEditorController* controller)
{
    switch (newLineType_)
    {
        case NormalNewline:
            executeNormalNewline(controller);
            break;

        case AddLineBefore:
            executeSpecialNewline(controller, false);
            break;

        case AddLineAfter:
            executeSpecialNewline(controller, true);
            break;

        default:
            Q_ASSERT( false && "Invalid newLineType!");
    }
}


/// Returns the command name
QString NewlineCommand::toString()
{
    return "NewlineCommand";
}

} // edbee
