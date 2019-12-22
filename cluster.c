#include "cluster.h"
#include <stdlib.h>


/*
Definição:
    Função feita para alocar espaço e criar um Cluster, inicializando seus valores com 0.
Retorno:
    Cluster*: endereço para o Cluster criado.
*/
Cluster* createCluster(){
    Cluster* c = malloc(sizeof(Cluster));
    c->data = calloc(32000, 1);
    c->curPos = 0;
    return c;
}

/*
Definição:
    Função simples que verifica se o Cluster ter um certo espaço livre, para saber se certo registro pode ser escrito nele.
Parâmetros:
    Cluster* c: endereço para o Cluster cujo espaço deseja-se verificar.
    int size: quantidade de espaço cuja disponibilidade no Cluster deseja-se verificar.
Retorno:
    char: um char com função de booleano, tera o valor inteiro 1, se o Cluster tiver espaço suficiente, ou 0, se não tiver.
*/
char clusterHasSpace(Cluster* c, int size){
    return c->curPos + size + 5 <= 32000 ? 1 : 0;
}

/*
Difinição:
    Função simples criada para preencher o Cluster até o final com o char definido para "lixo".
Parâmetros:
    Cluster* c: Cluster que deseja-se preencher.
    char trash: caracter atribuido para lixo, nesse caso '@'.
Retorno:
    int: quantidade de posições do Cluster preenchidas com lixo pela função.
*/
int fillCluster(Cluster* c, char trash){
    int count = 0;
    for(int i = c->curPos; i < 32000; i++, count++)
        c->data[i] = trash;
    return count;
}

/*
Difinição:
    Função simples definida para liberar a memoria atribuida para um Cluster.
Parâmetros:
    Cluster* c: ponteiro para o Cluster que deseja-se desalocar.
*/
void destroyCluster(Cluster* c){
    if(!c) return;
    if(c->data)free(c->data);
    free(c);
    return;
}

/*
Definição:
   Função feita para salvar um Cluster no final de um arquivo no caminho especificado.
Parâmetros:
    const char* savePath: caminho para o arquivo onde deseja-se salvar o Cluster.
    Cluster* c: ponteiro para o Cluster que deseja-se salvar.
*/
void saveCluster(const char* savePath, Cluster* c){
    FILE* fp = fopen(savePath, "ab");
    fwrite(c->data, 1, 32000, fp);
    fclose(fp);   
    return;
}

/*
Definição:
    Função feita para salvar o ultimo Cluster de um arquivo, para que seja salvado apenas a quantidade de dados uteis.
Parâmetros:
    const char* savePath: caminho para o arquivo onde deseja-se salvar o Cluster.
    Cluster* c: ponteiro para o Cluster que deseja-se salvar.
*/
void saveLastCluster(const char* savePath, Cluster* c){
    FILE* fp = fopen(savePath, "ab");
    fwrite(c->data, 1, c->curPos, fp);
    fclose(fp);   
    return;
}

/*
Definição:
    Função feita para salvar o Header, ou seja, primeiro Cluster de um arquivo, criando o arquivo e escrevendo a partir do inicio.
Parâmetros:
    const char* savePath: caminho para o arquivo onde deseja-se salvar o Cluster.
    Cluster* c: ponteiro para o Cluster que deseja-se salvar.
*/
void saveClusterHead(const char* savePath, Cluster* c){
    FILE* fp = fopen(savePath, "wb");
    fseek(fp, 0, SEEK_SET);
    fwrite(c->data, 1, 32000, fp);
    fclose(fp);   
    return;
}

/*
Definição:
    Função feita para ler um Cluster inteiro de um arquivo especificado.
Parâmetros:
    Cluster* c: ponteiro para um Cluster no qual deseja-se salvar o conteudo lido.
    FILE* fp: ponteiro para um arquivo de onde serão lidas as informações.
Retorno:
    int: numero de elementos lido ou EOF caso chegue no final do arquivo.
*/
int loadCluster(Cluster* c, FILE* fp){
    if(c->curPos != 0){
        destroyCluster(c);
        c = createCluster();
    }
    return  fread(c->data, 32000, 1, fp);
}

/*
Definição:
    Função simples para verificar se o Cluster esta no final.
Parâmetros:
    Cluster* c: Cluster em que deseja-se verificar se o final foi atingido.
Retorno:
    int: inteiro com função de booleano 1 se estiver no final e 0 se não estiver.
*/
int clusterEnd(Cluster* c){
    return c->curPos >= 32000;
}