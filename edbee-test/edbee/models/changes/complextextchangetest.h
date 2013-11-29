/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */

#pragma once

#include "util/test.h"

namespace edbee {

class ComplexTextChange;
class SingleTextChange;
class TextChange;
class TextDocument;

/// for testing the complex textchanges
class ComplexTextChangeTest : public edbee::test::TestCase
{
    Q_OBJECT

private slots:
    void init();
    void clean();

private slots:
    void testMoveChangesFromGroup_grow();
    void testMoveChangesFromGroup_backspace();
    void testMoveChangesFromGroup_delete();
    void testMoveChangesFromGroup_multiRangeOverlap();
    void testMoveChangesFromGroup_trippleDuplicateIssue();
    void testMoveChangesMergeTest1();
private slots:
    void testMoveChangesMergeTest2();

public slots:
    void testGiveSingleTextChange_addMerge();
    void testGiveSingleTextChange_overlap();

private:
    void runSingleTextChange( int offset, int length, const QString& replacement );
    void runSingleTextChange2( int   offset, int length, const QString& replacement );

    QString fillGroup( const QString& changes, ComplexTextChange* group=0);
    QString mergeResult(const QString& groupDef1, const QString& groupDef2 );

//    TextLineDataManager* manager();
//    LineDataListTextChange* createChange( int line, int length, int newLength );
//    LineDataListTextChange* takeChange(LineDataListTextChange* change);

    TextDocument* doc_;
    ComplexTextChange* group_;
    ComplexTextChange* group2_;
    QList<TextChange*> changeList_;
};


} // edbee

DECLARE_TEST(edbee::ComplexTextChangeTest );
