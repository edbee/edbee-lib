/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QTextCodec>
#include <QApplication>

#include "textcodec.h"

#include "debug.h"

namespace edbee {

TextCodecManager::TextCodecManager()
{
    // append all special encodings
    QList<QByteArray> encList;
    encList << "UTF-8" << "UTF-16" << "UTF-16BE" << "UTF-16LE" << "UTF-32" << "UTF-32BE" << "UTF-32LE";
    foreach( QByteArray enc, encList ) {
        QTextCodec* codec = QTextCodec::codecForName(enc);
        registerTextCodec( new TextCodec( QString(codec->name()), codec, QTextCodec::IgnoreHeader ) );
        registerTextCodec( new TextCodec( QString("%1 with BOM").arg( QString(codec->name()) ), codec, QTextCodec::DefaultConversion ) );
    }

    // append the items
    QList<QByteArray> names = QTextCodec::availableCodecs();
    qSort(names);
    foreach( QByteArray name, names) {
        QTextCodec* codec = QTextCodec::codecForName(name);
        if( !codecRefMap_.contains( codec->name() ) ) {
            TextCodec* textCodec = new TextCodec( name, codec, QTextCodec::DefaultConversion );
            registerTextCodec( textCodec );
        }
    }
}

TextCodecManager::~TextCodecManager()
{
    qDeleteAll(codecList_);
}


void TextCodecManager::registerTextCodec( TextCodec* codec )
{
    codecList_.append(codec);
    codecRefMap_.insert(codec->name(), codec);
}

TextCodec *TextCodecManager::codecForName(const QString& name)
{
    return codecRefMap_.value(name);
}


//----------------------------------------------------------


TextCodec::TextCodec( const QString& name, const QTextCodec* codec, QTextCodec::ConversionFlags flags)
    : name_(name)
    , codecRef_( codec )
    , flags_( flags )
{
}

const QTextCodec *TextCodec::codec()
{
    return codecRef_;
}

QTextEncoder* TextCodec::makeEncoder()
{
    return codec()->makeEncoder( flags_ );
}

QTextDecoder* TextCodec::makeDecoder()
{
    return codec()->makeDecoder( flags_ );
}


} // edbee
