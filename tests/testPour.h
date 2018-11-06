/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testPour.h
 * Author: hilkensb
 *
 * Created on Nov 5, 2018, 4:49:17 PM
 */

#ifndef TESTPOUR_H
#define TESTPOUR_H

#include <cppunit/extensions/HelperMacros.h>

class testPour : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testPour);

    CPPUNIT_TEST(testMethod);

    CPPUNIT_TEST_SUITE_END();

public:
    testPour();
    virtual ~testPour();
    void setUp();
    void tearDown();

private:
    void testMethod();
    void testFailedMethod();
};

#endif /* TESTPOUR_H */

