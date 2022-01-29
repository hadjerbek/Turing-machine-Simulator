#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "machineturing.h"

/**
* Renvoie une copie d'une chaine de caractère
*/
char* clone_string(char *string) {
  char *res = (char*) malloc(sizeof(char) * strlen(string) + 1);
  strcpy(res, string);
  return res;
}

/**
* Supprime les espaces au débuts et à la fin d'un chaine de caractère
* @param string : la chaine à trimer
* @return la chaine sans ses espaces en début et fin de chaine
*/
char* trim_string(char *string)
{
  // On ignore les espaces au debut de la chaine s'il y en a
  while(isspace((unsigned char)*string)) string++;
  if(*string == 0) return string;
  // On ignore les espaces a la fin de la chaine s'il y en a
  char *fin_str = string + strlen(string);
  while(fin_str > string && isspace(*--fin_str));
  *(fin_str+1) = '\0';

  // Retourne une copie de la chaine de caractères
  return clone_string(string);
}

transition creer_transition(char *etat, char sym_lu, char sym_ecrit, 
  char mvt, char *nouv_etat) {
  // Allocation mémoire de la transition résultat
  transition res = (transition) malloc(sizeof(struct transition_s));

  res->etat = etat;
  res->symbole_lu = sym_lu;
  res->symbole_ecrit = sym_ecrit;
  res->mouvement = mvt;
  res->nouvel_etat = nouv_etat;
  res->suivant = NULL;

  return res;
}

int transition_existe(MT mt, transition tr) {
  transition t = mt->transitions;
  while(t) {
     if(!strcmp(t->etat, tr->etat) && t->symbole_lu == tr->symbole_lu 
        && t->symbole_ecrit == tr->symbole_ecrit 
        && t->mouvement == tr->mouvement 
        && !strcmp(t->nouvel_etat, tr->nouvel_etat))
        return 1;
      
      t=t->suivant;
  }
  return 0;
}

void ajouter_transition(MT mt, transition nouveau, int distinct) {
  // On ajoute que si la liste peut contenir des doublons ou si
  // le nouvel élément n'appartient pas à la liste
  if(!distinct || !transition_existe(mt, nouveau)) {
    // Si la liste des transitions est vide, le nouvel élément devient la 
    // tête de liste
    if(mt->transitions == NULL) {
      mt->transitions = nouveau;
      mt->transitions_fin = nouveau;
    }
    // Sinon, on ajoute le nouvel élément à la fin de la liste
    else {
      mt->transitions_fin->suivant = nouveau;
      mt->transitions_fin = nouveau;
    }
  } else if(nouveau) free(nouveau);
}

void afficher_transitions(transition transitions) {
  printf("> TRANSITIONS DE LA MACHINE :\n");
  printf("Etat  |  Symbole lu  |  Symbole ecrit  |  Mouvement  |  Nouvel etat\n");
  while(transitions != NULL) {
  printf(" %s           %c                %c               %c             %s\n", 
    transitions->etat, transitions->symbole_lu, transitions->symbole_ecrit,
    transitions->mouvement, transitions->nouvel_etat);
    transitions = transitions->suivant;
  }
  printf("\n");
}


/**
* Récupère un état spécial d'une machine de Turing (l'état initial ou 
* l'état final) à partir de la ligne du fichier de description de la
* machine contenant la description dudit état.
* @param line : la ligne du fichier de description contenant la 
* description de l'état spécial souhaité
* @param constante : La constante attendu ('init' pour l'état initial, 
* 'accept' pour l'état final)
* @return la chaine de caractère représentant l'état spécial
*/
char* get_etat_special(char *line, char *constante)
{
  char *field;
  field = strtok(line, ":");   // Split de la ligne sur :
  if(field != NULL) 
  {
    // Suppression des espaces blancs
    char *field_trim = trim_string(field);
    // Si le champ récupéré correspond bien au champ attendu
    if(!strcmp(field_trim, constante)) 
    {
      field = strtok(NULL, ":");
      if(field) 
      {
        // Suppression des espaces blancs
        char *etat_trim = trim_string(field);
        if(strlen(etat_trim) > 0) // Un état est une chaîne de caractère
          return etat_trim;
        else {
          fprintf(stderr, "\n[ERR]: Erreur dans le code de la machine:"
          "un état est attendu après %s\n\n", constante);
          // Libération de l'espace mémoire allouée
          free(etat_trim); 
          return NULL;
        }
      }
    } else {
      fprintf(stderr, "\n[ERR]: Erreur dans le code de la machine :"
      "'%s' est attendu, '%s' trouvé\n\n", constante, field_trim);
      // Libération de l'espace mémoire allouée
      free(field_trim); 
      return NULL;
    }
  } else {
    fprintf(stderr, "\n[ERR]: Erreur dans le code de la machine : %s:" 
    "est attendu\n\n", constante);
    return NULL;
  }

  return NULL;
}


/**
* Récupère le champ d'une transition
* @param line : la ligne du fichier contenant les champs de la 
*               transition 
* @param line_clone : une copie de line (sert pour l'affichage des 
*                     messages d'erreurs)
* @param is_mvt : 1 si le champ à récupérer concerne le mouvement 
*                 (droite, gauche ou aucun), 0 sinon
* @param alpha : L'alphabet de la machine :
*                  -> l'alphabet d'entrée si le champ à récupérer 
*                     est un symbole lu,
*                  -> l'alphabet de travail si le champ à récupérer 
*                     est un symbole écrit
*                  -> NULL pour les autres champs
* @param sb : le symbole blanc du ruban
* @return le champ récupéré
*/
char* get_transition_field(char *line, char *line_clone, int is_mvt, 
  char *alpha, char sb) {
  const char *delim = ",";
  // On split la ligne pour récupérer les différents champs de la 
  // transition
  char *field = strtok(line, delim);
  if(!field) { // Si le champ est nul
    fprintf(stderr, "\n[ERR]: Erreur dans le code de la machine :\n"
            "Ligne concernée : %s\n"
            "Une transition doit être du format " 
            "etat%ssymbole_lu%snouvel_etat%ssymbole_ecrit%smouvement\n",
            line_clone, delim, delim, delim, delim);
    return NULL;
  }

  // Si le champ est le symbole blanc, on ne le supprime pas 
  // Ceci est nécessaire lorsque le symbole blanc est un espace
  if(strlen(field) == 1 && field[0] == sb) return field;

  // Suppression des espaces blancs
  char *field_trim = trim_string(field);
  int cond;

  if(is_mvt)
    // Si le champ à récupérer est le mouvement, 
    // il ne doit pas être dans l'une des conditions suivantes
    cond = strlen(field_trim) != 1 || (field_trim[0] != DROITE && 
           field_trim[0] != GAUCHE && field_trim[0] != AUCUN);

  else if(!alpha) 
    // Si le champ à récupérer est un état, il ne doit pas être dans
    // l'une des conditions suivantes
    cond = strlen(field_trim) == 0;

  else
    // Si le champ à récupérer est un symbole, il ne doit pas être
    // dans l'une des conditions suivantes
    cond = strlen(field_trim) != 1 || 
    (strchr(alpha, field_trim[0]) == NULL && *field_trim != sb);

  // Si le champ lu n'est pas de la bonne forme 
  // (ne respecte pas la bonne condition),
  if(cond) {  
    // on renvoie une erreur.
    fprintf(stderr, "\n[ERR]: Erreur dans le code de la machine :\n"
            "Ligne concernée : %s\n"
            "Une transition doit être du format " 
            "etat%ssymbole_lu%snouvel_etat%ssymbole_ecrit%smouvement\n",
            line_clone, delim, delim, delim, delim);

    if(is_mvt) 
      fprintf(stderr, "Le mouvement '%s' n'est pas valide. "
              "Les mouvements possibles sont : '<' , '>' et '-'\n\n", 
              field_trim);
    else if(!alpha) 
      fprintf(stderr, "L'état '%s' n'est pas valide. "
              "Un état doit être une chaîne caractères non vide.\n\n", 
              field_trim);
    else 
      fprintf(stderr, "Le symbole '%s' n'est pas valide. "
              "Il doit être un caractère dans l'alphabet d'entrée/de "
              "travail '%s'\n\n", field_trim, alpha);

    free(field_trim);
    return NULL;
  }

  return field_trim;
}


MT init_machine_turing(char *path, char *alphabets, char symbole_blanc) {
  FILE *F;
  char *line = NULL;
  char *line_clone;
  size_t length = 0;
  ssize_t read;
  char *line_trim;

  if((F = fopen(path, "r")) == NULL)
  {
    fprintf(stderr, "\n[ERR]: Echec de l'ouverture du fichier %s", path);
    perror("\n\n");
    return NULL;
  }

  MT mt = (MT) malloc(sizeof(struct MT_s));
  mt->transitions = NULL;
  mt->etat_in = NULL;
  mt->etat_fin = NULL;
  
  // Initialisation des alphabets de la matrice. entree doit contenir 
  // l'alphabet d'entrée et l'alphabet de travail de la machine, 
  // les deux séparés par ':', i.e alphabet_entree:alphabet_travail
  // Les alphabets sont composés de caractères.
  // Ex : 01:01_ 
  // Dans cet exemple, l'alphabet d'entrée est {0,1} et l'alphabet 
  // de travail {0,1}.
  mt->alphabet_entree = strtok(alphabets, ":"); 
  mt->alphabet_travail = strtok(NULL, ":");
  // Pour vérifier qu'on a le bon nombre de paramètre
  char *temp = strtok(NULL, ":"); 
  if(mt->alphabet_entree == NULL || mt->alphabet_travail == NULL 
     || temp != NULL) 
  {
    fprintf(stderr, "\n[ERR]: Alphabets de la machine incorrect, " 
     "l'entrée doit être de la forme : "
     "'alphabet_entree:alphabet_travail'\n\n");
    return NULL;
  }

  // Initialisation du symbole blanc
  mt->symbole_blanc = symbole_blanc;

  // Initiailisation des états et des transitions de la machine
  // On lit le fichier ligne par ligne
  while ((read = getline(&line, &length, F)) != -1) {
    // Pour chaque ligne non nul,
    line_trim = trim_string(line);
    if(strlen(line_trim) != 0)
    {
      // On vérifie s'il s'agit de la déclaration d'un état spécial 
      // (initial ou final) i.e. leurs lignes doivent respectivement 
      // commencer par 'init' ou 'accept'.
      if(!strncmp(line_trim, "init", 4)) {
        mt->etat_in = get_etat_special(line_trim, "init");
        if(!mt->etat_in) {
          free(line_trim);
          return NULL;
        }
      }
      else if(!strncmp(line_trim, "accept", 6)) 
      {
        mt->etat_fin = get_etat_special(line_trim, "accept");
        if(!mt->etat_fin) {
          free(line_trim);
          return NULL;
        }
      }

      else { 
        // Les autres lignes correspondent à la description des
        // transitions : la description d'une transition doit se faire 
        // sur une ligne (ex : 'q0,0,q1,0,>')

        // On garde une copie de la ligne car strtok modifie l'argument 
        // passé en param
        line_clone = clone_string(line_trim);

        // Récupération de l'ancien état de la transition
        char *ea = get_transition_field(line_trim, line_clone, 0, NULL,
                                        mt->symbole_blanc);
        if(!ea) return NULL;
        // Récupération du symbole lu de la transition
        char *sl = get_transition_field(NULL, line_clone, 0, mt->alphabet_entree, mt->symbole_blanc); 
        if(!sl) {
          // Libération des mémoire temporaires allouées
          free(ea);
          return NULL;
        }
        // Récupération du nouvel état de la transition
        char *ne = get_transition_field(NULL, line_clone, 0, NULL,
                                        mt->symbole_blanc);
        if(!ne) 
        {
          // Libération des mémoire temporaires allouées
          free(ea);
          free(sl);
          return NULL;
        }
        // Récupération du symbole à écrire de la transition
        char *se = get_transition_field(NULL, line_clone, 0, mt->alphabet_travail, mt->symbole_blanc); 
        if(!se) {
          // Libération des mémoire temporaires allouées
          free(ea);
          free(sl);
          free(ne);
          return NULL;
        }
        // Récupération du mouvement de la transition
        char *mv = get_transition_field(NULL, line_clone, 1, NULL,
                                        mt->symbole_blanc);
        if(!mv) 
        {
          // Libération des mémoire temporaires allouées
          free(ea);
          free(sl);
          free(ne);
          free(se);
          return NULL;
        }

        // Ajout de la nouvelle transition dans la machine
        ajouter_transition(mt, creer_transition(ea, *sl, *se, *mv, ne), 0);

        // Libération des mémoire temporaires
        free(line_clone);
      }

      free(line_trim);

    }

  }

  fclose(F);
  
  // Initialisation des autres états et du ruban de la machine
  mt->etat_courant = mt->etat_in;
  mt->ruban_courant = NULL;
  mt->tete_lecture = mt->ruban_courant;

  // Si l'état initial ou l'état final n'ont pas été configuré dans le code
  // de la machine, on renvoie une erreur. Ces étapes sont obligatoires
  if(!mt->etat_in || !mt->etat_fin) 
  {
    fprintf(stderr, "\n[ERR]: Erreur dans le code de la machine : "
            "Les états initiaux et/ou finaux sont manquants\n");
    return NULL;
  }
  
  return mt;
}


void free_mt(MT mt) {
  // Désalloue les espaces mémoires des transitions
  transition suivant;
  while(mt->transitions != NULL) {
    suivant = mt->transitions->suivant;
    free(mt->transitions->etat);
    free(mt->transitions->nouvel_etat);
    free(mt->transitions);
    mt->transitions = suivant;
  }

  // Libère l'espace mémoire des états de la machine
  if(mt->etat_in) free(mt->etat_in);
  if(mt->etat_fin) free(mt->etat_fin);

  // Désalloue l'espace mémoire du ruban
  free_ruban(mt->ruban_courant);

  free(mt);
}

/**
* Affiche le ruban de la machine de Turing mt et son état courant
*/
void afficher_ruban_machine(MT mt) {
  printf("\n> CONFIGURATION ACTUELLE DU RUBAN :\n");
  printf("            ETAT : %s\n ", mt->etat_courant);
  afficher_ruban(mt->ruban_courant);
  for(ruban r=mt->ruban_courant; r; r=r->droite) {
    printf("   ");
    if(r==mt->tete_lecture) {
      printf("^\n");
      break;
    } else printf(" ");
  }
}

void afficher_machine_turing(MT mt) {
  printf("\n\nAlphabet entrée          Alphabet de Travail\n"
         "   %s                         %s \n"
         "\nEtat initial             Etat final\n"
         "     %s                      %s\n\n", 
         mt->alphabet_entree, mt->alphabet_travail,
         mt->etat_in, mt->etat_fin);
  afficher_transitions(mt->transitions);
  afficher_ruban_machine(mt);
  printf("\n");
}

int simuler_etape(MT mt) {
  if(!mt->tete_lecture) return 0;
  for(transition tr = mt->transitions; tr != NULL; tr = tr->suivant) {
    // Recherche de la bonne transition à appliquer :
    // l'état de la transition = l'état courant de la machine
    // et le symbole lu de la transition = symbole lu sur le ruban de 
    // la machine
    if(!strcmp(tr->etat, mt->etat_courant) && tr->symbole_lu == 
       mt->tete_lecture->symbole ) {
      // Le nouvel état de la transition devient L'état courant de la 
      // machine
      mt->etat_courant = tr->nouvel_etat;
      mt->tete_lecture->symbole = tr->symbole_ecrit;
      switch(tr->mouvement) {
        case DROITE: 
          // La bande est semi-infinie vers la droite
          if(!mt->tete_lecture->droite) ruban_ajouter_droite(mt->tete_lecture, mt->symbole_blanc);
          mt->tete_lecture = mt->tete_lecture->droite;
          break;
        case GAUCHE: 
          mt->tete_lecture = mt->tete_lecture->gauche;
          break;
      }
      return 1;
    }
  }
  return 0;
}


int simuler_turing(MT mt, char *mot) {
  // Initialisation du ruban avec le mot d'entrée
  mt->ruban_courant = init_ruban(mot);
  // Initialisation de la tête lecture du ruban
  mt->tete_lecture = mt->ruban_courant;
  afficher_machine_turing(mt);
  while(mt->etat_courant != mt->etat_fin && simuler_etape(mt)) {
    afficher_ruban_machine(mt);
  }
  return !strcmp(mt->etat_courant, mt->etat_fin);
}

/**
* Renvoie le i-ème caractère du codage d'un symbole donné
* @param codage : le codage des symboles
* @param symbole : le symbole à coder
* @param symbole_blanc : le symbole blanc du ruban
* @param i : l'indice du caractère à renvoyer
* @return symbole_blanc si symbole est symbole_blanc, 
*         -1 en cas d'erreur, sinon, 
*         code symbole selon le codage dans codage et renvoie le
*         caractère à l'indice i.
*/
char coder(const char codage[4][2], char symbole, char symbole_blanc, 
           int i){
  // L'ordre du caractère du symbole dans l'alphabet. 
  // ex: a->0, b->1, c->2, d->3
  int ordre = symbole-'a';
  // Si le symbole n'est pas dans l'alphabet (est différent de a,b,c,d, 
  // et du symbole blanc), erreur.
  // (ce cas ne peut normalement jamais arriver, car on vérifie les
  // symboles et l'alphabet de la machine lors de l'initialisation
  // de celle-ci)
  if((ordre < 0 || ordre > 3) && symbole != symbole_blanc) return -1;
  return symbole == symbole_blanc ? symbole_blanc : codage[ordre][i];
}

MT machine_latin_vers_binaire(char *path_in, char *path_out) {
  // Alphabets d'entrée et de travail de la machine à convertir
  char alpha_latin[] = "abcd:abcd";
  // ici, on définit ESPACE comme le symbole blanc du ruban
  char sb = ' ';

  // Définition du codage de l'alphabet d'entrée de la machine à 
  // convertir (Au format {code(a),code(b),code(c),code(d)})
  const char codage[4][2] = {"00","01","10","11"};

  char sl, se; // symbole lu, symbole ecrit
  
  // Création et initialisation de la machine à convertir i.e 
  // travaillant sur l'alphabet d'entrée latin ({a,b,c,d})
  MT mt_latin = init_machine_turing(path_in, alpha_latin, sb);

  if(!mt_latin) return NULL;

  // Machine temporaire permettant de stocker uniquement les 
  // transitions de la machine convertie avant leur écriture 
  // dans le  fichier
  MT mt_transitions = (MT) malloc(sizeof(struct MT_s));
  mt_transitions->transitions = NULL;
  mt_transitions->ruban_courant = NULL;
  mt_transitions->etat_in = NULL;
  mt_transitions->etat_fin = NULL;

  // Convertion des transitions de la machine
  for(transition tr=mt_latin->transitions; tr; tr=tr->suivant) {
    // Pour chaque transition

    // Conversion des transitions :
    // On crée une nouvelle transition entre l'ancien état et un nouvel
    // état intermédiaire tout en convertissant les symbol lus et écrits
    // ex : Si la transition est : A,a,B,b,> 
    // On écrit : A,0,A1,0,>
    sl = coder(codage, tr->symbole_lu, mt_latin->symbole_blanc, 0);
    se=coder(codage, tr->symbole_ecrit, mt_latin->symbole_blanc, 0);
    if(sl == -1 || se == -1) 
    {
      free_mt(mt_latin);
      free_mt(mt_transitions);
      return NULL;
    }
    // Création de l'état intermédiaire avec snprintf
    // On concatène 1 ou 2 (si le symbole lu est 0 ou 1) à l'état
    // original pour obtenir ce nouvel état intermédiaire
    // (sera free dans free_mt)
    char *nouv_etat = (char*) malloc(sizeof(char) * strlen(tr->etat) + 3);
    snprintf(nouv_etat, sizeof(char)*strlen(tr->etat)+3, 
             "%s%d", tr->etat, abs(sl-'0'+1));
    ajouter_transition(mt_transitions, 
                       creer_transition(clone_string(tr->etat), 
                       sl, se, tr->mouvement, nouv_etat), 1);

    // On crée une transition pour relier l'état intermédiaire
    // à l'état de destination original tout en convertissant les 
    // symboles lus et écrits
    // ex : Si la transition est : A,a,B,b,> 
    // A ce stade, on écrit : A1,0,B,1,>
    sl = coder(codage, tr->symbole_lu, mt_latin->symbole_blanc, 1);
    se = coder(codage, tr->symbole_ecrit, mt_latin->symbole_blanc, 1);
    if(sl == -1 || se == -1) 
    {
      free_mt(mt_latin);
      free_mt(mt_transitions);
      return NULL;
    }
    // clone_string(nouv_etat) Sera free dans free_mt
    ajouter_transition(mt_transitions, 
      creer_transition(clone_string(nouv_etat), sl, se, 
                       tr->mouvement, 
                       clone_string(tr->nouvel_etat)), 1);

  }

  // Ecriture du code de la nouvelle machine obtenue :
  // Ouverture du fichier de destination
  FILE *F;
  if((F = fopen(path_out, "w")) == NULL)
  {
    fprintf(stderr, "\n[ERR]: Echec de l'ouverture du fichier %s", 
             path_out);
    perror("\n\n");
    return NULL;
  }

  // Ecriture des états initial et final
  fprintf(F, "init: %s\n", mt_latin->etat_in);
  fprintf(F, "accept: %s\n\n", mt_latin->etat_fin);

  // Ecriture des transitions encodées
  for(transition tr=mt_transitions->transitions; tr;tr=tr->suivant) {
    fprintf(F, "%s,%c,%s,%c,%c\n", tr->etat, tr->symbole_lu,
            tr->nouvel_etat, tr->symbole_ecrit, tr->mouvement);
  }

  fclose(F);

  free_mt(mt_transitions);

  return mt_latin;
}