#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ilcplex/cplex.h>
#include "structure.h"
#include "duale.h"


double methodeRelaxation(int **C,int *F,int *B,int *D,int n,int m){
int       solstat;
double    objval;
int       colcnt = 0;
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
status = buildmodelRelax (env, lp, C, F, D, B, n, m);
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
// Write solution status, objective and solution vector to the screen
status = CPXgetobjval (env, lp, &objval);
if ( status ) {
  fprintf (stderr,"No MIP objective value available.  Exiting...\n");
  goto TERMINATE;
}
printf ("\nSolution status = %d\n", solstat);
printf ("Solution value (min cout) = %f\n\n", objval);

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
return objval;
}

/*
Build model using indicator constraints and
semi-continuous variables
*/
int buildmodelRelax (CPXENVptr env, CPXLPptr lp, int **C, int *F, int *D, int *B, int n, int m){

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
ctype[varindex (i,j,m)] = 'C';
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

int varindex (int i, int j, int m){
return (m * i) + i + j;
}
void free_and_null (char **ptr){
if ( *ptr != NULL ) {
free (*ptr);
*ptr = NULL;
}
}
