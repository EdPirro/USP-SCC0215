/*
Arquivo que contem as definições de Metodos e Atributos referentes a uma Lista.
*/

#ifndef LIST_H
#define LIST_H



// Definição da strcut _node como tipo Node.
typedef struct _node Node;

// Definição da strcut _list como tipo List.
typedef struct _list List;

//Methods
int getNodeSize(Node* n);
long long getNodeOffset(Node* n);
List* createList();
Node* createNode(long long offset, int size, int priority);
void destroyNode(Node* n);
void destroyList(List* l);
void pushBack(List* l, Node* n);
void pushFront(List* l, Node* n);
Node* popFirst(List* l);
Node* getFirstFit(List* l, int size);
void clearList(List* l);
List* bucketSort(List* l, int min, int max);
List* sortByPriority(List* l, int min, int max);

/* DEBUG
void printList(List* l);
void printNode(Node* n);
*/





#endif // LIST_H