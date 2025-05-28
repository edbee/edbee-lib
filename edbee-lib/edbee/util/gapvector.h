// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QChar>
#include <QString>

// #define GAP_VECTOR_CLEAR_GAP

namespace edbee {

/// Is used to define a gap vector. A Gapvector is split in 2 parts. where the gap
/// is moved to the insertation/changing point. So reducing the movement of fields
template <typename T, typename Tsize = size_t>
class EDBEE_EXPORT GapVector {
public:
  GapVector(Tsize capacity = 16) : items_(nullptr), capacity_(0), gapBegin_(0), gapEnd_(0)  {
    items_    = new T[capacity];
  capacity_ = capacity;
  gapBegin_ = 0;
  gapEnd_   = capacity;
  growSize_ = 16;
}


~GapVector() {
  delete[] items_;
}


/// returns the used length of the data
inline Tsize length() const { return capacity_ - gapEnd_ + gapBegin_; }
inline Tsize gapSize() const { return gapEnd_ - gapBegin_; }
inline Tsize gapBegin() const { return gapBegin_; }
inline Tsize gapEnd() const { return gapEnd_; }
inline Tsize capacity() const { return capacity_; }


/// clears the data
void clear()
{
  delete[] items_;
  capacity_ = 16;
  items_    = new T[capacity_];
  gapBegin_ = 0;
  gapEnd_   = capacity_;
  growSize_ = 16;
}


protected:

/// Replaces the given text with the given data.
/// because the length of the source and target is the same in this method
/// no gap-moving is required
/// @param offset the target to move the data to
/// @param length the number of items to replace
/// @param data the data pointer with the source data
void replace(Tsize offset, size_t length, const T* data) {
  Q_ASSERT(offset <= this->length());
Q_ASSERT(offset + length <= capacity_);

// copy the first part
if (offset < gapBegin()) {
  Tsize len = qMin(gapBegin_ - offset, length); // issue 141, added -offset
  memcpy(items_ + offset, data, static_cast<Tsize>(sizeof(T)) * len);
  data      += len;   // increase the pointer
  offset    += len;
  length    -= len;
}

if (0 < length) {
  memcpy(items_ + offset + gapSize(), data, sizeof(T)*static_cast<Tsize>(length));
}
    }


/// Replaces the given text with the given data.
/// because the length of the source and target is the same in this method
/// no gap-moving is required
/// @param offset the target to move the data to
/// @param length the number of items to replace
/// @param data the data pointer with the source data
void fill(Tsize offset, size_t length, const T& data) {
  Q_ASSERT(offset <= this->length());
  Q_ASSERT(offset + length <= capacity_);

  // copy the first part
  if (offset < gapBegin()) {
    Tsize len = qMin(gapBegin_ - offset, length);
    for (Tsize i = 0; i < len; ++i) { items_ [offset + i] = data; }
    offset    += len;
    length    -= len;
  }

  if (0 < length) {
    offset += gapSize();
    for (Tsize i = 0; i < length; ++i) { items_ [offset + i] = data; }
  }
}


public:

/// Replaces the given items
/// @param offset the offset of the items to replace
/// @param length the number of items to replace
/// @param data an array with new items
/// @param newLength the number of items in the new array
void replace(Tsize offset, size_t length, const T* data, size_t newLength) {
  Tsize currentLength=this->length();
  Q_ASSERT((offset+length) <= currentLength);
  Q_ASSERT(offset + length <= capacity_);
  Q_UNUSED(currentLength);

Tsize gapSize = this->gapSize();

// Is it a 'delete' or 'insert' or 'replace' operation

// a replace operation (do not perform gap moving)
if (length == newLength) {
  replace(offset, length, data);

  // insert operation
} else if (length < newLength) {
  Tsize gapSizeRequired = newLength - length;
  ensureGapSize(gapSizeRequired);
  moveGapTo(offset + length);
  memcpy(items_ + offset, data, sizeof(T) * static_cast<Tsize>(newLength));
  gapBegin_ = offset + newLength;

  // delete operation
} else {
  moveGapTo(offset);
  memcpy(items_ + offset, data, sizeof(T) * static_cast<Tsize>(newLength));
  gapBegin_ = offset + newLength;
  gapEnd_   = offset + gapSize + length;
}

Q_ASSERT(gapBegin_ <= gapEnd_);
Q_ASSERT(this->gapSize() <= capacity_);
Q_ASSERT(this->length() <= capacity_);
    }


/// Replaces the given items with a single data item
/// @param offset the offset of the items to replace
/// @param lenth the number of items to replace
/// @param newLength the number of times to repeat data
void fill(Tsize offset, size_t length, const T& data, size_t newLength) {
  Tsize currentLength=this->length();
  Q_ASSERT((offset+length) <= currentLength);
  Q_ASSERT(offset + length <= capacity_);
  Q_UNUSED(currentLength);

  Tsize gapSize = this->gapSize();

  // Is it a 'delete' or 'insert' or 'replace' operation

  // a replace operation (do not perform gap moving)
  if (length == newLength) {
    fill(offset, length, data);

    // insert operation
  } else if (length < newLength) {
    Tsize gapSizeRequired = newLength - length;
    ensureGapSize(gapSizeRequired);
    moveGapTo( offset + length);
    for (Tsize i = 0; i < newLength; ++i) { items_[offset+i] = data; }
    gapBegin_ = offset + newLength;

    // delete operation
  } else {
    moveGapTo(offset);
    for (Tsize i = 0; i < newLength; ++i) { items_[offset+i] = data; }
    gapBegin_ = offset + newLength;
    gapEnd_   = offset + gapSize + length;
  }

  Q_ASSERT(gapBegin_ <= gapEnd_);
  Q_ASSERT(this->gapSize() <= capacity_);
  Q_ASSERT(this->length() <= capacity_);
}


/// convenient append method
void append(T t) {
  replace(length(), 0, &t, 1);
}


/// another append method
void append(const T* t, Tsize length) {
  replace(this->length(), 0, t, length);
}


/// Returns the item at the given index
T at(Tsize offset) const {
  Q_ASSERT(offset < length());
  if (offset < gapBegin_) {
    return items_[offset];
  } else {
    return items_[gapEnd_ + offset - gapBegin_];
  }
}


/// Sets an item at the given index
void set(Tsize offset, const T& value) {
  Q_ASSERT(offset < length());
  if (offset < gapBegin_) {
    items_[offset] = value;
  } else {
    items_[gapEnd_ + offset - gapBegin_] = value;
  }
}


/// Returns an index
T& operator[](Tsize offset) const {
  Q_ASSERT(offset < length());
  if (offset < gapBegin_) {
    return items_[offset];
  } else {
    return items_[gapEnd_ + offset - gapBegin_];
  }
}


/// Returns the 'raw' element at the given location
/// This method does NOT take in account the gap
T& rawAt(Tsize index) {
  Q_ASSERT(index < capacity_);
  return items_[index];
}


/// Copies the given range to the data pointer
void copyRange(QChar* data, Tsize offset, size_t length) const {
  Q_ASSERT(offset < this->length());
  Q_ASSERT((offset + length) <= this->length());

  // copy the first part
  if (offset < gapBegin()) {
    Tsize len = qMin(gapBegin_ - offset, length);
    memcpy(data, items_ + offset, sizeof(T)*static_cast<Tsize>(len));
    data      += len;   // increase the pointer
    offset    += len;
    length    -= len;
  }

  if (length > 0) {
    memcpy(data, items_ + offset + gapSize(), sizeof(T)*static_cast<Tsize>(length));
  }
}


/// This method returns a direct pointer to the 0-terminated buffer
/// This pointer is only valid as long as the buffer doesn't change
/// WARNING, this method MOVES the gap! Which means this method should NOT be used for a lot of operations
T* data() {
  ensureGapSize(1);
  moveGapTo(length());
  items_[length()] = QChar(); // a \0 character
  return items_;
}


//// moves the gap to the given position
//// Warning when the gap is moved after the length the gap shrinks
void moveGapTo(Tsize offset) {
  Q_ASSERT(offset <= capacity_);
  Q_ASSERT(offset <= length());
  if (offset != gapBegin_) {
    Tsize gapSize = this->gapSize();

    // move the the data right after the gap
    if (offset < gapBegin_) {
      memmove(items_ + offset + gapSize, items_ + offset, sizeof(T) * (gapBegin_ - offset));   // memmove( target, source, size )
    } else {
      memmove(items_ + gapBegin_, items_ + gapEnd_, sizeof(T) * (offset - gapBegin_));  // memmove( target, source, size )
    }
    gapBegin_ = offset;
    gapEnd_   = gapBegin_ + gapSize; // qMin( gapBegin_ + gapSize, capacity_ );

  }
  Q_ASSERT(gapBegin_ <= gapEnd_);

#ifdef GAP_VECTOR_CLEAR_GAP
  memset( items_+gapBegin_, 0, sizeof(T)*(gapEnd_-gapBegin_));
#endif
}


/// Mkes sure there's enough room for the insertation
void ensureGapSize(Tsize requiredSize) {
  if (gapSize() < requiredSize) {
    while (growSize_ < capacity_ / 6) { growSize_ *= 2; }
    resize(capacity_ + requiredSize + growSize_ - gapSize());
  }
}


/// resizes the array of data
void resize(Tsize newSize)
{
  if (capacity_ >= newSize) return;

  Tsize lengte = length();
  Q_ASSERT(lengte <= capacity_);
  /// TODO: optimize, so data is moved only once
  /// in other words, gap movement is not required over here!!
  /// this can be done with 2 memcopies
  //qlog_info() << "BEGIN resize: capacity =" << capacity_<< " => " << newSize;

  moveGapTo(lengte);
  T *newChars = new T[newSize];

  if (capacity_ > 0) {
    memmove(newChars, items_, sizeof(T) * lengte);
    delete[] items_;
  }
  items_ = newChars;
  capacity_ = newSize;
  gapEnd_   = newSize;

  // DEBUG gapsize
#ifdef GAP_VECTOR_CLEAR_GAP
  memset( items_+gapBegin_, 0, sizeof(T)*(gapEnd_-gapBegin_));
#endif
}


/// sets the growsize. The growsize if the amount to reserve extra
void setGrowSize(Tsize size) { growSize_ = size; }

/// returns the growsize
Tsize growSize() { return growSize_; }


/// Converts the 'gap-buffer' to a unit-test debugging string
QString getUnitTestString(QChar gapChar = '_') const {
  QString s;
  Tsize gapBegin = this->gapBegin();
  Tsize gapEnd   = this->gapEnd();
  Tsize capacity = this->capacity();

  for (Tsize i = 0; i < gapBegin; ++i) {
    if (items_[i].isNull()) {
      s.append("@");
    } else {
      s.append(items_[i]);
    }
  }
  s.append( "[" );
  for (Tsize i = gapBegin; i < gapEnd; ++i) {
    s.append(gapChar);
  }
  s.append( ">" );
  for (Tsize i=gapEnd; i<capacity; ++i) {
    if (items_[i].isNull()) {
      s.append("@");
    } else {
      s.append(items_[i]);
    }
  }
  return s;
}


/// Converts the 'gap-buffer' to a unit-test debugging string
QString getUnitTestString2() const {
  QString s;
  Tsize gapBegin = this->gapBegin();
  Tsize gapEnd   = this->gapEnd();
  Tsize capacity = this->capacity();

  for (Tsize i = 0; i < capacity; i++) {
    if (i) { s.append(","); }
    if (gapEnd == i) s.append(">");
    s.append(QStringLiteral("%1").arg( "X" ));
    if (gapBegin == i) s.append("[");
  }

  s.append(" | ");
  s.append(QStringLiteral("gapBegin: %1, gapEnd: %2, capacity: %3, length: %4").arg(gapBegin).arg(gapEnd).arg(capacity).arg(length()));
  return s;
}


protected:

T *items_;           ///< The item data
Tsize capacity_;    ///< The number of reserved bytes
Tsize gapBegin_;    ///< The start of the gap
Tsize gapEnd_;      ///< The end of the gap
Tsize growSize_;    ///< The size to grow extra
};


/// The character vecor to use
class EDBEE_EXPORT QCharGapVector : public GapVector<QChar, size_t>
{
public:

  QCharGapVector(size_t size = 16) : GapVector<QChar>(size){}

  /// initializes the vector with a given string
  QCharGapVector(const QString& data, size_t gapSize) : GapVector<QChar>(static_cast<size_t>(data.length()) + gapSize)
  {
    memcpy(items_, data.constData(), sizeof(QChar) * static_cast<size_t>(data.length()));
    gapBegin_ = static_cast<size_t>(data.length());
    gapEnd_ = capacity_;
  }


  /// Initializes the gapvector
  void init(const QString& data, size_t gapSize)
  {
    delete items_;
    capacity_ = static_cast<size_t>(data.length()) + gapSize;
    items_ = new QChar[capacity_];
    memcpy(items_, data.constData(), sizeof(QChar) * static_cast<size_t>(data.length()));
    gapBegin_ = static_cast<size_t>(data.length());
    gapEnd_ = capacity_;
    growSize_ = 16;
  }


  /// a string replace function
  void replaceString(size_t offset, size_t length, const QString& data) {
    replace(offset, length, data.constData(), static_cast<size_t>(data.length()));
  }


  /// Retrieve a QString part
  QString mid(size_t offset, size_t length) const
  {
    if (length == 0) return QString();

    QChar* data = new QChar[length];
    copyRange(data, offset, length);
    QString str(data, static_cast<qsizetype>(length));
    delete[] data;

    return str;
  }
};


/// A special GapVector class that isn't a gapvector. It forwards it's request to a normal vector class
/// (for debugging purposes) that isn't a gapv
/// This class is only used for debugging issuess with the gapvector
template <typename T, typename Tsize>
class EDBEE_EXPORT NoGapVector {
public:
  NoGapVector(Tsize capacity = 16) {
    Q_UNUSED(capacity);
  }

  ~NoGapVector() {
  }

  /// returns the used length of the data
  inline Tsize length() const { return items_.size(); }
  inline Tsize gapSize() const { return 0; }
  inline Tsize gapBegin() const { return 0; }
  inline Tsize gapEnd() const { return 0; }
  inline Tsize capacity() const { return items_.capacity(); }


  /// clears the data
  void clear()
  {
    items_.clear();
  }


public:

  /// Replaces the given items
  /// @param offset the offset of the items to replace
  /// @param lenth the number of items to replace
  /// @param data an array with new items
  /// @param newLength the number of items in the new array
  void replace(Tsize offset, Tsize length, const T* data, Tsize newLength) {
    items_.remove(offset, length);
  for (Tsize i = 0; i < newLength; i++) {
    items_.insert(offset+i,data[i]);
  }
}


/// this method replaces the given items with a single data item
/// @param offset the offset of the items to replace
/// @param lenth the number of items to replace
/// @param newLength the number of times to repeat data
void fill(Tsize offset, Tsize length, const T& data, Tsize newLength) {
  items_.remove(offset,length);
  for (Tsize i = 0; i < newLength; i++) {
    items_.insert(offset+i,data);
  }
}

/// convenient append method
void append(T t) {
  items_.append(t);
}


/// another append method
void append(const T* t, Tsize length) {
  for (Tsize i = 0; i < length; i++) {
    items_.append(t[i]);
  }
}


/// This method returns the item at the given index
T at(Tsize offset) const {
  return items_.at(offset);
}


/// This method sets an item at the given index
void set(Tsize offset, const T& value) {
  items_.replace(offset,value);
}


/// This method return an index
T& operator[](Tsize offset) {
  return items_[offset];
}

protected:

QVector<T> items_;
};


} // edbee
