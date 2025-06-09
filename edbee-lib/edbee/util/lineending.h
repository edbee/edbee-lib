// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

namespace edbee {


/// A special class to perform line-ending detection
class EDBEE_EXPORT LineEnding {
public:

    enum Type {
        UnixType = 0,
        WindowsType = 1,
        MacClassicType = 2,
        TypeCount
    };

protected:

    explicit LineEnding(LineEnding::Type ending, const char* chars, const char* escaped_chars, const char* name);
    virtual ~LineEnding();

public:
    static LineEnding* types();
    static int typeCount() { return TypeCount; }
    static LineEnding* get(size_t idx);

    static LineEnding* unixType();
    static LineEnding* windowsType();
    static LineEnding* macClassicType();

    static LineEnding* detect(const QString& str, LineEnding* unkownEnding = nullptr);

    virtual LineEnding::Type type() const { return type_; }

    virtual const char* chars() const { return charsRef_; }

    virtual const char* escapedChars() const { return escapedCharsRef_; }

    /// This method returns the name of the line ending
    virtual const char* name() const { return nameRef_; }

private:

    LineEnding::Type type_;         ///< The type of the line ending
    const char* charsRef_;          ///< The characters reference
    const char* escapedCharsRef_;   ///< The textual display of the chars
    const char* nameRef_;           ///< The name of these line ending
};



} // edbee
