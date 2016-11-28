#ifndef STRUCTURE_H
#define STRUCTURE_H

void affiche_matrice(int **A,int ligne,int colonne);
void affiche_tab_int(int *A, int colonne);
void affiche_tab_float(float *A, int colonne);

void initialiser_tab_int(int*a,int nb_var,int valeur);
void initialiser_tab_float(float* a,int nb_var,int valeur);
void initialiser_matrice(int** a,int nb_cte, int nb_var,int valeur);

#endif
