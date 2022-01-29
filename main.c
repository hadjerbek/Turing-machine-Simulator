#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "machineturing.h"

/**
* Simule une machine de turing sur un mot d'entrée et affiche le 
* résultat.
* @param path : chemin vers la machine à exécuter
* @param alphabets : les alphabets de la machine, au format
*                    alphabet_entree:alphabet_travail
* @param sb : le symbole blanc de la machine
* @param mot_entree : le mot d'entrée à simuler
* @return 1 en cas d'erreur lors de l'exécution, 0 sinon
*/
int simuler_machine(char *path, char *alphabets, char sb, 
                    char *mot_entree) {
  MT mt = init_machine_turing(path, alphabets, sb);
  if(!mt) return 1;
  if(simuler_turing(mt, mot_entree)) printf("ACCEPTE\n");
  else printf("REFUSE\n");

  free_mt(mt);

  return 0;
}

/**
* Code un mot d'entrée à exécuter sur la machine convertie en utilisant
* le codage code(a)=00,code(b)=01,code(c)=10,code(d)=11
*/
char* coder_mot(char *mot_entree) {
  const char codage[4][2] = {"00","01","10","11"};
  char *res = (char*) malloc(sizeof(char) * strlen(mot_entree) * 2 + 1);
  int ordre, i;
  
  for(i=0; i<strlen(mot_entree); i++) {
    // L'ordre du caractère du symbole i dans l'alphabet. 
    // ex: a->0, b->1, c->2, d->3
    ordre = mot_entree[i]-'a';
    // Si le symbole n'est pas dans l'alphabet 
    // (est différent de a,b,c et d), erreur.
    if(ordre < 0 || ordre > 3) 
    {
      free(res);
      return NULL;
    }

    res[i*2] = codage[ordre][0];
    res[i*2 + 1] = codage[ordre][1];
  }
  res[2*i] = '\0';

  return res;
}

/**
* Aide à l'utilisation du programme
*/
void usage() {
  fprintf(stderr, "Usage :[1]   ./simulation_mt PATH ALPHABETS SB\n"
                  "                 OU\n"
                  "       [2]  ./simulation_mt -C PATH_IN PATH_OUT\n"
        "[1] Simule la machine de turing decrit dans PATH\n"
        "[2] Convertit la machine de turing decrit dans PATH_IN, "
        "travaillant sur l'alphabet d'entree {a,b,c,d}\n"
        "    en une machine equivalente travaillant sur {0,1}. " 
        "Execute ensuite la nouvelle machine obtenue\n\n"
        "PARAMETRES\n"
        "[1]\n"
        "PATH        Chemin vers le fichier contenant la "
        "description de la machine de Turing à exécuter\n"
        "ALPHABETS   Les alphabets d'entrées et de travail de la "
        "machine. Doit être au format :\n"
        "            'alphabet_entree:alphabet_travail'\n"
        "            Chaque caractère du ruban (entrée/travail) "
        "doit appartenir à la chaine de caractère\n" 
        "            'alphabet_entree'/'alphabet_travail'\n"
        "SB           Le Symbole Blanc (vide) du ruban de la " 
        "machine\n\n"
        "[2]\n"
        "PATH_IN      Chemin du fichier contenant la description de la "
        "machine a convertir\n"
        "PATH_OUT     Fichier ou stocker le code de la machine convertit"
        "\n\n");
}

int main(int argc, char *argv[]) {
  if(argc != 4) {
    usage();
    return 1;
  }

  // Si option -C spécifié
  if(!strcmp(argv[1], "-C")) {
    MT mt_latin = machine_latin_vers_binaire(argv[2], 
                  argv[3]);
    if(!mt_latin) return 1;

    printf("\n========================================================================\n");
    printf("\nConversion de la machine de l'alphabet {a,b,c,d} vers {0,1} avec succès\n"
           "Code de la nouvelle machine dans %s\n", argv[3]);

    char alphabets[] = "01:01";
    printf("\n>>> SIMULATION DE LA MACHINE '%s'\n" 
           ">>> ALPHABET abcd:abcd\n", argv[3]);

    char *mot_entree = readline("\nMot d'entrée > ");

    // Converti le mot d'entree en binaire (en utilisant le codage)
    // code(a)=00, code(b)=01, code(c)=10, code(d)=11
    char *mot_bin = coder_mot(mot_entree);
    if(!mot_bin) {
      fprintf(stderr, "Le mot d'entree n'est pas correct. Il doit être "
              "dans l'alphabet abcd\n");
      free(mt_latin);
      return 1;
    }

    printf("Codage du mot d'entree : '%s' -> '%s'\n"
           "Simulation de la machine sur %s\n",
           mot_entree, mot_bin, mot_bin);

    int ret = simuler_machine(argv[3], alphabets, 
                              mt_latin->symbole_blanc, mot_bin);

    free_mt(mt_latin);

    return ret;
  }

  // Mode sans option (mode standard)
  printf("\n==========================================================================\n");
  printf("\n>>> SIMULATION DE LA MACHINE '%s'\n" 
          ">>> ALPHABET %s\n", argv[1], argv[2]);

  char *mot_entree = readline("\nMot d'entrée > ");

  return simuler_machine(argv[1], argv[2], argv[3][0], mot_entree);

}