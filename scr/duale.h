#ifndef DUALE_H
#define DUALE_H
double methodeRelaxation(int **C,int *F,int *B,int *D,int n,int m);
int buildmodelRelax (CPXENVptr env, CPXLPptr lp, int **C, int *F, int *D, int *B, int n, int m);
int varindex (int i, int j, int m);
void free_and_null (char **ptr);
#endif
