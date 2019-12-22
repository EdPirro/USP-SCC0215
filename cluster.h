/*
Arquivo que contem as definições de metodos e atributos referentes ao Cluster.
*/

#include <stdio.h>

#ifndef CLUSTER_H
#define CLUSTER_H

/*
Declaração da Struct "Cluster" que representa uma pagina de disco, tendo o elemento data que guarda o informação 
antes de ser salva e curPos para indicar o local de escrita atual.
*/
typedef struct _clst{
    char* data;
    int curPos;
}Cluster;


//Methods
Cluster* createCluster();
char clusterHasSpace(Cluster* c, int size);
int fillCluster(Cluster* c, char trash);
void destroyCluster(Cluster* c);
void saveCluster(const char* savePath, Cluster* c);
void saveLastCluster(const char* savePath, Cluster* c);
void saveClusterHead(const char* savePath, Cluster* c);
int loadCluster(Cluster* c, FILE* fp);
int clusterEnd(Cluster* c);

#endif // CLUSTER_H
