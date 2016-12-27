#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structure.h"

void initialiser_tab_int(int* a,int nb_var,int valeur){
    for (int i=0; i<nb_var; i++){
      a[i] = valeur;
    }
  }
void initialiser_tab_double(double* a,int nb_var,double valeur){
  for (int i=0; i<nb_var; i++){
    a[i] = valeur;
  }
}
void initialiser_tab_char(char* a,int nb_var,char valeur){
  for (int i=0; i<nb_var; i++){
    a[i] = valeur;
  }
}
void initialiser_matrice(int** a,int nb_cte, int nb_var,int valeur){
    for (int i=0; i<nb_cte; i++){
      for (int j=0; j<nb_var; j++){
        a[i][j] = valeur;
    }
  }
}
void initialiser_tab_float(float* a,int nb_var,int valeur){
    for (int i=0; i<nb_var; i++){
      a[i] = valeur;
    }
  }
void affiche_matrice(int **A,int ligne,int colonne){
  for(int j=0;j<ligne;j++){
    printf("    ");
    for(int i=0;i<colonne;i++){
      printf("%6d",A[j][i] );
    }
  	printf("\n");
  }
  printf("\n\n");
}
void affiche_tab_int(int *A, int colonne){
  for(int i=0;i<colonne;i++){
    printf("%6d", A[i]);
  }
  printf("\t\t");
}
void affiche_tab_float(float *A, int colonne){
  for(int i=0;i<colonne;i++){
    printf("%6f\t", A[i]);
  }
  printf("\n\n");
}
