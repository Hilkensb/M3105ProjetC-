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
    std::string nomFichier ="4_pour";
    ifstream fichierATester(nomFichier.c_str());

    Interpreteur interpreteur(fichierATester);
 
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Auncune erreur de syntaxe ",interpreteur.analyse());
    
    SymboleValue * symbole1 =  interpreteur.getTable().chercher("test1");
    SymboleValue * symbole2 =  interpreteur.getTable().chercher("test2");
    SymboleValue * symbole3 =  interpreteur.getTable().chercher("test3");
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test1 n'est pas définie",symbole1->estDefini(),false);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test2 n'est pas définie",symbole2->estDefini(),false);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test3 n'est pas définie",symbole3->estDefini(),false);
    
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Auncune erreur  à l'execution ",interpreteur.getArbre()->executer());
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test1 est  définie",symbole1->estDefini(),true);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test2 est  définie",symbole2->estDefini(),true);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test3 est  définie",symbole3->estDefini(),true);


    CPPUNIT_ASSERT_EQUAL_MESSAGE("test1 = 5",symbole1->getValeur(),5);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test2 = 5",symbole2->getValeur(),5);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test3 = 5",symbole3->getValeur(),5);

}



