#include "index.h"
#include "merge_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Declaração da struct "idxelem" que representa um elemento do index.
*/
struct _idxelem{
    char key[121];
    long long offset;
};

/*
Declaração da struct "indx" que representa o indice em si.
*/
struct _indx{
    IndexElem** data;
    int size;
};



/*
Definição:
    Função que cria um elemento do indice, que possui uma chave e o offset do registro que possui essa chave
Parâmetros:
    char* key: chave do elemento, no caso, nomeServidor (char*)
    long long offset: valor do offset do registro que possui essa chave no registro de dados.
Retorno:
    IndexElem* - ponteiro para o elemento de indice criado.
*/
IndexElem* createIndexElem(char* key, long long offset){
    IndexElem* ie = malloc(sizeof(IndexElem));
    for(int i = 0; i < 120; i++) ie->key[i] = '@';
    strcpy(ie->key, key);
    ie->offset = offset;
    return ie;
}



/*
Definição:
    Funcao que cria um indice, alocando a estrutura dinamicamente e inicializando os valores.
Retorno:
    Index* - ponteiro para o indice criado.
*/
Index* createIndex(){
    Index* i = malloc(sizeof(Index));
    i->data = NULL;
    i->size = 0;
    return i;
}



/*
Definição:
    Metodo que adiciona um elemento novo ao indice já existente.
Parâmetros:
    Index* i: ponteiro para a estrutura de indice.
    IndexElem* ie: elemento de indice que será adicionado.
*/
void addIndexElem(Index* i, IndexElem* ie){
    if(!i->size) i->data = malloc(sizeof(IndexElem*));
    else i->data = realloc(i->data, (i->size + 1) * sizeof(IndexElem*));
    i->data[i->size++] = ie;
    return;
}



/*
Definição:
    Procedimento que remove um elemento de um indice    
Parâmetros:
    Index* i: ponteiro para a estrutura de indice.
    int pos: posição do elemento de indice que deve ser removido.
Retorno:
    IndexElem* - ponteiro para o elemento removido do indice.
*/
IndexElem* removeIndex(Index* index, int pos){
    if(pos >= index->size) return NULL;
    IndexElem* ret = index->data[pos];
    for(int i = pos; i < index->size - 1; i++) index->data[i] = index->data[i + 1];
    index->size--;
    if(!index->size){
        free(index->data);
        index->data = NULL;
    }
    else index->data = realloc(index->data, index->size * sizeof(IndexElem*));
    return ret;
}



/*
Definição:
    Metodo que realiza a busca binária no indice, buscando pelo primeiro elemento que possui a chave igual a chave buscada.
Parâmetros:
    Index* i: ponteiro para a estrutura de indice.
    char* key: chave da busca, no caso, nomeServidor (char*)
Retorno:
    IndexElem* - ponteiro para o elemento de indice criado.
*/
int binarySearchIndex(Index* index, char* key){
    if(index->size == 0) return -1;
    if(index->size == 1){
        if(!strcmp(index->data[0]->key, key)) 0;
        return -1;
    }
    int begin = 0;
    int middle = index->size / 2;
    int end = index->size;
    while(middle != begin){
        int compare = strcmp(index->data[middle]->key, key);
        if(!compare) return middle;
        if(compare < 0){
            begin = middle;
            middle = (middle + end) / 2;
            continue;
        }
        if(compare > 0){
            end = middle;
            middle = (begin + middle) / 2;
            continue;
        }
    }
    return -1;
}



/*
Definição:
    Procedimento que retorna o elemento que está em uma posição determinada do indice.
Parâmetros:
    Index* i: ponteiro para a estrutura de indice.
    int pos: posição do elemento que será recuperado do indice.
Retorno:
    IndexElem* - ponteiro para o elemento que está na posição indicada.
*/
IndexElem* getIndexElem(Index* i, int pos){
    return i->data[pos];
}



/*
Definição:
    Funcao que libera toda a memória alocada por um elemento do indice.    
Parâmetros:
    IndexElem* - ponteiro para o elemento que deverá ser excluido.
*/
void destroyIndexElem(IndexElem* ie){
    if(ie) free(ie);
    return;
}



/*
Definição:
    Função que libera toda a memória alocada pela estrutura indice.
Parâmetros:
    Index* index: ponteiro para a estrutura de indice que será excluida
*/
void destroyIndex(Index* index){
    if(!index) return;
    for(int i = 0; i < index->size; i++) destroyIndexElem(index->data[i]);
    if(index->data) free(index->data);
    free(index);
    return;
}



/*
Definição:
    Metodo para retornar o offset de um elemento do indice.
Parâmetros:
    IndexElem* ie: ponteiro para o elemento do indice.
Retorno:
    long long - offset do elemento do indice.
*/
long long getIndexElemOffset(IndexElem* ie){
    if(!ie) return -1;
    return ie->offset;
}



/*
Definição:
    Metodo para retornar a chave de um elemento do indice.
Parâmetros:
    IndexElem* ie: ponteiro para o elemento do indice.
Retorno:
    char* - string chave do elemento de indice (nomeServidor).
*/
char* getIndexElemKey(IndexElem* ie){
    if(!ie) return NULL;
    return ie->key;
}



/*
Definição:
    Função auxiliar criada para realizar a comparação de dois elementos de indice que será utilizada no merge sort.
Parâmetros:
    const void* a: primeiro valor a ser comparado. Valor definido como void* para generalizar a comparação e a ordenação do merge-sort
    const void* b:  segundo valor a ser comparado. Valor definido como void* para generalizar a comparação e a ordenação do merge-sort
Retorno:
    int - indica qual dos dois registros deve vir antes
            > 0 se b > a
            = 0 se b == a
            < 0 se b < a 
*/
int compare(const void* a, const void* b){
    IndexElem* va = *((IndexElem**)a);
    IndexElem* vb = *((IndexElem**)b);
    return strcmp(vb->key, va->key);
}


/*
Definição:
    Funcao que realiza a ordenação do indice, que utiliza o merge-sort como algoritmo.
Parâmetros:
    Index* i: ponteiro para a estrutura de indice.
*/
void sortIndex(Index* i){
    MS_sort(i->data, i->size, sizeof(IndexElem*), compare);
}


/*
Definição:
    Função que define o status do arquivo de indice para o especificado.
Parâmetros:
    char status: Valor para o status desejada para o Headerdo indice.
    FILE* fp: ponteiro para o arquivo de indice
*/
void setIndexHeaderStatus(char status, FILE* fp){
    long temp = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fwrite(&status, 1, 1, fp);
    fseek(fp, temp, SEEK_SET);
}



/*
Definição:
    Função que cria o arquivo de indice a partir do indice existente na heap.
Parâmetros:
    Index* index: ponteiro para a estrutura de indice que armazena os elementos que devem ser escritos no arquivo.
    FILE* fp: ponteiro para o arquivo de indece que deseja-se escrever.
*/
void makeIndexFile(Index* index, FILE* fp){
    fseek(fp, 0, SEEK_SET);
    char status = '0';
    fwrite(&status, 1, 1, fp);
    fwrite(&index->size, 4, 1, fp);
    char trash = '@';
    for(int i = 0; i < 31995; i++) fwrite(&trash, 1, 1, fp);
    int size = index->size;
    for(int i = 0; i < size; i++){
        fwrite(index->data[i]->key, 1, 120, fp);
        fwrite(&index->data[i]->offset, 8, 1, fp);
    }
}


/*
Definição:
    Função que carrega cada elemento do arquivo de indice e salva no Index da heap.
Parâmetros:
    Index* index: ponteiro para struct Index onde serão guradados os elementos lidos do arquivo.
    FILE* fp: ponteiro para o arquivo de onde serão carregadas as informações do indice.
Retorno:
    int - Quantidade de paginas de disco acessadas para fazer a leitura de todos os elementos do indice.
*/
int loadIndex(Index* index, FILE* fp){
    fseek(fp, 1, SEEK_SET);
    int nIndex;
    fread(&nIndex, 4, 1, fp);
    fseek(fp, 32000, SEEK_SET);
    char keyTemp[121];
    long long offsetTemp;
    for(int i = 0; i < nIndex; i++){
        fread(keyTemp, 1, 120, fp);
        fread(&offsetTemp, 8, 1, fp);
        addIndexElem(index, createIndexElem(keyTemp, offsetTemp));
    }
    return nIndex / 250 + 2;
}

/*
Definição:
    Função auxiliar usada para debug, simplesmente imprime tdos os elementos do indice na saida padrão.
Parâmetros:
    Index* i: ponteiro para o Index que dese-ja se printar.
*/
void printIndex(Index* i){
    int size = i->size;
    for(int j = 0; j < size; j++) printf("Position: %d, Key: %s, Offset: %lld\n", j, i->data[j]->key, i->data[j]->offset);
}
