/**
 * Copyright 2011-2013 - Reliable Bits Software by Blommers IT. All Rights Reserved.
 * Author Rick Blommers
 */


#include <QMetaObject>
#include <QMetaMethod>
#include <QDebug>

#include "util/test.h"

namespace edbee { namespace test {


//=============================================================================
// TestResult
//=============================================================================

TestResult::TestResult(TestCase* testCase, const QString &methodName, const QString& description, const char* file, int lineNumber )
    : testCaseRef_(testCase)
    , methodName_( methodName )
    , description_( description )
    , fileNameRef_ ( file )
    , lineNumber_( lineNumber )
    , compareStatement_(false)
    , actualStatementRef_("")
    , expectedStatementRef_("")
    , actualValue_("")
    , expectedValue_("")
    , status_( Passed )
{
}

void TestResult::setBooleanResult(bool result, const char* statement)
{
    status_ = result ? Passed : Failed;
    actualStatementRef_ = statement;
}

void TestResult::setCompareResult(bool result, const QString &actualValue, const QString &expectedValue, const char *actualStatement, const char *expectedStatement)
{
    status_ = result ? Passed : Failed;
    compareStatement_ = true;
    actualValue_ = actualValue;
    expectedValue_ = expectedValue;
    actualStatementRef_ = actualStatement;
    expectedStatementRef_ = expectedStatement;

}


void TestResult::setSkip()
{
    status_ = Skipped;
}




//=============================================================================
// TestCase
//=============================================================================




TestCase::TestCase()
    : engineRef_(0)
{
}

/// this method returns the outputhandler
OutputHandler *TestCase::out()
{
    Q_ASSERT(engine());
    return engine()->outputHandler();
}

void TestCase::testTrueImpl(bool condition, const char* statement, const QString& description, const char* file, int line)
{
    TestResult *testResult = new TestResult( engine()->currentTest(), engine()->currentMethodName(), description, file, line );
    testResult->setBooleanResult( condition, statement );
    engine()->giveTestResult(testResult);
}

void TestCase::testEqualImpl(bool result, const QString& actual, const QString& expected, const char* actualStatement, const char* expectedStatement, const QString& description, const char* file, int line)
{
    TestResult* testResult = new TestResult( engine()->currentTest(), engine()->currentMethodName(), description, file, line );
    testResult->setCompareResult( result, actual, expected, actualStatement, expectedStatement );
    engine()->giveTestResult(testResult);
}


void TestCase::testSkipImpl(const QString& description, const char* file, int line)
{
    TestResult* testResult = new TestResult( engine()->currentTest(), engine()->currentMethodName(), description, file, line );
    testResult->setSkip();
    engine()->giveTestResult(testResult);
}


/// this method returns the current testcase
TestCase* TestCase::currentTest()
{
    return engine()->currentTest();
}


// returns the current method name
QString TestCase::currentMethodName()
{
    return engine()->currentMethodName();
}


/// Gives the testresult to the engine
void TestCase::giveTestResultToEngine(TestResult *result)
{
    engine()->giveTestResult(result);
}



//=============================================================================
// QDebug output handler
//=============================================================================

OutputHandler::OutputHandler()
{
}

OutputHandler::~OutputHandler()
{
}

void OutputHandler::startRunAll(TestEngine* engine)
{
    Q_UNUSED(engine);
    qDebug() << "**************** Start Various Bits Test **************";
}

void OutputHandler::endRunAll(TestEngine* engine)
{
    int passedCount=0;
    int failedCount=0;
    int skippedCount=0;
    foreach( TestResult *result, engine->testResultList() ) {
        switch( result->status() ) {
            case TestResult::Passed: ++passedCount; break;
            case TestResult::Failed: ++failedCount; break;
            case TestResult::Skipped:  ++skippedCount; break;
            default: Q_ASSERT_X(false, "TestResult Status","Unkown status returned!");
        }
    }

    qDebug() << " ** completed: " << passedCount << " passed,  " << failedCount << " failures, " << skippedCount << " skipped";
    if( failedCount > 0 ) {
        qDebug() << " ___  _   _  _    ___  __  ";
        qDebug() << "| __|/ \\ | || |  | __||  \\ ";
        qDebug() << "| _|| o || || |_ | _| | o )";
        qDebug() << "|_| |_n_||_||___||___||__/ ";
        qDebug() << "";

    }

}


void OutputHandler::startTestCase( TestEngine* engine )
{
    qDebug() << "class" << engine->currentClassName() << " ================";
}

void OutputHandler::endTestCase( TestEngine* engine )
{
    Q_UNUSED(engine);
    qDebug() << "";

}

void OutputHandler::startTestMethod( TestEngine* engine)
{
    buffer_.append( "- ");
    buffer_.append( engine->currentMethodName());
    buffer_.append( " ");
}

void OutputHandler::endTestMethod( TestEngine* engine)
{
    Q_UNUSED(engine);

    // write the result
    qDebug() << "" << (const char*)QString("%1%2%3").arg(buffer_).arg(failBuffer_).arg(outBuffer_).toLatin1();
//    qDebug() << QString("%1%2%3\n").arg(buffer_).arg(failBuffer_).arg(outBuffer_);

    buffer_.clear();
    failBuffer_.clear();
    outBuffer_.clear();
}


/// This method is called if a test result is added
void OutputHandler::testResultAdded( TestEngine* engine, TestResult* testResult)
{
    Q_UNUSED(engine);
    switch( testResult->status() )
    {
        case TestResult::Passed:
            buffer_.append(".");
            break;

        case TestResult::Skipped:
            buffer_.append("SKIPPED");
            break;

        case TestResult::Failed:
            buffer_.append("*");

            // compare statement
            if( testResult->compareStatement() ) {
                failBuffer_.append( "\n" );
                failBuffer_.append( "    [FAILURE] ").append(testResult->actualStatement()).append(" == ").append(testResult->expectedStatement()).append("\n");
                if( !testResult->description().isEmpty() ) failBuffer_.append("      ").append(testResult->description()).append("\n");
                failBuffer_.append( "      expected: [").append(testResult->expectedValue()).append("]\n");
                failBuffer_.append( "      actual  : [").append(testResult->actualValue()).append("]\n");
                failBuffer_.append( "      ").append(testResult->fileName()).append(":").append( QString("%1").arg(testResult->lineNumber()) );//.append("\n");
            } else {
                failBuffer_.append( "\n" );
                failBuffer_.append( "    [FAILURE] ").append(testResult->statement()).append("\n");
                if( !testResult->description().isEmpty() ) failBuffer_.append("      ").append(testResult->description()).append("\n");
                failBuffer_.append( "      ").append(testResult->fileName()).append(":").append( QString("%1").arg(testResult->lineNumber()) );//.append("\n");
            }
            break;

    }

}



//=============================================================================
// Test Engine
//=============================================================================

TestEngine::TestEngine()
    : outputHandlerRef_(0)
    , currentTestRef_(0)
{
    static OutputHandler outputHandler; // default to a qdebug outputhandler
    outputHandlerRef_ = &outputHandler;
}

TestEngine::~TestEngine()
{
    qDeleteAll( testResultList_ );
    testResultList_.clear();
}


/// this method returns true if the given object is found
bool TestEngine::hasTest(TestCase* object)
{
    if (testRefList_.contains(object)) return true;
    foreach (TestCase* test, testRefList_) {
        //todo: waarom zouden we niet meerdere instanties kunnen hebben?
        if (test->objectName() == object->objectName()) return true;
    }
    return false;
}

/// this metohd adds a test
void TestEngine::addTest(TestCase* object)
{
    if (!hasTest(object)) testRefList_.append(object);
}

/// This method runs all tests
int TestEngine::runAll()
{

    outputHandler()->startRunAll( this );

    int ret = 0;
    foreach( TestCase* test, testRefList_) {
        ret += run(test);
    }

    outputHandler()->endRunAll( this );

    return ret;

}

/// This class executes all tests on the given test object (all private Slots)
/// @param object hte object that needs to be run
int TestEngine::run(TestCase* test)
{
    // start a test
    currentTestRef_ = test;
    currentTestRef_->setEngine(this);

    // retrieve the meta object  and invoke al private slots
    const QMetaObject *metaObject = test->metaObject();

    // retrieve all methods
    int initTestCaseIndex  = metaObject->indexOfSlot("initTestCase()");
    int initTestIndex      = metaObject->indexOfSlot("init()");
    int cleanTestCaseIndex = metaObject->indexOfSlot("cleanTestCase()");
    int cleanTestIndex     = metaObject->indexOfSlot("clean()");

    // next log the class
    outputHandler()->startTestCase( this );

    // initTestCase() will be called before the first testfunction is executed.
    if( initTestCaseIndex >= 0 ) metaObject->invokeMethod(test,"initTestCase");

    // this method invokes all private slots
    for( int methodIdx=0, methodCount=metaObject->methodCount(); methodIdx < methodCount; ++methodIdx ) {

        // invoke the method
        const QMetaMethod method = metaObject->method( methodIdx );
        if( method.access() == QMetaMethod::Private && method.methodType() == QMetaMethod::Slot ) {

            currentMethodName_ = QString( method.methodSignature() );
            if( currentMethodName_.startsWith('_') ) continue; // skip underscore names

            if( currentMethodName_.compare("init()") == 0 || currentMethodName_.compare("clean()")==0 ||
                currentMethodName_.compare("initTestCase()")==0 || currentMethodName_.compare("cleanupTestCase()")==0 ) continue;

            outputHandler()->startTestMethod( this );

            // invoke the test (init) test (clean)
            if( initTestIndex >= 0 ) metaObject->invokeMethod(test,"init");
            method.invoke( test );
            if( cleanTestIndex >= 0 ) metaObject->invokeMethod(test,"clean");


            outputHandler()->endTestMethod( this );

            currentMethodName_.clear();

        }
    }


    // cleanupTestCase() will be called after the last testfunction was executed.
    if( cleanTestCaseIndex >= 0 ) metaObject->invokeMethod(test,"cleanupTestCase");

    // close it
    outputHandler()->endTestCase( this );


    currentTestRef_->setEngine(0);
    currentTestRef_ = 0;


    return 0;

}

/// This method runs the given test
int TestEngine::run(const QString& name)
{
    foreach (TestCase* test, testRefList_) {
        if (test->objectName() == name) {
            return this->run( test );
        }
    }
    return 0;
}

/// Appends the test result
void TestEngine::giveTestResult(TestResult* testResult)
{
    testResultList_.append(testResult);
    outputHandler()->testResultAdded( this, testResult );
}

}} // edbee::test
