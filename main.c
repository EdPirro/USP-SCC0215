/*
Integrantes do Grupo:
    Nome:                             nUSP:
    Alexandre Galocha Pinto Junior    10734706 
    Eduardo Pirro                     10734665

Motivo da escolha do trabalho do Eduardo:
    O desempate seria nas notas, mas ambos os trabalhos anteriores tiveram as mesmas notas.
    Portanto o critério de escolha foi o tempo de execução no caso de ponto extra do trabalho 2.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cluster.h"
#include "list.h"
#include "record.h"
#include "merge_sort.h"
#include "index.h"



/*
Definição:
    Função do monitor Matheus Carvalho para printar na saida padrão o arquivo binario.
Parâmetros:
    char* nomeArquivoBinario: nome(filepath) do arquivo binario que deseja-se printar.
*/
void binarioNaTela2(char *nomeArquivoBinario) {

    /* Escolha essa função se você já fechou o ponteiro de arquivo 'FILE *'.
    *  Ela vai abrir de novo para leitura e depois fechar. */

    unsigned char *mb;
    unsigned long i;
    size_t fl;
    FILE *fs;
    if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela2): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar? Se você não fechou ele, pode usar a outra função, binarioNaTela1, ou pode fechar ele antes de chamar essa função!\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *) malloc(fl);
    fread(mb, 1, fl, fs);
    for(i = 0; i < fl; i += sizeof(unsigned char)) {
        printf("%02X ", mb[i]);
        if((i + 1) % 16 == 0)   printf("\n");
    }
    free(mb);
    fclose(fs);
}



/*
Definição:
    Função simples definida para ler, da entrada padrão, um valor que esteja no formato "valor"
Retorno:
    char*: a string lida, caso a string seja "NULO" ele retorna NULL.
*/
char* getStringRemove(int get){
    char* string = malloc(500);
    char c;
    int size = 0;
    c = fgetc(stdin);
    if(c == 'N'){
        scanf("%s", string);
        if(get) fgetc(stdin);
        free(string);
        return NULL;
    }
    for(int i = 0; 1; i++){
        c = fgetc(stdin);
        if(c == '\"'){
            if(get) fgetc(stdin);
            break;
        }
        string[size++] = c;
    }
    string[size] = '\0'; 
    return string;
}



/*
Definição:
    Função que gera o arquivo binário a partir de um caminho especificado que sera a origem dos dados, usado de funçoes como "writeRecord()" e "saveCluster()" e fazendo as
    checagens necessarias para garantir coisas como: Um regitro deve ser escrito totalmente em apenas uma pagina de diso, o ultimo registro de cada pagina tem em seu tamanho
    tudo que falta para chegar ao final da pagina, e a ultima pagina não deve ser preenchida com lixo.
Parâmetros:
    const char* fin: caminho para o arquivo de entrada, que sera lido.
    const char* fout: caminho para o arquivo de saida, onde as informações serão salvas, ou seja, o binario resultante da operação.
*/
void makeBin(const char* fin, const char* fout){
    Header* h = createHeader();
    Cluster* c = createCluster();
    FILE* csv = fopen(fin, "r");
    if(!csv){
        printf("Falha no carregamento do arquivo.\n");
        return;
    }

    getHeader(h, csv);
    saveHeader(fout, h, c);
    destroyCluster(c);
    c = createCluster();
    //Cria 2 Records, um para ser o atual, ou seja, o que será lido e salvo no passo atual
    //e um que será o anterior para ter seu tamanho alterado caso seja o ultimo Record do Cluster e tenha que guardar o tamanho do lixo.
    Record* lastRec = NULL;
    Record* currRec = createRecord();

    while(getRecord(csv, currRec) != EOF){ //le Records enquanto não chegar ao final do arquivo.
        //verifica se tem espaço disponivel no Cluster.
        if(clusterHasSpace(c, currRec->tamanhoRegistro)){ 
            //Se tiver, simplemente salva o Record atual, faz o anterior receber o atual e então aloca um novo para ser o atual.
            writeRecord(c, currRec, h);
            if(lastRec) destroyRecord(lastRec);
            lastRec = currRec;
            currRec = createRecord();
        }
        else{

            //Caso o Cluster não tenha espaço, o Record anterior eh reescrito para que seu tamanho indique ate o final do Cluster.
            int lastRecordSize = lastRec->tamanhoRegistro; //Salva o tamanho anterior para saber o offset da função "reWriteRecord()".
            lastRec->tamanhoRegistro += 32000 - c->curPos; //Atribui o novo tamanho ao campo de tamanho do Record.
            reWriteRecord(c, lastRec, lastRecordSize, h); //Chama a função de reescrita de um Record na posição especifcada no Cluster.

            //Apos reescrveer o Cluster com as informações corretas, ele eh preenchido, salvo no arquivo, destruido e criado novamente, fazendo um Cluster novo e vazio.
            fillCluster(c, '@');
            saveCluster(fout, c);
            destroyCluster(c);
            c = createCluster();

            //Depois disso tudo o Record atual eh escrito no Cluster, faz o anterior receber o atual e então cria um novo para ser o atual no proximo passo.
            writeRecord(c, currRec, h);
            if(lastRec) destroyRecord(lastRec);
            lastRec = currRec;
            currRec = createRecord();
        }
    }

    destroyRecord(currRec);

    //Se dpois disso ainda tiver algo escrito no Cluster ele deve ser salvo como o ultimo da pagina.
    if(c->curPos) saveLastCluster(fout, c);
    
    
    destroyCluster(c);
    destroyRecord(lastRec);

    fclose(csv);
    destroyHeader(h);
    printf("arquivoTrab1.bin");
    return;
}



/* 
Definição:
    Função usada como Debug, teria função de imprimir as informações do Header.
Parâmetros:
    Header* h: Header cujas informações desejaria-se imprimir.
void printHeader(Header* h){
    printf("status: %d\n", (int)h->status);
    printf("topoLista: %lld\n", h->topoLista);
    for(int i = 0; i < 5; i++)
        printf("tag: %c, desc: %s\n", h->tagCampo[i], h->desCampo[i]);
    return;
}*/



/*
Defnição:
    Função que le o arquivo binário ate o final e imprime na saida padrão todos os registros contidos nele de acordo com a especificação.
Parâmetros:
    const char* file: caminho para o arquivo que deseja-se ler.
*/
void readBin(const char* file){
    int nClusters = 0;
    int nRecord = 0;
    FILE* fp = fopen(file, "rb");
    if(!fp){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    Header* h = createHeader();
    Cluster* c = createCluster();
    Record* r = createRecord();
    if(loadCluster(c, fp) == EOF){
        destroyCluster(c);
        destroyHeader(h);
        destroyRecord(r);
        fclose(fp);
        printf("Registro inexistente.");
        return;
    } 
    nClusters++;
    loadHeader(h, c);
    destroyCluster(c);
    if(h->status == '0'){
        printf("Falha no processamento do arquivo.");
        return;
    }
    c = createCluster(c);
    loadCluster(c, fp);
    nClusters++;
    while(1){
        //Le ate o fim do arquivo.
        if(!loadRecord(r, c)) break;

        //Verifica se o Cluster esta no final.
        if(clusterEnd(c)){
            //Caso esteja, ele eh destruido e um novo eh aberto.
            destroyCluster(c);
            c = createCluster();
            loadCluster(c, fp);
            nClusters++;
        }
        //Para cada passo o Record eh printado.
        printRecord(r);
        nRecord++;
    }
    destroyCluster(c);
    destroyHeader(h);
    destroyRecord(r);
    fclose(fp);

    //Caso nenhum registro seja exibido aparece uma mensagem de erro.
    if(!nRecord) printf("Registro inexistente.");

    //Senao aparece o numero de paginas acessadas.
    else printf("Número de páginas de disco acessadas: %d\n", nClusters);
    return;
}



/*
Definição:
    Função que le o campo pelo qual a busca será realizada e então realiza a leitura do valor que sera buscado retornando como void* que será interpretado corretamente
    apenas com a ajuda do int op.
Parâmetros:
    int* op: endereço para um inteiro onde sera guardado o numero referente ao campo interessado pela busca, ou seja:
             0 - busca pelo id.
             1 - busca pelo salario.
             2 - busca pelo telefone.
             3 - busca pelo nome.
             4 - busca pelo cargo.
             -1 - caso não seja nenhuma opção acima.
Retorno:
    void*: valor de item que será buscado, definido como void* pois, dependendo de op será interpretado de uma maneira, ou seja:
           se op = 0, deve ser interpretado como int*.
           se op = 1, deve ser interpretado como double*.
           se op = 2, 3 ou 4, deve ser interpretado como char* (string).
           caso não seja encontrado o campo o valor retornado sera NULL.

*/
void* getValue(int* op){
    char name[50];
    scanf("%s ", name);
    if(!strcmp(name, "idServidor")){
        int* value = malloc(sizeof(int));
        scanf("%d", value);
        *op = 0;
        return value;
    }
    if(!strcmp(name, "salarioServidor")){
        double* value = malloc(sizeof(double));
        scanf("%lf", value);
        *op = 1;
        return value;
    }
    if(!strcmp(name, "telefoneServidor")){
        char* value = malloc(15);
        scanf("%s", value);
        *op = 2;
        return value;
    }
    if(!strcmp(name, "nomeServidor")){
        int size;
        char* value = getString(stdin, &size);
        *op = 3;
        return value;
    }
    if(!strcmp(name, "cargoServidor")){
        int size;
        char* value = getString(stdin, &size);
        *op = 4;
        return value;
    }
    *op = -1;
    return NULL;
}



/*
Definição:
    Função que le o campo pelo qual a busca será realizada e então realiza a leitura do valor que sera buscado retornando como void* que será interpretado corretamente
    apenas com a ajuda do int op. Uma versão do getValue que serve para a remoção pois ao pegar a string desejada procura por alguma no formato "valor".
Parâmetros:
    int* op: endereço para um inteiro onde sera guardado o numero referente ao campo interessado pela busca, ou seja:
             0 - busca pelo id.
             1 - busca pelo salario.
             2 - busca pelo telefone.
             3 - busca pelo nome.
             4 - busca pelo cargo.
             -1 - caso não seja nenhuma opção acima.
Retorno:
    void*: valor de item que será buscado, definido como void* pois, dependendo de op será interpretado de uma maneira, ou seja:
           se op = 0, deve ser interpretado como int*.
           se op = 1, deve ser interpretado como double*.
           se op = 2, 3 ou 4, deve ser interpretado como char* (string).
           caso não seja encontrado o campo o valor retornado sera NULL.

*/
void* getValueRemove(int* op){
    char name[50];
    scanf("%s ", name);
    if(!strcmp(name, "idServidor")){
        int* value = malloc(sizeof(int));
        scanf("%d", value);
        *op = 0;
        return value;
    }
    if(!strcmp(name, "salarioServidor")){
        double* value = malloc(sizeof(double));
        char aux[50];
        scanf("%s", aux);
        *op = 1;
        if(!strcmp(aux, "NULO")) *op = 5;
        else *value = atof(aux);
        return value;
    }
    if(!strcmp(name, "telefoneServidor")){
        char* value = getStringRemove(0);
        *op = 2;
        return value;
    }
    if(!strcmp(name, "nomeServidor")){
        char* value = getStringRemove(0);
        *op = 3;
        return value;
    }
    if(!strcmp(name, "cargoServidor")){
        char* value = getStringRemove(0);
        *op = 4;
        return value;
    }
    *op = -1;
    return NULL;
}



/*
Definição:
    Função que le todos os registros de um arquivo cujo caminho deve ser especificado e verifica sua concordancia com o valor buscado, printando apenas os registros que 
    atenderem à essas exigencias, a função le ate o final do arquivo exceto quando a busca for realizada pelo idServidor que então acaba assim que o registro correspondente
    for encontrado.
Parâmetros:
    const char* file: caminho para o arquivo binário do qual os registros devem ser lidos.
*/
void searchBin(const char* file){
    int nClusters = 0;
    int op;
    int printedRecordsCount = 0;
    FILE* fp = fopen(file, "rb");
    if(!fp){
        printf("Falha no processamento do arquivo.");
        return;
    }
    //Recebe o valor buscado e operação.
    void* value;
    value = getValue(&op);
    if(!value){
        printf("Falha no processamento do arquivo.");
        return;
    }
    Header* h = createHeader();
    Cluster* c = createCluster();
    Record* r = createRecord();
    if(loadCluster(c, fp) == EOF){
        destroyCluster(c);
        destroyHeader(h);
        destroyRecord(r);
        fclose(fp);
        printf("Falha no processamento do arquivo.");
        return;
    } 
    nClusters++;
    loadHeader(h, c);
    destroyCluster(c);
    if(h->status == '0'){
        printf("Falha no processamento do arquivo.");
        return;
    }
    c = createCluster(c);
    if(loadCluster(c, fp) == EOF){
        destroyCluster(c);
        destroyHeader(h);
        destroyRecord(r);
        fclose(fp);
        printf("Número de páginas de disco acessadas: %d\n", nClusters);
        return;
    }
    nClusters++;
    while(1){
        //Le ate o final do arquivo.
        if(!loadRecord(r, c)) break;
        if(clusterEnd(c)){
            //Caso o Cluster atual esteja no final, ele eh destruido e um novo eh carregado.
            destroyCluster(c);
            c = createCluster();
            loadCluster(c, fp);
            nClusters++;
        }

        //guarda a quantidade de Records printados.
        printedRecordsCount += printIfSearchedRecord(r, op, value, h);

        //Caso a busca seja realizada por id, tendo ja algum Record printado a função ja termina.
        if(!op && printedRecordsCount) break;
    }
    destroyCluster(c);
    destroyHeader(h);
    destroyRecord(r);
    fclose(fp);

    //Se nenhum atendeu as exigencias, uma mensagem de erro eh impressa.
    if(!printedRecordsCount) printf("Registro inexistente.\n");  

    //Se não o numero de paginas acessadas eh printado.
    else printf("Número de páginas de disco acessadas: %d\n", nClusters);
    free(value);
    return;
}



/*
Definição:
    Função que remove todos os registros que atendam determinados criterios de busca. O algoritmo le todos os criterios de busca e passa apenas uma vez pelo arquivo removendo
    qualquer registro que atenda a alguma das condiçoes, tomando o cuidado necessario para que a lista de remoção seja construida adequadamente.
Parâmetros:
    const char* file: caminho para o arquivo binário do qual os registros devem ser lidos e removidos. 
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
int removeRecord(const char* file){
    FILE* bin = fopen(file, "r+b"); // abre o arquivo como leitura e escrita.

    //Caso não consiga abrir o programa finaliza com um erro.
    if(!bin){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    
    }
    Header* h = createHeader();
    loadHeaderNC(h, bin); // Carrega o Header do arquivo.

    //Caso o status do Header seja '0' ele esta inconsistente e portanto não deve ser processado.
    if(h->status == '0'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(bin);
        return 0;
    }
    h->status = '0'; // atualiza o status do arquivo para '0' enquando o processa (voltara para '1' apos terminar a execução).
    saveHeaderNC(h, bin);

    List* l = createList();
    int min, max;
    makeRemovedList(l, bin, h->topoLista, &min, &max); // Inicialmente pega a lista ja existente no arquivo.

    //le a quantidade de buscas que serão feitas e pega o op e o value de cada busca guardando-os em vetores.
    int nSearches;
    scanf("%d", &nSearches);
    int* op  = malloc(nSearches * sizeof(int));
    void** value = malloc(nSearches * sizeof(void*));
    for(int i = 0; i < nSearches; i++) value[i] = getValueRemove(&op[i]);

    // encontra o tamanho do arquivo para condição de parada.
    fseek(bin, 0, SEEK_END);
    long fileSize = ftell(bin); 
    fseek(bin, 32000, SEEK_SET);

    Record* r = createRecord(); // cria a estrutura Record que sera usada para ler cada registro do arquivo.

    // le então todos os registros do arquivo ate chegar a um EOF ou ate que a posição atual no arquivo seja igual ao seu tamanho.
    while(loadRecordNC(r, bin) != EOF){ 
        for(int i = 0; i < nSearches; i++){

            //para cada iteração se o arquivo atender a alguma das nSearches exigencias ele sera removido.
            if(isSearchedRecord(r, op[i], value[i])){

                // apos removido ele eh inserido na lista com prioridade igual ao i (numero da busca que resultou verdadeiro).
                removeRecordInPos(r, bin, l, &min, &max, i);
                break;
            }
        }
        if(ftell(bin) == fileSize) break;
    }

    // apos realizar todas as remoções os estruturas auxiliares são desalocadas.
    destroyRecord(r);
    free(op);
    if(value){
        for(int i = 0; i < nSearches; i++) 
            if(value[i]) free(value[i]);
        free(value);
    }

    // a lista eh então ordenada (decrescentemente) pela prioridade e então (crescentemente) pelo tamanho de cada elemento.
    l = sortByPriority(l, -1, nSearches - 1);
    l = bucketSort(l, min, max);

    // apos tudo isso escreve-se no arquivo a fila contruida, alterando o encadeamentoLista de cada registro garantindo encadeamento correto.
    makeFileRemovedList(l, bin, h); 

    // status volta a ser '1' e enfim todas as estruturas são desalocadas e o arquivo eh fechado evitando vazamento de memoria.
    h->status = '1';
    saveHeaderNC(h, bin);
    destroyHeader(h);
    destroyList(l);
    fclose(bin);
    return 1;
}



/*
Definição:
    Função simples definida para ler um registro da entrada padão para ser adicionado, de acordo com a especificação de entrada da funcionalidade 5.
    Essa função garante que os valores serão corretamente atribuidos.
Parâmetros:
    Record* r: Record no qual será salvo o registro lido.
*/
void getRecordStdin(Record* r){
    r->removido = '-';
    r->encadeamentoLista = -1;
    r->tamanhoRegistro = 34;
    char aux[200];


    scanf("%d %s ", &r->idServidor, aux);


    if(!strcmp(aux, "NULO")) r->salarioServidor = -1;
    else{
        r->salarioServidor = atof(aux);
    }

    r->telefoneServidor = getStringRemove(1);
    

    r->nomeServidor = getStringRemove(1);

    if(r->nomeServidor) r->tamanhoRegistro += (5 + strlen(r->nomeServidor) + 1);

    r->cargoServidor = getStringRemove(0);

    if(r->cargoServidor) r->tamanhoRegistro += (5 + strlen(r->cargoServidor) + 1);

    return;   
}



/*
Definição:
    Metodo criado para adicionar um novo registro no arquivo levando em consideração os espacos removidos logicamente e usando a tecnica de first-fit (que, por ser uma lista 
    ordenada crescentemente em função do tamanho, acaba se tornando um best-fit) para escrever o novo registra na posição correta. Caso não seja encontrado nenhuma posição 
    satisfatoria entra as removidas ele sera salvo no final do arquivo.
Parâmetros:
    const char* file: caminho para o arquivo binário no qual os registros devem ser adicionados.
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
int addRecord(const char* file){
    FILE* bin = fopen(file, "r+b"); // Abre o arquivo como leitura e escrita.

    // Caso não consiga abrir o arquivo especificado o programa retorna com uma mensagem de erro.
    if(!bin){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Carrega o Header e verifica se o status eh '0', caso seja o programa termina com erro.
    Header* h = createHeader();
    loadHeaderNC(h, bin);
    if(h->status == '0'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(bin);
        return 0;
    }
    h->status = '0'; // setta o status para '0' definindo o arquivo como inconsistente enquando esta fazendo modificações no mesmo.
    saveHeaderNC(h, bin);

    // Cria a lista a partir do encadeamendo ja existente no arquivo antes do programa ser executado.
    List* l = createList();
    int max, min;
    makeRemovedList(l, bin, h->topoLista, &min, &max);

    // Le o numero de operações e as realiza uma a uma.
    int nOp;
    scanf("%d", &nOp);
    for(int i = 0; i < nOp; i++){

        // Para cada novo registro, ele eh lido e então salvo (a função saveNewRecord() garante que este sera salvo na posição correta).
        Record* r = createRecord();
        getRecordStdin(r);
        saveNewRecord(r, l, bin, h);
        destroyRecord(r);
    }

    //Apos todas as inserções forem realizadas a lista de removidos existente no arquivo eh atualizada.
    makeFileRemovedList(l, bin, h);

    // Muda-se o status para '1' garantindo integridade dos dados, libera as estruturas usadas e fecha o arquivo evitando vazamento de memoria.
    h->status = '1';
    saveHeaderNC(h, bin);
    destroyHeader(h);
    destroyList(l);
    fclose(bin);
    return 1;
}



/*
Definição:
    Procedimento que atualiza todos os registros que condizam com as buscas realizadas, mudando os campos especificados e garantindo que esse registro seja salvo na posição 
    correta no arquivo, levando em consideração os campos logicamente removidos e usando o sistema de "best-fit".
Parâmetros:
    const char* file: caminho para o arquivo binário que contem os registros que devem ser atualizados.
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
int updateRecord(const char* file){
    FILE* bin = fopen(file, "r+b"); // Abre o arquivo como leitura e escrita.

    // Caso não seja possivel abrir o arquivo, o programa retorna com uma mensagem de erro.
    if(!bin){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Carrega o Header e verifica se o status eh '0', caso seja o programa retorna um erro.
    Header* h = createHeader();
    loadHeaderNC(h, bin); 
    if(h->status == '0'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(bin);
        return 0;
    }
    h->status = '0'; // modifica o status para '0' de modo a indicar que os dados estao instaveis enquando estao sendo atualizados.
    saveHeaderNC(h, bin);

    // Cria uma lista de removidos a partir da ja existente no arquivo antes da execução do programa.
    List* l = createList();
    int max, min;
    makeRemovedList(l, bin, h->topoLista, &min, &max);

    // Le o numero de buscas que serão realizadas e, para cada uma, sempre que encontrar algum registro que atenda suas condiçoes ele sera atualizado.
    int nOp;
    scanf("%d", &nOp);
    for(int i = 0; i < nOp; i++){

        // Para cada iteração le-se o valor dos campos de busca e de atualização.
        int op;
        void* value = getValueRemove(&op);
        int updateTag;
        void* updateValue = getValueRemove(&updateTag);

        // vai ate o final do arquivo para pegar seu tamanho e usa-lo como caso de parada.
        fseek(bin, 0, SEEK_END);
        long fileSize = ftell(bin);
        fseek(bin, 32000, SEEK_SET);

        Record* r = createRecord(); // Cria a estrutura que armazenará cada um dos registros lidos.
        while(loadRecordNC(r, bin) != EOF){

            // Passa-se então sobre todos os registros do arquivo checando se ele atende as condiçoes de busca.
            if(isSearchedRecord(r, op, value)){

                //Se atender ele eh atualizado (a função updateRecordInPos() garante que o registro sera atualizado e salvo corretamente).
                long currOffset = ftell(bin);
                updateRecordInPos(r, updateTag, updateValue, bin, h, &l, &min, &max);
                fseek(bin, currOffset, SEEK_SET); // garante que o arquivo estara na posição correta apos o algoritmo de atualização.
                if(!op) break;
            }
            if(ftell(bin) == fileSize) break;
        }

        // Depois de cada busca feita pelo arquivo todo, algumas estruturas auxiliares locais são liberadas (sendo novamente realocadas caso ainda 
        // existam buscas a serem realizadas).
        destroyRecord(r);
        if(value) free(value);
        free(updateValue);
    }

    //Apos a atualização e todos os registros desejados, a lista no arquivo eh atualizada.
    l = bucketSort(l, min, max);
    makeFileRemovedList(l, bin, h);

    // O status eh modificado novamente para '1' garantindo integridade dos dados, as estruturas utilizadas são desalocadas e o arquivo fechado, evitando vazamento de memoria.
    h->status = '1';
    saveHeaderNC(h, bin);
    destroyHeader(h);
    destroyList(l);
    fclose(bin);
    return 1;
}



/*
Definição:
    Função auxiliar criada para realizar a comparação de dois registros que será utilizada no merge sort.
Parâmetros:
    const void* a: primeiro valor a ser comparado. Valor definido como void* para generalizar a comparação e a ordenação do merge-sort
    const void* b:  segundo valor a ser comparado. Valor definido como void* para generalizar a comparação e a ordenação do merge-sort
Retorno:
    int - indica qual dos dois registros deve vir antes
            > 0 se b > a
            = 0 se b == a
            < 0 se b < a 
*/
int compareRecordById(const void* a, const void* b){
    return ((*((Record**)b))->idServidor - (*((Record**)a))->idServidor);
}



/*
Definição:
    Procedimento que realiza a ordenação de um arquivo baseado no id de cada registro, descartando os registros removidos e os espaços preenchidos
    por lixo antes de reescrever os registros de maneira ordenada. Utiliza o merge-sort para realizar a ordenação interna.
Parâmetros:
    const char* input: caminho para o arquivo binário que contem os registros que devem ser ordenados.
    const char* output: caminho para o arquivo binário onde devem ser escritos os registros após serem ordenados.
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
int sortFile(const char* input, const char* output){
    FILE* in = fopen(input, "rb");
    if(!in){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    //Carrega o header e checa se o ststus eh '1', garantindo consistencia
    Header* h = createHeader();
    Cluster* c = createCluster();
    loadCluster(c, in);
    loadHeader(h, c);
    destroyCluster(c);

    if(h->status != '1'){
        destroyHeader(h);
        fclose(in);
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Atualiza o status da entrada para '0'.
    h->status = '0';
    saveHeaderNC(h, in);

    c = createCluster();
    saveHeader(output, h, c); // salva o Header na saida com status = '0'.
    destroyCluster(c);

    Record** recordVector = malloc(sizeof(Record*)) ; // guarda todos os registros para dps ordenar.
    Record* r = createRecord(); // guarda cada registro temporariamente

    // Vai ate o final do arquivo pega o tamanho e volta para a posição logo apos o cabeçalho
    fseek(in, 0, SEEK_END);
    long fileSize = ftell(in);
    fseek(in, 32000, SEEK_SET);

    int nRecords = 0;
    while (loadRecordNC(r, in) != EOF){

        // Se o registro estiver removido ele não eh inserido no vetor.
        if(r->removido == '*'){
            if(ftell(in) >= fileSize) break;
            else continue;
        }
        

        r->tamanhoRegistro = getActualSize(r); // pega o tamanho real , sem lixo, do registro.

        // Para cada registro, aumenta uma posição no vetor e coloca o novo registro no final.
        recordVector = realloc(recordVector, ++nRecords * sizeof(Record*)); 
        recordVector[nRecords - 1] = r;
        r = createRecord();
        if(ftell(in) >= fileSize) break;
    }
    destroyRecord(r);

    //Ordena o vetor de registros em ordem crescente usado o MergeSort.
    MS_sort(recordVector, nRecords, sizeof(Record*), compareRecordById);


    // Salava o header como 1 no arquivo de entrada e o fecha ja que não vai mais ser usado
    h->status = '1';
    saveHeaderNC(h, in); 
    fclose(in);

    h->topoLista = -1; // no arquivo novo não vai ter registros removidos então o topop lista dele eh -1.

    c = createCluster();
    for(int i = 0; i < nRecords; i++){

        //Pacar cada record checa-se se o cluster atual tem espaço.
        if(!clusterHasSpace(c, recordVector[i]->tamanhoRegistro)){

            //Caso não tenha, o ultimo registro tem seu tamanho atualizado, o Cluster eh preenchido com lixo, salvo, destruido e outro eh criado.
            int lastRecordSize = recordVector[i - 1]->tamanhoRegistro;
            recordVector[i - 1]->tamanhoRegistro += 32000 - c->curPos;
            reWriteRecord(c, recordVector[i - 1], lastRecordSize, h);
            fillCluster(c, '@');
            saveCluster(output, c);
            destroyCluster(c);
            c = createCluster();
        }

        // de qualque jeito o registro atual eh escrito no Cluster atual.
        writeRecord(c, recordVector[i], h);
    }

    // O ultimo Cluster eh salvo e destruido.
    saveLastCluster(output, c);
    destroyCluster(c);

    // o header do arquivo criado eh atualizado para ter status = '1' e topoLista = -1
    FILE* out = fopen(output, "r+b");
    saveHeaderNC(h, out);


    //Libera as variaveis auxiliares.
    for(int i = 0; i < nRecords; i++) destroyRecord(recordVector[i]);
    free(recordVector);
    destroyHeader(h);
    fclose(out);
    return 1;
}



/*
Definição:
    Funcao auxiliar que escreve um registro em um cluster, verificando de há espaço no cluster para que ele seja escrito. Caso não tenha espaço, o ultimo registro
    do cluster é alterado para englobar o lixo escrito no fim do cluster, e então o registro é escrito em um novo cluster que é criado. 
Parâmetros:
    Record* r: registro atual que deve tentar ser escrito no cluster.
    Record* last: ultimo registro escrito no cluster, que será alterado caso o registro atual não caiba no cluster.
    Cluster** ct: ponteiro para o cluster onde será realizada a escrita do registro. É alterado caso o novo Cluster seja criado.
    const char* output: caminho para o arquivo binário onde devem ser escritos os clusters que estejam completamente preenchidos.
    Header* h: ponteiro para o header que possui as informações das tags dos campos.
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
void writeRecordInCluster(Record* r, Record* last, Cluster** ct, const char* output, Header* h){
    Cluster* c = *ct;
    if(!clusterHasSpace(c, r->tamanhoRegistro)){

        //Caso não tenha, o ultimo registro tem seu tamanho atualizado, o Cluster eh preenchido com lixo, salvo, destruido e outro eh criado.
        int lastRecordSize = last->tamanhoRegistro;
        last->tamanhoRegistro += 32000 - c->curPos;
        reWriteRecord(c, last, lastRecordSize, h);
        fillCluster(c, '@');
        saveCluster(output, c);
        destroyCluster(c);
        c = createCluster();
    }

    // de qualque jeito o registro atual eh escrito no Cluster atual.
    writeRecord(c, r, h);
    *ct = c;
}



/*
Definição:
    Procedimento que realiza a fusão de dois arquivos binários e escreve o resultado em um terceiro arquivo binário. A comparação dos registros é relativa
    ao id. Se um id é maior, o registro é maior. Se os ids são iguais, o registro é reescrito apenas uma vez.
Parâmetros:
    const char* input1: caminho para um dos arquivos binarios de onde serão pegos os registros para realizar a fusão.
    const char* input2: caminho para outro dos arquivos binarios de onde serão pegos os registros para realizar a fusão.
    const char* output: caminho para o arquivo onde serão escritos os registros mesclados de ambos arquivos de entrada.
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
int mergeFile(const char* input1, const char* input2, const char* output){

    // Abre ambos os arquivos para leitura e verifica se obteve sucesso.
    FILE* inA = fopen(input1, "rb");
    if(!inA){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    FILE* inB = fopen(input2, "rb");
    if(!inB){
        fclose(inA);
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Carrega ambos os headers e verifica se o status eh '1' em ambos.
    Header* h[2];
    h[0] = createHeader();
    h[1] = createHeader();
    loadHeaderNC(h[0], inA);
    loadHeaderNC(h[1], inB);

    if(h[0]->status == '0' || h[1]->status == '0'){
        destroyHeader(h[0]);
        destroyHeader(h[1]);
        fclose(inA);
        fclose(inB);
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Coloca ambos os Headers para 0 indicando inconsistencia.
    h[0]->status = '0';
    saveHeaderNC(h[0], inA);
    h[1]->status = '0';
    saveHeaderNC(h[1], inB);

    // Salva o Header do arquivo de saida com status = '0';
    Cluster* c = createCluster();
    saveHeader(output, h[0], c);
    destroyCluster(c);


    // Encontra os tamanhos dos 
    fseek(inA, 0, SEEK_END);
    long ASize = ftell(inA);
    fseek(inB, 0, SEEK_END);
    long BSize = ftell(inB);
    

    // declara os dois records auxiliares a e b e os inicia com o primeiro valor de inA e inB.
    Record* a = createRecord();
    Record* b = createRecord();
    Record* lastWritten = NULL;

    // volta parao inicio do arquivo de dados (dps do header) para começar a leitura dos registros.
    fseek(inA, 32000, SEEK_SET);
    fseek(inB, 32000, SEEK_SET);

    loadRecordNC(a, inA);
    loadRecordNC(b, inB);


    c = createCluster();
    while(1){

        if(a->removido == '*'){
            if((ftell(inA) >= ASize)) break;
            loadRecordNC(a, inA);
            continue;
        }

        if(b->removido == '*'){
            if((ftell(inB) >= BSize)) break;
            loadRecordNC(b, inB);
            continue;
        }

        // Caso ambos sejam válidos, o programa continua, agora verificando se os registros são iguais.
        if(a->idServidor == b->idServidor){

            // Caso sejam iguais, o registro do primeiro arquivo eh salvo e, lê-se um novo a e um novo b, garantindo que não havera repetição de registros.
            a->tamanhoRegistro = getActualSize(a);
            writeRecordInCluster(a, lastWritten, &c, output, h[0]);

            if(lastWritten) destroyRecord(lastWritten);
            lastWritten = a;
            a = createRecord();

            if((ftell(inA) >= ASize) || (ftell(inB) >= BSize)) break;
            loadRecordNC(a, inA);
            loadRecordNC(b, inB);
            continue;
        }
        else if(a->idServidor < b->idServidor){

            // Caso a seja menor que b, então ele eh salvo e um novo registro do primeiro arquivo eh salvo em a.
            a->tamanhoRegistro = getActualSize(a);
            writeRecordInCluster(a, lastWritten, &c, output, h[0]);

            if(lastWritten) destroyRecord(lastWritten);
            lastWritten = a;

            if(ftell(inA) >= ASize) break;
            a = createRecord();
            loadRecordNC(a, inA);
            continue;
        }
        else{

            // Caso contrario, b eh menor que a, então ele eh salvo e um novo registro do segundo arquivo eh salvo em b.
            b->tamanhoRegistro = getActualSize(b);
            writeRecordInCluster(b, lastWritten, &c, output, h[0]);

            if(lastWritten) destroyRecord(lastWritten);
            lastWritten = b;

            if((ftell(inB) >= BSize)) break;
            b = createRecord();
            loadRecordNC(b, inB);
            continue;
        }
    }

    // Apos algum dos arquivos acabarem, o programa continua escrevendo o que restar no outro arquivo, ate que ambos cheguem ao final.
    // Ou caso tenham acabado os arquivos, mas ainda falte escrever um registro (escreve o B, acabou o arquivo A, mas o ultimo registro de A ainda não foi escrito)
    if((!(ftell(inA) >= ASize)) || (lastWritten == b && lastWritten != a)) {
        b = NULL;
        while(1){
            if(a->removido == '*'){
                if((ftell(inA) >= ASize)) break;
                loadRecordNC(a, inA);
                continue;
            }

            a->tamanhoRegistro = getActualSize(a);
            writeRecordInCluster(a, lastWritten, &c, output, h[0]);

            if(lastWritten) destroyRecord(lastWritten);
            lastWritten = a;
            a = createRecord();

            if((ftell(inA) >= ASize)) break;
            loadRecordNC(a, inA);
        }
    }

    // Ou caso tenham acabado os arquivos, mas ainda falte escrever um registro (escreve o A, acabou o arquivo B, mas o ultimo registro de B ainda não foi escrito)
    if((!(ftell(inA) >= ASize)) || (lastWritten == a && lastWritten != b)) {
        a = NULL;
        while(1){
            if(b->removido == '*'){
                if((ftell(inB) >= BSize)) break;
                loadRecordNC(b, inB);
                continue;
            }
            b->tamanhoRegistro = getActualSize(b);
            writeRecordInCluster(b, lastWritten, &c, output, h[0]);

            if(lastWritten) destroyRecord(lastWritten);
            lastWritten = b;
            b = createRecord();

            if((ftell(inB) >= BSize)) break;
            loadRecordNC(b, inB);
        }
    }

    // O ultimo cluster aberto eh salvo no arquivo de saida.
    saveLastCluster(output, c);

    // O status de ambos os arquivos de entrada são atualizados com '1', garantindo consistencia
    h[0]->status = '1';
    h[1]->status = '1';
    saveHeaderNC(h[0], inA);
    saveHeaderNC(h[1], inB);
    fclose(inA);
    fclose(inB);

    // Salva-se agora o header no arquivo de saida como tendo status = '1' e topoLista = -1, pois não tem nenhum registro logicamente removido. 
    FILE* out = fopen(output, "r+b");
    h[0]->topoLista = -1;
    saveHeaderNC(h[0], out);
    fclose(out);


    // Variaveis auxiliares são liberadas.
    destroyRecord(lastWritten);
    if(a) destroyRecord(a);
    if(b) destroyRecord(b);
    destroyHeader(h[0]);
    destroyHeader(h[1]);
    destroyCluster(c);
    return 1;
}



/*
Definição:
    Procedimento que realiza a coincidencia de dois arquivos binários e escreve o resultado em um terceiro arquivo binário. A comparação dos registros é relativa
    ao id. Se um id é maior, o registro é maior. Se os ids são iguais, o registro é reescrito apenas uma vez.
Parâmetros:
    const char* input1: caminho para um dos arquivos binarios de onde serão pegos os registros para realizar a coincidencia.
    const char* input2: caminho para outro dos arquivos binarios de onde serão pegos os registros para realizar a coincidencia.
    const char* output: caminho para o arquivo onde serão escritos os registros mesclados de ambos arquivos de entrada.
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
int matchFile(const char* input1, const char* input2, const char* output){

    // Abre ambos os arquivos para leitura e verifica se obteve sucesso.
    FILE* inA = fopen(input1, "rb");
    if(!inA){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    FILE* inB = fopen(input2, "rb");
    if(!inB){
        fclose(inA);
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Carrega ambos os headers e verifica se o status eh '1' em ambos.
    Header* h[2];
    h[0] = createHeader();
    h[1] = createHeader();
    loadHeaderNC(h[0], inA);
    loadHeaderNC(h[1], inB);

    if(h[0]->status == '0' || h[1]->status == '0'){
        destroyHeader(h[0]);
        destroyHeader(h[1]);
        fclose(inA);
        fclose(inB);
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Coloca ambos os Headers para 0 indicando inconsistencia.
    h[0]->status = '0';
    saveHeaderNC(h[0], inA);
    h[1]->status = '0';
    saveHeaderNC(h[1], inB);

    // Salva o Header do arquivo de saida com status = '0';
    Cluster* c = createCluster();
    saveHeader(output, h[0], c);
    destroyCluster(c);


    // Encontra os tamanhos dos 
    fseek(inA, 0, SEEK_END);
    long ASize = ftell(inA);
    fseek(inB, 0, SEEK_END);
    long BSize = ftell(inB);
    

    // declara os dois records auxiliares a e b e os inicia com o primeiro valor de inA e inB.
    Record* a = createRecord();
    Record* b = createRecord();
    Record* lastWritten = NULL;

    // volta parao inicio do arquivo de dados (dps do header) para começar a leitura dos registros.
    fseek(inA, 32000, SEEK_SET);
    fseek(inB, 32000, SEEK_SET);

    loadRecordNC(a, inA);
    loadRecordNC(b, inB);


    c = createCluster();
    while(1){

        if(a->removido == '*'){
            if((ftell(inA) >= ASize)) break;
            loadRecordNC(a, inA);
            continue;
        }

        if(b->removido == '*'){
            if((ftell(inB) >= BSize)) break;
            loadRecordNC(b, inB);
            continue;
        }

        // Caso ambos sejam válidos, o programa continua, agora verificando se os registros são iguais.
        if(a->idServidor == b->idServidor){

            // Caso sejam iguais, o primeiro eh escrito e ambos são lidos.
            a->tamanhoRegistro = getActualSize(a);
            writeRecordInCluster(a, lastWritten, &c, output, h[0]);

            if(lastWritten) destroyRecord(lastWritten);
            lastWritten = a;
            a = createRecord();

            if((ftell(inA) >= ASize) || (ftell(inB) >= BSize)) break;
            loadRecordNC(a, inA);
            loadRecordNC(b, inB);
            continue;
        }
        else if(a->idServidor < b->idServidor){

            // Caso a seja menor que b, um novo registro do primeiro arquivo eh salvo em a.
            a->tamanhoRegistro = getActualSize(a);
            if((ftell(inA) >= ASize)) break;
            loadRecordNC(a, inA);
            continue;
        }
        else{

            // Caso contrario, b eh menor que a, então um novo registro do segundo arquivo eh salvo em b.
            b->tamanhoRegistro = getActualSize(b);
            if((ftell(inB) >= BSize)) break;
            loadRecordNC(b, inB);
            continue;
        }
    }

    // Apos algum dos arquivos acabarem, o programa acaba pois, sendo um match, não tem mais como os registros serem iguais.
    // O ultimo cluster aberto eh salvo no arquivo de saida.
    saveLastCluster(output, c);

    // O status de ambos os arquivos de entrada são atualizados com '1', garantindo consistencia
    h[0]->status = '1';
    h[1]->status = '1';
    saveHeaderNC(h[0], inA);
    saveHeaderNC(h[1], inB);
    fclose(inA);
    fclose(inB);

    // Salva-se agora o header no arquivo de saida como tendo status = '1' e topoLista = -1, pois não tem nenhum registro logicamente removido. 
    FILE* out = fopen(output, "r+b");
    h[0]->topoLista = -1;
    saveHeaderNC(h[0], out);
    fclose(out);


    // Variaveis auxiliares são liberadas.
    destroyRecord(lastWritten);
    destroyRecord(a);
    destroyRecord(b);
    destroyHeader(h[0]);
    destroyHeader(h[1]);
    destroyCluster(c);
    return 1;
}



/*
Definição:
    Função auxiliar que lê um arquivo binário e cria um indice na heap para cada registro lido (e válido) do arquivo, armazenando
    os indices em um vetor de indices.   
Parâmetros:
    Index* i: é a estrutura de indices que contem um vetor para armazenar informações de cada registro válido do arquivo
    FILE* bin: arquivo de onde serão lidos os registros para a criação do indice
*/
void getIndexFromBin(Index* i, FILE* bin){

    //Vai ate o final do arquivo e guarda seu tamanho.
    fseek(bin, 0, SEEK_END);
    long size = ftell(bin); 
    fseek(bin, 32000, SEEK_SET);
    Record* r = createRecord();
    long long offset = 32000;
    while(1){

        // Passa por todos os registros do arquivo (ate o final) e, caso seja um arquivo valido, seu offse e seu nome 
        // são salvos no vetor de indices.
        if(ftell(bin) >= size) break;
        offset = ftell(bin);
        loadRecordNC(r, bin);
        if(r->removido == '*') continue;
        if(!r->nomeServidor) continue;
        addIndexElem(i, createIndexElem(r->nomeServidor, offset));
    }

    // Depois de adicionar todos os elementos do indice eles são ordenados.
    sortIndex(i);
    destroyRecord(r);
    return;
}



/*
Definição:
    Função que cria um arquivo de indices a partir de um arquivo binário que possui todos os registros de dados, verificando quais possuem valores válidos
    para a indexação. A indexação é realizada em ordem alfabética e o campo utilizado para a sua realização é o 'nomeServidor'.
Parâmetros:
    const char* in: caminho para o arquivo binário de dados para o qual será realizada a indexação
    const char* out: caminho para o arquivo binário onde estará contido o indice para os registros
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
int makeIndex(const char* in, const char* out){
    FILE* bin = fopen(in, "rb");

    // Verifica a existencia e a integridade do arquivo.
    if(!bin){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    Header* h = createHeader();
    loadHeaderNC(h, bin);
    if(h->status != '1'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(bin);
        return 0;
    }

    // Setta o header para '0' indicando que o arquivo não esta integro
    h->status = '0';
    saveHeaderNC(h, bin);

    
    Index* i = createIndex(); // Cria o indice
    FILE* index = fopen(out, "wb");
    getIndexFromBin(i, bin); //  Insere os elementos no indice a partir do arquivo de dados.
    makeIndexFile(i, index); //  Cria um arquivo de indice a partir dos dados na heap.

    // Coloca o header de ambos os arquivos como '1' garantido integridade dos dados e libera as estruturas evitando vazamento de 
    //memoria.
    setIndexHeaderStatus('1', index); 
    h->status = '1';
    saveHeaderNC(h, bin);
    destroyHeader(h);
    destroyIndex(i);
    fclose(bin);
    fclose(index);
    return 1;
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
int compareIndexElemByOffset(const void* ea, const void* eb){
    IndexElem* a = *((IndexElem**)ea);
    IndexElem* b = *((IndexElem**)eb);
    return getIndexElemOffset(b) - getIndexElemOffset(a);
}



/*
Definição:
    Funcao que realiza a busca por 'nomeServidor' em um arquivo binário, porém, utilzando sempre o arquivo de indice, para otimizar a busca.
Parâmetros:
    const char* in: caminho para o arquivo binário do qual os registros devem ser lidos.
    const char* indexPath: caminho para o arquivo binário que contem o indice para o arquivo de dados.
*/
void searchIndex(const char* in, const char* indexPath){
    FILE* bin = fopen(in, "r+b");

    // Abre todos os arquivos e verifica integridade dos dados.
    // Caso não estejam integros a função simplesmente retorna sem fazer nada.
    // Caso esteja ela continua.
    if(!bin){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    FILE* index = fopen(indexPath, "r+b");
    if(!index){
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
    Header* h = createHeader();
    loadHeaderNC(h, bin);
    if(h->status != '1'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(index);
        fclose(bin);
        return;
    }
    char indexStatus;
    fread(&indexStatus, 1, 1, index);
    if(indexStatus != '1'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(index);
        fclose(bin);
        return;
    }

    // Coloca nos headers '0' pois os dados estão sendo manipulados e (possivelmente) modificados.
    h->status = '0';
    saveHeaderNC(h, bin);
    setIndexHeaderStatus('0', index);

    // cria o indice e carrega suas informações  partir do arquivo de indice.
    int nIndexPages;
    Index* i = createIndex();
    nIndexPages = loadIndex(i, index);

    Record* r = createRecord();


    // Inicializa um vetor que gardará todas as posições do indice que atendam aos criterios buscados.
    IndexElem** indexElemVector = malloc(1 * sizeof(IndexElem*));
    int foundRecords = 0;

    int nAccess = 1; // inicia o numero de acessos a disco como 1 (leitura do header apenas).
    

    // le o criterio de busca.
    int op;
    void* key = getValue(&op);
    int pos = binarySearchIndex(i, (char*)key); // busca a primeira posição.
    if(pos == -1){
        // Caso não tenha nenhum elemento no indice que condiza com a busca ele não existe.
        printf("Registro inexistente.\n");
    }
    else{

        //Se não, enquanto tiver elementos que atendam aos criterios de buscas eles são retirados do indice e colocado
        // no vetor auxiliar (indexElemVector).
        while(pos != -1){
            IndexElem* ie = getIndexElem(i, pos);
            indexElemVector = realloc(indexElemVector, ++foundRecords * sizeof(IndexElem*));
            indexElemVector[foundRecords - 1] = removeIndex(i, pos);
            pos = binarySearchIndex(i, (char*)key);
        }

        // Apos encontrar todos os elemento buscados, eles são ordenados de acordo com o offset para serem então printados
        // em ordem de aparicção.
        MS_sort(indexElemVector, foundRecords, sizeof(IndexElem*), compareIndexElemByOffset);


        // Itera-se sobre s registros encontrados, carregando-os e printando-os, caso uma nova pagina seja acessada,
        //nAcessa eh incrementado, controlando a quantidade de acessos ao arquivo de dados.
        int lastCluster = -1;
        for(int j = 0; j < foundRecords; j++){
            fseek(bin, getIndexElemOffset(indexElemVector[j]), SEEK_SET);
            loadRecordNC(r, bin);
            if(getIndexElemOffset(indexElemVector[j]) / 32000 != lastCluster) nAccess++;
            printSearched(r, h);
            lastCluster = (getIndexElemOffset(indexElemVector[j]) / 32000);
        } 

        // Ao final, a quantidade de acessos a dsco eh imprimida na tela.
        printf("Número de páginas de disco para carregar o arquivo de índice: %d\n", nIndexPages);
        printf("Número de páginas de disco para acessar o arquivo de dados: %d\n", nAccess);
    }


    destroyRecord(r);
    destroyIndex(i);

    // o staatus dos headers de ambos os arquivos são modificados novamente para '1' e as estruturas auxiliares são fechadas
    // e liberadas evitando vazamento de memoria.
    setIndexHeaderStatus('1', index);
    h->status = '1';
    saveHeaderNC(h, bin);
    destroyHeader(h);
    for(int j = 0; j < foundRecords; j++) destroyIndexElem(indexElemVector[j]);
    free(indexElemVector);
    free(key);
    fclose(bin);
    fclose(index);
    return;
}



/*
Definição:
    Função que remove todos os registros que atendam determinados criterios de busca. O algoritmo utiliza o nomeServidor como critério de busca
    e remove os registros, tomando o cuidado necessario para que a lista de remoção seja construida adequadamente.
Parâmetros:
    const char* in: caminho para o arquivo binário do qual os registros devem ser lidos.
    const char* indexPath: caminho para o arquivo binário que contem o indice para o arquivo de dados.
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
int removeIndexedRecord(const char* in, const char* indexPath){

    // Abre todos os arquivos e verifica integridade dos dados.
    // Caso não estejam integros a função simplesmente retorna sem fazer nada.
    // Caso esteja ela continua.
    FILE* bin = fopen(in, "r+b");
    if(!bin){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    FILE* index = fopen(indexPath, "r+b");
    if(!index){
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return 0;
    }
    Header* h = createHeader();
    loadHeaderNC(h, bin);
    if(h->status != '1'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(index);
        fclose(bin);
        return 0;
    }
    char indexStatus;
    fread(&indexStatus, 1, 1, index);
    if(indexStatus != '1'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(index);
        fclose(bin);
        return 0;
    }

    // Coloca nos headers '0' pois os dados estão sendo manipulados e (possivelmente) modificados.
    h->status = '0';
    saveHeaderNC(h, bin);
    setIndexHeaderStatus('0', index);

    // cria o indice e carrega suas informações  partir do arquivo de indice.
    int nIndexPages;
    Index* i = createIndex();
    nIndexPages = loadIndex(i, index);

    // Cria a lista de removidos a partir do encadeamento ja existente no arquivo de dados.
    List* l = createList();
    int max = -1;
    int min = -1;
    makeRemovedList(l, bin, h->topoLista, &min, &max);

    Record* r = createRecord();

    // Le a quantidade de buscas que serão realizadas para remover os elementos.
    int nSearches = 0;
    scanf("%d", &nSearches);
    for(int j = 0; j < nSearches; j++){

        //Para cada busca, le-se o argumento buscado no nomeServidor.
        int op;
        void* key = getValueRemove(&op);
        int pos = binarySearchIndex(i, (char*)key); // Uma busca binaria eh realizada no indece.
        while(pos != -1){

            // Enquanto existirem elementos no indice que correspondem a busca, o algoritmo de remoção eh chamado para
            // o registro indicado pelo indice.
            IndexElem* ie = getIndexElem(i, pos);
            fseek(bin, getIndexElemOffset(ie), SEEK_SET);
            loadRecordNC(r, bin);
            removeRecordInPos(r, bin, l, &min, &max, j); // faz as modificações necessarias no arquivo e na lista.
            destroyIndexElem(removeIndex(i, pos)); 
            pos = binarySearchIndex(i, (char*)key);
        }
        free(key);
    }
    
    // O arquivo de indice eh fechado e re-escrito.
    fclose(index);
    index = fopen(indexPath, "wb");
    
    makeIndexFile(i, index);

    destroyRecord(r);
    destroyIndex(i);

    // tem, então seu header salvo como '1'.
    h->status = '1';
    setIndexHeaderStatus('1', index);

    // a lista eh ordenada e o encadeamento do arquivo de dados eh re feito.
    l = sortByPriority(l, -1, nSearches - 1);
    l = bucketSort(l, min, max);
    makeFileRemovedList(l, bin, h);
    destroyList(l);

    // O header do arquivo de daods eh atualizado e as estruturas auxiliares liberadas evitando vazamento de memoria.
    saveHeaderNC(h, bin);
    destroyHeader(h);
    fclose(index);
    fclose(bin);
    return 1;
}



/*
Definição:
    Metodo criado para adicionar um novo registro no arquivo levando em consideração os espacos removidos logicamente e usando a tecnica de first-fit (que, por ser uma lista 
    ordenada crescentemente em função do tamanho, acaba se tornando um best-fit) para escrever o novo registra na posição correta. Caso não seja encontrado nenhuma posição 
    satisfatoria entra as removidas ele sera salvo no final do arquivo. Após a inserção o elemento é adicionado no indice, que é reordenado e reescrito no disco.
Parâmetros:
    const char* in: caminho para o arquivo binário do qual os registros devem ser lidos.
    const char* indexPath: caminho para o arquivo binário que contem o indice para o arquivo de dados.
Retorno:
    int - int com função de booleano, 0 - caso a função tenha encontrado algum erro e 1 caso tenha ocorrido corretamente.
*/
int addIndexedRecord(const char* in, const char* indexPath){

    // Abre todos os arquivos e verifica integridade dos dados.
    // Caso não estejam integros a função simplesmente retorna sem fazer nada.
    // Caso esteja ela continua.
    FILE* bin = fopen(in, "r+b");
    if(!bin){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    FILE* index = fopen(indexPath, "r+b");
    if(!index){
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return 0;
    }
    Header* h = createHeader();
    loadHeaderNC(h, bin);
    if(h->status != '1'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(index);
        fclose(bin);
        return 0;
    }
    char indexStatus;
    fread(&indexStatus, 1, 1, index);
    if(indexStatus != '1'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(index);
        fclose(bin);
        return 0;
    }

    // Coloca nos headers '0' pois os dados estão sendo manipulados e (possivelmente) modificados.
    h->status = '0';
    saveHeaderNC(h, bin);
    setIndexHeaderStatus('0', index);

    // cria o indice e carrega suas informações  partir do arquivo de indice.
    int nIndexPages;
    Index* i = createIndex();
    nIndexPages = loadIndex(i, index);

    // Cria a lista de removidos a partir do encadeamento ja existente no arquivo de dados.
    List* l = createList();
    int max = -1;
    int min = -1;
    makeRemovedList(l, bin, h->topoLista, &min, &max);

    
    // le a quantidade de registros que serão adicionados.
    int nAdd;
    scanf("%d", &nAdd);
    for(int j = 0; j < nAdd; j++){

        // Para cada um deles, um egistro eh lido a partir da entrada padrão.
        Record* r = createRecord();
        getRecordStdin(r);

        // se tiver um nomeServidor, ele eh adicionado no indice e então usa-se o algoritmo de adição de registro
        // garantindo que ele será salvo na posiçõ correta.
        if(r->nomeServidor)
            addIndexElem(i, createIndexElem(r->nomeServidor, saveNewRecord(r, l, bin, h)));
        else saveNewRecord(r, l, bin, h);
        destroyRecord(r);
    }
     

    // O arquivo de indice eh fechado e re-escrito.
    fclose(index);
    index = fopen(indexPath, "wb");
    sortIndex(i);
    makeIndexFile(i, index);
    destroyIndex(i);


    h->status = '1';
    setIndexHeaderStatus('1', index);

    // O encadeamento do arquivo de dados eh re-feito.
    makeFileRemovedList(l, bin, h);
    destroyList(l);

    // Os headers são escritos com '1' e as estruturas extras são liberadas evitando vazamento de memoria.
    saveHeaderNC(h, bin);
    destroyHeader(h);
    fclose(index);
    fclose(bin);
    return 1;
}



/*
Definição:
    Procedimento auxiliar que realiza uma busca utilizando o arquivo de indices, criada de modo que possa ser realizada a comparação 
    com o procedimento que não utiliza indices (em numero de paginas acessadas)    
Parâmetros:
    FILE* bin: stream de dados de onde serão lidos os registros
    Index* i: estrutura de indices que armazena "ponteiros" para os registro no arquivo de dados
    void* searchKey: valor que será buscado nos registros (char*, nomeServidor sempre)
    Header* h: ponteiro para a estrutura que possui os dados do cabeçalho do arquivo de dados
    int indexClusters: numero de páginas de disco acessadas para carregar o arquivo de indices para a heap
Retorno:
    int - numero de acessos a disco realizados pela busca
*/
int searchWithIndexToCompare(FILE* bin, Index* i, void* searchKey, Header* h, int indexClusters){

    // Funcioanmento igual a função searchIndex, apenas com modificações para que os argumentos sejam recebidos de forma
    // diferente e que a quantidade de paginas acessada seja retornada, possibilitando a comparação
    int nIndexPages = indexClusters;
    Record* r = createRecord();

    IndexElem** indexElemVector = malloc(1 * sizeof(IndexElem*));
    int foundRecords = 0;

    int nAccess = 1;
    int op;
    void* key = searchKey;
    int pos = binarySearchIndex(i, (char*)key);
    if(pos == -1) printf("Registro inexistente.\n");
    else{
        while(pos != -1){
            IndexElem* ie = getIndexElem(i, pos);
            indexElemVector = realloc(indexElemVector, ++foundRecords * sizeof(IndexElem*));
            indexElemVector[foundRecords - 1] = removeIndex(i, pos);
            pos = binarySearchIndex(i, (char*)key);
        }

        MS_sort(indexElemVector, foundRecords, sizeof(IndexElem*), compareIndexElemByOffset);
        
        int lastCluster = -1;
        for(int j = 0; j < foundRecords; j++){
            fseek(bin, getIndexElemOffset(indexElemVector[j]), SEEK_SET);
            loadRecordNC(r, bin);
            if(getIndexElemOffset(indexElemVector[j]) / 32000 != lastCluster) nAccess++;
            printSearched(r, h);
            lastCluster = getIndexElemOffset(indexElemVector[j]) / 32000;
        } 

    }
    
    printf("Número de páginas de disco para carregar o arquivo de índice: %d\n", nIndexPages);
    printf("Número de páginas de disco para acessar o arquivo de dados: %d\n", nAccess);

    destroyRecord(r);

    for(int j = 0; j < foundRecords; j++) destroyIndexElem(indexElemVector[j]);
    free(indexElemVector);
    return nAccess;
}



/*
Definição:
    Procedimento auxiliar que realiza uma busca sem utlizar o arquivo de indices, criada de modo que possa ser realizada a comparação 
    com o procedimento que utiliza indices (em numero de paginas acessadas)    
Parâmetros:
    FILE* fp: stream de dados de onde serão lidos os registros
    int searchOp: inteiro que indica qual o campo será utilizado na realização da busca
    void* key: valor que será buscado nos registros (char*, nomeServidor sempre)
    Header* h: ponteiro para a estrutura que possui os dados do cabeçalho do arquivo de dados
Retorno:
    int - numero de acessos a disco realizados pela busca
*/
int searchWithoutIndexToCompare(FILE* fp, int searchOp, void* key, Header* h){

    // Funcioanmento igual a função searchBin, apenas com modificações para que os argumentos sejam recebidos de forma
    // diferente e que a quantidade de paginas acessada seja retornada, possibilitando a comparação
    fseek(fp, 32000, SEEK_SET);
    int nClusters = 0;
    int op = searchOp;
    int printedRecordsCount = 0;
    void* value = key;
    Cluster* c = createCluster();
    Record* r = createRecord();
    nClusters++;
    if(loadCluster(c, fp) == EOF){
        destroyCluster(c);
        destroyHeader(h);
        destroyRecord(r);
        fclose(fp);
        printf("Número de páginas de disco acessadas: %d\n", nClusters);
        return nClusters;
    }
    nClusters++;
    while(1){
        //Le ate o final do arquivo.
        if(!loadRecord(r, c)) break;
        if(clusterEnd(c)){
            //Caso o Cluster atual esteja no final, ele eh destruido e um novo eh carregado.
            destroyCluster(c);
            c = createCluster();
            loadCluster(c, fp);
            nClusters++;
        }

        //guarda a quantidade de Records printados.
        printedRecordsCount += printIfSearchedRecord(r, op, value, h);

        //Caso a busca seja realizada por id, tendo ja algum Record printado a função ja termina.
        if(!op && printedRecordsCount) break;
    }
    destroyCluster(c);
    destroyRecord(r);

    //Se nenhum atendeu as exigencias, uma mensagem de erro eh impressa.
    if(!printedRecordsCount) printf("Registro inexistente.\n");  

    //Se não o numero de paginas acessadas eh printado.
    printf("Número de páginas de disco acessadas: %d\n", nClusters);
    return nClusters;
}



/*
Definição:
    Função que compara as buscas realizadas das duas formas possíveis: com a utilização do arquivo de indices ou sem a utilização do mesmo.
    Compara o número de paginas de disco acessadas em cada um dos casos e imprime a diferença entre eles.   
Parâmetros:
    const char* in: caminho para o arquivo de dados que contém todos os registros de dados 
    const char* indexPath: caminho para o arquivo que possui o indice para o arquivo de dados
*/
void compareSearches(const char* in, const char* indexPath){

    // Abre todos os arquivos e verifica integridade dos dados.
    // Caso não estejam integros a função simplesmente retorna sem fazer nada.
    // Caso esteja ela continua.
    FILE* bin = fopen(in, "r+b");
    if(!bin){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    FILE* index = fopen(indexPath, "r+b");
    if(!index){
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
    Header* h = createHeader();
    loadHeaderNC(h, bin);
    if(h->status != '1'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(index);
        fclose(bin);
        return;
    }
    char indexStatus;
    fread(&indexStatus, 1, 1, index);
    if(indexStatus != '1'){
        printf("Falha no processamento do arquivo.\n");
        destroyHeader(h);
        fclose(index);
        fclose(bin);
        return;
    }

    // Coloca nos headers '0' pois os dados estão sendo manipulados e (possivelmente) modificados.
    h->status = '0';
    saveHeaderNC(h, bin);
    setIndexHeaderStatus('0', index);

    // cria o indice e carrega suas informações  partir do arquivo de indice.
    int nIndexPages;
    Index* i = createIndex();
    nIndexPages = loadIndex(i, index);

    // o argumento da busca eh lido para ser passado em ambas as funções igualmente.
    int op;
    void* key = getValue(&op);

    // Ambas as buscas (com e sem indice) são chamadas e o seus resultados são salvos para comparação.
    printf("*** Realizando a busca sem o auxílio de índice\n");
    int nClustersNI = searchWithoutIndexToCompare(bin, op, key, h);
    printf("*** Realizando a busca com o auxílio de um índice secundário fortemente ligado\n");
    int nClustersI = searchWithIndexToCompare(bin, i, key, h, nIndexPages);


    // A diferençã de paginas de discos eh apresentada na tela.
    printf("\nA diferença no número de páginas de disco acessadas: %d\n", nClustersNI - nClustersI);
    

    destroyIndex(i);

    // Headers são atualizados com '1' e estruturas auxiliares são liberadas evitando vazamento de memoria.
    h->status = '1';
    setIndexHeaderStatus('1', index);

    saveHeaderNC(h, bin);
    destroyHeader(h);
    free(key);
    fclose(index);
    fclose(bin);
    return;
}



/*
Definição:
    Função principal do programa, serve apenas para controlar a operação desejada e chamar a função que a resolverá.
*/
int main(void){
    int op;
    char file[50];
    char file2[50];
    char output[50];
    scanf("%d", &op);
    switch (op){
        case 1:     
            scanf("%s", file);
            makeBin(file, "arquivoTrab1.bin");
            break;
        case 2:
            scanf("%s", file);
            readBin(file);
            break;
        case 3:
            scanf("%s", file);
            searchBin(file);
            break;
        case 4:
            scanf("%s", file);
            if(removeRecord(file)) binarioNaTela2(file);
            break;
        case 5:
            scanf("%s", file);
            if(addRecord(file)) binarioNaTela2(file);
            break;
        case 6:
            scanf("%s", file);
            if(updateRecord(file)) binarioNaTela2(file);
            break;
        case 7:
            scanf("%s", file);
            scanf("%s", output);
            if(sortFile(file, output)) binarioNaTela2(output);
            break;
        case 8:
            scanf("%s", file);
            scanf("%s", file2);
            scanf("%s", output);
            if(mergeFile(file, file2, output)) binarioNaTela2(output);
            break;
        case 9:
            scanf("%s", file);
            scanf("%s", file2);
            scanf("%s", output);
            if(matchFile(file, file2, output)) binarioNaTela2(output);
            break;
        case 10:
            scanf("%s", file);
            scanf("%s", output);
            if(makeIndex(file, output)) binarioNaTela2(output);
            break;
        case 11:
            scanf("%s", file);
            scanf("%s", file2);
            searchIndex(file, file2);
            break;
        case 12:
            scanf("%s", file);
            scanf("%s", file2);
            if(removeIndexedRecord(file, file2)) binarioNaTela2(file2);
            break;
        case 13:
            scanf("%s", file);
            scanf("%s", file2);
            if(addIndexedRecord(file, file2)) binarioNaTela2(file2);
            break;
        case 14:
            scanf("%s", file);
            scanf("%s", file2);
            compareSearches(file, file2);
            break;

        default:
            break;
    }
    return 0;
}

 