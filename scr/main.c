/* ==================================================================================================
   Format des instances :
     number of rows (m), number of columns (n)
     coût d'affectation c(ij),j=1,...,m; i=1,...n
     Demande D(j), ,j=1,...,m
     Coût d'installation ou d'ouverture F(i), ,i=1,...,n
     Capacité des sites B(i), ,i=1,...,n
     */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ilcplex/cplex.h>
#include "structure.h"
#include "grasp.h"

/*------------------------------------------------------------
#  Prototype
--------------------------------------------------------------
*/
void liberation (int **X,int n);
int evaluer_fonction(int **X,int **C,int *F,int *Y,int n,int m);

/*------------------------------------------------------------
#  Main
--------------------------------------------------------------
*/

int main(int argc, char const *argv[]) {
  int n=0,m=0;
  FILE* fichier = NULL;
  if (argc < 2) {
    exit(1);
  }
  //srand(0); //
  srand(time(NULL));
  //sinon aucun problème avec les paramètres
  for (int indice = 1; indice < argc; indice++) {
    int **C;
    int **X;
    int *Y;
    int *F;
    int *B;
    int *D;
    // Ouverture du fichier d'entree/sortie
    //La taille du chemin vaut au plus 250 caractères
    char chemin[250] = "../instances/";
    strcat(chemin, argv[indice]);
    strcat(chemin, ".txt");
    fichier   = fopen(chemin,"r");
    // lecture nombre de clients
      fscanf(fichier, "%d", &m);
      printf("\n =========================== INSTANCES  %s =========================== \n",chemin);
      printf("le nbre de client %d\n",m);
    // lecture nombre de sites
      fscanf(fichier, "%d", &n);
      printf("le nbre de sites %d\n",n);
      //allocations de memoires pour nos tableaux
      C = malloc(m * sizeof(*C));
      X = malloc(n * sizeof(*X));
      if(C == NULL || X == NULL ){
        printf("ERREUR ALLOCATION\n");
        exit(1);
      }
      for(int i=0 ; i < m ; i++){
         C[i] = malloc(n * sizeof(**C) );
         if(C[i] == NULL){
           printf("ERREUR ALLOCATION\n");
           exit(0);
         }
       }
       initialiser_matrice(C,m,n,0);
       for(int i=0 ; i < n ; i++){
          X[i] = malloc(m * sizeof(**X) );
          if(X[i] == NULL){
            printf("ERREUR ALLOCATION\n");
            exit(0);
          }
        }
        initialiser_matrice(X,n,m,0);
        //variable binaire indiquant l'ouverture d'un site
        Y = malloc(n * sizeof(int));
        //coût d'installation des sites
        F = malloc(n * sizeof(int));
        //capacite de couverture pour les sites
        B = malloc(n * sizeof(int));
        //Demande des clients
        D = malloc(m * sizeof(int));
        if(Y == NULL || F == NULL || B == NULL || D == NULL){
          printf("ERREUR ALLOCATION\n");
          exit(1);
        }
        initialiser_tab_int(Y,n,0);
        initialiser_tab_int(F,n,0);
        initialiser_tab_int(B,n,0);
        initialiser_tab_int(D,m,0);

        //Tout est alloué donc remplissage de tableau
        float tmp = 0;

        /* lecture matrice des couts affectation */
        for (int j=0; j< m; j++){
          for(int i=0; i< n;i++){
            fscanf(fichier,"%f",&tmp);
            C[j][i] = (int) tmp;
          }
        }
        affiche_matrice(C,m,n);
        printf("\n");
        /* lecture vecteur demande */
        for (int j=0; j< m; j++){
          fscanf(fichier,"%f",&tmp);
          D[j] = (int) tmp;
        }
        printf("\n Demande :");
        affiche_tab_int(D,m);
        printf("\n");
        /* lecture vecteur cout installation des sites */
        for (int i=0; i< n; i++){
          fscanf(fichier,"%f",&tmp);
          F[i] = (int) tmp;
        }
        printf("\n installation :");
        affiche_tab_int(F,n);
        printf("\n");
        /* lecture vecteur capacite des sites */
        for (int i=0; i< n; i++){
          fscanf(fichier,"%f",&tmp);
          B[i] = (int) tmp;
        }
        printf("\n capacite :\n");
        affiche_tab_int(B,n);
        printf("\n\n  ");

        fclose(fichier);
        //         =============================  DEBUT EXECUTION PROGRAMME =====================================



        int iteration = 1;
        /* Borne primale : heuristique
        *  n : le nombre de site
        *  m : le nombre de clients
        *  iteration : le nombre d'iteration grasp
        */
        methodeGrasp(C,X,Y,F,B,D,n,m,iteration);
        int z = evaluer_fonction(X,C,F,Y,n,m);
        printf(" Z vaut %d\n",z);
        printf("\n Les sites ouverts\n");
        for (int i = 0; i < n; i++) {
          printf("%d\t",i+1);
        }
        printf("\n Les clients fournis\n");
        for (int i = 0; i < n; i++) {
          for (int j = 0; j < m; j++) {
            if (X[i][j] == 1) {
              printf("%d\t",j+1);
            }
          }
        }

        /* Borne Duale : methode exacte
        */

        /*
        int status; /* contient le type d'erreur rencontré en cas de problème
        CPXENVptr env = CPXopenCPLEX (&status); /* ouvre un environnement Cplex */
        /*en cas d'erreur...
        if ( env == NULL ) {
        char  errmsg[CPXMESSAGEBUFSIZE];
        fprintf (stderr, "Could not open CPLEX environment.\n");
        CPXgeterrorstring (env, status, errmsg);
        fprintf (stderr, "%s", errmsg);
        goto TERMINATE;
        }
        // Turn on output to the screen
        status = CPXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
        if ( status ) {
            fprintf (stderr,"Failure to turn on screen indicator, error %d.\n", status);
            goto TERMINATE;
        }
        // Turn on data checking
        status = CPXsetintparam (env, CPXPARAM_Read_DataCheck, CPX_ON);
        if ( status ) {
          fprintf (stderr,"Failure to turn on data checking, error %d.\n", status);
          goto TERMINATE;
        }

        //crée un PL vide
        CPXLPptr lp = CPXcreateprob (env, &status, "SSCFLP");
        // traiter l'erreur...
        if ( lp == NULL ) {
          fprintf (stderr, "Failed to create LP.\n");
          goto TERMINATE;
        }
        //int CPXnewcols (env, lp,n*m, double *coeff, double *lb,double *ub, char *ctype, char **colname)
        //cplex



                TERMINATE:
                  //libration de tous les allocations
                  if ( lp != NULL ) {
                    status = CPXfreeprob (env, &lp);
                    if ( status ) {
                      fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
                    }
                  }
                  // Free up the CPLEX environment, if necessary
                  if ( env != NULL ) {
                    status = CPXcloseCPLEX (&env);
                    /* Note that CPXcloseCPLEX produces no output,
                    so the only way to see the cause of the error is to use
                    CPXgeterrorstring.  For other CPLEX routines, the errors will
                    be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON. */
            /*        if ( status ){
                      char  errmsg[CPXMESSAGEBUFSIZE];
                      fprintf (stderr, "Could not close CPLEX environment.\n");
                      CPXgeterrorstring (env, status, errmsg);
                      fprintf (stderr, "%s", errmsg);
                      printf("ERROR CPLEX ENVIRONNEMENT LIBERATION\n");
                    }
                  }

        */











        //FILE* f = popen("gnuplot -persist", "w");
    /*
        // Sorties graphiques sur gnuplot (valide sur linux et osx) ==================
        FILE* f = popen("GNUPLOT_PATH", "w");
        fprintf(f, " set grid xtics \n");
        fprintf(f, " set grid ytics \n");
        fprintf(f, " set xlabel \"Iteration(s)\" \n");
        fprintf(f, " set xrange [0:%d]\n", iteration_grasp);
        fprintf(f, " set ylabel \"Valeurs de z(x)\" \n");
        //fprintf(f, " set yrange [0:500]\n");
        fprintf(f, " set title \"GRASP | iterations: %d | alpha: %4.2f\" \n", iteration_grasp, alpha);
        fprintf(f, " plot \"outResultats.res\"    u 1:2  t 'phase 1'  w points ps 1, \"outResultats.res\"    u 1:3  t 'phase 2'  w lines \n");
        fflush(f);
        sleep(30);
    //    Sleep(30000); //sleep(30); // necessaire sur linux, inutile sur OSX
        pclose(f);

    */



        //Liberations des espaces alloués
        free(B);
        free(D);
        free(F);
        free(Y);
        //free(C);
        //free(X);

        liberation(C,m);
        liberation(X,n);

  }
    return 0;
}

// Fonction liberation
void liberation (int **ptr,int n){
    for (int  i = 0; i < n; i++) {
          free(ptr[i]);
    }
     free (ptr);
     //*ptr = NULL;
}
int evaluer_fonction(int **X,int **C,int *F,int *Y,int n,int m){
  int retour = 0;
  for (int i = 0; i < n; i++) {
    if (Y[i] == 1) {
      retour = retour + F[i];
    }
  }
  for (int j = 0; j < m; j++) {
    for (int i = 0; i < n; i++) {
      if (X[i][j] == 1) {
        retour = retour + C[j][i];
        break;
      }
    }
  }
  return retour;
}
