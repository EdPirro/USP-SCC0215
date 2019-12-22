#include "list.h"
#include <stdlib.h>

//Declaração da struct _node que sera o membro de cada lista.
struct _node{
    long long offset; // distancia a partir do inicio do arquivo ate o registro logicamente removido.
    int size; // espaço disponivel no offset indicado.
    int priority; // prioridade da remoção.
    struct _node* next; // proximo elemento da lista.
};

//Declaração da struct _list que representa a lista.
struct _list{
    Node* begin; // primeira posição da lista.
    Node* end; // ultima posição da lista.
};

/*
Definição:
    Função simples que aloca espaço e inicia as variaveis de uma Lista com NULL.
Retorno:
    List* - ponteiro para a lista criada.
*/
List* createList(){
    List* l = malloc(sizeof(List));
    l->begin = NULL;
    l->end = NULL;
    return l;
}

/*
Definição:
    Getter do atributo size de um No (necessario por causa da modularização e "encapsulamento" da struct _node).
Parâmetros:
    Node* n: ponteiro para o Node cujo atributo size deseja-se saber.
Retorno:
    int - valor no campo size do Node*.
*/
int getNodeSize(Node* n){
    if(!n) return -1;
    return n->size;
}

/*
Definição:
    Getter do atributo offset de um No (necessario por causa da modularização e "encapsulamento" da struct _node).
Parâmetros:
    Node* n: ponteiro para o Node cujo atributo offset deseja-se saber.
Retorno:
    long long - valor no campo offset do Node*.
*/
long long getNodeOffset(Node* n){
    if(!n) return -1;
    return n->offset;
}

/*
Definição:
    Função simples que aloca espaço e inicia as variaveis de um No com os valores definidos como argumento.
Parâmetros:
    long long offset: valor desejado para ser o offset do no.
    int size: valor desejado para ser o tamanho do no.
    int priority: valor desejado para ser a prioridade do no.
Retorno:
    List* - ponteiro para a lista criada.
*/
Node* createNode(long long offset, int size, int priority){
    Node* n = malloc(sizeof(Node));
    n->priority = priority;
    n->offset = offset;
    n->size = size;
    n->next = NULL;
    return n;
}

/*
Definição:
    Função Basica que destroi um Node.
Parâmetros:
    Node* n: ponteiro para o Node que deseja-se destruir.
*/
void destroyNode(Node* n){
    if(n) free(n);
    return;
}

/*
Definição:
    Função Basica que destroi um List.
Parâmetros:
    List* l: ponteiro para o List que deseja-se destruir.
*/
void destroyList(List* l){
    if(!l) return;
    Node* curr = l->begin;
    while(curr){
        Node* aux = curr->next;
        destroyNode(curr);
        curr = aux;
    }
    free(l);
    l = NULL;
    return;
}


/*
Definição:
    Função simples que insere um Node no final de um List.
Parâmetros:
    List* l: List no qual deseja-se inserir um elemento.
    Node* n: elemento que deseja-se inserir na lista.
*/
void pushBack(List* l, Node* n){
    if(!l) return;
    if(!l->begin) l->begin = n;
    else l->end->next = n;
    l->end = n;
    return;
}

/*
Definição:
    Função simples que insere um Node no começo de um List.
Parâmetros:
    List* l: List no qual deseja-se inserir um elemento.
    Node* n: elemento que deseja-se inserir na lista.
*/
void pushFront(List* l, Node* n){
    if(!l) return;
    if(!l->end) l->end = n;
    else n->next = l->begin;
    l->begin = n;
    return;
}

/*
Definição:
    Procedimento que itera sobre a lista ate encontrar o primeiro elemento que tenha tamanho maior que o especificado, ou seja o first-fit.
Parâmetros:
    List* l: List do qual deseja-se retirar o first-fit.
    int size: tamanho minimo que deve ser suportado pelo Node.
Retorno:
    Node* - o no que representa o first-fit buscado, retornando NULL caso ele "não exista".
*/
Node* getFirstFit(List* l, int size){
    if(!l) return NULL; // se nao existe lista retorna NULL.
    Node* curr = l->begin;
    Node* last = NULL;
    while(curr){
        if(size <= curr->size){ // considerando que esta ordenada a lista, assim que encontrar um node com tamanho suficiente ele eh o best fit.
            if(!last) l->begin = curr->next; // se for o primeiro muda o inicio da lista.
            else last->next = curr->next; // se não muda o anterior a ele.
            if(l->end == curr) l->end = last; // se for o ultimo, end eh atualizado para o anterior.
            return curr; // retorna o node
        }
        last = curr; // passo
        curr = curr->next;
    }
    return NULL; // se nenhum node tiver tamanho suficiente entao retorna NULL.
}

/*
Definição:
    Função simples que limpa logicamente uma lista.
Parâmetros:
    List* l: List que deseja-se limpar.
*/
void clearList(List* l){
    l->begin = NULL;
    l->end = NULL;
    return;
}


/*
Definição:
    Algoritmo de ordenação estavel que usa de um vetor auxiliar para ordenar os elementos da lista crescentemente de acordo com o tamanho desponivel no espaço de memoria
    representado por cada Node da lista.
    (Complexiadade O(n + ALPHA), sendo n o numero de elementos da lista e ALPHA a amplitude dos dados, que seria o tamanho do vetor auxiliar).
Parâmetros:
    List* l: lista que deseja-se ordenar.
    int min: menor valor de tamanho contido na lista (Otimiza o uso de espaço).
    int max: maior valor de tamanho contido na lista (Otimiza o uso de espaço).
Retorno:
    List* - um novo ponteiro para uma lista agora ordenada. 
    OBS: a lista passada como argumento eh destruida no algoritmo, portanto a lista retorna deve ser utilizada.
*/
List* bucketSort(List* l, int min, int max){
    if(!l) return l; // se nao existir lista retorna.
    Node* curr = l->begin;
    if(!curr) return l; //  se estiver vazia retorna.
    List** bucket = calloc((max - min + 1), sizeof(List*)); // cira um vetor de Lists com max-min+1 elementos para inserir os elementos de acordo com seus tamanhos.
    
    
    while(curr){ // itera sobre a lista.
        int size = (curr->size - min); // define o size, que sera a posição no bucket que ele assumira.
        if(bucket[size]){
            pushBack(bucket[size], createNode(curr->offset, curr->size, curr->priority)); // se na posiçao desejada ja exite outro elemento ele eh inserido atras.
        }
        else{ // caso nao exista entao a lista eh criada e esse elemento inserido.
            bucket[size] = createList();
            pushBack(bucket[size], createNode(curr->offset, curr->size, curr->priority)); 
        }
        curr = curr->next;
    }

    destroyList(l);
    l = createList();
    
    
    for(int i = 0; i < (max - min + 1); i++){ // itera agora sobre o bucket 
    //como esse iteração eh feita de maneira crescente no vetor auxiliar, garante-se que os elementos vao sair em ordem crescente de tamanho disponivel.
        if(bucket[i]){ // se algum elemento estiver na posição do bucket eles sao removidos na ordem que entraram e inseridos na lista
            for(curr = bucket[i]->begin; curr; curr = curr->next) pushBack(l, createNode(curr->offset, curr->size, curr->priority));
            destroyList(bucket[i]);
        }
    }
    free(bucket);
    return l;
}


/*
Definição:
    Algoritmo de ordenação estavel que usa de um vetor auxiliar para ordenar os elementos da lista decrescentemente de acordo com a prioridade de remoção de cada Node.
    (Complexiadade O(n + ALPHA), sendo n o numero de elementos da lista e ALPHA a amplitude dos dados, que seria o tamanho do vetor auxiliar).
Parâmetros:
    List* l: lista que deseja-se ordenar.
    int min: menor valor de prioridade contido na lista (Otimiza o uso de espaço).
    int max: maior valor de prioridade contido na lista (Otimiza o uso de espaço).
Retorno:
    List* - um novo ponteiro para uma lista agora ordenada. 
    OBS: a lista passada como argumento eh destruida no algoritmo, portanto a lista retorna deve ser utilizada.
*/
List* sortByPriority(List* l, int min, int max){
    if(!l) return l; // se nao existir lista retorna.
    Node* curr = l->begin;
    if(!curr) return l; //  se estiver vazia retorna.
    List** bucket = calloc((max - min + 1), sizeof(List*)); // cira um vetor de Lists com max-min+1 elementos para inserir os elementos de acordo com suas prioridades.
    
    
    while(curr){ // itera sobre a lista.
        int prty = (curr->priority - min); // define prty, que sera a posição no bucket que ele assumira.
        if(bucket[prty]){
            pushBack(bucket[prty], createNode(curr->offset, curr->size, curr->priority)); // se na posiçao desejada ja exite outro elemento ele eh inserido atras.
        }
        else{ // caso nao exista entao a lista eh criada e esse elemento inserido.
            bucket[prty] = createList();
            pushBack(bucket[prty], createNode(curr->offset, curr->size, curr->priority)); 
        }
        curr = curr->next;
    }

    destroyList(l);
    l = createList();
    
    
    for(int i = (max - min); i >= 0; i--){ // itera agora sobre o bucket 
    //como esse iteração eh feita de maneira decrescente no vetor auxiliar, garante-se que os elementos vao sair em ordem decrescente de prioridade.
        if(bucket[i]){ // se algum elemento estiver na posição do bucket eles sao removidos na ordem que entraram e inseridos na lista
            for(curr = bucket[i]->begin; curr; curr = curr->next) pushBack(l, createNode(curr->offset, curr->size, curr->priority));
            destroyList(bucket[i]);
        }
    }
    free(bucket);
    return l;
}



/*
Definição:
    Função simples que remove o primeiro elemento da lista.
Paraâmetros:
    List* l: lista da qual deseja-se remover oprimeiro elemento.
Retorno:
    Node* - ponteiro para o Node removido pela função.
*/
Node* popFirst(List* l){
    if(!l) return NULL;
    Node* ret = l->begin;
    if(!ret) return NULL;
    l->begin = ret->next;
    if(l->end == ret) l->end = NULL;
    return ret;
}


// Funções Usadas apenas para debug::
/*
#include <stdio.h>

Definição:
    Função que itera sobre todos os elementos da lista printando suas informaçoes.
Parâmetros:
    List* l: lista cujos elementos deseja-se printar.
void printList(List* l){
    int i = 1;
    for(Node* curr = l->begin; curr; curr = curr->next){
        printf("Posição: %d, Tamanho: %d, Offset: %lld, Prioridade: %d\n", i, curr->size, curr->offset, curr->priority);
        i++;
    }
    return;
}


Definição:
    Função que printa o tamanho e o offset de um Node.
Parâmetros:
    Node* n: Node cujas informaçoes deseja-se printar.
void printNode(Node* n){
    printf("Tamanho: %d\nOffset: %lld\n", n->size, n->offset);
    return;
}

*/
