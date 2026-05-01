// edbee - Copyright (c) 2012-2026 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "treesittertextlexer.h"

#include <QStack>
#include <QThread>

#include "tree_sitter/api.h"

#include "edbee/models/grammars/treesittertextgrammar.h"
#include "edbee/models/textdocument.h"
#include "edbee/models/textdocumentscopes.h"
#include "edbee/edbee.h"

#include "edbee/debug.h"

namespace edbee {

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
#define TS_UTF16_ENCODING TSInputEncodingUTF16LE
#elif Q_BYTE_ORDER == Q_BIG_ENDIAN
#define TS_UTF16_ENCODING TSInputEncodingUTF16BE
#else
#error "Unkown Q_BYTE_ORDER"
#endif

struct TsInputPayload {
    const QChar* chars;
    uint32_t length;
};

const char* ts_input_callback(void* raw_payload, uint32_t byte_index, TSPoint position, uint32_t* bytes_read) {
    TsInputPayload* payload = static_cast<TsInputPayload*>(raw_payload);

    // Cast payload to your QChar* buffer and offset
    const QChar *chars = payload->chars;
    uint32_t utf16_index = byte_index / 2;  // since each QChar is 2 bytes

    // Bounds check
    if (utf16_index >= static_cast<uint32_t>(payload->length)) {
        *bytes_read = 0;
        return nullptr; // end of input
    }

    // Return pointer to the current QChar (as raw bytes)
    *bytes_read = 2;  // each QChar is 2 bytes
    return reinterpret_cast<const char*>(&chars[utf16_index]);
};


// extra the text from a node
QString extract_node_text(const TSNode node, const char* source) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t len = ts_node_end_byte(node) - start;

    // Create QString directly from source buffer slice
    return QString::fromUtf8(source + start, len);
}


// extra the text from a node
QString extract_node_text(const TSNode node, const QChar* source) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t len = ts_node_end_byte(node) - start;

    // Create QString directly from source buffer slice
    return QString(source + start, len);
}

void dumpScopes(TextDocumentScopes* scopes)
{
    // qDebug() << "MultiLineScopes:---";
    // QStringList items = QString(scopes->toString()).split("|");
    // foreach(const QString& item, items) {
    //     qDebug() << item;
    // }

    qDebug() << "Per Line:---";
    for (size_t i = 0; i < scopes->scopedLineCount(); ++i) {
        ScopedTextRangeList* range = scopes->scopedRangesAtLine(i);
        if (range) {
            qDebug() << QStringLiteral("- %1: %2\n").arg(i).arg(range->toString());
        } else {
            qDebug() << QString("- nullptr @ index %1").arg(i);
        }
    }
}


// ==========================================


/// Constructs the grammar textlexer
/// @param scopes a reference to the scopes model
TreeSitterTextLexer::TreeSitterTextLexer(TreeSitterTextGrammar* grammar, TextDocumentScopes* scopes)
    : TextLexer(grammar, scopes)
    , tsParser_(nullptr)
    , tsTree_(nullptr)
{
    tsParser_ = ts_parser_new();
    if (!ts_parser_set_language(tsParser_, grammar->language())) {
        qDebug() << "Error settings parser language to " <<  grammar->displayName();
    }
}


TreeSitterTextLexer::~TreeSitterTextLexer()
{
    if (tsTree_) {
        ts_tree_delete(tsTree_);
    }
    if (tsParser_) {
        ts_parser_delete(tsParser_);
    }
}


void TreeSitterTextLexer::updateHighlightScopes(uint32_t startByte, uint32_t endByte)
{
    TextDocumentScopes* scopes = this->textScopes();
    size_t startOffset = startByte / 2;
    size_t endOffset = endByte / 2;
    qDebug() << "=======updateHighlightScopes("<<startByte << ", " << endByte << ") => (" << startOffset << ", " << endOffset << ") (doc: " << this->textDocument()->length() <<") ============";
qDebug() << "ON DELETE, document length is smaller then this range..." ;
//dumpScopes(scopes);


    TSQuery* query = treeSitterTextGrammar()->query("highlights");
    Q_ASSERT(query);
    if (!query) return;

    // Get the root node of the syntax tree.
    TSNode root_node = ts_tree_root_node(tsTree_);
    // char *string = ts_node_string(root_node);
    // qDebug() << "Syntax tree: " <<  string;
    // free(string);

    // Execute it
    // ref: <https://blog.pulsar-edit.dev/tag/tree-sitter/>
    // ref: <https://www.perplexity.ai/search/i-m-using-trees-titter-to-exec-CDxmM65VTEWd_KTydkn_Qw>
    TSQueryCursor* cursor = ts_query_cursor_new();
    if (startByte != UINT32_MAX && endByte != 0) {

qDebug() << "--------------------------------- (cleanup): " << startOffset << ", " << endOffset;
dumpScopes(scopes);
qDebug() << " ===> ";
        scopes->removeScopesAfterOffset(scopes->textDocument()->length()); // always delete the scopes past the end of the document

        scopes->removeScopesWithinOffsets(startOffset, endOffset);
dumpScopes(scopes);

        ts_query_cursor_set_byte_range(cursor, startByte, endByte);

    } else {
qDebug() << "SET DEFAULT SCOPE !!!!";
        scopes->setDefaultScope(this->grammar()->defaultScopeName(), nullptr);
    }


    ts_query_cursor_exec(cursor, query, root_node);

    /// This more like a hacke, multi-line scopes are only relevant for regexp matches
    /// Somehow edbee requires at least one multi-lined scoped.
    /// Maybe in the future  use a different way of handling the scopes. For now just set the multi-ranged scope
    TextScope* scopeRef = Edbee::instance()->scopeManager()->refTextScope(grammar()->defaultScopeName());
    MultiLineScopedTextRangeSet& scopedRanges = textScopes()->scopedRanges();
    if (scopedRanges.rangeCount() == 0) {
        MultiLineScopedTextRange* multiRange = new MultiLineScopedTextRange(0, textScopes()->textDocument()->length(), scopeRef);
        scopes->giveMultiLineScopedTextRange(multiRange);
    } else {
        Q_ASSERT(scopedRanges.rangeCount() == 1);
        scopedRanges.scopedRange(0).setLength(scopes->textDocument()->length());
        scopedRanges.scopedRange(0).setScope(scopeRef);
    }

    // the raw pointer to the text buffer
    QChar* source_code = textDocument()->buffer()->rawDataPointer();

    TSQueryMatch match;
    uint32_t match_count = 0;
    while (ts_query_cursor_next_match(cursor, &match)) {
        // qDebug() << "> Match";
        //qDebug() << "  * id: " << match.id;
        //qDebug() << "  * pattern_index: " << match.pattern_index;
        //qDebug() << "  * captures: ";

        for (uint32_t i = 0; i < match.capture_count; i++ ) {
            TSQueryCapture capture = match.captures[i];
            TSNode& node = capture.node;
            TSPoint startPoint = ts_node_start_point(node);
            TSPoint endPoint = ts_node_end_point(node);
            uint32_t startByte = ts_node_start_byte(node);
            uint32_t endByte = ts_node_end_byte(node);

            // This is done because UTF16 columns are given in bytes
            int startcolumn = startPoint.column / 2;
            int endcolumn = endPoint.column / 2;


            char* nodeString = ts_node_string(node);

            QString text = extract_node_text(capture.node, source_code);

            uint32_t nameLength = 0;
            const char* name = ts_query_capture_name_for_id(query, capture.index, &nameLength);
            QString captureName = QString::fromUtf8(name, nameLength);

            // convert it to text-mate compatible strins?
            QString highlightName = captureName;
            if (captureName == "string") {
                highlightName = "string.quoted.double.js";
            }

            // qDebug() << "   - " << i << ": " << capture.index << ": " << nodeString
            //          << " | start " << startPoint.row << "," << startPoint.column << "(" << startByte << ")"
            //          << " | end " << endPoint.row << "," << endPoint.column << "(" << endByte << ")"
            //          <<  " => " << text << " : " << captureName
            //     ;

            if (captureName == "string.special.key") {
                continue;
            }

            // Shit, this isn't really line based anymore
            // We can completely change the way highlighing works
            // for now try to convert it to the existing highlighting system
            if (startPoint.row == endPoint.row) {
                // alread a scope range list here. (append to it)
                uint32_t line = startPoint.row;
                ScopedTextRangeList* lineRangeList = scopes->scopedRangesAtLine(line);
                qDebug() << " -- lineRangeList " << line << " = " << lineRangeList;
                if (!lineRangeList) {
                    lineRangeList = new ScopedTextRangeList();
                    //lineRangeList->giveRange(new ScopedTextRange(0, textDocument()->lineLength(line), Edbee::instance()->scopeManager()->refTextScope("source.json")));
                    lineRangeList->giveRange(new ScopedTextRange(0, textDocument()->lineLength(line), scopes->defaultScopedRange().scope()));
                    scopes->giveLineScopedRangeList(line, lineRangeList);
                    qDebug()<< "CREATE! " << line;
                }
                // lineRangeList->giveRange(new ScopedTextRange(startByte, endByte, Edbee::instance()->scopeManager()->refTextScope(captureName)));
                lineRangeList->giveRange(new ScopedTextRange(startcolumn, endcolumn, Edbee::instance()->scopeManager()->refTextScope(highlightName)));

            } else {
                // single line scope
                qDebug() << "TODO: add a multiline scope HERE???";

                //for (int line = startPoint.row; line <= endPoint.row; line++) {
                //}
            }

            // scopes->j
            free(nodeString);

            qDebug() << scopes->toString();
        }

    qDebug() << "--------------------------------------------------------- (after scoping)";
    dumpScopes(scopes);

    qDebug() <<"";

        // for (uint32_t i = 0; i < ts_query_capture_count(query); ++i) {
        //     // TODO Capture it!
        //     // TSNode node = ts_node_make_factory(match.captures[i * 2], match.captures[i * 2 + 1]);  // Simplified; use proper indexing
        //     // const char *capture_name = ts_query_capture_name_for_id(query, i, NULL);
        //     // char *text = ts_node_text(node, source_code);  // Assumes source buffer available
        //     // qDebug() << QString("Capture '%1': %2").arg(capture_name, text);
        //     // free(text);

        //     qDebug() << "id:" << match.id << ", pattern_index: " << match.pattern_index << ", capture_acount: " << match.capture_count;
        // }
    }

    ts_query_cursor_delete(cursor);
}



/// This method is called to notify the lexer some data has been changed
void TreeSitterTextLexer::textChanged(const TextBufferChange& change)
{
qDebug() << ">>>>>>>>>>>> textChanged: " << change.toDebugString() << "<<<<<<<<<<<<<<";

    TextDocument* doc = textDocument();
    TextDocumentScopes* docScopes = textScopes();

    if (!tsTree_) return;


    // PERFORM THE EDIT
    //=================
    size_t newEndCol = 0;
    if (change.newLineOffsets().count() > 0) {
        newEndCol = change.newLineOffsets().last() - (change.offset() + change.newTextLength());
    }

    // FIXME:/TODO: Figure out columsn with editing

    TSInputEdit tsInputEdit = {
        .start_byte = static_cast<uint32_t>(change.offset()),
        .old_end_byte = static_cast<uint32_t>(change.offset() + change.length()),
        .new_end_byte = static_cast<uint32_t>(change.offset() + change.newTextLength()),
        .start_point = { static_cast<uint32_t>(change.line()), static_cast<uint32_t>(textDocument()->columnFromOffset(change.offset())) },
        .old_end_point  = {
            static_cast<uint32_t>(change.line() + change.lineCount()),
            0 // TODO: Figure out what the OLD column was !!!
        },
        .new_end_point = {
            static_cast<uint32_t>(change.line() + change.newLineCount()),
            static_cast<uint32_t>(newEndCol)
        }
    };

    ts_tree_edit(tsTree_, &tsInputEdit);

    // edit the tree
    QChar* chars = textDocument()->buffer()->rawDataPointer();
    size_t sourceLength = textDocument()->buffer()->length();

    // Map to a format that matches how Tree‑sitter UTF‑16 APIs expect `uint16_t*`
    // Note: if you are on big‑endian, adjust endianness (this example assumes LE).
    // const uint16_t* utf16_text = reinterpret_cast<const uint16_t *>(chars);
    const char* bytes = reinterpret_cast<const char*>(chars);
    TsInputPayload payload = { chars, static_cast<uint32_t>(sourceLength) };


    TSInput input = {
        .payload = &payload,
        .read = ts_input_callback,
        .encoding = TSInputEncodingUTF16LE
    };

    TSTree* oldTree = tsTree_;
    tsTree_ = ts_parser_parse(tsParser_, oldTree, input);


    // Next we can get the affect RANGES <https://www.perplexity.ai/search/i-m-tree-sitter-to-partially-u-MeQufzxdSImB0.9m3W51Sg>
    // and re-executee the highlithing query on the affected areas
    // ==============================================================

    uint32_t num_ranges;
    TSRange *ranges = ts_tree_get_changed_ranges(oldTree, tsTree_, &num_ranges);
    uint32_t min_start = UINT32_MAX, max_end = 0;
    for (uint32_t i = 0; i < num_ranges; i++) {
        min_start = std::min(min_start, ranges[i].start_byte);
        max_end = std::max(max_end, ranges[i].end_byte);
    }
    free(ranges);


    // update the highlights for the given bytes
    updateHighlightScopes(min_start, max_end);


    ts_tree_delete(oldTree);

}




/// returns the  regexgrammar
TreeSitterTextGrammar* TreeSitterTextLexer::treeSitterTextGrammar()
{
    TreeSitterTextGrammar* result = dynamic_cast<TreeSitterTextGrammar*>(grammar());
    Q_ASSERT(result);
    return result;
}


/// TODO: Implement lexRange for the treesitter parser
void TreeSitterTextLexer::lexRange(size_t beginOffset, size_t endOffset)
{
//    qDebug() << "lexRange TREESITTER: " << beginOffset << "-" << endOffset;
//     if (beginOffset == 0 && endOffset == textDocument()->length()) {
// qDebug() << "lexRange TREESITTER: " << beginOffset << "-" << endOffset;
// qDebug() << "FULL CLEAN!!";
//         ts_tree_delete(tsTree_);
//         tsTree_ = nullptr;
//         textScopes()->removeScopesAfterOffset(beginOffset);
//     }
    updateTsTree();


    // only set the scoped offset if less and not indepdent
    // if (currentDocOffset < docScopes->lastScopedOffset()) {
    //     if (!independent) {
    //         docScopes->setLastScopedOffset(currentDocOffset);
    //         docScopes->removeScopesAfterOffset(currentDocOffset);
    //     }
    //     // further down the document, update the scope
    // } else {
    //     docScopes->setLastScopedOffset(currentDocOffset);
    //     docScopes->removeScopesAfterOffset(currentDocOffset);
    // }
}

void TreeSitterTextLexer::updateTsTree()
{

    if (!tsTree_) {
        // ts_tree_delete(tsTree_);
        tsTree_ = createTsTree();
        qDebug() << textDocument()->buffer()->text();

        updateHighlightScopes();

        textScopes()->setLastScopedOffset(textDocument()->length()); // done :-) (not really used)
    }
}

/// Creates a TSTree from the full d ocument
///
/// Using Qt C++ with TreeSitter. I have a buffer with QChars. (Raw pointer to QChar array) How do I use tree-sitter to parse/access this?
/// <https://www.perplexity.ai/search/using-qt-c-with-treesitter-i-h-ediSuV.5Tf6x9x9hJT7DQw>
///
/// TODO: Use sample with Direct UTF‑16: using TSInput callback
/// TODO: Move this code to TextLexer??
TSTree* TreeSitterTextLexer::createTsTree()
{
    QChar* chars = textDocument()->buffer()->rawDataPointer();
    size_t sourceLength = textDocument()->buffer()->length();

    // Map to a format that matches how Tree‑sitter UTF‑16 APIs expect `uint16_t*`
    // Note: if you are on big‑endian, adjust endianness (this example assumes LE).
    // const uint16_t* utf16_text = reinterpret_cast<const uint16_t *>(chars);
    // const char* bytes = reinterpret_cast<const char*>(chars);
    TsInputPayload payload = { chars, static_cast<uint32_t>(sourceLength) };

    TSInput input = {
        .payload = &payload,
        .read = ts_input_callback,
        .encoding = TSInputEncodingUTF16LE
    };

    TSTree *tree = ts_parser_parse(tsParser_, nullptr, input);
    return tree;
}


} // edbee
