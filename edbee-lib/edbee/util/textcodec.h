/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#include <QObject>
#include <QList>
#include <QHash>
#include <QTextEncoder>

namespace edbee {

class TextCodec;

class TextCodecManager
{
public:
    TextCodecManager();
    ~TextCodecManager();

public:
    void registerTextCodec( TextCodec* codec );
    QList<TextCodec*> codecList() { return codecList_; }
    TextCodec* codecForName( const QString& name );


private:
    QList<TextCodec*> codecList_;
    QHash<QString,TextCodec*> codecRefMap_;

    friend class TextEditorManager;
};



class TextCodec
{
public:
    static QList<TextCodec*> all();
    static void registerTextCodec( TextCodec* codec );

    TextCodec( const QString& name, const QTextCodec* codec, QTextCodec::ConversionFlags flags );
    const QTextCodec* codec();
    QTextEncoder *makeEncoder();
    QTextDecoder* makeDecoder();

    QString name() { return name_; }

private:
    QString name_;                          ///< The name of this codec
    const QTextCodec* codecRef_;            ///< The QT Codec
    QTextCodec::ConversionFlags flags_;     ///< Special conversion flags

};

} // edbee
