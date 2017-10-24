#include "textautocompleteprovider.h"

#include "edbee/models/textdocument.h"
#include "edbee/models/textdocumentscopes.h"

#include "edbee/debug.h"

namespace edbee {

TextAutoCompleteItem::TextAutoCompleteItem(const QString &label)
    : label_(label)
{
}


/// returns the autocmomplete label
QString TextAutoCompleteItem::label() const
{
    return label_;
}


/// Compares the given text-autocomplete item with the label
/// This methd should return a match score, where a score of 0 means NO match
int TextAutoCompleteItem::matchLabelScore(TextDocument *document, const TextRange &range, const QString &word)
{
    /// For now a simple prefix-prefix search. Later fuzzy search.
    /// Inspiration:
    /// - https://www.quora.com/How-is-the-fuzzy-search-algorithm-in-Sublime-Text-designed-How-would-you-design-something-similar)
    /// - https://github.com/renstrom/fuzzysearch
    /// We probably need to calculate a score
    return label_.startsWith(word) ? 1 : 0;
}



// -----------------------------


/// Adds the given string to the list
StringTextAutoCompleteProvider::~StringTextAutoCompleteProvider()
{
    qDeleteAll(itemList_);
}

/// Search auto-complete items in the list
QList<TextAutoCompleteItem *> StringTextAutoCompleteProvider::findAutoCompleteItemsForRange(TextDocument *document, const TextRange &range, const QString &word)
{
    QList<TextAutoCompleteItem *> result;
    foreach( TextAutoCompleteItem* item, itemList_ ) {
        if( item->matchLabelScore(document,range,word)) {
            result.append(item);
        }
    }
    return result;
}


/// directly add a label
void StringTextAutoCompleteProvider::add(const QString &label)
{
    itemList_.push_back( new TextAutoCompleteItem(label));
}


/// gives the given autocomplete item
void StringTextAutoCompleteProvider::give(TextAutoCompleteItem *item)
{
    itemList_.push_back(item);
}




// ------------------------------

TextAutoCompleteProviderList::TextAutoCompleteProviderList(TextAutoCompleteProvider *parentProvider)
    : parentProviderRef_(parentProvider)
{
}

TextAutoCompleteProviderList::~TextAutoCompleteProviderList()
{
    qDeleteAll(providerList_);
    providerList_.clear();
}


/// builds an auto-complete list by querying all providers
QList<TextAutoCompleteItem *> TextAutoCompleteProviderList::findAutoCompleteItemsForRange(TextDocument *document, const TextRange &range, const QString &word)
{
    QList<TextAutoCompleteItem*> result;
    foreach( TextAutoCompleteProvider* provider, providerList_) {
        result.append(provider->findAutoCompleteItemsForRange(document,range,word));
    }
    if( parentProviderRef_ ) {
        result.append(parentProviderRef_->findAutoCompleteItemsForRange(document,range,word));
    }
    return result;
}


/// adds a provider
void TextAutoCompleteProviderList::giveProvider(TextAutoCompleteProvider *provider)
{
    providerList_.push_back(provider);
}


/// set the parent provider
void TextAutoCompleteProviderList::setParentProvider(TextAutoCompleteProvider *provider)
{
    parentProviderRef_ = provider;
}



// -----------------------------
/*


/// retuns all auto complete items for the given list
QList<TextAutoCompleteItem *> ScopeBasedTextAutoCompleteProvider::findAutoCompleteItemsForRange(TextDocument *document, TextRange &range)
{
//    TextScopeList scopes = document->scopes()->scopesAtOffset(range.caret());
//    if( scopes.cont)
    /// TODO: Not yet implemented

    return QList<TextAutoCompleteItem*>();
}

*/

// -----------------------------
/*

TextAutoCompleteManager::TextAutoCompleteManager()
{
}

TextAutoCompleteManager::~TextAutoCompleteProvider()
{
    qDeleteAll(providerList_);
}

void TextAutoCompleteManager::giveProvider(TextAutoCompleteManager::~TextAutoCompleteProvider *provider)
{
    providerList_.append(provider);
}
*/

}
