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
int buildmodel (CPXENVptr env, CPXLPptr lp, int **C, int *F, int *D, int *B, int n, int m);
int varindex (int i, int j);
void free_and_null (char **ptr);

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
    int borne_primale;
    int borne_duale;
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
        borne_primale = evaluer_fonction(X,C,F,Y,n,m);
        printf(" LA BORNE PRIMALE = %d\n",borne_primale);
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
        printf("\n");

        /* Borne Duale : methode exacte
        */

        /* Declare variables and arrays where we will store the
        optimization results including the status, objective value,
        and variable values.
         */

        int       solstat;
        double    objval;
        int       colcnt = 0;
        double    *x = NULL;
        int status;
        CPXENVptr env = CPXopenCPLEX (&status); // ouvre un environnement Cplex

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
        // Build the model
        status = buildmodel (env, lp, C, F, D, B, n, m);
        if ( status ) {
          fprintf (stderr, "Failed to build model.\n");
           goto TERMINATE;
         }

         // Write a copy of the problem to a file.
         status = CPXwriteprob (env, lp, "SSCFLP.lp", NULL);
         if ( status ) {
           fprintf (stderr, "Failed to write LP to disk.\n");
           goto TERMINATE;
        }
        // Optimize the problem and obtain solution.
        status = CPXmipopt (env, lp);
        if ( status ) {
          fprintf (stderr, "Failed to optimize MIP.\n");
          goto TERMINATE;
        }
        solstat = CPXgetstat (env, lp);
        // Write solution status, objective and solution vector to the screen.
        printf ("\nSolution status = %d\n", solstat);
        status = CPXgetobjval (env, lp, &objval);
        if ( status ) {
          fprintf (stderr,"No MIP objective value available.  Exiting...\n");
          goto TERMINATE;
        }
        printf ("Solution value (min cout) = %f\n\n", objval);


        //int CPXnewcols (env, lp,n*m, double *coeff, double *lb,double *ub, char *ctype, char **colname)
        printf("POPULATE YOUR DATA IN CPLEX \n");


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
            if ( status ){
              char  errmsg[CPXMESSAGEBUFSIZE];
              fprintf (stderr, "Could not close CPLEX environment.\n");
              CPXgeterrorstring (env, status, errmsg);
              fprintf (stderr, "%s", errmsg);
              printf("ERROR CPLEX ENVIRONNEMENT LIBERATION\n");
            }
          }


          /**
          * Branch and bound algorithme
          */

          //borne_duale =
          //branch_and_bound(X,Y,F,C,B,D,borne_duale,borne_primale)





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
/*
  Build model using indicator constraints and
  semi-continuous variables
*/
int buildmodel (CPXENVptr env, CPXLPptr lp, int **C, int *F, int *D, int *B, int n, int m){

   int colcnt = 2 * m * n;
   int     *obj     = NULL;
   int     *lb      = NULL;
   int     *ub      = NULL;
   char    *ctype   = NULL;
   int     *rmatind = NULL;
   int     *rmatval = NULL;
   int     *rmatbeg = NULL;
   int         *rhs = NULL;
   int        *sense = NULL;
   int    indicator;
   int    status = 0;

   /* Allocate colcnt-sized arrays */

   obj     = (int *) malloc (colcnt * sizeof(int));
   lb      = (double *) malloc (colcnt * sizeof(int));
   ub      = (double *) malloc (colcnt * sizeof(int));
   ctype   = (char *)   malloc (colcnt * sizeof(char));
   rmatind = (int * )   malloc (colcnt * sizeof(int));
   rmatval = (double *) malloc (colcnt * sizeof(double));

   /*
   int     *rmatbeg = NULL;
   int     *rmatbeg = NULL;
   int         *rhs = NULL;
   int        *sense = NULL;
   */

   if ( obj     == NULL ||
        lb      == NULL ||
        ub      == NULL ||
        ctype   == NULL ||
        rmatind == NULL ||
        rmatval == NULL   ) {
      fprintf (stderr, "Could not allocate colcnt arrays\n");
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }

   for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {

         /* capacite d'affectation entre i et j */

         obj[varindex(i,j,0)]    = C[j][i];
         lb[varindex (i,j,0)]    = 0;
         ub[varindex (i,j,0)]    = 1;
         ctype[varindex (i,j,0)] = 'B';

         obj[varindex (i,j,1)]   = 0;
         lb[varindex (i,j,1)]    = 0;
         ub[varindex (i,j,1)]    = 1;
         ctype[varindex (i,j,1)] = 'B';
      }
   }

   status = CPXnewcols (env, lp, colcnt, obj, lb, ub, ctype, NULL);
   if ( status ) {
      fprintf (stderr, "Could not add new columns.\n");
      goto TERMINATE;
   }


   TERMINATE:
    free_and_null ((char **)&obj);
    free_and_null ((char **)&lb);
    free_and_null ((char **)&ub);
    free_and_null ((char **)&ctype);
    free_and_null ((char **)&rmatind);
    free_and_null ((char **)&rmatval);

   return (status);

}

int varindex (int i, int j){
   return (2 * j * i + 2 * j);
}
void free_and_null (char **ptr){
   if ( *ptr != NULL ) {
      free (*ptr);
      *ptr = NULL;
   }
}
