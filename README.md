# Simulation de Machine de Turing 

## Description du programme
Le but de ce programme est d’executer pas à pas des machines de Turing, ainsi que de simuler des machines de Turing complexes par des machines simples.

## Utilisation du Programme 
**Usage** :[1]   ./simulation_mt PATH ALPHABETS SB
                 OU
       [2]  ./simulation_mt -C PATH_IN PATH_OUT
[1] Simule la machine de turing decrit dans PATH
[2] Convertit la machine de turing decrit dans PATH_IN, travaillant sur l'alphabet d'entree {a,b,c,d}
    en une machine equivalente travaillant sur {0,1}. Execute ensuite la nouvelle machine obtenue

**PARAMETRES**
[1]
PATH        Chemin vers le fichier contenant la description de la machine de Turing à exécuter
ALPHABETS   Les alphabets d'entrées et de travail de la machine. Doit être au format :
            'alphabet_entree:alphabet_travail'
            Chaque caractère du ruban (entrée/travail) doit appartenir à la chaine de caractère
            'alphabet_entree'/'alphabet_travail'
SB           Le Symbole Blanc (vide) du ruban de la machine

[2]
PATH_IN      Chemin du fichier contenant la description de la machine a convertir
PATH_OUT     Fichier ou stocker le code de la machine convertit

