/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testPour.cpp
 * Author: hilkensb
 *
 * Created on Nov 5, 2018, 4:49:19 PM
 */

#include "testPour.h"
#include "../Interpreteur.h"
#include "../TableSymboles.h"


CPPUNIT_TEST_SUITE_REGISTRATION(testPour);

testPour::testPour() {
}

testPour::~testPour() {
}

void testPour::setUp() {
}

void testPour::tearDown() {
}

void testPour::testMethod() {
    std::string nomFichier ="pour";
    ifstream fichierATester(nomFichier.c_str());

    Interpreteur interpreteur(fichierATester);

    
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Auncune erreur de syntaxe ",interpreteur.analyse());
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Auncune erreur  à l'execution ",interpreteur.getArbre()->executer());
    
    SymboleValue * symbole1 =  interpreteur.getTable().chercher("test1");
    SymboleValue * symbole2 =  interpreteur.getTable().chercher("test2");
        SymboleValue * symbole3 =  interpreteur.getTable().chercher("test3");

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test1 = 5",symbole1->getValeur(),5);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test2 = 5",symbole2->getValeur(),5);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test3 = 5",symbole3->getValeur(),5);

}



