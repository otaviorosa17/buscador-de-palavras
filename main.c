#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

int linesNum;
int wordsNum;

char** storeLines(char* filename){ // lê o arquivo dado como parâmetro e devolve um array de strings, sendo cada string uma linha do texto
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE* fptr = fopen(filename, "r");
    
    if (fptr==NULL) {
        printf("erro ao abrir o arquivo\n");
    }

    int i = 0;
    int max = 0;
    while ((read = getline(&line, &len, fptr))!=-1) {
        if (max<read) {
            max = read;
        }
        i++;
    }

    fseek(fptr, 0,SEEK_SET);

    char** text = (char**) malloc(((i+1)*sizeof(char*)));

    if (text==NULL) {
        printf("erro ao alocar memória para text\n");
    }

    int j = 0;
    while ((read = getline(&line, &len, fptr))!=-1) {
        line[strcspn(line,"\n")] = '\0';
        text[j] = strdup(line);
        j++;
    }
    text[j] = NULL;
    free(line);
    fclose(fptr);
    return text;
}

char*** indexWords(char ** text) { // recebe um array de strings e devolve um array de linhas, onde cada linha é uma array de palavras
    int i,j,k;
    wordsNum = 0;
    char*** result = (char***)malloc(1);
    for (i=0; text[i]; i++) {
        char* word = strtok(text[i], " \t-");
        char** words = (char**)malloc(1*sizeof(char*));
        words[0] = NULL;
        for (j=0; word; j++) {
            words = (char**)realloc(words, (j+2)*sizeof(char*));
            words[j] = strdup(word);
            word = strtok(NULL, " \t-");
        }
        words[j] = NULL;
        result = (char***)realloc(result,(i+2)*sizeof(char**));
        result[i] = (char**)malloc((j+1)*sizeof(char*));
        for (k=0; k<=j; k++) {
            result[i][k] = words[k];
            wordsNum++;
        }
        free(words);
        free(text[i]);
    }
    free(text);
    result[i] = NULL;
    return result;
}

char*** formatText(char* filename) { // recebe o nome de um arquivo .txt como parâmetro 
    char** buffer = storeLines(filename); // e devolve e devolve um array de linhas,
    char *** text = indexWords(buffer); // onde cada linha é uma array de palavras 
    return text;
}

// criar função que organize as palavras em ordem alfabética

char** createList(char*** text) { // cria uma lista de palavras com letras minúsculas
    int i,j,k,l,index;
    int totalWords = 0;
    index = 0;
    for (i=0;text[i]!=NULL;i++) {
        for (j=0;text[i][j]!=NULL;j++) {
            totalWords++;
        }
    }
    char** list = (char**)malloc((totalWords+1)*sizeof(char*));
    
    if(list == NULL) {
        printf("erro ao alocar list");
    }

    for (i=0;text[i]!=NULL;i++) {
        for (j=0;text[i][j]!=NULL;j++) {
            int len = strlen(text[i][j]);
            char* word = (char*)malloc((len+1)*sizeof(char));
            if (word == NULL) {
                printf("erro ao alocar word");
                exit(EXIT_FAILURE);
            }
            int wordIndex = 0;
            for (k=0;k<len;k++) {
                if(isalpha(text[i][j][k])) {
                    word[wordIndex] = tolower((unsigned char)text[i][j][k]);
                    wordIndex++;
                }
            }
            word[wordIndex] = '\0';
            list[index++] = word;
        }
    }    
    list[index] = NULL;
    return list;
}

int listLength(char** list) { // devolve o tamanho de uma lista de palavras
    int i;
    for(i=0;list[i]!= NULL;i++);
    return i;
}





struct lista { //definição de struct lista
    int line;
    char* word;
};

int partition(struct lista* data, int left, int right) {
    char* pivot = data[right].word;
    int i = left-1;
    struct lista aux;
    for (int j=left; j<right ; j++) {
        if(data[j].word) {
            if (pivot == NULL || strcmp(data[j].word, pivot) <= 0) {
                i++;
                struct lista aux = data[i];
                data[i] = data[j];
                data[j] = aux;
            }
        }
        else if (pivot == NULL) {
            i++;
            struct lista aux = data[i];
            data[i] = data[j];
            data[j] = aux;
        }
    }
    aux = data[i+1];
    data[i+1] = data[right];
    data[right] = aux;
    return i+1;
}

struct lista* structListSortQuicksort(struct lista* data, int left, int right) {
    if (left<right) {
        int pivot_index = partition(data, left, right);
        structListSortQuicksort(data, left, pivot_index-1);
        structListSortQuicksort(data, pivot_index + 1, right);
    }
    return data;
}

struct lista * createStructList(char*** text) { // cria um array de structs a partir do tipo char*** text
    char** list = createList(text);
    int len = listLength(list);
    struct lista* data = (struct lista*)malloc((len+1)*sizeof(struct lista));
    if (data == NULL) {
        printf("erro em createStructList()\n");
        exit(EXIT_FAILURE);
    }
    int i,j,index;
    index = 0;
    for (i=0;text[i]!=NULL;i++) {
        for (j=0;text[i][j]!=NULL;j++) {
            data[index].line = i;
            data[index].word = strdup(list[index]);
            index++;
        }
    }
    linesNum = i;
    for (i=0;text[i]!=NULL;i++) {
        for (j=0;text[i][j]!=NULL;j++) {
            free(text[i][j]);
        }
        free(text[i]);
    }
    data[index].word = NULL;
    free(text);
    for(index=0;index<len;index++) {
        free(list[index]);
    }
    free(list);


    data = structListSortQuicksort(data,0,len);
    return data;
}


int structListLen(struct lista* data) { // devolve o tamanho de um "array de structs"
    int i;
    for(i = 0; data[i].word; i++);
    return i;
}

struct lista* searchWordList(struct lista* data, char* word, int len) { //busca binária de uma palavra em um "array de structs"
    int i, j,k, mid, left, right,check, length;
    check=0;
    left = 0;
    right = len-1;
    while (left<=right) {
        mid = (left + right)/2;
        if (strcmp(data[mid].word,word)==0) {
            check++;
            break;
        }
        if (strcmp(data[mid].word,word)<0) {
            left = mid +1;
        }
        if (strcmp(data[mid].word,word)>0) {
            right = mid-1;
        }
    }

    if (check == 1) {
        for (i = mid; i >= 0 && strcmp(data[i].word, word) == 0; i--);
        for (j = mid + 1; j < len && strcmp(data[j].word, word) == 0; j++);
        length = j - i - 1;
        struct lista* foundedWords = (struct lista*)malloc((length+1) * sizeof(struct lista));
        if (foundedWords != NULL) {
            for (k = 0; k < length; k++) {
                if (i+k+1<len) {
                    foundedWords[k] = data[i + 1 + k];
                }
            }
            foundedWords[length].word = NULL;
        }

        return foundedWords;
    }

    return NULL;
}

void n2sortArray(int* array,int len) { // ordena um array com complexidade n ao quadrado
    int i,j,aux;
    for(i=0;i<len;i++) { 
        for (j = i+1; j<len; j++) {
            if (array[i]>array[j]) {
                aux = array[i];
                array[i] = array[j];
                array[j] = aux;
            }
        }
    }
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int arrayPartition(int arr[], int left, int right) {
    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[right]);
    return i + 1;
}

void arrayQuicksort(int arr[], int left, int right) {
    if (left < right) {
        int pivotIndex = arrayPartition(arr, left, right);

        arrayQuicksort(arr, left, pivotIndex - 1);
        arrayQuicksort(arr, pivotIndex + 1, right);
    }
}


char* toLowerString (char* string) { // transforma todos os caracteres de uma string para letra minuscula
    int i;
    int len = strlen(string);
    char* result = (char*)malloc((strlen(string)+1)*sizeof(char));
    if (result==NULL) {
        printf("erro na de alocação em toLowerString()");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < len; i++) {
        result[i] = tolower((unsigned char)string[i]);
    }
    result[len] = '\0';
    return result;
}

typedef struct no_arvore {
    char* word;
    int counter;
    int lines[1000000];
    struct no_arvore* l;
    struct no_arvore* r;
} No;

typedef struct {
    No* root;
} Arvore;

Arvore* createTree() {
    Arvore* arvore = (Arvore*)malloc(sizeof(Arvore));
    arvore->root = NULL;
    return arvore; 
}

int insert_ord_rec(No* root, No* new, int line) {

    if(strcmp(new->word,root->word)==0) {
        root->counter++;
        int i;
        for(i=0; root->lines[i] != -1; i++);
        root->lines[i] = line;
        root->lines[i+1] = -1;
        free(new->word);
        free(new);
        return 0;
    }
    else if (strcmp(new->word,root->word)<0) {
        if(root->l) return insert_ord_rec(root->l,new,line);
        else root->l = new;
        return 1;
    }
    else if (strcmp(new->word,root->word)>0) {
        if(root->r) return insert_ord_rec(root->r,new,line);
        else root->r = new;
        return 1;
    }
}

int insert_ord(Arvore* arvore, char* word, int line) {
    No* new = (No*)malloc(sizeof(No));
    new->word = strdup(word);
    new->l = new->r = NULL;
    new->counter = 1;
    new->lines[0] = line;
    new->lines[1] = -1;
    if (arvore->root) return insert_ord_rec(arvore->root,new,line);
    arvore->root = new;
    return 1;
}

Arvore* createStructTree(char*** text) {
    Arvore* arvore = createTree();
    char** list = createList(text);
    int len = listLength(list);
    int i,j,index;
    index=0;
    for (i=0;text[i]!=NULL;i++) {
        for (j=0;text[i][j]!=NULL;j++) {
            insert_ord(arvore,list[index],i);
            index++;
        }
    }
    linesNum = i;
    return arvore;
}

void imprime_rec(No * node){
	if(node){
		imprime_rec(node->l);
		printf(" %s(%d)[%d]\n", node->word,node->counter,node->lines[0]);
		imprime_rec(node->r);
	}
}

void imprime(Arvore * arvore){

	printf("Elementos na arvore:\n");
	imprime_rec(arvore->root);
	printf("\n");
}

No * busca_bin_rec(No * node, char* word){

	if(node){
		if(strcmp(node->word,word)==0) return node;
		if(strcmp(word,node->word)<0) return busca_bin_rec(node->l, word);
		return busca_bin_rec(node->r, word);
	}

	return NULL;
}

No * busca_bin(Arvore * arvore, char* word){
	return busca_bin_rec(arvore->root, word);	
}

struct lista* searchWordTree(Arvore* arvore, char* word, int len) {
    No* node = busca_bin(arvore, word);
}

void freeNo(No* node) {
    if (node) {
        free(node->word);
        freeNo(node->l);
        freeNo(node->r);
        free(node);
    }
}

void freeArvore(Arvore* arvore) {
    if (arvore) {
        freeNo(arvore->root);
        free(arvore);
    }
}

void freeStructListaElement(struct lista* element) {
    free(element->word);
}

void freeStructListaArray(struct lista* data) {
    if (data) {
        for (int i = 0; data[i].word != NULL; i++) {
            freeStructListaElement(&data[i]);
        }
        free(data);
    }
}

int main(int argc, char** agrv) {
    if (1) {
        char input[255];
        char comando[255];
        char palavra[255];
        char*** text = formatText(agrv[1]);
        char** lines = storeLines(agrv[1]);
        if (strcmp(agrv[2],"lista")==0) {
            clock_t t;
            t = clock();
            struct lista* data = createStructList(text);
            int len = structListLen(data);
            t = clock() - t;
            double tempoConstruir,tempoBusca;
            tempoConstruir = 1000*((double)t)/CLOCKS_PER_SEC;
            printf("Tipo de indice: '%s'\n", agrv[2]);
            printf("Arquivo texto: '%s'\n", agrv[1]);
            printf("Numero de linhas no arquivo: %d\n", linesNum);
            printf("Tempo para carregar o arquivo e construir o indice: %.4lf ms\n",tempoConstruir);
            while(1) {
                printf("> ");
                if (fgets(input, sizeof(input), stdin) == NULL) {
                    break;
                }
                input[strcspn(input, "\n")] = '\0';
                if (sscanf(input, "%s %s", comando, palavra) == 2) {
                    if (strcmp(comando,"busca")==0) {
                        t = clock();
                        char* lowedPalavra = toLowerString(palavra);
                        struct lista* palavrasEncontradas = searchWordList(data, lowedPalavra, len);
                        t = clock() - t; 
                        if (palavrasEncontradas != NULL) {
                            int ocorrencias = structListLen(palavrasEncontradas);
                            int* linhas = (int*)malloc(ocorrencias*sizeof(int));
                            int i,j;
                            for (i=0; i<ocorrencias; i++) {
                                linhas[i] = palavrasEncontradas[i].line;
                            }
                            arrayQuicksort(linhas,0,ocorrencias-1);
                            printf("Existem %d ocorrências da palavra '%s' na(s) seguinte(s) linha(s):\n", ocorrencias, palavra);
                            
                            printf("%05d: %s\n", linhas[0]+1, lines[linhas[0]]);
                            for(j=1;j<ocorrencias;j++) {
                                if(linhas[j]!=linhas[j-1])
                                    printf("%05d: %s\n", linhas[j]+1, lines[linhas[j]]);
                            }
                            tempoBusca = 1000*((double)t)/CLOCKS_PER_SEC;
                            printf("Tempo de busca: %.4lf ms\n", tempoBusca);
                            free(linhas);
                        }
                        else {
                            printf("Palavra '%s' nao encontrada.\n", palavra);
                        }
                        free(lowedPalavra);
                        free(palavrasEncontradas);
                    }
                    else {
                        printf("Opcao invalida!\n");
                    }
                }
                else if (strcmp(comando,"fim")==0) {
                    break;
                }
                else {
                    printf("Opcao invalida!\n");
                }
            }
            freeStructListaArray(data);
        }
        else if (strcmp(agrv[2],"arvore")==0) {
            clock_t t;
            t = clock();
            Arvore* arvore = createStructTree(text);
            t = clock() - t;
            double tempoConstruir,tempoBusca;
            tempoConstruir = 1000*((double)t)/CLOCKS_PER_SEC;
            printf("Tipo de indice: '%s'\n", agrv[2]);
            printf("Arquivo texto: '%s'\n", agrv[1]);
            printf("Numero de linhas no arquivo: %d\n", linesNum);
            printf("Tempo para carregar o arquivo e construir o indice: %.4lf ms\n",tempoConstruir);
            while(1) {
                printf("> ");
                if (fgets(input, sizeof(input), stdin) == NULL) {
                    break;
                }
                input[strcspn(input, "\n")] = '\0';
                if (sscanf(input, "%s %s", comando, palavra) == 2) {
                    if (strcmp(comando,"busca")==0) {
                        t = clock();
                        char* lowedPalavra = toLowerString(palavra);
                        No* node = busca_bin(arvore,lowedPalavra);
                        t = clock() - t; 
                        if (node != NULL) {
                            int i,j;
                            n2sortArray(node->lines,node->counter);
                            printf("Existem %d ocorrências da palavra '%s' na(s) seguinte(s) linha(s):\n", node->counter, palavra);
                            
                            printf("%05d: %s\n", node->lines[0]+1, lines[node->lines[0]]);
                            for(j=1;j<node->counter;j++) {
                                if(node->lines[j]!=node->lines[j-1])
                                    printf("%05d: %s\n", node->lines[j]+1, lines[node->lines[j]]);
                            }
                            tempoBusca = 1000*((double)t)/CLOCKS_PER_SEC;
                            printf("Tempo de busca: %.4lf ms\n", tempoBusca);
                        }
                        else {
                            printf("Palavra '%s' nao encontrada.\n", palavra);
                        }
                        free(lowedPalavra);
                    }
                    else {
                        printf("Opcao invalida!\n");
                    }
                }
                else if (strcmp(comando,"fim")==0) {
                    break;
                }
                else {
                    printf("Opcao invalida!\n");
                }
            }
            freeArvore(arvore);
        }
        for(int k = 0; k<listLength(lines); k++) {
            free(lines[k]);
        }
        free(lines);
        return 0;
    }
    return 1;
}

