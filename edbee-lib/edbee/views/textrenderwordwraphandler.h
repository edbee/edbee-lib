#pragma once

#include <QVector>

namespace edbee {

class TextDocument;
class TextRenderer;

/// A class used for calculating line positions, offsets
/// and caching text-layotus
class TextRendererWordwrapHandler
{
public:
    TextRendererWordwrapHandler(TextRenderer* renderer);
    virtual ~TextRendererWordwrapHandler();

    TextDocument* textDocument();
    int yPosForLine( int line );
    int lineIndexForYpos( int y );
    int lineHeight(int line);
    void invalidateFromLine(int fromLine);

protected:

    TextRenderer* renderRef_;
    QVector<int> wordwrapLineYOffsetCache_;          ///< A cache which remembers the y-offset of EVERY line (in the case of word wrapping)
    friend class TextRenderer;
};

}
