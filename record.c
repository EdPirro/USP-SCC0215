#include "record.h"
#include <stdlib.h>
#include <string.h>


/*
Definição:
    Função definida para ler uma string de um arquivo a partir da posição atual ate encontrar um dos seguintes characteres: ',', '\r', '\0'.
    Feita para garantir a leitura correta do arquivo csv.
Parâmetros:
    FILE* fp: ponteiro para o arquivo do qual deseja-se ler.
    int* stringSize: ponteiro para um inteiro no qual sera armazenado o tamanho da string lida. (contando com o '\0').
Retorno:
    char*: a string lida, caso a string lida esteja vazia, ou seja, size = 0 ele retorna NULL.
*/
char* getString(FILE* fp, int* stringSize){
    char* string = malloc(500);
    int size = 0;
    char c;
    while(fscanf(fp, "%c", &c) != EOF){
        if(c == ',' || c == '\n' || c == '\r') break;
        else{
            string[size++] = c;
        }
    }
    if(!size){
        *stringSize = 0;
        free(string);
        return NULL;
    }
    if(string[size - 1] != '\0') string[size++] = '\0';
    *stringSize = size;
    return string;
}

/*
Definição:
    Função criada para facilitar a apresentação dos dados, ela printa uma quantidade definida de espaços para representar um campo nulo de tamanho fixo.
Parâmetro:
    int size: quantidade de espaçoes que devem ser printados.
*/
void printNullFixString(int size){
    for(int i = 0; i < size; i++) printf(" ");
    return;
}

/*
Definição:
    Função que aloca espaço para um Header, ja inicializando seus parâmetros com os valores desejados.
Retorno:
    Header*: ponteiro para o Header criado.
*/
Header* createHeader(){
    Header* h = malloc(sizeof(Header));
    h->status = 0; //inicia status com 0 para saber que ainda não esta estavel (sera mudado na hora de salva-lo apenas).
    h->topoLista = -1; // Para esse trabalho topoLista sempre = -1.
    h->tagCampo[0] = 'i';
    h->tagCampo[1] = 's';
    h->tagCampo[2] = 't';
    h->tagCampo[3] = 'n';
    h->tagCampo[4] = 'c';
    for(int i = 0; i < 5; i++) strncpy(h->desCampo[i], "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", 40);
    return h;
}

/*
Definição:
    Função criada para ler as descrições dos campos apartir de um arquivo e armazená-las no Header, a função usa de outra função 
    ja definida: "getString()", para leitura dos dados.
Parâmetros:
    Header* h: ponteiro para o Header no qual deseja-se salvar as informações.
    FILE* csv: ponteiro para um arquivo a partir do qual serão lidas as informações necessárias.
*/
void getHeader(Header* h, FILE* csv){
    char* aux;
    int size;
    for(int i = 0; i < 5; i++){
        aux = getString(csv, &size);
        strncpy(h->desCampo[i], aux, size);
        free(aux);
    }
    return;
}

/*
Definição:
    Função ciada para salvar o Header diretemente, ou seja, escrevendo-o no Cluster e ja salvando o Cluster no arquivo especificado, a função primeiramente
    salva cada um dos campos do Header no Cluster e então usa a função: "saveClusterHead()" para salva-lo no arquivo corretamente.
Parâmetros:
    const char* filePath: caminho para o arquivo no qual deseja-se salvar o Header.
    Header* h: ponteiro para o Header que deseja-se salvar.
    Clusrter* c: ponteiro para o Cluster que será usado para salvar o Header.
*/
void saveHeader(const char* filePath, Header* h, Cluster* c){
    h->status = '1';                                              //Muda status pra 1 antes de salvar Garantindo consistencia dos dados.
    memmove(&(c->data[c->curPos]), &h->status, 1);
    c->curPos ++;
    memmove(&(c->data[c->curPos]), &h->topoLista, 8);
    c->curPos += 8;
    memmove(&(c->data[c->curPos]), &h->tagCampo[0], 1);
    c->curPos ++;
    memmove(&(c->data[c->curPos]), &h->desCampo[0][0], 40);
    c->curPos += 40;
    memmove(&(c->data[c->curPos]), &h->tagCampo[1], 1);
    c->curPos ++;
    memmove(&(c->data[c->curPos]), &h->desCampo[1][0], 40);
    c->curPos += 40;
    memmove(&(c->data[c->curPos]), &h->tagCampo[2], 1);
    c->curPos ++;
    memmove(&(c->data[c->curPos]), &h->desCampo[2][0], 40);
    c->curPos += 40;
    memmove(&(c->data[c->curPos]), &h->tagCampo[3], 1);
    c->curPos ++;
    memmove(&(c->data[c->curPos]), &h->desCampo[3][0], 40);
    c->curPos += 40;
    memmove(&(c->data[c->curPos]), &h->tagCampo[4], 1);
    c->curPos ++;
    memmove(&(c->data[c->curPos]), &h->desCampo[4][0], 40);
    c->curPos += 40;

    fillCluster(c, '@');
    saveClusterHead(filePath, c);
    return;
}

/*
Definição:
    Função que salva os elementos de um Header diretamente no arquivo sem o uso de Clusters.
Parâmetros:
    Header* h: ponteiro do Header que contem as informações que deseja-se salvar.
    FILE* fp: ponteiro para o arquivo no qual deseja-se escrever.
*/
void saveHeaderNC(Header* h, FILE* fp){
    fseek(fp, 0, SEEK_SET);
    fwrite(&h->status, 1, 1, fp);
    fwrite(&h->topoLista, 8, 1, fp);
    fwrite(&h->tagCampo[0], 1, 1, fp);
    fwrite(&h->desCampo[0][0], 1, 40, fp);
    fwrite(&h->tagCampo[1], 1, 1, fp);
    fwrite(&h->desCampo[1][0], 1, 40, fp);
    fwrite(&h->tagCampo[2], 1, 1, fp);
    fwrite(&h->desCampo[2][0], 1, 40, fp);
    fwrite(&h->tagCampo[3], 1, 1, fp);
    fwrite(&h->desCampo[3][0], 1, 40, fp);
    fwrite(&h->tagCampo[4], 1, 1, fp);
    fwrite(&h->desCampo[4][0], 1, 40, fp);
    return;
}

/*
Definição:
    Função básica que libera o espaço na memoria alocado para o Header.
Parâmetros:
    Header* h: ponteiro para o Header cujo espaço alocado deseja-se desalocar.
*/
void destroyHeader(Header* h){
    if(h) free(h);
    return;
}

/*
Definição:
    Função que le um Header a partir de um cluster e salva suas informações.
Parâmetros:
    Header* h: Header onde as informações serão salvas.
    Cluster* c: Cluster de onde as informações serão lidas.
*/
void loadHeader(Header* h,Cluster* c){
    memmove(&h->status, &(c->data[c->curPos]), 1);
    c->curPos++;
    memmove(&h->topoLista, &(c->data[c->curPos]), 8);
    c->curPos += 8;
    memmove(&h->tagCampo[0], &(c->data[c->curPos]), 1);
    c->curPos++;
    memmove(&h->desCampo[0][0], &(c->data[c->curPos]), 40);
    c->curPos += 40;
    memmove(&h->tagCampo[1], &(c->data[c->curPos]), 1);
    c->curPos++;
    memmove(&h->desCampo[1][0], &(c->data[c->curPos]), 40);
    c->curPos += 40;
    memmove(&h->tagCampo[2], &(c->data[c->curPos]), 1);
    c->curPos++;
    memmove(&h->desCampo[2][0], &(c->data[c->curPos]), 40);
    c->curPos += 40;
    memmove(&h->tagCampo[3], &(c->data[c->curPos]), 1);
    c->curPos++;
    memmove(&h->desCampo[3][0], &(c->data[c->curPos]), 40);
    c->curPos += 40;
    memmove(&h->tagCampo[4], &(c->data[c->curPos]), 1);
    c->curPos++;
    memmove(&h->desCampo[4][0], &(c->data[c->curPos]), 40);
    c->curPos += 40;
    return;
}

/*
Definição:
    Função que carrega os elementos de um Header diretamente no arquivo sem o uso de Clusters.
Parâmetros:
    Header* h: ponteiro do Header onde deseja-se salvar as informações.
    FILE* fp: ponteiro para o arquivo do qual deseja-se ler as informações.
*/
void loadHeaderNC(Header* h, FILE* fp){
    fseek(fp, 0, SEEK_SET);
    fread(&h->status, 1, 1,  fp);
    fread(&h->topoLista, 8,  1, fp);
    fread(&h->tagCampo[0], 1,  1, fp);
    fread(&h->desCampo[0][0], 1,  40, fp);
    fread(&h->tagCampo[1], 1,  1, fp);
    fread(&h->desCampo[1][0], 1,  40, fp);
    fread(&h->tagCampo[2], 1,  1, fp);
    fread(&h->desCampo[2][0], 1,  40, fp);
    fread(&h->tagCampo[3], 1,  1, fp);
    fread(&h->desCampo[3][0], 1,  40, fp);
    fread(&h->tagCampo[4], 1,  1, fp);
    fread(&h->desCampo[4][0], 1,  40, fp);
    return;
}

/*
Definição:
    Função usada para criar um Record, alocando espaço necessario e inicializando seus ponteiros como NULL.
Retorno:
    Record*: Record criado.
*/
Record* createRecord(){
    Record* r = malloc(sizeof(Record));
    r->removido = '-';
    r->telefoneServidor = NULL;
    r->cargoServidor = NULL;
    r->nomeServidor = NULL;
    return r;
}

/*
Definição:
    Função que le, a partir de um arquivo especificado, as informaçoes necessarias para criar o Record, quando a leitura retornar EOF, então a funçao tambem o fará.
    OBS: quando os campos de tamanho variavel nomeServidor e cargoServidor não tiverem valores declarados no csv a função "getString" ja os atribui valor NULL.
Parâmetros:
    FILE* csv: ponteiro para o arquivo, a partir do qual serão lidas as informações necssarias.
    Record* r: ponteiro para o record no qual serão armazenadas as informações.
Retorno:
    int: inteiro que representa se foi possivel realizar a leitura, neste caso retorna !EOF, ou se o final do arquivo foi atingido, retornando EOF.

*/
int getRecord(FILE* csv, Record* r){
    if(fscanf(csv, "%d,%lf,", &r->idServidor, &r->salarioServidor) != EOF){
        r->tamanhoRegistro = 34;
        int size;
        r->telefoneServidor = getString(csv, &size);
        r->nomeServidor = getString(csv, &size);
        if(r->nomeServidor) r->tamanhoRegistro += 5;        
        r->tamanhoRegistro += size;
        r->cargoServidor = getString(csv, &size);
        if(r->cargoServidor) r->tamanhoRegistro += 5; 
        r->tamanhoRegistro += size;
        r->removido = '-';
        r->encadeamentoLista = -1;
        return !EOF;
    }
    r->telefoneServidor = NULL;
    r->nomeServidor = NULL;
    r->cargoServidor = NULL;
    return EOF;
}

/*
Definição:
    Função simples que desaloca a memoria atribuida para o Record.
Parâmetros:
    Record* r: ponteiro para o Record que deseja-se destruir.
*/
void destroyRecord(Record* r){
    if(!r) return;
    if(r->telefoneServidor) free(r->telefoneServidor);
    if(r->nomeServidor) free(r->nomeServidor);
    if(r->cargoServidor) free(r->cargoServidor);
    free(r);
    return;
}

/*
Definição:
    Função desenvolvida para escrever as informações de um Record em um Cluster, usando do Header para saber a tag dos Campos variaveis e fazendo todas as checagens 
    para saber o que deve ou não ser salvo e de que forma.
Parâmetros:
    Cluster* c: ponteiro para o Cluster no qual deseja-se escrever.
    Record* r: ponteiro para o record cujas informações deseja-se escever no Cluster.
    Headrer* h: ponteiro para o Header que contem as informações da tag dos campos.
*/
void writeRecord(Cluster* c, Record* r, Header* h){
    memmove(&(c->data[c->curPos]), &r->removido, 1);
    c->curPos++;

    memmove(&(c->data[c->curPos]), &r->tamanhoRegistro, 4);
    c->curPos += 4;

    memmove(&(c->data[c->curPos]), &r->encadeamentoLista, 8);
    c->curPos += 8;

    memmove(&(c->data[c->curPos]), &r->idServidor, 4);
    c->curPos += 4;

    memmove(&(c->data[c->curPos]), &r->salarioServidor, 8);
    c->curPos += 8;

    //Verifica se telefoneServidor não eh nulo.
    if(r->telefoneServidor){
        //Se não for nulo então ele eh salvo no Cluster.
        memmove(&(c->data[c->curPos]), r->telefoneServidor, 14);
        c->curPos += 14;
    }else{
        //Se for nulo então a string "\0@@@@@@@@@@@@@" eh salva no Cluster.
        char aux[14];
        strncpy(aux, "@@@@@@@@@@@@@@", 14);
        aux[0] = '\0';
        memmove(&(c->data[c->curPos]), aux, 14);
        c->curPos += 14;
    }

    int size;

    //Verifica se nomeServidor eh não-nulo, caso não seja, nada dele eh salvo.
    if(r->nomeServidor){
        //Caso seja, ele, seu tamanho e sua tag são salvas.
        size = strlen(r->nomeServidor) + 2;
        memmove(&(c->data[c->curPos]), &size, 4);
        c->curPos += 4;
        memmove(&(c->data[c->curPos]), &h->tagCampo[3], 1);
        c->curPos++;
        size--;
        memmove(&(c->data[c->curPos]), r->nomeServidor, size);
        c->curPos += size;
    }

    //Verifica se cargoServidor eh não-nulo, caso não seja, nada dele eh salvo.
    if(r->cargoServidor){
        //Caso seja, ele, seu tamanho e sua tag são salvas.
        size = strlen(r->cargoServidor) + 2;
        memmove(&(c->data[c->curPos]), &size, 4);
        c->curPos += 4;
        memmove(&(c->data[c->curPos]), &h->tagCampo[4], 1);
        c->curPos++;
        size--;
        memmove(&(c->data[c->curPos]), r->cargoServidor, size);
        c->curPos += size;
    }

    return;  
}



/*
Definição:
    Função simples criada para reescrever o Cluster, acessando uma posição que esta (offset+5) atras da atual, criada para modificar o 
    tamanho do ultimo Record de cada Cluster com a quantidade de lixo usada para preencher a pagina antes de salvar.
Parâmetros:
    Cluster* c: ponteiro para o Cluster que deseja-se modificar.
    Record* r: Record que deseja-se escrever na posição especificada do Cluster.
    int offset: distancia a ser recuada da posição atual do Cluster para alcançar a posição desejada.
    Header* h: Header que contem as informações da tag dos campos que sera usado na hora de re-escrever o Record.
*/
void reWriteRecord(Cluster* c, Record* r, int offset, Header* h){
    c->curPos -= (offset + 5);
    writeRecord(c, r, h);
    return;
}


/*
Definição:
    Função simples que marca um regitro como removido logicamente diretamente no arquivo.
Parâmetros:
    Record* r: ponteiro do Record que deseja-se marcar como removido.
    long long offset: posição do registro.
    FILE* fp: ponteiro para o arquivo no qual serão escritas as modificações.
*/
void markRemoved(Record* r, long long offset, FILE* fp){
    long currOffset = ftell(fp);
    fseek(fp, offset, SEEK_SET);
    fwrite(&r->removido, 1, 1, fp);
    fseek(fp, currOffset, SEEK_SET);
    return;
}


/*
Definição:
    Função que carrega um registro a partir de um Cluster, fazendo todas as verificações necessarias para atribuir corretamente campos nulos e vazios.
Parâmetros:
    Record* r: Record no qual serão armazenadas as informações lidas.
    Cluster* c: Cluster do qual serão lidas as informaçoes.
Retorno:
    int com função de booleno, retorna 0 caso não seja carregado corretamente o Record, ou seja removido, e retorna 1, caso seja corretamente carregado.
*/
int loadRecord(Record* r, Cluster* c){
    //cargoServidor e nomeServidor serão com certeza realocados, entao logo de inicio se existirem são desalocados evitando vazamento de memoria.
    if(r->cargoServidor){
        free(r->cargoServidor);
        r->cargoServidor = NULL;
    }
    if(r->nomeServidor){
        free(r->nomeServidor);
        r->nomeServidor = NULL;
    }

    memmove(&r->removido, &(c->data[c->curPos]),  1);
    c->curPos++;

    if(r->removido != '-' && r->removido != '*') return 0; // Verifica se foi removido ou se esta ledo algo indevidamente.

    memmove(&r->tamanhoRegistro, &(c->data[c->curPos]), 4);
    c->curPos += 4;

    int startPos = c->curPos;
    memmove(&r->encadeamentoLista, &(c->data[c->curPos]), 8);
    c->curPos += 8;
    memmove(&r->idServidor,&(c->data[c->curPos]), 4);
    c->curPos += 4;
    memmove(&r->salarioServidor,&(c->data[c->curPos]), 8);
    c->curPos += 8;

    if(!r->telefoneServidor) r->telefoneServidor = malloc(15);
    memmove(r->telefoneServidor, &(c->data[c->curPos]), 14);
    r->telefoneServidor[14] = '\0';
    c->curPos += 14;

    
    //Verifica se ja esta no final do registro, ou seja, ambos os campos variaveis sao nulos.
    if(c->curPos >= startPos + r->tamanhoRegistro){
        //Caso seja então atribui a ambos os valores nulos e retorna.
        r->cargoServidor = NULL;
        r->nomeServidor = NULL;
        c->curPos = startPos + r->tamanhoRegistro;
        return 1;
    }
    //So passa daqui se estiver no lugar certo ou for o ultimo record do cluster.

    int auxSize;
    char auxTag;

    memmove(&auxSize, &(c->data[c->curPos]), 4);
    c->curPos += 4;
    memmove(&auxTag, &(c->data[c->curPos]), 1);
    c->curPos++; 
    auxSize--;


    //Caso seja o ultimo do cluster e nao tenha nenhum dos campos lera lixo como tag, entao atribui null nas strings e retorna.
    if(auxTag == '@'){ 
        r->cargoServidor = NULL;
        r->nomeServidor = NULL;
        c->curPos = startPos + r->tamanhoRegistro;
        return 1;
    }

    //Caso leia um c entao tem apenas o ultimo campo, atribui null pro nome pega o segundo campo e retorna (ultimo registro funciona).
    if(auxTag == 'c'){
        r->nomeServidor = NULL;
        r->cargoServidor = malloc(auxSize);
        memmove(r->cargoServidor, &(c->data[c->curPos]), auxSize);
        c->curPos += auxSize;
        c->curPos = startPos + r->tamanhoRegistro;
        return 1;
    }
    r->nomeServidor = malloc(auxSize);
    memmove(r->nomeServidor, &(c->data[c->curPos]), auxSize);
    c->curPos += auxSize;

    //Verifica se o registro ja esta no final, ou seja não tem o ultimo campo.
    if(c->curPos == startPos + r->tamanhoRegistro){
        //Caso esteja, então no ultimo campo eh atribuido null e retorna.
        r->cargoServidor = NULL;
        c->curPos = startPos + r->tamanhoRegistro;
        return 1;
    }
    //So passa daqui caso tenha o quinto campo ou seja o ultimo do cluser.

    memmove(&auxSize, &(c->data[c->curPos]), 4);
    c->curPos += 4;
    memmove(&auxTag, &(c->data[c->curPos]), 1);
    c->curPos++;
    auxSize--;

    //Caso seja o ultimo registro do cluster e nao tenha apenas o ultimo campo lera lixo como essa tag, atribui entao valor null e retorna.
    if(auxTag == '@'){
        r->cargoServidor = NULL;
        c->curPos = startPos + r->tamanhoRegistro;
        return 1;
    }

    r->cargoServidor = malloc(auxSize);
    memmove(r->cargoServidor, &(c->data[c->curPos]), auxSize);
    c->curPos += auxSize;
    c->curPos = startPos + r->tamanhoRegistro;
    return 1;
}

/*
Definição:
    Função que carrega um Record a partir de um arquivo diretamente sem o uso de Clusters ja fazendo as verificações necessarias para atribuir os valores corretos para
    cada campo do registro.
Parâmetros:
    Record* r: ponteiro para o registro no qual as informações serão salvas.
    FILE* fp: ponteiro para o arquivo do qual serão lidas as informações.
Retorno:
    int - retorna EOF quando chega-se no fum de um arquivo e !EOF quando executa corretamente.
*/
int loadRecordNC(Record* r, FILE* fp){
    if(r->cargoServidor){
        free(r->cargoServidor);
        r->cargoServidor = NULL;
    }
    if(r->nomeServidor){
        free(r->nomeServidor);
        r->nomeServidor = NULL;
    }

    if(fread(&r->removido, 1, 1, fp) == EOF) return EOF;

    if(r->removido != '-' && r->removido !='*') return EOF;

    fread(&r->tamanhoRegistro, 4, 1, fp);
    long long curr = ftell(fp);
    fread(&r->encadeamentoLista, 8, 1, fp);
    fread(&r->idServidor, 4, 1, fp);
    fread(&r->salarioServidor, 8, 1, fp);
    if(!r->telefoneServidor) r->telefoneServidor = malloc(15);
    fread(r->telefoneServidor, 1, 14, fp);
    r->telefoneServidor[14] = '\0';

    // se apos essas leituras estiver no final do arquivo então não tem campos variaveis e retorna.
    if(ftell(fp) == (curr + r->tamanhoRegistro)) return !EOF;

    int auxSize;
    char auxTag;
    fread(&auxSize, 4, 1, fp);
    fread(&auxTag, 1, 1, fp);
    auxSize--;

    //Senão o auxTag eh lido e verifica-se sua igualdade com 'c' e 'n'.
    if(auxTag == 'c'){
        //Caso seja 'c' isso significa que tem apenas o cargo servidor então o nome eh mantido como NULL, cargo eh lido e a função finaliza.
        r->cargoServidor = malloc(auxSize);
        fread(r->cargoServidor, 1, auxSize, fp);
        fseek(fp, curr + r->tamanhoRegistro, SEEK_SET);
        return !EOF;
    }
    else if(auxTag == 'n'){
        //Caso seja 'n' o nome eh lido e a função continua.
        r->nomeServidor = malloc(auxSize);
        fread(r->nomeServidor, 1, auxSize, fp);
    }
    else{
        //Caso não seja nenhum dos dois, provavelmente eh o ultimo de um Cluster e a função retorna.
        fseek(fp, curr + r->tamanhoRegistro, SEEK_SET);
        return !EOF;
    }

    // se ja esta no final então so tem o campo de nome, o campo do cargo eh deixado como NULL e a função retorna.
    if(ftell(fp) == (curr + r->tamanhoRegistro)) return 1;

    fread(&auxSize, 4, 1, fp);
    fread(&auxTag, 1, 1, fp);
    auxSize--;

    // senão a tag eh lida e checase sua igualdade com 'c'.
    if(auxTag == 'c'){
        //Se for igual o cargo eh lido.
        r->cargoServidor = malloc(auxSize);
        fread(r->cargoServidor, 1, auxSize, fp);
    }

    //Apos atribuir todos os valores a seus respectivos campos a função retorna.
    fseek(fp, curr + r->tamanhoRegistro, SEEK_SET);
    return !EOF;
}



/*
Definição:
    Função que imprime os valores do registro de acordo com a especificação do trabalho, fazendo os devidos tratamentos para campos nulos.
Parâmetros:
    Record* r: Record que deseja-se printar.
*/
void printRecord(Record* r){
    if(r->removido == '*') return;
    //Para os campos fixos, eh verificado se seus valores são nulos, caso seja são printados espaços, caso não sejam, então seus valores reais são printados.
    if(!r->idServidor || r->idServidor == -1) printNullFixString(5);
    else printf("%d ", r->idServidor);
    if(r->salarioServidor == -1) printNullFixString(9);
    else printf("%.2lf ", r->salarioServidor);
    if(!r->telefoneServidor || !strcmp(r->telefoneServidor, "\0@@@@@@@@@@@@@")) printNullFixString(14);
    else printf("%s", r->telefoneServidor);

    //Para os campos de tamanho variavel verifica-se se são não-nulos, caso sejam então sao printaods, caso não nada acontace.
    if(r->nomeServidor){
        printf(" %d %s", (int)strlen(r->nomeServidor), r->nomeServidor);
    }
    if(r->cargoServidor){
        printf(" %d %s", (int)strlen(r->cargoServidor), r->cargoServidor);
    }

    //Finaliza a impressão com um '\n' dividindo os registros em linhas diferentes.
    printf("\n");
    return;
}

/*
Definição:
    Função que imprime Record de acordo com o especificado usando os valores da descrição dos campos contidos no Header.
Parâmetros:
    Record* r: Record que deseja-se imprimir.
    Header* h: Header que contem as informações necessarias de descrição dos campos.
*/
void printSearched(Record* r, Header* h){
    if(r->idServidor == -1) printf("%s: valor nao declarado\n", h->desCampo[0]);
    else printf("%s: %d\n", h->desCampo[0], r->idServidor);
    if(r->salarioServidor == -1) printf("%s: valor nao declarado\n", h->desCampo[1]);
    else printf("%s: %.2lf\n", h->desCampo[1], r->salarioServidor);
    if(!r->telefoneServidor || !strcmp(r->telefoneServidor, "\0@@@@@@@@@@@@@")) printf("%s: valor nao declarado\n", h->desCampo[2]);
    else printf("%s: %s\n", h->desCampo[2], r->telefoneServidor);
    if(!r->nomeServidor) printf("%s: valor nao declarado\n", h->desCampo[3]);
    else printf("%s: %s\n", h->desCampo[3], r->nomeServidor);
    if(!r->cargoServidor) printf("%s: valor nao declarado\n", h->desCampo[4]);
    else printf("%s: %s\n", h->desCampo[4], r->cargoServidor);
    printf("\n");
    return;
}

/*
Definição:
    Função que verifica, a partir da opção selecionada, se o valor no registro eh igual ao buscado, caso seja, ele manda o registro ser impresso.
Parâmetros:
    Record* r: Record cuja concordancia com o valor buscado deseja-se verificar.
    int op: indicador da operação da busca realizada, definida pela função "getValue()", usada para interpretar corretamente o void* value.
    void* value: um tipo generico de endereço que, quando corretamente interpretado, armazena o valor buscado no registro.
    Header* h: Header que contem as informações necessarias de descrição dos campos, usado para printar o Record corretamente apenas. 
*/
int printIfSearchedRecord(Record* r, int op, void* value, Header* h){
    if(isSearchedRecord(r, op, value)){
        printSearched(r, h);
        return 1;
    }
    return 0;
}


/*
Definição:
    Função que preenche o arquivo com lixo a partir da posição begin ate a end, que devem ser especificadas. 
Parâmetros:
    FILE* fp: ponteiro para o arquivo em que o lixo sera escrito.
    long long begin: offset de inicio da escrita.
    long long end: offset do fim da escrita de lixo.
    char trash: caracter que sera escrito como lixo.
*/
void fillWithTrash(FILE* fp, long long begin, long long end, char trash){
    long currOffset = ftell(fp);
    fseek(fp, begin, SEEK_SET);
    for(long long i = 0; i < end - begin; i++){
        fwrite(&trash, 1, 1, fp);
    }
    fseek(fp, currOffset, SEEK_SET);
    return;
}


/*
Definição:
    Função que simpplesmente verifica se o registro atende aos criterios de busca especificados.
Parâmetros:
    Record* r: Record cuja concordancia com o valor buscado deseja-se verificar.
    int op: indicador da operação da busca realizada, definida pela função "getValueRemove()", usada para interpretar corretamente o void* value.
    void* value: um tipo generico de endereço que, quando corretamente interpretado, armazena o valor buscado no registro.
*/
int isSearchedRecord(Record* r, int op, void* value){
    if(r->removido == '*') return 0;
    if(!value){
        if(op == 2 && (!r->telefoneServidor || !strlen(r->telefoneServidor))) return 1;
        if(op == 3 && !r->nomeServidor) return 1;
        if(op == 4 && !r->cargoServidor) return 1;
        return 0;
    }
    if(op == 0 && r->idServidor == *((int*)value)) return 1; 
    if(op == 1 && r->salarioServidor == *((double*)value)) return 1;  
    if(r->telefoneServidor && op == 2 && !strcmp(r->telefoneServidor, (char*)value)) return 1;
    if(r->nomeServidor && op == 3 && !strcmp(r->nomeServidor, (char*)value)) return 1;
    if(r->cargoServidor && op == 4 && !strcmp(r->cargoServidor, (char*)value)) return 1;
    if(op == 5 && (r->salarioServidor == 0.0 || r->salarioServidor == -1.0)) return 1;
    return 0;
}



/*
Definição:
    Função que le a lista a partir do encadeamento presente em um arquivo, armazenando cada espaço logicamente removido como um elemento da lista.
Parâmetros:
    List* l: lista na partir a qual serão escritor os elementos lidos do encadeamento do arquivo.
    FILE* fp: ponteiro para o arquivo do qual a lista devera ser lida.
    lon long topoLista: offset inicial, valor do encadeamento presente no cabeçalho.
    int* mn: ponteiro para um inteiro que guarda o menor tamanho dentre os registros lidos (usado na ordenação).
    int* mx: ponteiro para um inteiro que guarda o maior tamanho dentre os registros lidos (usado na ordenação).
*/
void makeRemovedList(List* l, FILE* fp, long long topoLista, int* mn, int* mx){
    clearList(l);
    int max = -1;
    int min = -1;
    while(topoLista != -1){
        fseek(fp, (topoLista + 1), SEEK_SET);
        int size;
        fread(&size, 4, 1, fp);
        if(min == -1){
            min = size;
            max = size;
        }else{
            if(size > max)  max = size;
            else if(size < min) min = size;
        }
        pushBack(l, createNode(topoLista, size, -1));
        fread(&topoLista, 8, 1, fp);
    }
    *mx = max;
    *mn = min;
    return;
}


/*
Definição:
    Função que remove logicamente um registro que se encontra na posição atual, marcando o campode removido com '*' e preeenchendo os outros com lixo.
Parâmetros:
    Record* r: ponteiro para um record que guarda informações do registro a ser removido.
    FILE* fp: ponteiro para o arquivo no qual o registro se encontra de onde sera removido.
    Listr* l: lista na qual o registro removido sera inserido para formar o encadiamento do arquivo.
    int* min: ponteiro para um inteiro que guarda o menor tamanho dentre os registros lidos (usado na ordenação).
    int* max: ponteiro para um inteiro que guarda o maior tamanho dentre os registros lidos (usado na ordenação).
    int priority: inteiro que representa a prioridade da remoção.

*/
void removeRecordInPos(Record* r, FILE* fp, List* l, int* min, int* max, int priority){
    r->removido = '*';
    long long offset = ftell(fp) - (r->tamanhoRegistro + 5);
    pushFront(l, createNode(offset, r->tamanhoRegistro, priority));
    if(*min == -1){
        *min = r->tamanhoRegistro;
        *max = r->tamanhoRegistro;
    } else{
        if(r->tamanhoRegistro > *max) *max = r->tamanhoRegistro;
        if(r->tamanhoRegistro < *min) *min = r->tamanhoRegistro;
    }
    markRemoved(r, offset, fp);
    fillWithTrash(fp, (offset + 13), (long long)ftell(fp), '@');
    return;
}


/*
Definição:
    Função que escreve o encadeamento da lista de removidos no arquivo, acessando os registros e alterando o valor do campo encadeamentoLista.
Parâmetros:
    List* l: lista a partir da qual serão tirados os elementos para serem escritos no encadeamento do arquivo.
    FILE* fp: ponteiro para o arquivo no qual a lista devera ser escrita.
    Header* h: ponteiro para um cabeçalho no qual serão escrito o primeiro offset.
*/
void makeFileRemovedList(List* l, FILE* fp, Header* h){
    Node* curr = popFirst(l);
    long long offset = getNodeOffset(curr);
    h->topoLista = offset;
    while(offset != -1){
        fseek(fp, (offset + 5), SEEK_SET);
        free(curr);
        curr = popFirst(l);
        offset = getNodeOffset(curr);
        fwrite(&offset, 8, 1, fp);
    }
    return;
}


/*
Definição:
    Função que escreve um registro diretamente no offset especificado, sem uso de Clusters.
Parâmetros:
    Record* r: ponteiro para um record que contem as informações do registro.
    long long offset: distancia a partir do inicio do arquivo onde o registro deve ser escrito.
    FILE* fp: ponteiro para o arquivo em que o registro sera escrito.
    Header* h: ponteiro para um cabeçalho que contem dados necessarios para a escrita do registro.
*/
void saveNewRecordInOffset(Record* r, long long offset, FILE* fp, Header* h){
    fseek(fp, offset, SEEK_SET);
    r->removido = '-';
    fwrite(&r->removido, 1, 1, fp);

    fseek(fp, 4, SEEK_CUR);
    
    r->encadeamentoLista = -1;
    fwrite(&r->encadeamentoLista, 8, 1, fp);
    fwrite(&r->idServidor, 4, 1, fp);
    fwrite(&r->salarioServidor, 8, 1, fp);

    //Verifica se telefoneServidor não eh nulo.
    if(r->telefoneServidor) fwrite(r->telefoneServidor, 1, 14, fp);
    else{
        //Se for nulo então a string "\0@@@@@@@@@@@@@" eh salva.
        char aux[14];
        strncpy(aux, "@@@@@@@@@@@@@@", 14);
        aux[0] = '\0';
        fwrite(aux, 1, 14, fp);
    }

    int size;

    //Verifica se nomeServidor eh não-nulo, caso não seja, nada dele eh salvo.
    if(r->nomeServidor){
        //Caso seja, ele, seu tamanho e sua tag são salvas.
        size = strlen(r->nomeServidor) + 2;
        fwrite(&size, 4, 1, fp);
        fwrite(&h->tagCampo[3], 1, 1, fp);
        size--;
        fwrite(r->nomeServidor, 1, size, fp);
    }

    //Verifica se cargoServidor eh não-nulo, caso não seja, nada dele eh salvo.
    if(r->cargoServidor){
        //Caso seja, ele, seu tamanho e sua tag são salvas.
        size = strlen(r->cargoServidor) + 2;
        fwrite(&size, 4, 1, fp);
        fwrite(&h->tagCampo[4], 1, 1, fp);
        size--;
        fwrite(r->cargoServidor, 1, size, fp);
    }

    return;
}


/*


//COMENTAR DE NOVO.

Definição:
    Procedimento que salva um registro novo no arquivo, tomando os devidos cuidados para que seja escrito na melhor posição  levando em consideração 
    os registros logicamente removidos.
Parâmetros:
    Record* r: record que contem as informações do registro que deve ser escrito.
    List* l: lista de registros logicamente removidos.
    FILE* fp: ponteiro para o arquivo onde o registro sera escrito.
    Header* h: header que contem informações necessarias para a escrita do arquivo.
*/
long long saveNewRecord(Record* r, List* l, FILE* fp, Header* h){

    // Primeiro encontra-se o best-fit.
    Node* bestFit  = getFirstFit(l, r->tamanhoRegistro);
    if(bestFit){
        long long ret = getNodeOffset(bestFit);
        //Caso ele exista o registro eh salvo na posição indicada por ele.
        saveNewRecordInOffset(r, getNodeOffset(bestFit), fp, h);
        free(bestFit);
        return ret;
    }

    //Caso não exista um registro removido best-fit, o registro sera salvo no final do arquivo.
    long positionBefore = ftell(fp);

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    long long ret = size;

    // Lê-se o ultimo cluster disponivel no arquivo
    long lastClusterOffset = (long)(((int)(size / 32000)) * 32000);
    fseek(fp, lastClusterOffset, SEEK_SET);
    Cluster* c = createCluster();
    loadCluster(c, fp);

    //Cria os records que guardarao o ultimo registro ja escrito.
    Record* temp = createRecord();
    Record* last = NULL;
    while(loadRecord(temp, c)){
        destroyRecord(last);
        last = temp;
        temp = createRecord();
    }
    c->curPos--;
    destroyRecord(temp);

    // Checa-se se ainda tem, espaço suficiente na pagina de disco para a escrita do novo registro.
    if(clusterHasSpace(c, r->tamanhoRegistro)){

        //Se tiver ele simplesmente eh escrito no final do Cluster.
        writeRecord(c, r, h);
        fseek(fp, lastClusterOffset, SEEK_SET);
    }
    else{

        // Caso não tenha, o Cluster eh preenchido com lixo, o tamanho do ultimo registro eh atualizado, a pagina escrita e uma nova criada.
        int lastRecordSize = last->tamanhoRegistro;
        last->tamanhoRegistro += 32000 - c->curPos;
        reWriteRecord(c, last, lastRecordSize, h);
        fillCluster(c, '@');
        fseek(fp, lastClusterOffset, SEEK_SET);
        fwrite(c->data, 1, 32000, fp);
        destroyCluster(c);
        c = createCluster();

        // Apos isso o registro eh escrito no novo Cluster criado.
        writeRecord(c, r, h);
    }

    // Finalmente a pagina que contem o registro eh escrita no arquivo.
    fwrite(c->data, 1, c->curPos, fp);
    destroyCluster(c);
    destroyRecord(last);

    fseek(fp, positionBefore, SEEK_SET);
    return ret;
}


/*
Definição:
    Função simples que descobre o valor real (compactado, sem lixo) de um registro.
Parâmetros:
    Record* r: registro cujo real tamanho deseja-se descobrir.
Retorno:
    int - o real tamanho do registro.
*/
int getActualSize(Record* r){
    int size = 34; // 8(encadiamentoLista) + 4(idServidor) + 8(salarioServidor) + 14(telefoneServidor).
    if(r->nomeServidor) size += strlen(r->nomeServidor) + 1 + 5;
    if(r->cargoServidor) size += strlen(r->cargoServidor) + 1 + 5;
    return size;
}


/*
Definição:
    Metodo que atualiza o rgistro que se encontra na posição atual, tomando os cuidados para que seja escrito no melhor lugar, ou seja,
    caso seu tamanho ainda seja suportado pelo espaço que ocupa ele não eh realocado, caso contrario ele eh removido e reinserido no best-fit.
Parâmetros:
    Record* r: registro que contem as infomações, que sera atualizado.
    int updateTag: inteiro que representa a tag do campo que sera atualizado, usado para interpretar corretamento o void* updateValue.
    void* updateValue: void* que, ao ser interpretado corretamente guarda o valor novo do campo que deseja-se atualizar.
    FILE* fp: ponteiro para o arquivo onde serão feitas as modificações do registro.
    Header* h: header com informações importantes para a escrita do registro.
    List** l: endereço para o inicio de uma lista que sera usada para definir a melhor posição caso o registro apos a atualização
              tenha um tamanho maior do que sua posição de memoria original (passado como referencia pois pode ser alterado).
    int* min: ponteiro para um inteiro que guarda o menor tamanho dentre os registros lidos (usado na ordenação).
    int* max: ponteiro para um inteiro que guarda o maior tamanho dentre os registros lidos (usado na ordenação).
*/
void updateRecordInPos(Record* r, int updateTag, void* updateValue, FILE* fp, Header* h, List** l, int* min, int* max){

    // Guarda o tamanho inicial da posição de memoria.
    int sizeBeforeUpdate = r->tamanhoRegistro;
    int newSize = -1;
    int oldSize = -1;

    // Faz as checagens necessarias para atualizar corretamento o campo desejado. caso seja de tamanho cariavel, o tamanho do registro eh
    // atualizado.
    switch (updateTag){
        case 0:
            r->idServidor = *((int*)updateValue);
            break;
        case 1:
            r->salarioServidor = *((double*)updateValue);
            break;
        case 2:
            if(r->telefoneServidor) free(r->telefoneServidor);
            if(!updateValue) r->telefoneServidor = updateValue;
            else{
                r->telefoneServidor = malloc(strlen(updateValue) + 1);
                strcpy(r->telefoneServidor, updateValue);
            }
            break;
        case 3:
            if(updateValue) newSize = strlen(updateValue) + 1 + 5;
            else newSize = 0;
            if(r->nomeServidor) oldSize = strlen(r->nomeServidor) + 1 + 5;
            else oldSize = 0;
            if(r->nomeServidor) free(r->nomeServidor);
            if(!updateValue) r->nomeServidor = updateValue;
            else{
                r->nomeServidor = malloc(strlen(updateValue) + 1);
                strcpy(r->nomeServidor, updateValue);
            }
            r->tamanhoRegistro = getActualSize(r);
            break;
        case 4:
            if(updateValue) newSize = strlen(updateValue) + 1 + 5;
            else newSize = 0;
            if(r->cargoServidor) oldSize = strlen(r->cargoServidor) + 1 + 5;
            else oldSize = 0;
            if(r->cargoServidor) free(r->cargoServidor);
            if(!updateValue) r->cargoServidor = updateValue;
            else{
                r->cargoServidor = malloc(strlen(updateValue) + 1);
                strcpy(r->cargoServidor, updateValue);
            }
            r->tamanhoRegistro = getActualSize(r);
            break;
        default:
            break;
    }

    // Compara-se então o tamanho real do registro apos a atualização com o tamanho original do espaço de memoria por ele ocupado.
    if(r->tamanhoRegistro > sizeBeforeUpdate){
        
        // Caso o novo tamanho seja maior, o registro eh removido e reinserido.
        r->removido = '*';
        r->tamanhoRegistro = sizeBeforeUpdate;

        removeRecordInPos(r, fp, *l, min, max, 0);
        *l = bucketSort(*l, *min, *max);

        r->removido = '-';
        r->tamanhoRegistro = getActualSize(r);

        saveNewRecord(r, *l, fp, h);
    }else{

        // Caso o novo tamanho seja menor que o espaço anteriormente usado, o registro eh reescrito na mesma posição.
        r->tamanhoRegistro = sizeBeforeUpdate;
        //printf("Resultado Menor\n");
        long long currOffset = ftell(fp);
        saveNewRecordInOffset(r, (long long)(currOffset - (sizeBeforeUpdate + 5)), fp, h);
        fillWithTrash(fp, (currOffset - (oldSize - newSize)), currOffset, '@');
    }
    return;
}