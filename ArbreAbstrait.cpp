#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
        m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
    if (instruction != nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
    int valeur = m_expression->executer(); // On exécute (évalue) l'expression
    ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
    int og, od, valeur;
    if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
    if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
    // Et on combine les deux opérandes en fonctions de l'opérateur
    if (this->m_operateur == "+") valeur = (og + od);
    else if (this->m_operateur == "-") valeur = (og - od);
    else if (this->m_operateur == "*") valeur = (og * od);
    else if (this->m_operateur == "==") valeur = (og == od);
    else if (this->m_operateur == "!=") valeur = (og != od);
    else if (this->m_operateur == "<") valeur = (og < od);
    else if (this->m_operateur == ">") valeur = (og > od);
    else if (this->m_operateur == "<=") valeur = (og <= od);
    else if (this->m_operateur == ">=") valeur = (og >= od);
    else if (this->m_operateur == "et") valeur = (og && od);
    else if (this->m_operateur == "ou") valeur = (og || od);
    else if (this->m_operateur == "non") valeur = (!og);
    else if (this->m_operateur == "/") {
        if (od == 0) throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}
/*
////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}
 */
////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while (m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}
void NoeudInstTantQue::traduitEnCPP(ostream & cout, unsigned int indentation) const {}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche() {
}

void NoeudInstSiRiche::ajoute(Noeud* noeud) {
    m_grosVecteur.push_back(noeud);
}

int NoeudInstSiRiche::executer() {
    int i = 0;
    while ((i < m_grosVecteur.size() || (m_grosVecteur.size() % 2 && i < m_grosVecteur.size() - 1))
            && !m_grosVecteur[i]->executer())
        i += 2;
    if (i < m_grosVecteur.size())
        i == m_grosVecteur.size() - 1 ? m_grosVecteur[i]->executer() : m_grosVecteur[i + 1]->executer();
    return 0;
}

void NoeudInstSiRiche::traduitEnCPP(ostream & cout, unsigned int indentation) const {
 cout << setw(4*indentation) << "" << "if ("; // Ecrit "if (" avec un décalage de 4*indentation espaces
 m_condition->traduitEnCPP(cout, 0); // Traduit la condition en C++ sans décalage
 cout << ") {"<< endl; // Ecrit ") {" et passe à la ligne
 m_sequence->traduitEnCPP(cout, indentation+1); // Traduit en C++ la séquence avec indentation augmentée
 cout << setw(4*indentation) << "" << "}" << endl; // Ecrit "}" avec l'indentation initiale et passe à la ligne
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence) : m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
    do {
        m_sequence->executer();
    } while (m_condition->executer());
    return 0; // La valeur renvoyée ne représente rien !
}
void NoeudInstRepeter::traduitEnCPP(ostream & cout, unsigned int indentation) const {}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* condition, Noeud* sequence, Noeud* affectationDebut, Noeud* affectationFin)
: m_condition(condition), m_sequence(sequence), m_affectationDebut(affectationDebut), m_affectationFin(affectationFin) {
}

int NoeudInstPour::executer() {
    if (m_affectationDebut != NULL) {
        m_affectationDebut->executer();
    }
    while (m_condition->executer()) {
        if (m_affectationFin != NULL) {
            m_affectationFin->executer();
        }
        m_sequence->executer();
    }
}
void NoeudInstPour::traduitEnCPP(ostream & cout, unsigned int indentation) const {}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire() {
}

void NoeudInstEcrire::ajoute(Noeud* expression) {
    m_vecteur.push_back(expression);
}

int NoeudInstEcrire::executer() {
    // Exécute l'instruction ecrire
    for (auto param : m_vecteur)
        if (typeid (*param) == typeid (SymboleValue) && *((SymboleValue*) param) == "<CHAINE>") {
            cout << ((SymboleValue*) param)->getChaine().substr(1,((SymboleValue*) param)->getChaine().length()-2);
        } else {
            cout << param->executer();
        }
}
void NoeudInstEcrire::traduitEnCPP(ostream & cout, unsigned int indentation) const {}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire() {
}

void NoeudInstLire::ajoute(Noeud* variable) {
    m_vecteur.push_back(variable);
}

int NoeudInstLire::executer() {
    // Exécute l'instruction ecrire
    for (auto param : m_vecteur){
        int var;
        cin >> var ;
        ((SymboleValue*) param) ->setValeur(var);
        }
}
void NoeudInstLire::traduitEnCPP(ostream & cout, unsigned int indentation) const {}



