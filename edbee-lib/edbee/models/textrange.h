// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QObject>
#include <QVector>

#include "edbee/models/textbuffer.h"

namespace edbee {

class TextDocument;

/// A single text region
/// A region constists of an anchor and a caret:
/// The anchor defines the 'start' of the range. Then caret the end.
///
/// Some definitions:
/// - caret == anchor  => length=0
///
/// samples  ( [ = anchor,  > = caret )
///   a[>bcdef    (anchor=1, caret=1)  => ""
///   a[b>cdef    (anchor=1, caret=2)  => "b"
///   a<b]cdef    (anchor=2, caret=1)  => "b"
///
class EDBEE_EXPORT TextRange {
public:
    TextRange(size_t anchor = 0, size_t caret = 0) : anchor_(anchor), caret_(caret) {}

    inline size_t anchor() const { return anchor_; }
    inline size_t caret() const { return caret_; }

    /// returns the minimal value
    inline size_t min() const { return qMin(caret_, anchor_); }
    inline size_t max() const { return qMax(caret_, anchor_); }

    /// returns the minimal variable reference
    inline size_t& minVar() { return caret_ < anchor_ ? caret_ : anchor_; }
    inline size_t& maxVar() { return caret_ < anchor_ ? anchor_: caret_; }

    inline size_t length() const { return (caret_ > anchor_) ? caret_ - anchor_ : anchor_ - caret_; }

    void fixCaretForUnicode(TextDocument* doc, int direction);

    void setAnchor(size_t anchor) { anchor_ = anchor; }
    void setAnchorBounded(TextDocument* doc, size_t anchor);
    void setAnchorBounded(TextDocument* doc, ptrdiff_t anchor);
    void setCaret(size_t caret) { caret_ = caret; }
    void setCaretBounded(TextDocument* doc, size_t caret);
    void setCaretBounded(TextDocument* doc, ptrdiff_t caret);
    void setLength(size_t newLength);

    void set(size_t anchor, size_t caret) { anchor_ = anchor; caret_ = caret; }
    void reset() { anchor_ = caret_; }
    bool hasSelection() const  { return anchor_ != caret_; }
    bool isEmpty() const { return anchor_==caret_; }
    void clearSelection() { caret_ = anchor_;  }

    QString toString() const;

    void moveCaret(TextDocument* doc, int amount);
    void moveCaretOrDeselect( TextDocument* doc, int amount);
    size_t moveWhileChar(TextDocument* doc, size_t pos, int amount, const QString& chars);
    size_t moveUntilChar(TextDocument* doc, size_t pos, int amount, const QString& chars);
    void moveCaretWhileChar(TextDocument* doc, int amount, const QString& chars);
    void moveCaretUntilChar(TextDocument* doc, int amount, const QString& chars);
    void moveAnchortWhileChar(TextDocument* doc, int amount, const QString& chars);
    void moveAnchorUntilChar(TextDocument* doc, int amount, const QString& chars);
    void moveCaretByCharGroup(TextDocument* doc, int amount, const QString& whitespace, const QStringList& characterGroups);
    void moveCaretToLineBoundary(TextDocument* doc, int amount, const QString& whitespace);
    void moveCaretToWordBoundaryAtOffset(TextDocument* doc, size_t offset);
    void moveCaretToLineBoundaryAtOffset(TextDocument* doc, size_t offset);

    void expandToFullLine(TextDocument* doc, int amount);
    void deselectTrailingNewLine(TextDocument* doc);
    void expandToWord(TextDocument* doc, const QString& whitespace, const QStringList& characterGroups);
    void expandToIncludeRange(TextRange& range);

    void forceBounds(TextDocument* doc);

    bool equals(const TextRange &range);
    bool touches(TextRange& range);
    bool contains(size_t pos);

    void assertValid() const;

    static bool lessThan(TextRange& r1, TextRange& r2);

private:
    size_t anchor_;        ///< The position of the anchor
    size_t caret_;         ///< The position of the caret
};


//======================================================================

/// This abstract class represents a set of textranges
/// The ranges are kept ordered and will not contain overlapping regions.
///
/// Every method automatically orders and merges overlapping ranges.
/// Except when the changing_ flag is != 0. The sorting and merging only happens
/// when changing is 0. This way it possible to add/update muliple rages without the direct
/// performance hit of sorting and merging.
class EDBEE_EXPORT TextRangeSetBase {
public:
    TextRangeSetBase(TextDocument* doc);
    virtual ~TextRangeSetBase() {}

    virtual size_t rangeCount() const  = 0;
    virtual TextRange& range(size_t idx) = 0;
    virtual const TextRange& constRange(size_t idx) const = 0;
    virtual void addRange(size_t anchor, size_t caret) = 0;
    virtual void addRange(const TextRange& range) = 0;
    virtual void removeRange(size_t idx) = 0;
    virtual void clear() = 0;
    virtual void toSingleRange() = 0;
    virtual void sortRanges() = 0;

    TextRange& lastRange();
    TextRange& firstRange();

    size_t rangeIndexAtOffset(size_t offset);
    bool rangesBetweenOffsets(size_t offsetBegin, size_t offsetEnd, size_t& firstIndex, size_t& lastIndex);
    bool rangesBetweenOffsetsExlusiveEnd(size_t offsetBegin, size_t offsetEnd, size_t& firstIndex, size_t& lastIndex);
    bool rangesAtLine(size_t line, size_t& firstIndex, size_t& lastIndex);
    bool rangesAtLineExclusiveEnd(size_t line, size_t& firstIndex, size_t& lastIndex);
    bool hasSelection();
    bool equals(TextRangeSetBase& sel);
    void replaceAll(const TextRangeSetBase& base);

    QString getSelectedText();
    QString getSelectedTextExpandedToFullLines();

    QString rangesAsString() const;

    // changing
    void beginChanges();
    void endChanges();
    void endChangesWithoutProcessing();
    bool changing() const;

    void resetAnchors();
    void clearSelection();

    void addTextRanges(const TextRangeSetBase& sel);
    void substractTextRanges(const TextRangeSetBase& sel);
    void substractRange(size_t min, size_t max);

    // selection
    void expandToFullLines(int amount);
    void expandToWords(const QString& whitespace, const QStringList& characterGroups);
    void selectWordAt(size_t offset , const QString& whitespace, const QStringList& characterGroups);
    void toggleWordSelectionAt(size_t offset, const QString& whitespace, const QStringList& characterGroups);

    // movement
    void moveCarets(int amount);
    void moveCaretsOrDeselect(int amount);
    void moveCaretsByCharGroup(int amount, const QString& whitespace, const QStringList& charGroups);
    void moveCaretsToLineBoundary(int direction, const QString& whitespace);
    //    void moveCaretsByLine( int amount );  //< Impossible to do without view when having a flexible font, guess that's why it wasn't implemented

    // changing
    // void growSelectionAtBegin( int amount );
    void changeSpatial(size_t pos, size_t length, size_t newLength, bool sticky = false, bool performDelete = false);

    void setRange(size_t anchor, size_t caret, size_t index = 0);
    void setRange(const TextRange& range, size_t index = 0);

    virtual void processChangesIfRequired(bool joinBorders=false);

  // getters
    TextDocument* textDocument() const;
    //TextBuffer* textBuffer() const { return textBufferRef_; }

    void mergeOverlappingRanges(bool joinBorders);

protected:

    TextDocument* textDocumentRef_;       ///< The reference to the textbuffer
    int changing_;                       ///< A (integer) boolean for handling changes between beginChagnes and endChanges
};


//======================================================================


/// The basic textrange class. A simple class of textrange with a simple vector implementation
class EDBEE_EXPORT TextRangeSet : public TextRangeSetBase
{
public:
    TextRangeSet(TextDocument* doc);
    TextRangeSet(const TextRangeSet& sel);
    TextRangeSet(const TextRangeSet* sel);
    virtual ~TextRangeSet() {}

    TextRangeSet& operator=(const TextRangeSet& sel);
    TextRangeSet* clone() const;

    virtual size_t rangeCount() const { return static_cast<size_t>(selectionRanges_.size()); }
    virtual TextRange& range(size_t idx);
    virtual const TextRange& constRange(size_t idx) const;
    virtual void addRange(size_t anchor, size_t caret);
    virtual void addRange(const TextRange& range);
    virtual void removeRange(size_t idx);
    virtual void clear();
    virtual void toSingleRange();
    virtual void sortRanges();

    virtual void assertValid() const;

private:

    QVector<TextRange> selectionRanges_;     ///< A list of selection ranges. After endChanges this array is sorted and non-overlapping!
};


//======================================================================


/// A smart QObject implemenation of a TextRangeSet which listens to changes
/// in the document. When a change happens it's changes the spatial of the ranges
///
/// The stickymode is used to change the behavior of the changes the the textChange event.
/// To put it simple, you should enable stickymode if this selection is the one you are
/// using to modify the document
///
/// The delete mode is used to tell the rangeset if 'deleted' ranges need to be deleted
/// or simply need to be moved
class EDBEE_EXPORT DynamicTextRangeSet : public QObject, public TextRangeSet
{
Q_OBJECT

public:
    DynamicTextRangeSet(TextDocument* doc, bool stickyMode=false, bool deleteMode=false, QObject* parent=nullptr);
    DynamicTextRangeSet(const TextRangeSet& sel, bool stickyMode=false, bool deleteMode=false, QObject* parent=nullptr);
    DynamicTextRangeSet(const TextRangeSet* sel, bool stickyMode=false, bool deleteMode=false, QObject* parent=nullptr);
    virtual ~DynamicTextRangeSet();

    void setStickyMode(bool mode);
    bool stickyMode() const;

    void setDeleteMode(bool mode);
    bool deleteMode() const;

public slots:
    void textChanged(edbee::TextBufferChange change, QString oldText = QString());

private:
    bool stickyMode_;                       ///< Sticky mode means if this rangeset is the current selection (This requires a different approach)
    bool deleteMode_;                       ///< When delete mode is enabled ranges are deleted. If it's false ranges are moved to the left
};


} // edbee
