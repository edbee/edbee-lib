#include "textautocompleteprovider.h"

#include "edbee/models/textdocument.h"
#include "edbee/models/textdocumentscopes.h"
#include "edbee/models/textgrammar.h"

#include "edbee/debug.h"

namespace edbee {

TextAutoCompleteItem::TextAutoCompleteItem(const QString &label, const QString &usage, const QString &type)
    : label_(label),
      usage_(usage),
      type_(type)
{
}


/// returns the autocmomplete label
QString TextAutoCompleteItem::label() const
{
    return label_;
}

QString TextAutoCompleteItem::usage() const
{
    return usage_;
}

QString TextAutoCompleteItem::type() const
{
    return type_;
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
    //and       break     do        else      elseif
    //end       false     for       function  if
    //in        local     nil       not       or
    //repeat    return    then      true      until     while
    //edbee->grammarManager()
    //if( word.length() < 3 || word.toLower() == "and" || word.toLower() == "break" || word.toLower() == "else" || word.toLower() == "elseif" || word.toLower())
    if( word.length() < 3 )
        return 0;
    if ( label_.toLower().startsWith(word.toLower()) ) {
        return 1;
    } else if ( label_.toLower().contains(word.toLower()) ) {
        return 2;
    } else {
        return 0;
    }
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
    QMultiMap<int, TextAutoCompleteItem *> items;

    foreach( TextAutoCompleteItem* item, itemList_ ) {
        int match = item->matchLabelScore(document,range,word);
        if( match ) {
            items.insert(match, item);
        }
    }
    return items.values();
}


/// directly add a label
void StringTextAutoCompleteProvider::add(const QString &label, const QString &usage, const QString &type)
{
    itemList_.push_back(new TextAutoCompleteItem(label, usage=="" ? label + "()" : usage, type));
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
