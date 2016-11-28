#ifndef LISTE_H
#define LISTE_H
//declaration de type
typedef struct element{
  int contenu;
  struct element *next;
}element;
typedef element* listes;
listes ajoutListe(listes maListe, int valeur);
int rechecherListe(listes maListe, int valeur);
int minimumListe(listes maListe);
int sizeListe(listes maListe);
int recupererPosition(listes maListe,int min);
#endif
