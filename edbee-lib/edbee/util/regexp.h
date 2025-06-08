// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QString>

namespace edbee {

/// The minimal engine we currently require for handling regexpt.
/// It may grow in the future
class EDBEE_EXPORT RegExpEngine {
public:
    virtual ~RegExpEngine() {}
    virtual QString pattern() = 0;
    virtual bool isValid() = 0;
    virtual QString error() = 0;

    /// returns the index of the given substring
    /// @param offset the offset to start, negative is to search from the end (-1 from the last position)
    /// @return the matched index of (std::string::npos on no match)
    virtual size_t indexIn(const QString& str, ptrdiff_t offset) = 0;

    /// returns the index of the given substring
    /// @param offset the offset to start, negative is to search from the end (-1 from the last position)
    /// @return the matched index of (std::string::npos on no match)
    virtual size_t indexIn(const QChar* str, ptrdiff_t offset, size_t length) = 0;

    /// returns the last index of the given substring
    /// @param offset the offset to start, negative is to search from the end (-1 from the last position)
    /// @return the matched index of (std::string::npos on no match)
    virtual size_t lastIndexIn(const QString& str, ptrdiff_t offset) = 0;
    virtual size_t lastIndexIn(const QChar* str, ptrdiff_t offset, size_t length) = 0;

    /// returns the position of the given match or std::string::npos if there's no nth match
    virtual size_t pos(size_t nth = 0) const = 0;

    /// returns the length of the given match or std::string::npos if there's no nth match
    virtual size_t len(size_t nth = 0) const = 0;

    /// returns the nth match or std::string::npos if there's no nth match
    virtual QString cap(size_t nth = 0) const = 0;
};



/// A class for matching QStrings with the Oniguruma API
/// We need this Regular Expression library to be able to support tmLanguages fully
/// I tried to make this class as close as possible to the QRegExp library
class EDBEE_EXPORT RegExp {
public:
    enum Engine {
        EngineOniguruma = 1,
        EngineQRegExp = 2
        // EngineQRegularExpression = 3,

//        QRegExp::RegExp	0	A rich Perl-like pattern matching syntax. This is the default.
//        QRegExp::RegExp2	3	Like RegExp, but with greedy quantifiers. (Introduced in Qt 4.2.)
//        QRegExp::Wildcard	1	This provides a simple pattern matching syntax similar to that used by shells (command interpreters) for "file globbing". See QRegExp wildcard matching.
//        QRegExp::WildcardUnix	4	This is similar to Wildcard but with the behavior of a Unix shell. The wildcard characters can be escaped with the character "\".
//        QRegExp::FixedString	2	The pattern is a fixed string. This is equivalent to using the RegExp pattern on a string in which all metacharacters are escaped using escape().
//        QRegExp::W3CXmlSchema11	5	The pattern is a regular expression as defined by the W3C XML Schema 1.1 specification.
    };

    enum Syntax {
        SyntaxDefault,          /// The default syntax
        SyntaxFixedString       /// A plain fixed string
    };

    RegExp(const QString& pattern, bool caseSensitive=true, Syntax syntax=SyntaxDefault, Engine engine=EngineOniguruma);
    virtual ~RegExp();

    static QString escape( const QString& str, Engine engine=EngineOniguruma );

    bool isValid() const;
    QString errorString() const ;
    QString pattern() const ;


    // size_t indexIn(const QString& str, ptrdiff_t offset = 0);
    size_t indexIn(const QString& str, size_t offset = 0);
    // size_t indexIn(const QChar* str, ptrdiff_t offset, size_t length);
    size_t indexIn(const QChar* str, size_t offset, size_t length);
    // size_t lastIndexIn(const QString& str, ptrdiff_t offset = -1);
    size_t lastIndexIn(const QString& str, size_t offset);
    // size_t lastIndexIn(const QChar* str, ptrdiff_t offset, size_t length);
    size_t lastIndexIn(const QChar* str, size_t offset, size_t length);
    size_t pos(size_t nth = 0) const;
    size_t len(size_t nth = 0) const;
    QString cap(size_t nth = 0) const;

    /// matched length is equal to pos-0-length
    size_t matchedLength() { return len(0); }

private:
    RegExpEngine* d_;       ///< The private data member
};

} // edbee
