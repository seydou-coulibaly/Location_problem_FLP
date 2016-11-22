#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void initialiser(int* a,int n,int valeur){
    for (int i=0; i< n; i++){
      a[i] = valeur;
    }
  }
void initialiserMatrice(int** a,int n,int m, int valeur){
  for(int i=0 ; i < n ; i++){
    for(int j=0 ; j < m ; j++){
      a[i][j] = valeur;
    }
  }
}
void afficheTableau(int *A, int n){
    for(int i=0;i< n;i++){
        printf("%4d", A[i]);
    }
    printf("\t\t");
  }
void afficheMatrice(int **A,int ligne,int colonne){
  for(int j=0;j<ligne;j++){
    printf("    ");
    for(int i=0;i<colonne;i++){
        printf("%4d",A[j][i] );
    }
    	printf("\n");
  }
    printf("\n\n");
}
