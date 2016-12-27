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
#include "duale.h"

/*------------------------------------------------------------
#  Prototype
--------------------------------------------------------------
*/
void liberation (int **X,int n);
int evaluer_fonction(int **X,int **C,int *F,int *Y,int n,int m);
int buildmodel (CPXENVptr env, CPXLPptr lp, int **C, int *F, int *D, int *B, int n, int m);


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
    double borne_duale;
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
        double temps_init = clock();
        methodeGrasp(C,X,Y,F,B,D,n,m,iteration);
        borne_primale = evaluer_fonction(X,C,F,Y,n,m);
        double temps_final = clock();
        printf("****************************  Borne Primale  *************************\n");
        printf(" BORNE PRIMALE = %d\n",borne_primale);
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
        double temps_heuristic;
        temps_heuristic = (double)(temps_final - temps_init) / CLOCKS_PER_SEC;
        printf("Temps heuristique = %f\n",temps_heuristic );

        /* Borne Duale : Relaxation
        */

        borne_duale =  methodeRelaxation(C,F,B,D,n,m);
        printf("****************************  Relaxation  *************************\n");
        printf("BORNE DUALE = %f\n",borne_duale);

        /* methode exacte
        */


        /* Declare variables and arrays where we will store the
        optimization results including the status, objective value,
        and variable values.
         */
         printf("****************************  METAHODE EXACTE  *************************\n");
        int       solstat;
        double    objval;

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
        printf("POPULATE YOUR DATA IN CPLEX \n");
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
        double *vector = NULL;
        vector         = (double *) malloc ((n+(n * m)) * sizeof(double));
        if ( vector    == NULL) {
          fprintf (stderr, "Could not allocate colcnt arrays\n");
          status = CPXERR_NO_MEMORY;
          goto TERMINATE;
        }

        status = CPXsolution (env, lp, &solstat, &objval, vector, NULL, NULL, NULL);
        if ( status ) {
          fprintf (stderr, "Failed to obtain solution.\n");
          goto TERMINATE;
        }

        //solstat = CPXgetstat (env, lp);
        // Write solution status, objective and solution vector to the screen
        //status = CPXgetobjval (env, lp, &objval);
        /*
        if ( status ) {
          fprintf (stderr,"No MIP objective value available.  Exiting...\n");
          goto TERMINATE;
        }
        */
        printf ("\nSolution status = %d\n", solstat);
        printf ("Solution value (min cout) = %f\n\n", objval);

        //Affichage element de vector
        for (int i = 0; i < n; i++) {
          for (int j = 0; j < m; j++) {
            X[i][j] = (int ) vector[varindex(i,j,m)];
          }
          Y[i] = (int) vector[((i+1)*m+i)];
        }

        free_and_null ((char **)&vector);




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

   int colcnt = n + (m * n);
   double *obj     = NULL;
   double *lb      = NULL;
   double *ub      = NULL;
   char   *ctype   = NULL;
   int    *rmatind = NULL;
   double *rmatval = NULL;
   int    *rmatbeg = NULL;
   double     *rhs = NULL;
   char     *sense = NULL;
   int    status = 0;

   /* Allocate colcnt-sized arrays */
   obj     = (double *) malloc (colcnt * sizeof(double));
   lb      = (double *) malloc (colcnt * sizeof(double));
   ub      = (double *) malloc (colcnt * sizeof(double));
   ctype   = (char *)   malloc (colcnt * sizeof(char));
   rmatind = (int * )   malloc ((n * m) * sizeof(int));
   rmatval = (double *) malloc ((n * m) * sizeof(double));
   rhs     = (double *) malloc (m * sizeof(double));
   rmatbeg = (int    *) malloc (m * sizeof(int));
   sense   = (char *) malloc (m * sizeof(char));

   if ( obj     == NULL ||
        lb      == NULL ||
        ub      == NULL ||
        ctype   == NULL ||
        rmatind == NULL ||
        rmatval == NULL ||
        rmatbeg == NULL ||
        sense   == NULL ||
        rhs     == NULL   ) {
      fprintf (stderr, "Could not allocate colcnt arrays\n");
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   }
   initialiser_tab_double(obj,colcnt,0.0);
   initialiser_tab_double(lb,colcnt,0.0);
   initialiser_tab_double(ub,colcnt,0.0);
   initialiser_tab_double(rmatval,(n * m),0.0);
   initialiser_tab_double(rhs,m,1.0);
   initialiser_tab_int(rmatind,(n * m),0);
   initialiser_tab_int(rmatbeg,m,0);
   initialiser_tab_char(ctype,colcnt,'B');
   initialiser_tab_char(sense,m,'E');
  //printf("index\n");
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
     // capacite d'affectation entre i et j
     //printf("%d\t",varindex(i,j,m));
     obj[varindex(i,j,m)]    = C[j][i];
     lb[varindex (i,j,m)]    = 0;
     ub[varindex (i,j,m)]    = 1;
     ctype[varindex (i,j,m)] = 'B';
   }
      //le dernier (indicateur) -> ((i+1)*m+i)
      //printf("\nindex indicator %d\n",((i+1)*m+i) );
      obj[((i+1)*m+i)]    = F[i];
      lb[((i+1)*m+i)]    = 0;
      ub[((i+1)*m+i)]    = 1;
      ctype[((i+1)*m+i)] = 'B';
   }

   status = CPXnewcols (env, lp, colcnt, obj, lb, ub, ctype, NULL);
   if ( status ) {
      fprintf (stderr, "Could not add new columns.\n");
      goto TERMINATE;
   }

   free_and_null ((char **)&obj);
   free_and_null ((char **)&lb);
   free_and_null ((char **)&ub);
   free_and_null ((char **)&ctype);

    // Now add the constraints

    //Première contrainte

    //rmatval
    for (int i = 0; i < (n * m); i++) {
      rmatval[i] = 1.0;
    }
    //rmatind
    int k = 0;
    for (int j = 0; j < m; j++) {
      for (int i = 0; i < n; i++) {
        rmatind[k] = varindex (i,j,m);
        k++;
      }
    }
    //rmatbeg
    for (int j = 0; j < m; j++) {
      rmatbeg[j] = j * n;
    }
    //rhs et sense
    /*
    for (int i = 0; i < m; i++) {
      rhs[i]   = 1.0;
    }
    */
    status = CPXaddrows (env, lp, 0, m, (n * m), rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);

    free_and_null ((char **)&rmatind);
    free_and_null ((char **)&rmatval);
    free_and_null ((char **)&rmatbeg);
    free_and_null ((char **)&rhs);
    free_and_null ((char **)&sense);

    //Deuxieme contrainte

    rmatind = (int * )   malloc (colcnt * sizeof(int));
    rmatval = (double *) malloc (colcnt * sizeof(double));
    rhs     = (double *) malloc (n * sizeof(double));
    rmatbeg = (int    *) malloc (n * sizeof(int));
    sense   = (char *) malloc (n * sizeof(char));
    if ( rmatind == NULL ||
         rmatval == NULL ||
         rmatbeg == NULL ||
         sense   == NULL ||
         rhs     == NULL   ) {
       fprintf (stderr, "Could not allocate colcnt arrays\n");
       status = CPXERR_NO_MEMORY;
       goto TERMINATE;
    }

    //rmatval
    k = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        rmatval[k] = D[j];
        k++;
      }
      rmatval[k] = -B[i];
      k++;
    }
    //rmatind
    k = 0;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        rmatind[k] = varindex (i,j,m);
        k++;
      }
      rmatind[k] = ((i+1)*m+i);
      k++;
    }
    //rmatbeg
    for (int i = 0; i < n; i++) {
      rmatbeg[i] = (i * m) +i;
    }
    //rhs et sense
    for (int i = 0; i < n; i++) {
      rhs[i]   = 0.0;
      sense[i] = 'L';
    }
    status = CPXaddrows (env, lp, 0, n, colcnt, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);


   TERMINATE:
    free_and_null ((char **)&obj);
    free_and_null ((char **)&lb);
    free_and_null ((char **)&ub);
    free_and_null ((char **)&ctype);
    free_and_null ((char **)&rmatind);
    free_and_null ((char **)&rmatval);
    free_and_null ((char **)&rmatbeg);
    free_and_null ((char **)&rhs);
    free_and_null ((char **)&sense);

    printf("\n");
   return (status);


}
