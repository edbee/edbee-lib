// edbee - Copyright (c) 2012-2025 by Rick Blommers and contributors
// SPDX-License-Identifier: MIT

#include "lineoffsetvectortest.h"

#include "edbee/models/textbuffer.h"
#include "edbee/util/lineoffsetvector.h"

#include "edbee/debug.h"


namespace edbee {

#define testLov(v,expected) testEqual(v.toUnitTestString(),expected)
#define testLov2(v,expected,expectedRaw) { testEqual(v.toUnitTestString(),expected); testEqual(v.toUnitTestFilledString(),expectedRaw); }

void apply(LineOffsetVector& v, size_t line, size_t removeCount, size_t addCount, ptrdiff_t* data, ptrdiff_t offsetDelta)
{
    {
        QDebug dbg(QtDebugMsg);
        dbg << " - " << v.toUnitTestString() << "=>" << v.toUnitTestFilledString();
        dbg << "\n";
        dbg << " > apply(line:" << line << ", del:" << removeCount << ", add:" << addCount << ", {";
        for (size_t i = 0; i < addCount; ++i) {
            dbg << data[i];
        }
        dbg << "},"<< offsetDelta << ")";
        dbg << "\n";
    }

    v.applyChange(line, removeCount, addCount, data, offsetDelta);
    qDebug() << " - " << v.toUnitTestString() << "=>" << v.toUnitTestFilledString();
}


void LineOffsetVectorTest::applyChange()
{
    LineOffsetVector v;

    // Offset only changes
    //====================
    qDebug() << "===" << "test change offset before delta ==";
    {
        // Positive Delta
        v.initForUnitTesting(3, 2, { 2, 4, 6, 8 }); // => "2,4,[3>6,8" => "2,4,9,11"
        apply(v, 0, 0, 0, nullptr, 10);
        testLov2(v, "2,[13>1,6,8", "2,14,19,21");

        // Negative Delta
        v.initForUnitTesting(3, 2, { 2, 4, 6, 8 }); // => "2,4,[3>6,8" => "2,4,9,11"
        apply(v, 0, 0, 0, nullptr, -1);
        testLov2(v, "2,[2>1,6,8", "2,3,8,10");

        // Invalid Negative Delta
        // * This fails the asserton (because the inserted offset needs bigger the the offset before, so we don't invoke it
        // v.initForUnitTesting(3, 2, 2, 4, 6, 8, std::string::npos); // => "2,4,[3>6,8" => "2,4,9,11"
        // apply(v, 0, 0, 0, nullptr, -5);
        // testLov2(v, "2,[-2>1,6,8", "2,-1,4,6");
    }
    qDebug() << "===" << "test change offset at delta ==";
    {
        // Positive Delta
        v.initForUnitTesting(3, 2, { 2, 4, 6, 8 }); // => "2,4,[3>6,8" => "2,4,9,11"
        apply(v, 1, 0, 0, nullptr, 10);
        testLov2(v, "2,4,[13>6,8", "2,4,19,21");

        // Negative Delta
        v.initForUnitTesting(3, 2, { 2, 4, 6, 8 }); // => "2,4,[3>6,8" => "2,4,9,11"
        apply(v, 1, 0, 0, nullptr, -1);
        testLov2(v, "2,4,[2>6,8", "2,4,8,10");

        // Invalid Negative Delta
        // * This fails the asserton (because the inserted offset needs bigger the the offset before, so we don't invoke it
        // apply(v, 1, 0, 0, nullptr, -5);
        // testLov2(v, "2,4,[-2>6,8", "2,4,4,6");
    }

    qDebug() << "===" << "test change offset past delta ==";
    {
        // Positvie Delta
        v.initForUnitTesting(3, 2, { 2, 4, 6, 8, 10 }); // => "2,4,[3>6,8,10" => "2,4,9,11,13"
        apply(v, 3, 0, 0, nullptr, 10);
        testLov2(v, "2,4,9,11,[13>10", "2,4,9,11,23");

        // Negative delta
        v.initForUnitTesting(3, 2, { 2, 4, 6, 8, 10 }); // => "2,4,[3>6,8,10" => "2,4,9,11,13"
        apply(v, 2, 0, 0, nullptr, -1);
        testLov2(v, "2,4,9,[2>8,10", "2,4,9,10,12");

        // Invalid Negative Delta
        // * This fails the asserton (because the inserted offset needs bigger the the offset before, so we don't invoke it
        // v.initForUnitTesting(3, 2, 2, 4, 6, 8, 10, std::string::npos); // => "2,4,[3>6,8,10" => "2,4,9,11,13"
        // apply(v, 2, 0, 0, nullptr, -2);
        // testLov2(v, "2,4,9,[1>8,10", "2,4,9,9,12");
    }

/*

    // Line Changes
    //=============

    // test change offset before delta (not passing past delta)
    qDebug() << "===" << "test change offset before delta (not passing past delta) ==";
    {
        v.initForUnitTesting(3, 2, 2, 4, 6, 8, 10, std::string::npos); // => "2,4,[3>6,8,10" => "2,4,9,11,13"

        size_t newOffsets[] = { 3 };
        apply(v, 0, 0, 1, newOffsets, 4);
        testLov2(v,"3,[4>2,4,9,11", "3,6,13,15,17");
    }

    // test change offset after delta
    qDebug() << "===" << "test change offset after delta ==";
    {
        v.initForUnitTesting(3, 2, 2, 4, 6, 8, 10, std::string::npos); // => "2,4[3>6,8"

        size_t newOffsets[] = { 14 };
        apply(v, 3, 0, 1, newOffsets, 10);
        testLov(v,"2,4,9,14[6>8,10");
    }
*/
}


void LineOffsetVectorTest::testMoveDeltaToIndex()
{
    LineOffsetVector v;
    v.initForUnitTesting(3, 2, { 2, 4, 6, 8 });
    testLov2(v, "2,4,[3>6,8", "2,4,9,11");

    // move to the left
    v.moveDeltaToIndex(1);
    testLov2(v, "2,[3>1,6,8", "2,4,9,11");

    // move to the right
    v.moveDeltaToIndex(2);
    testLov2(v, "2,4,[3>6,8", "2,4,9,11");

    // one step further
    v.moveDeltaToIndex(3);
    testLov2(v, "2,4,9,[3>8", "2,4,9,11");

    // moving to the left 2 steps should force the delta-0 rule (2>1)
    v.moveDeltaToIndex(1);
    testLov2(v,"2,[0>4,9,11", "2,4,9,11");
}


void LineOffsetVectorTest::testChangeOffsetDelta()
{
    LineOffsetVector v;
    v.initForUnitTesting(3, 2, { 2, 4, 6, 8 }); // offsetdelta, offsetindex, offsets...
    testLov2(v,"2,4,[3>6,8", "2,4,9,11");

    // test the same location
    v.changeOffsetDelta(2, 4);
    testLov2(v, "2,4,[7>6,8", "2,4,13,15");

    // test an offset before the current location
    v.changeOffsetDelta(1, -4);
    //testLov(v,"2[6>-2,6,8");
    // testLov(v,"2[6>6,8");
    testLov2(v, "2,[3>1,6,8", "2,4,9,11");

    // test a change
    v.changeOffsetDelta(1, 1);
    // testLov(v,"2[2>-2,6,8");
    testLov2(v, "2,[4>1,6,8", "2,5,10,12");

    // test an offset after the current location
    v.changeOffsetDelta(4, -1);
    //testLov(v,"2,0,8[0>8");  // <<< THIS Seems like a bug!!!! (offset need to increase?)
    testLov2(v, "2,5,10,12,[0>", "2,5,10,12");

}


#define V_TEXT_REPLACED(offset,lengthIn,str) do {\
    QString qstr(str); \
    TextBufferChange change(&v, offset, lengthIn, qstr.data(), qstr.length() ); \
    v.applyChange(change); \
} while(false)


void LineOffsetVectorTest::testTextReplaced()
{
    LineOffsetVector v;
    V_TEXT_REPLACED(0,0,"a\nb\nc\nd\ne");
    testLov2(v, "0,2,4,6,8,[0>", "0,2,4,6,8");

    // next insert a newline
    V_TEXT_REPLACED(3,0,"\n");
    testLov2(v, "0,2,4,[1>4,6,8", "0,2,4,5,7,9");

    v.moveDeltaToIndex(0);
    testLov2(v, "[0>0,2,4,5,7,9", "0,2,4,5,7,9");

    V_TEXT_REPLACED(0,0,"\n");
    testLov2(v, "0,1,[1>2,4,5,7,9", "0,1,3,5,6,8,10");

    V_TEXT_REPLACED(0,11,"");
    testLov2(v, "0,[0>", "0");

}

void LineOffsetVectorTest::testFindLineFromOffset()
{
    LineOffsetVector v;

    // offset 0,4
    v.initForUnitTesting(0, 0, { 0, 4 });
    testEqual(v.findLineFromOffset(0), 0);
    testEqual(v.findLineFromOffset(1), 0);
    testEqual(v.findLineFromOffset(2), 0);
    testEqual(v.findLineFromOffset(3), 0);
    testEqual(v.findLineFromOffset(4), 1);


    // offsets: 0,4
    v.initForUnitTesting(2, 1, { 0, 2 });
    testEqual(v.findLineFromOffset(0), 0);
    testEqual(v.findLineFromOffset(1), 0);
    testEqual(v.findLineFromOffset(2), 0);
    testEqual(v.findLineFromOffset(3), 0);
    testEqual(v.findLineFromOffset(4), 1);
}

} // edbee
