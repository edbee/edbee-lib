// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "textdocumentscopestest.h"

#include "edbee/models/textdocumentscopes.h"
#include "edbee/edbee.h"

#include "edbee/debug.h"

namespace edbee {


void TextDocumentScopesTest::testStartsWith()
{
    TextScopeManager* sm = Edbee::instance()->scopeManager();
    TextScope* source = sm->refTextScope("aa.bb.cc.dd.ee");

    testTrue(source->startsWith(sm->refTextScope("aa.bb")));
    testTrue(source->startsWith(sm->refTextScope("aa.*")));
    testTrue(source->startsWith(sm->refTextScope("*.bb")));
    testTrue(source->startsWith(sm->refTextScope("*")));
    testFalse(source->startsWith(sm->refTextScope("bb")));
    testFalse(source->startsWith(sm->refTextScope("bb")));
}


void TextDocumentScopesTest::testRindexOf()
{
    TextScopeManager* sm = Edbee::instance()->scopeManager();
    TextScope* source = sm->refTextScope("aa.bb.cc.dd.ee");

    testEqual(source->rindexOf(sm->refTextScope("aa.bb")), 0);
    testEqual(source->rindexOf(sm->refTextScope("cc.dd")), 2);
    testEqual(source->rindexOf(sm->refTextScope("dd.ee")), 3);
    testEqual(source->rindexOf(sm->refTextScope("bb.aa")), std::string::npos);

    testEqual(source->rindexOf(sm->refTextScope("*.cc.*.ee")), 1);
    testEqual(source->rindexOf(sm->refTextScope("aa.bb.cc.dd.ee")), 0);

    testEqual(source->rindexOf(sm->refTextScope("*") ), 4);
}

/// This method tests the score selector
void TextDocumentScopesTest::testScopeSelectorRanking()
{
    TextScopeManager* sm = Edbee::instance()->scopeManager();
    sm->reset();

    TextScopeList* multiScope = sm->createTextScopeList("text.html.markdown meta.paragraph.markdown markup.bold.markdown");

    QList<TextScopeSelector*> selectors;
    selectors.append(new TextScopeSelector("text.* markup.bold"));
    selectors.append(new TextScopeSelector("text markup.bold"));
    selectors.append(new TextScopeSelector("markup.bold"));
    selectors.append(new TextScopeSelector("text.html meta.*.markdown markup"));
    selectors.append(new TextScopeSelector("text.html meta.* markup"));
    selectors.append(new TextScopeSelector("text.html * markup"));
    selectors.append(new TextScopeSelector("text.html markup"));
    selectors.append(new TextScopeSelector("text markup"));
    selectors.append(new TextScopeSelector("markup"));
    selectors.append(new TextScopeSelector("text.html"));
    selectors.append(new TextScopeSelector("text"));

    double lastRank = 1.0;
    for (qsizetype i = 0; i < selectors.size(); ++i)
    {
        TextScopeSelector* sel = selectors.at(i);
        double rank = sel->calculateMatchScore(multiScope);
        if (!(rank < lastRank)) {
            qlog_info() << "SCOPES: " << multiScope->toString();
            qlog_info() << "  PREV: " << selectors.at(i-1)->toString();
            qlog_info() << "  rank: " << lastRank;
            qlog_info() << "";
            qlog_info() << "   NEW: " << sel->toString();
            qlog_info() << "  rank: " << rank;
        }

        testTrue(rank < lastRank);
        lastRank = rank;
    }
    qDeleteAll(selectors);
    delete multiScope;
}

} // edbee
