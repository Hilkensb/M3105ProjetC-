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

void NoeudSeqInst::traduitEnCPP(ostream & cout, unsigned int indentation)const {
     for (auto ptr : m_instructions) {
        ptr->traduitEnCPP(cout,indentation);    // on traduit chaque instruction de la séquence en c++
        cout << ";" << endl;   //l'instruction en c++ doit se terminer par un ;
    }
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

void NoeudAffectation::traduitEnCPP(ostream & cout, unsigned int indentation)const {
    cout << setw(4 * indentation) << ""<< ((SymboleValue*) m_variable)->getChaine() << " = " ;
    ((NoeudOperateurBinaire*) m_expression)->traduitEnCPP(cout, 0);// on traduit l'affectation en c++

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

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout, unsigned int indentation)const {
    cout << setw(indentation)<< "";
    if(m_operateur.getChaine() == "non"){
        cout << "!";//traduction du non en c++
    }
    m_operandeGauche->traduitEnCPP(cout,0);//Traduction de l'opérande de gauche en c++
    if(m_operateur.getChaine() != "non"){
        if(m_operateur.getChaine() == "et"){
            cout << "&&" ;//traduction du et en c++
        }
        else if(m_operateur.getChaine() == "ou"){
            cout << "||";//traduction du ou en c++
        }
        else{
           cout << m_operateur.getChaine(); 
        }
    }
   if(m_operandeDroit != nullptr){
        m_operandeDroit->traduitEnCPP(cout, 0); //Traduction de l'opérande droit, s'il existe , en c++
    }
}

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
 
 void NoeudInstSi::traduitEnCPP(ostream & cout, unsigned int indentation) const {

  cout << setw(4*indentation) << "" << "if ("; // Ecrit "if (" avec un décalage de 4*indentation espaces
 m_condition->traduitEnCPP(cout, 0); // Traduit la condition en C++ sans décalage
 cout << ") {"<< endl; // Ecrit ") {" et passe à la ligne
 m_sequence->traduitEnCPP(cout, indentation+1); // Traduit en C++ la séquence avec indentation augmentée
 cout << setw(4*indentation) << "" << "}" << endl; // Ecrit "}" avec l'indentation initiale et passe à la ligne
}
 
 
 
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

void NoeudInstTantQue::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation) << "" << "if ("; // Ecrit "if (" avec un décalage de 4*indentation espaces
    m_condition->traduitEnCPP(cout, 0); // Traduit la condition en C++ sans décalage
    cout << ") {" << endl; // Ecrit ") {" et passe à la ligne
    m_sequence->traduitEnCPP(cout, indentation + 1); // Traduit en C++ la séquence avec indentation augmentée
    cout << setw(4*indentation) << "" << "}" ; // Ecrit "}" avec l'indentation initiale et passe à la ligne
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche() {
}

void NoeudInstSiRiche::ajoute(Noeud* noeud) {
    m_vecteur.push_back(noeud);
}

int NoeudInstSiRiche::executer() {
    int i = 0;
    while ((i < m_vecteur.size() || (m_vecteur.size() % 2 && i < m_vecteur.size() - 1))
            && !m_vecteur[i]->executer()) // comme on stocke tout dans un vecteur, on dois verifer différent élément du vecteur(taille par rapport à i, si taille du vecteur est paire...) 
        i += 2;//incrémente de 2 car on veut avancer à la prochaine condition 
    if (i < m_vecteur.size())
        i == m_vecteur.size() - 1 ? m_vecteur[i]->executer() : m_vecteur[i + 1]->executer();//executer soir la condition soit la séquence
    return 0;
}

void NoeudInstSiRiche::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    int i = 0;
    cout << setw(4 * indentation) << "" << "if (";
    m_vecteur[i]->traduitEnCPP(cout, 0);//Traduction de la condition du if en c++
    cout << ") {" << endl;
    m_vecteur[i+1]->traduitEnCPP(cout, indentation + 1);//Traduction de la séquence d'instruction du if en c++
    cout << setw(4*indentation) << "" <<  "}";
    
       
    while ((i < m_vecteur.size() || (m_vecteur.size() % 2 && i < m_vecteur.size() - 1))&& !m_vecteur[i]->executer()) {
        i += 2;
        if (i < m_vecteur.size()) {
            if (i != 0){
                if (i == m_vecteur.size() - 1) {
                    cout<< setw( indentation) << ""<< " else{ "<<endl;
                    m_vecteur[i]->traduitEnCPP(cout, indentation+1);//Traduction de la séquence d'instruction du else en c++
                    cout <<setw(4*indentation) << "" << "}" ;
                } else {
                    cout<<setw(indentation) << "" << "elseif(";
                    m_vecteur[i]->traduitEnCPP(cout, 0);//Traduction de la condition du elseif en c++
                    cout << "){"<<endl;
                    m_vecteur[i + 1]->traduitEnCPP(cout, indentation+1);//Traduction de la séquence d'instruction du elseif en c++
                    cout <<setw(4*indentation) << "" << "}";

                }
            }
        }
    }
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

void NoeudInstRepeter::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "do{"<<endl<<setw(4 * (indentation+1))<< "";
    m_sequence->traduitEnCPP(cout, 0);//Traduction de la séquence d'instruction du do while en c++
    cout << setw(4 * indentation) << "" << "}While(";
    m_condition->traduitEnCPP(cout, 0);//Traduction de la condition du do while en c++
    cout << ")" ;

}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* condition, Noeud* sequence, Noeud* affectationDebut, Noeud* affectationFin)
: m_condition(condition), m_sequence(sequence), m_affectationDebut(affectationDebut), m_affectationFin(affectationFin) {
}

int NoeudInstPour::executer() {
    if (m_affectationDebut != NULL) {
        m_affectationDebut->executer();//exécute la premiére affectation dans le pour si elle existe
    }
    while (m_condition->executer()) { 
        if (m_affectationFin != NULL) {
            m_affectationFin->executer();//exécute la deuxiéme affectation dans le pour si elle existe et tant qu'on à la condition du pour
        }
        m_sequence->executer();//exécute la séquence d'instruction
    }
}

void NoeudInstPour::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"for(";
    if (m_affectationDebut != nullptr) {
        m_affectationDebut->traduitEnCPP(cout,0);
        cout<<";";
    }
    m_condition->traduitEnCPP(cout,0);
     if (m_affectationFin != nullptr) {
         cout << ";";
         m_affectationFin->traduitEnCPP(cout,0);
        }
    cout<<"){"<<endl;
    m_sequence->traduitEnCPP(cout,indentation+1);//Traduit la séquence d'instruction du pour en c++
    cout<< setw(4*indentation)<<""<<"}";
    
}

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
            cout << ((SymboleValue*) param)->getChaine().substr(1, ((SymboleValue*) param)->getChaine().length() - 2);
        } else {
            cout << param->executer();
        }
}

void NoeudInstEcrire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation) << "" << "cout ";
    for(auto param : m_vecteur){
        cout << " << ";
        param->traduitEnCPP(cout,0);  //Traduit le contenue de m_vecteur en c++            
    }
}

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
    for (auto param : m_vecteur) {
        int var;
        cin >> var;
        ((SymboleValue*) param) ->setValeur(var);
    }
}

void NoeudInstLire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"cin ";
    for (auto param : m_vecteur) {
        cout<<">>" ;
         param->traduitEnCPP(cout,0); //Traduit le contenue de m_vecteur en c++            
    }
}



