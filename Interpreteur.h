#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

#include "Symbole.h"
#include "Lecteur.h"
#include "Exceptions.h"
#include "TableSymboles.h"
#include "ArbreAbstrait.h"

class Interpreteur {
public:
	Interpreteur(ifstream & fichier);   // Construit un interpréteur pour interpreter
	                                    //  le programme dans  fichier 
                                      
	void analyse();                     // Si le contenu du fichier est conforme à la grammaire,
	                                    //   cette méthode se termine normalement et affiche un message "Syntaxe correcte".
                                      //   la table des symboles (ts) et l'arbre abstrait (arbre) auront été construits
	                                    // Sinon, une exception sera levée
        int getCompteurErreur() const;// retourne le nombre d'erreur syntaxique
        void incrementeCompteurErrreur();//incrémente la valeur m_compteurErreur
        
        
        void traduitEnCPP(ostream & cout, unsigned int indentation) const ;

        
	inline const TableSymboles & getTable () const  { return m_table;    } // accesseur	
	inline Noeud* getArbre () const { return m_arbre; }                    // accesseur
	
private:
    Lecteur        m_lecteur;  // Le lecteur de symboles utilisé pour analyser le fichier
    TableSymboles  m_table;    // La table des symboles valués
    Noeud*         m_arbre;    // L'arbre abstrait
    int            m_compteurErreur=0; //initilisation d'un compteur d'erreur qui s'incrément pour chaque exception syntaxique (SyntaxeException)


    // Implémentation de la grammaire
    Noeud*  programme();    //   <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    Noeud*  seqInst();	    //     <seqInst> ::= <inst> { <inst> }
    Noeud*  inst();	    //        <inst> ::= <affectation> ; | <instSi>
    Noeud*  affectation();  // <affectation> ::= <variable> = <expression> 
    
    Noeud* expression();    // <expression> ::= <terme>{ +<terme> | -<terme> }
    Noeud* facteur();       // <terme>::= <facteur> { * <facteur> | / <facteur>}
    Noeud* terme();         // <facteur>::= <entier> | <variable> | - <expBool> | non <expBool> | (<expBool>)
    Noeud* expBool();       // <expBool>::= <relationET> { ou <relationEt> }
    Noeud* relationEt();    //<relationEt>::= <relation> { et <relation> }
    Noeud* relation();      //<relation>::= <expression> { <opRel> <expression>}
                            //<opRel>::= == | != | < | <= | > | >=

    Noeud*  instSi();       //<instSi> ::= si ( <expression> ) <seqInst> finsi
    Noeud*  instSiRiche();  //<intSiRiche> ::= si (<expression>) <seqInst> {sinonsi (<expression>) <seqInst> } [sinon <seqInst>] finsi
    Noeud*  instTantQue();  //<instTantQue> ::=tantque( <expression>) <seqInst>fintantque
    Noeud*  instRepeter();  //<instRepeter> ::=repeter<sequInst> jusqua (<expression> )
    Noeud*  instPour();     //<instPour> ::=pour ([<affectation>] ; <expression> ; [<affectation>] ) <seqInst> finpour
    Noeud*  instEcrire();   //<instEcrire> ::=ecrire( <expression> | <chaine> {, <expresson> | <chaine> } )
    Noeud*  instLire();     //<instLire> ::=lire(<variable> { , <variable> } )
    Noeud*  instSelon();   // <instSelon> ::= selon ( <facteur> ) { cas <facteur> : <seqInst> } [defaut : <seqInst>] finselon

    
    // Outils pour simplifier l'analyse syntaxique
    void tester (const string & symboleAttendu) const throw (SyntaxeException);   // Si symbole courant != symboleAttendu, on lève une exception
    void testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException); // Si symbole courant != symboleAttendu, on lève une exception, sinon on avance
    void erreur (const string & mess) const throw (SyntaxeException);             // Lève une exception "contenant" le message mess
};

#endif /* INTERPRETEUR_H */
