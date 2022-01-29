#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ruban.h"


ruban init_case_ruban(char symbole) {
  ruban res = (ruban) malloc(sizeof(struct ruban_s));
  if(res == NULL) {
    perror("Erreur d'allocation de la mémoire d'une case du ruban.\n");
    return NULL;
  }
  res->symbole = symbole;
  res->droite = NULL;
  res->gauche = NULL;
  return res;
}

ruban ruban_ajouter_gauche(ruban case_courante, char symbole) {
  ruban c = init_case_ruban(symbole);
  if(c == NULL) return NULL;
  if(case_courante == NULL) case_courante = c;
  else{
    c->gauche = case_courante->gauche;
    c->droite = case_courante;
    if(case_courante->gauche != NULL) case_courante->gauche->droite = c;
    case_courante->gauche = c;
  }
  return c;
}

ruban ruban_ajouter_droite(ruban case_courante, char symbole) {
  ruban c = init_case_ruban(symbole);
  if(c == NULL) return NULL;
  if(case_courante == NULL) case_courante = c;
  else{
    c->gauche = case_courante;
    c->droite = case_courante->droite;
    if(case_courante->droite != NULL) case_courante->droite->gauche = c;
    case_courante->droite = c;
  }
  return c;
}

ruban init_ruban(char *mot) {
  if(strlen(mot) < 1) return NULL;
  ruban r = init_case_ruban(mot[0]);
  ruban cur = r;
  for(int i = 1; i<strlen(mot); i++) {
    cur = ruban_ajouter_droite(cur, mot[i]);
  }
  return r;
}

void afficher_ruban(ruban r) {
  // Affiche les symboles de chaque case du ruban
  while(r != NULL) {
    printf("| %c ", r->symbole);
    r = r->droite;
  }
  printf("|\n");
}

void free_ruban(ruban r) {
  ruban next;
  while(r != NULL) {
    next = r->droite;
    free(r);
    r = next;
  }
}
