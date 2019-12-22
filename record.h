/*
Arquivo que contem as definições de metodos e atributos referentes aos registros, incluindo Header(registro de cabeçalho) e Record(registro normal de dados).
*/

#include "cluster.h"
#include "list.h"
#include <stdio.h>

#ifndef RECORD_H
#define RECORD_H

/*
Declaração da struct "Header" que representa o cabeçalho com seus respectivos atributos.
*/
typedef struct _regHead{ // "214 bytes"
    char status;        // 1byte
    long long topoLista;// 8bytes
    char tagCampo[5];      // 5 bytes
    char desCampo[5][40]; // 200bytes
}Header;


/*
Declaração da struct "Record" que representa o registro com seus respectivos atributos.
*/
typedef struct _reg{ // 34 + nS + cS + tagCampo * 2 + tamCampo * 2
    int idServidor;                // 4 bytes
    double salarioServidor;        // 8 bytes
    char* telefoneServidor;        // 14 bytes
    char* nomeServidor;            // nS bytes
    char* cargoServidor;           // cS bytes
    int tamanhoRegistro;          // 4 bytes  não entra no tamanho.     

    //Para implementaçoes futuras
    char removido;                // 1 byte não entra no tamanho.
    long long encadeamentoLista;  // 8 bytes

}Record;


//Methods
char* getString(FILE* fp, int* stringSize);
void printNullFixString(int size);
Header* createHeader();
void getHeader(Header* h, FILE* csv);
void loadHeaderNC(Header* h, FILE* fp);
void saveHeader(const char* filePath, Header* h, Cluster* c);
void saveHeaderNC(Header* h, FILE* fp);
void destroyHeader(Header* h);
void loadHeader(Header* h, Cluster* c);
Record* createRecord();
int getRecord(FILE* csv, Record* r);
int getActualSize(Record* r);
void destroyRecord(Record* r);
void writeRecord(Cluster* c, Record* r, Header* h);
void reWriteRecord(Cluster* c, Record* r, int offset, Header* h);
int loadRecord(Record* r, Cluster* c);
int loadRecordNC(Record* r, FILE* fp);
void printRecord(Record* r);
void printSearched(Record* r, Header* h);
int isSearchedRecord(Record* r, int op, void* value);
int printIfSearchedRecord(Record* r, int op, void* value, Header* h);
void fillWithTrash(FILE* fp, long long begin, long long end, char trash);
void makeRemovedList(List* l, FILE* fp, long long topoLista, int* mn, int* mx);
void makeFileRemovedList(List* l, FILE* fp, Header* h);
void removeRecordInPos(Record* r, FILE* fp, List* l, int* min, int* max, int priority);
void markRemoved(Record* r, long long offset, FILE* fp);
long long saveNewRecord(Record* r, List* l, FILE* fp, Header* h);
void saveNewRecordInOffset(Record* r, long long offset, FILE* fp, Header* h);
void updateRecordInPos(Record* r, int updateTag, void* updateValue, FILE* fp, Header* h, List** l, int* min, int* max);


#endif // RECORD_H