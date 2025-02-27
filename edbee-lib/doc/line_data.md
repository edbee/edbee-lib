# Line data

Linedata support add the support for adding custom data to TextDocument lines.
These data items are automaticly moved/delete when inserting/deleting lines.

The line data is a fixed with array with pointers and is reserved for every line.
The enum `TextLineDataPredefinedFields` defines the predefined fields that are built in edbee.
The `PredefinedFieldCount' value can be used to find out where to start defining your custom types.

To use extra custom fields you are required to set the number of fields to reserve.
This can be done with `editor->textDocument()->lineDataManager()->setFieldsPerLine(3)`
which needs to have at least the length of PredefinedFieldCount.


## General usage

To set a file for a given line you can invoke the giveLineData method.

```cpp
textDocument()->giveLineData(line, edbee::LineAppendTextLayoutFormatListField, data);
```

Removing can be done by supplying a nullptr to the data

```cpp
textDocument()->giveLineData(line, edbee::LineAppendTextLayoutFormatListField, nullptr);
```

## Predefined variable `LineAppendTextLayoutFormatListField`

The predefined `LineAppendTextLayoutFormatListField` field type can be used to
add custom `<QTextLayout::Format>` ranges to the QTextLayout of a line.

For example, to add a waved red underline:

```cpp
// Set the line data for a given line (assuming this line is in the document!!)
int line = 2;

// build a list of QTextLayout::FormatRanges
QList<QTextLayout::FormatRange> formatRanges;

QTextLayout::FormatRange formatRange;
formatRange.start = 0;
formatRange.length = editor->textDocument()->lineLength(line);
formatRange.format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
formatRange.format.setUnderlineColor(Qt::red);
formatRanges.append(formatRange);

// build a line data item (which is added to the TextLineData)
edbee::LineAppendTextLayoutFormatListData *formatRangesListData = new edbee::LineAppendTextLayoutFormatListData(formatRanges);
editor->textDocument()->giveLineData(line, edbee::LineAppendTextLayoutFormatListField, formatRangesListData);
```

Because every line has it's own QTextLayout the range offset of the given line is 0.



