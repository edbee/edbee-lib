#pragma once

#include "edbee/exports.h"

#include <QList>
#include <QString>
#include <QMultiMap>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QStandardItem>

namespace edbee {

class TextDocument;
class TextRange;

/// An autocomplete item that's being returned
/// Currently simply a string.
/// It's placed in a seperate class for future extentions (LSP: https://github.com/Microsoft/language-server-protocol/blob/master/protocol.md#textDocument_completion)
class EDBEE_EXPORT TextAutoCompleteItem {
public:
    TextAutoCompleteItem( const QString& label, const int kind, const QString& detail, const QString& documentation );
    QString label() const;
    int kind() const;
    QString detail() const;
    QString documentation() const;

    int matchLabelScore( TextDocument* document, const TextRange& range, const QString& word );

protected:
    QString label_;
    int kind_;
    QString detail_;
    QString documentation_;
};


/// A base autocomplete provider
class EDBEE_EXPORT TextAutoCompleteProvider {
public:
    virtual ~TextAutoCompleteProvider() {}
    virtual QList<TextAutoCompleteItem*> findAutoCompleteItemsForRange( TextDocument* document, const TextRange& range, const QString& word ) = 0;
};


/// a fixed stringlist autocompleter
class EDBEE_EXPORT StringTextAutoCompleteProvider : public TextAutoCompleteProvider
{
public:
    virtual ~StringTextAutoCompleteProvider();
    virtual QList<TextAutoCompleteItem*> findAutoCompleteItemsForRange( TextDocument* document, const TextRange& range, const QString& word ) ;

    virtual void add(const QString& label, const int kind, const QString& detail = "", const QString& documentation = "");
    virtual void give(TextAutoCompleteItem* item);
protected:
    QList<TextAutoCompleteItem*> itemList_;

};



/// The instance autocomplete provider, managers which autocompleters are available for
/// the given editor.
///
/// The idea is the following
///   TextDocument -> TextAutoCompleteProvider
///                     -> Static provider for the current editor
///                     -> Dynamic document content autocompletion provider
///                     -> Reference to Global autocompletion
///                     -> .. add custom local providers..
///   Edbee
///       -> TextAutoCompleteManager
///           -> plain text autocomplete provider global
///           -> Scope Autocompleter
///           -> .. add custom global providers ..
///
class EDBEE_EXPORT TextAutoCompleteProviderList : public TextAutoCompleteProvider
{
public:
    TextAutoCompleteProviderList( TextAutoCompleteProvider* parentProvider=0);
    virtual ~TextAutoCompleteProviderList();

    virtual QList<TextAutoCompleteItem*> findAutoCompleteItemsForRange(TextDocument* document, const TextRange &range, const QString& word );
    virtual void giveProvider(TextAutoCompleteProvider* provider);
    virtual void setParentProvider(TextAutoCompleteProvider* provider);

protected:
    QList<TextAutoCompleteProvider*> providerList_;         ///< The autocomplete provider list
    TextAutoCompleteProvider* parentProviderRef_;           ///< A reference to the global autocomplete provider
};



/*
/// a grammar based autocomplete list
class EDBEE_EXPORT ScopeBasedTextAutoCompleteProvider : public TextAutoCompleteProvider
{
public:
    virtual QList<TextAutoCompleteItem*> findAutoCompleteItemsForRange( TextDocument* document, TextRange& range );

protected:
    /// TODO: Add a scope-based list with autocomplete listgs :S
};


/// a texteditor widget specific provider
class EDBEE_EXPORT TextEditorSpecificTextAutoCompleteProvider : public TextAutoCompleteProvider
{
public:
    virtual QList<TextAutoCompleteItem*> findAutoCompleteItemsForRange( TextDocument* document, TextRange& range );

};




/// This class is used to manage the automcomplete provider
class EDBEE_EXPORT TextAutoCompleteManager {
public:
    TextAutoCompleteManager();
    virtual ~TextAutoCompleteProvider();

    void giveProvider(TextAutoCompleteProvider* provider);

protected:

    QList<TextAutoCompleteProvider*> providerList_;
};

*/

}
