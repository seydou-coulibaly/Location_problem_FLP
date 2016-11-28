#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grasp.h"
#include "structure.h"
#include "liste.h"
void methodeGrasp(int **C,int **X,int *Y,int *F,int *B,int *D,int n, int m,int iteration){
	float alpha = 0;
	int nbre = 0;
	do {
		nbre++;
		grasp(C,X,Y,F,B,D,n,m,alpha);
	} while(nbre < iteration);
}
void grasp(int **C,int **X,int *Y,int *F,int *B,int *D,int n, int m,float alpha){
  float *PHI = NULL;
  int *assigner = NULL;
  float phi_min;
  float phi_max;
  float test;
  int choix_site;
  listes RCL = NULL ;
	int tailleRCL;
  int somme;
	int capacite;
  int nbr_non_assigner;
  PHI = malloc( n * sizeof(int));
  assigner = malloc( m * sizeof(int));
  if (PHI == NULL || assigner == NULL){
      printf("Insuffisance de memoire pour allouer une table de contraintes ou d'utilités\n");
      exit(1);
  }
  initialiser_tab_int(assigner,m,0);
  initialiser_tab_float(PHI,n,0);
  do {
    //calcule PHI pour chaque site non ouvert
    for(int i = 0 ;i < n;++i){
      somme = 0;
      if (Y[i] == 0) {
        //calculer somme cout affectation des clients non assignés au site i
        somme = calcule_somme_affectation(C,assigner,m,i);
        nbr_non_assigner = nombre_non_assigner(assigner,m);
        PHI[i] = (F[i] + somme)/nbr_non_assigner;
      }
    }
    //recuper PHI de min et maximum
    phi_min = recuperer_min_phi(PHI,Y,n);
    phi_max = recuperer_max_phi(PHI,Y,n);
		//printf(" mini = %f\n",phi_min);
		//printf("\t%f\n",phi_max);
    for (int i = 0; i < n; i++) {
			if (Y[i] == 0) {
				test = (PHI[i] - phi_min)/(phi_max - phi_min);
				if (test <= alpha ) {
	        //l'ajouter dans le RCL
	        RCL = ajoutListe(RCL,i);
	      }
			}
    }

    //faire un rand entre 1 et la taille du RCL
		tailleRCL = sizeListe(RCL);
		if (tailleRCL == 0) {
			printf("INSTANCES IMPOSSIBLE : tous les sites sont ouverts mais tous les clients ne sont pas fournis\n");
		}
		else if (tailleRCL == 1) {
	    choix_site = 1;
	  }
		else{
	    choix_site = rand()%(tailleRCL-1) +1;
	  }
    choix_site = recupererPosition(RCL,choix_site);
    //le site choix_site est ouvert et sa capacite vaut B[i]
    Y[choix_site] = 1;

    capacite = B[choix_site];
    for (int j = 0; j < m; j++) {
      if (assigner[j] == 0) {
        //tester si la contrainte de capacite ne sera pas violé
        if (capacite >= D[j]) {
          //faire l'affectation
          X[choix_site][j] = 1;
          //mettre à la capacite
          capacite = capacite - D[j];
          //marquer j comme étant assigner
          assigner[j] = 1;
        }
      }
    }

		//vider la RCL
		listes temp;
		while (RCL != NULL) {
			temp = RCL;
			RCL = RCL->next;
			free(temp);
		}
  } while( clients_assignees(assigner,m)!= m);

  /*Liberation de l'espace alouer*/
  free(PHI);
  free(assigner);
}
int clients_assignees(int *assigner,int m){
  int nombre = 0;
  for (int i = 0; i < m; i++) {
    if (assigner[i] == 1) {
      nombre++;
    }
    else{
      break;
    }
  }
  return nombre;
}
int calcule_somme_affectation(int **C,int *assigner,int m,int i){
  int somme = 0;
  for (int j = 0; j < m; j++) {
    if (assigner[j] == 0) {
      somme = somme + C[j][i];
    }
  }
  return somme;
}
int nombre_non_assigner(int *assigner,int m){
  int nbr = 0;
  for (int  i = 0; i < m; i++) {
    if (assigner[i] == 0) {
      nbr++;
    }
  }
  return nbr;
}
float recuperer_min_phi(float *PHI,int *Y,int n){
  float retour = 1000000;
  for (int i = 0; i < n; i++) {
    if (Y[i] == 0) {
      if (PHI[i] < retour) {
        retour = PHI[i];
      }
    }
  }
  return retour;
}
float recuperer_max_phi(float *PHI,int *Y,int n){
  float retour = 0;
  for (int i = 0; i < n; i++) {
    if (Y[i] == 0) {
      if (PHI[i] > retour) {
        retour = PHI[i];
      }
    }
  }
  return retour;
}
