// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "edbee/exports.h"

#include <QObject>
#include <QList>
#include <QHash>
#include <QTextEncoder>

namespace edbee {

class TextCodec;

/// The codec manager is used for managing codecs
/// You shouldnt' instantiatie this class, it's better to access the
/// codec manager instantiated via the edbee::Edbee
class EDBEE_EXPORT TextCodecManager {
public:
    TextCodecManager();
    ~TextCodecManager();

public:
    void giveTextCodec( TextCodec* codec );
    QList<TextCodec*> codecList();
    TextCodec* codecForName( const QString& name );

private:
    QList<TextCodec*> codecList_;
    QHash<QString,TextCodec*> codecRefMap_;
};


/// This class represents a single text codec
/// The codec has a name and contains methods to create encoders and decoders
class EDBEE_EXPORT TextCodec {
public:
    TextCodec( const QString& name, const QTextCodec* codec, QTextCodec::ConversionFlags flags );
    const QTextCodec* codec();
    QTextEncoder* makeEncoder();
    QTextDecoder* makeDecoder();

    QString name() { return name_; }

private:
    QString name_;                          ///< The name of this codec
    const QTextCodec* codecRef_;            ///< The QT Codec
    QTextCodec::ConversionFlags flags_;     ///< Special conversion flags

};

} // edbee
