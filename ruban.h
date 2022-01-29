#ifndef _ruban_h_
#define _ruban_h_

/**
* Structure de données permettant de stcoker le ruban d'une machine de 
* Turing. Le ruban de la machine est une liste doublement chainée de 
* cases permettant l'écriture des symboles.
* symbole -> le symbole contenu dans la case courante du ruban
* gauche -> pointeur vers la case gauche de la case courante
* droite -> pointeur vers la case droite de la case courante
*/
struct ruban_s {
  char symbole;
  struct ruban_s* droite;
  struct ruban_s* gauche;
};
typedef struct ruban_s* ruban;


/**
* Initialise une case du ruban
* @param symbole : le symbole contenu dans la case
* @return la case créée et Initialisée
*/
ruban init_case_ruban(char symbole);


/**
* Ajoute une case à la gauche de la case courante du ruban
* @param r : liste doublement chainée représentant le ruban
* @param s : le symbole à ajouter au ruban
* @return un pointeur vers la case du ruban ajoutée, NULL en cas d'erreur
*/
ruban ruban_ajouter_gauche(ruban case_courante, char symbole);

/**
* Ajoute une case à la droite de la case courante du ruban
* @param r : liste doublement chainée représentant le ruban
* @param s : le symbole à ajouter au ruban
* @return un pointeur vers la case du ruban ajoutée, NULL en cas 
* d'erreur
*/
ruban ruban_ajouter_droite(ruban case_courante, char symbole);

/**
* Initialise un ruban à partir d'un mot d'entrée
* @param mot : le mot d'entrée
* @return le ruban Initialisé
*/
ruban init_ruban(char *mot);

/**
* Affiche la configuration courante d'un ruban d'une machine de Turing
* @param r : le ruban à afficher
*/
void afficher_ruban(ruban case_courante);

/**
* Libère l'espace mémoire allouée pour un ruban
* @param r : l'espace mémoire à désallouer
*/
void free_ruban(ruban r);


#endif

