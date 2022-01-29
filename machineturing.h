#ifndef _machineturing_h_
#define _machineturing_h_

#include "ruban.h"

#define DROITE '>'
#define GAUCHE '<'
#define AUCUN '-'


/**
* Structure de données permettant de stocker les transitions d'une 
* machine de turing. Cette structure est une liste chainée de 
* transitions. Une transition est repésentée par :
* etat -> l'ancien état de la transition
* symbole_lu -> le symbole lu sur le ruban
* symbole_ecrit -> le symbole à écrire
* mouvement -> le déplacement à suivre, '>' vers droite, 
*              '<' vers la gauche et '-' rester sur place
* nouvel_etat -> le nouvel état
* suivant -> pointeur vers la prochaine transition
*/
struct transition_s {
  char *etat;
  char symbole_lu;
  char symbole_ecrit;
  char mouvement;
  char *nouvel_etat;
  struct transition_s *suivant;
};
typedef struct transition_s* transition;


/**
* Structure de données permettant de stocker une machine de turing.
* Une machine de Turing est repésentée par :
* alphabet_entree -> L'alphabet d'entree de la machine : 
*                    un ensemble de caractères acceptés par la machine.
*                    Chaque caractères de la chaine est considéré comme
*                    un symbole accepté en entrée.
* alphabet_travail -> L'alphabet de travail de la machine. 
*                     Comme précédemment, chaque caractères de la
*                     chaine est considéré comme un symbole de travail.
* etat_in -> L'état initial de la machine
* etat_fin -> L'état final de la machine
* symbole_blanc -> Le symbole blanc (vide) du ruban. En plus des
*                  caractères des alphabets, ce symbole est accepté sur
*                  le ruban.
* transitions -> Pointeur vers le permier élément de la liste des 
*                transitions. 
* transitions_fin -> Pointeur vers le dernier élément de la liste des 
*                    transitions. 
* etat_courant -> L'état courant de la machine
* ruban_courant -> L'état courant du ruban de la machine.
* tete_lecture -> Pointeur vers la tête de lecture du ruban.
*/
struct MT_s {
  char *alphabet_entree;
  char *alphabet_travail;
  char *etat_in;
  char *etat_fin;
  char symbole_blanc;
  transition transitions;
  transition transitions_fin;
  char *etat_courant;
  ruban ruban_courant;
  ruban tete_lecture;
};
typedef struct MT_s* MT;

/**
* Construit une nouvelle transition avec les paramètres d'une transition
*/
transition creer_transition(char *etat, char sym_lu, char sym_ecrit, char mvt, char *nouv_etat);

/**
* Vérifie si une transition existe déjà ou non dans la liste ; 
* parcoure la liste des transitions et vérifie si la transition 
* est égale à l'une des transitions existante
* @param mt : la machine de turing contenant les transitions
* @param tr : la transition à vérifier
* @return 1 si tr appartient à mt->transitions, 0 sinon
*/
int transition_existe(MT mt, transition tr);

/**
* Ajoute une nouvelle transition à la fin de la liste chainée des
* transitions de la machine
* @param mt : la machine de Turing où ajouter la nouvelle transition
* @param nouveau : la nouvelle transition à ajouter
* @param distinct : 1 pour une liste où tous les éléments sont distincts
*                   i.e on n'insert pas de doublon dans la liste
*                   0 pour une liste avec doublon accepté
*/
void ajouter_transition(MT mt, transition nouveau, int distinct);

/**
* Affiche une liste chainée de transition
* @param transitions : la liste chainée de transitions à afficher
*/
void afficher_transitions(transition transitions);

/**
* Initialise une machine de Turing à partir d'un fichier contenant la 
* description de la machine.
* Le langage de description de machine utilisé est celui du site 
* https://turingmachinesimulator.com/, à la seule différence que les 
* descriptions des transitions se font ici sur une seule ligne, au lieu 
* de deux. Par exemple, au lieu de :
  q0,0
  q1,0,>
* La description ici se fait sur une seule ligne, c'est à dire on aura :
  q0,0,q1,0,>
* Hormi cette distinction, nous adoptons le reste du langage. 
* Les espaces et les sauts de lignes sont bien autorisés.
* @param path : chemin vers le fichier contenant la description de la 
*               machine
* @param alphabets : les alphabets de la machine, doit être au format 
*                    alphabet_entree:alphabet_travail
* @param symbole_blanc : Le symbole blanc (vide) du ruban
* @return la machine de Turing construite
*/
MT init_machine_turing(char *path, char *alphabets, char symbole_blanc);

/**
* Libère l'espace mémoire alloué à une machine de Turing
* @param mt : l'espace mémoire à désallouer
*/
void free_mt(MT mt);

/**
* Affiche une machine de Turing et sa configuration courante.
* @param mt -> La machine de Turing à afficher
*/
void afficher_machine_turing(MT mt);

/**
* Exécute un pas de calcul d'une machine de Turing. 
* Le ruban de la machine est supposé initialisé.
* @param mt : la machine de Turing à exécuter.
* @param symbole_lu : le symbole (caractère) courant de l'entrée
* @return 0 si le calcul entier de la machine est terminé, 1 sinon
*/
int simuler_etape(MT mt);

/**
* Simule le calcul d'une machine de Turing sur un mot jusqu'à atteindre 
* l'état final.
* @param mt : la machine de Turing à exécuter.
* param mot : le mot d'entrée à exécuter sur la machine
* @return 1 si le mot est accepté, 0 sinon
*/
int simuler_turing(MT mt, char *mot);

/**
* Cette fonction lit dans un fichier le code d'une machine de Turing 
*qui travaille sur l'alphabet d'entrée {a,b,c,d} et qui écrit dans un 
* autre fichier le code d'une machine équivalente travaillant sur 
* l'alphabet {0,1} avec le codage suivant :
* code(a) = 00, code(b) = 01, code(c) = 10, code(d) = 11
* Cette fonction est spécifiquement fait pour cet usage.
* @param path_in : fichier contenant la description de la machine 
* à convertir (travaillant sur l'alphabet {a,b,c,d})
* @param path_out : fichier qui contiendra la description de la machine 
*                   résultat (travaillant sur l'alphabet {0,1})
* @return la machine d'origine (codé dans path_in) ou NULL en cas d'erreur
*/
MT machine_latin_vers_binaire(char *path_in, char *path_out);


#endif