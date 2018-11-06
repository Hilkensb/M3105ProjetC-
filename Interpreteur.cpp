#include "Interpreteur.h"
#include <stdlib.h>
#include <typeinfo>
#include <iostream>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
    m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
    static char messageWhat[256];
    if (m_lecteur.getSymbole() != symboleAttendu) {
        sprintf(messageWhat,
                "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
                m_lecteur.getLigne(), m_lecteur.getColonne(),
                symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
        throw SyntaxeException(messageWhat);
    }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
    tester(symboleAttendu);
    m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
    // Lève une exception contenant le message et le symbole courant trouvé
    // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
    static char messageWhat[256];
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    testerEtAvancer("procedure");
    testerEtAvancer("principale");
    testerEtAvancer("(");
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finproc");
    tester("<FINDEFICHIER>");
    return sequence;
}

Noeud* Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    NoeudSeqInst* sequence = new NoeudSeqInst();
    do {
        sequence->ajoute(inst());
    } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si"
            || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter"
            || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire"
            || m_lecteur.getSymbole() == "lire"|| m_lecteur.getSymbole() == "selon");
    // Tant que le symbole courant est un début possible d'instruction...
    // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
    return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    try {
        if (m_lecteur.getSymbole() == "<VARIABLE>") {
            Noeud *affect = affectation();
            testerEtAvancer(";");
            return affect;
        } else if (m_lecteur.getSymbole() == "si")
            // return instSi();
            return instSiRiche();
            // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
        else if (m_lecteur.getSymbole() == "tantque") {
            return instTantQue();
        } else if (m_lecteur.getSymbole() == "repeter") {
            return instRepeter();
        } else if (m_lecteur.getSymbole() == "pour") {
            return instPour();
        } else if (m_lecteur.getSymbole() == "ecrire") {
            return instEcrire();
        } else if (m_lecteur.getSymbole() == "lire") {
            return instLire();
        } else if (m_lecteur.getSymbole() == "selon") {
            return instSelon();    
        } else erreur("Instruction incorrecte");
    } catch (SyntaxeException & e) {
        while (m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "si"
                && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter"
                && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire"
                && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() !="selon" && m_lecteur.getSymbole() != "finproc") {
            m_lecteur.avancer();// on avance tant que une varibale ,une debut d'instrucion ou une fin d'instruction n'a pas était rencontrer
        }
        incrementeCompteurErrreur();
        m_arbre = NULL;
        cout << e.what() << endl;
        return nullptr;
    }
}

Noeud* Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression>  | <variable>++| <variable>--
    tester("<VARIABLE>");
    Noeud* unNoeud;
    Noeud* varDroit;
            
    varDroit = m_table.chercheAjoute(Symbole("1"));

    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
    m_lecteur.avancer();
    if (m_lecteur.getSymbole() == "=" ){
        m_lecteur.avancer();
        Noeud* exp = expression(); // On mémorise l'expression trouvée
        return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
    }else if(m_lecteur.getSymbole() == "++" ){
        m_lecteur.avancer();
        unNoeud= new NoeudOperateurBinaire(Symbole("+"), var, varDroit); 
    }else if (m_lecteur.getSymbole() == "--" ){
        m_lecteur.avancer();
        unNoeud =  new NoeudOperateurBinaire(Symbole("-"), var, varDroit); 
    }
    return new NoeudAffectation(var, unNoeud);// On renvoie un noeud affectation

}

Noeud* Interpreteur::expression() {
    // <expression> ::= <terme>{ +<terme> | -<terme> }
        Noeud* term = terme();
        while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-") {
            Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
            m_lecteur.avancer();
            Noeud* termeDroit = terme(); // On mémorise le terme droit
            term = new NoeudOperateurBinaire(operateur, term, termeDroit); // Et on construit un noeud opérateur binaire
        }
        return term; // On renvoie term qui pointe sur la racine de l'expression
}

Noeud * Interpreteur::terme() {
    // <terme>::= <facteur> { * <facteur> | / <facteur>}
        Noeud* fact = facteur();
        while (m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/") {
            Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
            m_lecteur.avancer();
            Noeud* facteurSuiv = facteur(); // On mémorise le terme droit
            fact = new NoeudOperateurBinaire(operateur, fact, facteurSuiv); // Et on construit un noeud opérateur binaire
        }
        return fact; 
}

Noeud* Interpreteur::facteur() {
    // <facteur>::= <entier> | <variable> | - <expBool> | non <expBool> | (<expBool>)
        Noeud* fact = nullptr;
        if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>" || m_lecteur.getSymbole() == "<CHAINE>" ) {
            fact = m_table.chercheAjoute(m_lecteur.getSymbole());
            m_lecteur.avancer();
        } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
            m_lecteur.avancer();
            fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), expBool());
        } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
            m_lecteur.avancer();
            fact = new NoeudOperateurBinaire(Symbole("non"), expBool(), nullptr);
        } else if (m_lecteur.getSymbole() == "(") { // ( <expBool>)
            m_lecteur.avancer();
            fact = expBool();
            testerEtAvancer(")");
        } else
            erreur("Facteur incorrect");
        return fact;
}

Noeud * Interpreteur::expBool() {
    // <expBool>::= <relationET> { ou <relationEt> }
        Noeud * relationet = relationEt();
        while (m_lecteur.getSymbole() == "ou") {
            Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
            m_lecteur.avancer();
            Noeud* relationEtDroit = relationEt(); // On mémorise le terme droit
            relationet = new NoeudOperateurBinaire(operateur, relationet, relationEtDroit); // Et on construit un noeud opérateur binaire
        }
        return relationet;
}

Noeud * Interpreteur::relationEt(){   
    //<relationEt>::= <relation> { et <relation> }
        Noeud * rela = relation();
        while (m_lecteur.getSymbole() == "et") {
            Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
            m_lecteur.avancer();
            Noeud* relDroit = relation(); // On mémorise le terme droit
            rela = new NoeudOperateurBinaire(operateur, rela, relDroit); // On construit un noeud opérateur binaire
        }
        return rela;   
}

Noeud * Interpreteur::relation(){
    //<relation>::= <expression> { <opRel> <expression>}
    //<opRel>::= == | != | < | <= | > | >=
        Noeud* expres = expression();
        while (m_lecteur.getSymbole() == "=="||m_lecteur.getSymbole() == "!="||m_lecteur.getSymbole() == "<"
                ||m_lecteur.getSymbole() == "<="||m_lecteur.getSymbole() == ">"||m_lecteur.getSymbole() == ">=") {
            Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
            m_lecteur.avancer();
            Noeud* expressionDroite = expression(); // On mémorise le terme droit
            expres = new NoeudOperateurBinaire(operateur, expres, expressionDroite); // On construit un noeud opérateur binaire
        }
        return expres;  
} 

 Noeud* Interpreteur::instSi() {
  // <instSi> ::= si ( <expression> ) <seqInst> finsi
  testerEtAvancer("si");
  testerEtAvancer("(");
  Noeud* condition = expBool();    // On mémorise la condition
  testerEtAvancer(")");
  Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
  testerEtAvancer("finsi");
  return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instTantQue() {
    //<instTantQue> ::=tantque( <expression>) <seqInst>fintantque
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expBool(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst();  // On mémorise la séquence d'instruction
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);   // Et on renvoie un Noeud Inst Tant Que

}

Noeud* Interpreteur::instSiRiche() {
    //<intSiRiche> ::= si (<expression>) <seqInst> {sinonsi (<expression>) <seqInst> } [sinon <seqInst>] finsi
    NoeudInstSiRiche* unNoeud = new NoeudInstSiRiche;
    testerEtAvancer("si");
    testerEtAvancer("(");
    unNoeud->ajoute(expBool()); //  On ajoute une condition a unNoeud
    testerEtAvancer(")");
    unNoeud->ajoute(seqInst()); // On ajoute une séquence d'instruction a unNoeud
    while (m_lecteur.getSymbole() == "sinonsi") {
        m_lecteur.avancer();
        testerEtAvancer("(");
        unNoeud->ajoute(expBool()); //  On ajoute une condition a unNoeud
        testerEtAvancer(")");
        unNoeud->ajoute(seqInst());         // On ajoute une séquence d'instruction a unNoeud
    }
    if (m_lecteur.getSymbole() == "sinon") {
        m_lecteur.avancer();
        unNoeud->ajoute(seqInst()); // On ajoute une séquence d'instruction a unNoeud
    }
    testerEtAvancer("finsi");
    return unNoeud; // Et on renvoie unNoeud 
}

Noeud* Interpreteur::instRepeter() {
    //<instRepeter> ::=repeter<sequInst> jusqua (<expression> )
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();        // On mémorise la séquence d'instruction
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expBool();       // On mémorise la condition
    testerEtAvancer(")");
    return new NoeudInstRepeter(condition, sequence);//Et on renvoie un Noeud Inst Repeter
}

Noeud* Interpreteur::instPour() {
    //<instPour> ::=pour ([<affectation>] ; <expression> ; [<affectation>] ) <seqInst> finpour
    testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud* affectationDebut = NULL;
    if (m_lecteur.getSymbole() == ";") {
        m_lecteur.avancer();                
    } else {
        affectationDebut = affectation();  // On mémorise une affectation
        m_lecteur.avancer();            
    }
    Noeud* condition = expBool();         // On mémorise la condition
    testerEtAvancer(";");
    Noeud* affectationFin = NULL;       
    if (m_lecteur.getSymbole() == ")") {
        m_lecteur.avancer();
    } else {
        affectationFin = affectation(); // On mémorise une affectation
        m_lecteur.avancer();
    }
    Noeud* sequence = seqInst();  // On mémorise une affectation
    testerEtAvancer("finpour");
    return new NoeudInstPour(condition, sequence, affectationDebut, affectationFin);//Et on renvoie un Noeud inst Pour
}

Noeud* Interpreteur::instEcrire() {
    //<instEcrire> ::=ecrire( <expression> | <chaine> {, <expresson> | <chaine> } )
    Noeud* fact = nullptr;
    NoeudInstEcrire* unNoeud = new NoeudInstEcrire;
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    if (m_lecteur.getSymbole() == "<CHAINE>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole());
        unNoeud->ajoute(fact);
        testerEtAvancer("<CHAINE>");
    } else {
        unNoeud->ajoute(expBool());
    }
    while (m_lecteur.getSymbole() == ",") {
        m_lecteur.avancer();
        if (m_lecteur.getSymbole() == "<CHAINE>") {
            fact = m_table.chercheAjoute(m_lecteur.getSymbole());
            unNoeud->ajoute(fact);
            testerEtAvancer("<CHAINE>");
        } else {
            unNoeud->ajoute(expBool());
        }
    }
    testerEtAvancer(")");
    return unNoeud;
}

Noeud* Interpreteur::instLire() {
    //<instLire> ::=lire(<variable> { , <variable> } )
    NoeudInstLire* unNoeud = new NoeudInstLire;
    testerEtAvancer("lire");
    testerEtAvancer("(");
    unNoeud ->ajoute(expBool());

    while (m_lecteur.getSymbole() == ",") {
        m_lecteur.avancer();
        unNoeud ->ajoute(expBool());
    }
    testerEtAvancer(")");
    return unNoeud;
}


Noeud* Interpreteur::instSelon(){
    // <instSelon> ::= selon ( <facteur> ) { cas <facteur> : <seqInst> } [defaut : <seqInst>] finselon
     NoeudInstSiRiche* unNoeud = new NoeudInstSiRiche;
     testerEtAvancer("selon");
     testerEtAvancer("(");
     Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole());
     m_lecteur.avancer();
    testerEtAvancer(")");
    
  while (m_lecteur.getSymbole() == "cas") {

        m_lecteur.avancer();
        tester("<ENTIER>");//test s'il y a bien un entier après cas
        Noeud* varCas = m_table.chercheAjoute(m_lecteur.getSymbole());
        m_lecteur.avancer();
        testerEtAvancer(":");

        // on ajoute un NoeudOperateurBinaire  et une squence d'instruction au noeu
        unNoeud->ajoute(new NoeudOperateurBinaire(Symbole("=="),var,varCas));
        unNoeud->ajoute(seqInst());

    }
    if(m_lecteur.getSymbole() == "defaut"){
        m_lecteur.avancer();
        testerEtAvancer(":");

        unNoeud->ajoute(seqInst());
        
    }
    
     testerEtAvancer("finselon");
     
    return unNoeud; // Et on renvoie unNoeud 
}


int Interpreteur::getCompteurErreur() const {
    // retourne le nombre d'erreur syntaxique 
    return m_compteurErreur;
}

void Interpreteur::incrementeCompteurErrreur() {
    //incrément de 1 la valeur m_compteurErreur
    m_compteurErreur++;
}

void Interpreteur::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(4 * indentation) << "" << "using namespace std;" << endl;
    cout << setw(4 * indentation) << "" << "int main() {" << endl; // Début d’un programme C++
    // Ecrire en C++ la déclaration des variables présentes dans le programme
    for (int i = 0; i < m_table.getTaille(); i++) {
        if (m_table[i] == "<VARIABLE>") {
            cout << setw(4 * (indentation + 1)) << "" << "int " << m_table[i].getChaine() << ";" << endl;
        }
    }
    getArbre()->traduitEnCPP(cout, indentation + 1); // lance l'opération traduitEnCPP sur la racine
    cout << setw(4 * (indentation + 1)) << "" << "return 0;" << endl;
    cout << setw(4 * indentation) << "}" << endl; // Fin d’un programme C++
}
