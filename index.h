/*
Arquivo que contem as definições de metodos e atributos referentes aos indices.
*/

#include <stdio.h>

typedef struct _idxelem IndexElem;
typedef struct _indx Index;


//Methods
IndexElem* createIndexElem(char* key, long long offset);
Index* createIndex();
void addIndexElem(Index* i, IndexElem* ie);
IndexElem* removeIndex(Index* index, int pos);
int binarySearchIndex(Index* index, char* key);
void destroyIndexElem(IndexElem* ie);
void destroyIndex(Index* index);
long long getIndexElemOffset(IndexElem* ie);
char* getIndexElemKey(IndexElem* ie);
void sortIndex(Index* i);
void setIndexHeaderStatus(char status, FILE* fp);
void makeIndexFile(Index* index, FILE* fp);
int loadIndex(Index* index, FILE* fp);
void printIndex(Index* i);
IndexElem* getIndexElem(Index* i, int pos);