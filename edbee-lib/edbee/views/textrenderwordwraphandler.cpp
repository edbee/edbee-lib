#include "textrenderwordwraphandler.h"

#include <QTextLayout>

#include "edbee/models/textdocument.h"
#include "edbee/views/textrenderer.h"

#include "edbee/debug.h"

namespace edbee {

TextRendererWordwrapHandler::TextRendererWordwrapHandler(TextRenderer* renderer)
    : renderRef_(renderer)
{

}

TextRendererWordwrapHandler::~TextRendererWordwrapHandler()
{

}


TextDocument* TextRendererWordwrapHandler::textDocument()
{
    return renderRef_->textDocument();
}

int TextRendererWordwrapHandler::yPosForLine( int line )
{
    if( line < wordwrapLineYOffsetCache_.length() ) {
        return wordwrapLineYOffsetCache_.at(line);
    }
#if 1
    // calculate line-heights
    wordwrapLineYOffsetCache_.clear();
    wordwrapLineYOffsetCache_.append(0);
    int y = 0;
    for( int i=0, cnt=textDocument()->lineCount()+1; i<cnt; ++i ) {
        QTextLayout* layout = renderRef_->textLayoutForLine(i);
        if( layout ) {
           y += layout->boundingRect().height();
        } else {
qDebug() << "SINGLE LINE HEIGHT";
           y += renderRef_->lineHeightSingleLine();
        }
        wordwrapLineYOffsetCache_.append(y);
    }
    if( line < wordwrapLineYOffsetCache_.length() ) {
      return wordwrapLineYOffsetCache_.at(line);
    } else {
      return  wordwrapLineYOffsetCache_.last();
    }
#else

    // else calculate the y position of the given line
    if( wordwrapLineYOffsetCache_.empty()) {
        wordwrapLineYOffsetCache_.append(0);
    }

    int y = wordwrapLineYOffsetCache_.last();
    int idx = wordwrapLineYOffsetCache_.length();

    int toLine = qMin(line,textDocument()->lineCount()+1);

    for( int i=idx; i <= toLine; i++ ) {
        QTextLayout* layout = renderRef_->textLayoutForLine(i);
        if( layout ) {
            y += layout->boundingRect().height();
        } else {
            y += renderRef_->lineHeightSingleLine();
        }
        wordwrapLineYOffsetCache_.append(y);
    }
    return wordwrapLineYOffsetCache_.at(toLine);

//       if( idx == 0 ) {
//           wordwrapLineYOffsetCache_.append(0);
//           QTextLayout* layout = textLayoutForLine(0);
//           y += layout->boundingRect().height();
//           idx+=1;
//           wordwrapLineYOffsetCache_.append(y);
//qDebug() << " * 0:" << layout->boundingRect().height() << "(" << y << ")";
//       }

//       for( int i=idx; i<=line; ++i) {
//         QTextLayout* layout = textLayoutForLine(i);
//         if( layout ) {
//            y += layout->boundingRect().height();
//qDebug() << " * "<< i<<":" << layout->boundingRect().height() << "(" << y << ")";
//         } else {
//             qDebug() << "SINGLE LINE HEIGHT";
//            y += this->lineHeightSingleLine();
//         }
//         wordwrapLineYOffsetCache_.append(y);
//       }
//       wordwrapLineYOffsetCache_.append(y);

qDebug() << wordwrapLineYOffsetCache_;

#endif
    return y;

}

int TextRendererWordwrapHandler::lineIndexForYpos( int y )
{
    // within current caching range
    if( wordwrapLineYOffsetCache_.length() > 0 && y <= wordwrapLineYOffsetCache_.last() ) {
        QVector<int>::iterator itr = qUpperBound(wordwrapLineYOffsetCache_.begin(), wordwrapLineYOffsetCache_.end(), y);
        return (itr - wordwrapLineYOffsetCache_.begin());
    }

    // else we need to find the position
    for( int line=wordwrapLineYOffsetCache_.length(), cnt = textDocument()->lineCount()-1; line<cnt; ++line ) {
        if( yPosForLine(line) > y ) {
            return line;
        }
    }
    return textDocument()->lineCount();
}

int TextRendererWordwrapHandler::lineHeight(int line)
{
    QTextLayout* layout = renderRef_->textLayoutForLine(line);
    return layout->boundingRect().height();
}


void TextRendererWordwrapHandler::invalidateFromLine(int fromLine) {
    if( fromLine == 0 ) {
        wordwrapLineYOffsetCache_.clear();
    } else if( wordwrapLineYOffsetCache_.length() >= fromLine ) {
        wordwrapLineYOffsetCache_.remove(fromLine,wordwrapLineYOffsetCache_.length() - fromLine );
    }
}


}
