#ifndef GRASP_H
#define GRASP_H
void methodeGrasp(int **C,int **X,int *Y,int *F,int *B,int *D,int n, int m,int iteration);
void grasp(int **C,int **X,int *Y,int *F,int *B,int *D,int n, int m,float alpha);
float recuperer_max_phi(float *PHI,int *Y,int n);
float recuperer_min_phi(float *PHI,int *Y,int n);
int nombre_non_assigner(int *assigner,int m);
int calcule_somme_affectation(int **C,int *assigner,int m,int i);
int clients_assignees(int *assigner,int m);
#endif
