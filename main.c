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

    char** text = (char**) malloc((i*sizeof(char*)));

    if (text==NULL) {
        printf("erro ao alocar memória para text\n");
    }

    int j = 0;
    while ((read = getline(&line, &len, fptr))!=-1) {
        line[strcspn(line,"\n")] = '\0';
        text[j] = (char*) malloc((max+1)*sizeof(char));
        strcpy(text[j],line);
        j++;
    }
    text[j] = NULL;
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
    }
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



/* int partition(char*** list, int left, int right) { tentativa quicksort
    char* pivot = *list[right];
    int i = left-1;
    char* aux;
    for (int j=left; j<=left-1 ; j++) {
        if (strcmp(*list[j],pivot)<=0) {
            i++;
            aux = *list[i];
            *list[i] = *list[j];
            *list[j] = aux;
        }
    }
    aux = *list[i+1];
    *list[i+1] = *list[right];
    *list[right] = aux;
    return i+1;
}

char** listSortQuicksort(char*** list, int left, int right) {
    if (left<right) {
        int pivot_index = partition(list, left, right);
        listSortQuicksort(list, left, pivot_index-1);
        listSortQuicksort(list, pivot_index + 1, right);
    }
    return *list;
} */


struct lista { //definição de struct lista
    int line;
    char* word;
};


struct lista * createStructList(char*** text) { // cria um array de structs a partir do tipo char*** text
    char** list = createList(text);
    int len = listLength(list);
    struct lista* data = (struct lista*)malloc(len*sizeof(struct lista));
    int i,j,index;
    index = 0;
    for (i=0;text[i]!=NULL;i++) {
        for (j=0;text[i][j]!=NULL;j++) {
            data[index].line = i;
            data[index].word = strdup(list[index]);
            //free(list[index]);
            index++;
        }
    }
    linesNum = i;
    data[index].word = NULL;

    struct lista aux;
    for(i=0;i<len;i++) { 
        for (j = i+1; j<len; j++) {
            if (strcmp(data[i].word,data[j].word)>0) {
                aux = data[i];
                data[i] = data[j];
                data[j] = aux;
            }
        }
    }
    // free(list);
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

char* toLowerString (char* string) { // transforma todos os caracteres de uma string para letra minuscula
    int i;
    char* result = (char*)malloc((strlen(string)+1)*sizeof(char));
    for (i=0;i<=strlen(string)+1; i++) {
        result[i] = tolower(string[i]);
    }
    return result;
}

typedef struct no_arvore {
    char* word;
    int counter;
    int lines[255];
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
        for(i=0;root->lines[i]!=-1; i++);
        root->lines[i] = line;
        root->lines[i+1] = -1;
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
    new->word = word;
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
            //free(list[index]);
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


int main() {
    char estrutura[255];
    char nomeArquivo[255];
    char comando[255];
    char palavra[255];
    scanf("%s %s", nomeArquivo, estrutura);
    char*** text = formatText(nomeArquivo);
    if (strcmp(estrutura,"lista")==0) {
        clock_t t;
        t = clock();
        struct lista* data = createStructList(text);
        int len = structListLen(data);
        t = clock() - t;
        double tempoConstruir,tempoBusca;
        tempoConstruir = 1000*((double)t)/CLOCKS_PER_SEC;
        printf("Tipo de indice: '%s'\n", estrutura);
        printf("Arquivo texto: '%s'\n", nomeArquivo);
        printf("Numero de linhas no arquivo: %d\n", linesNum);
        printf("Tempo para carregar o arquivo e construir o indice: %.4lf ms\n",tempoConstruir);
        while(1) {
            printf("> ");
            scanf("%s",comando);
            if (strcmp(comando,"fim")==0) {
                break;
            }
            else if (strcmp(comando,"busca")==0) {
                scanf("%s", palavra);
                t = clock();
                char* lowedPalavra = toLowerString(palavra);
                struct lista* palavrasEncontradas = searchWordList(data, lowedPalavra, len);
                t = clock() - t; 
                if (palavrasEncontradas != NULL) {
                    int ocorrencias = structListLen(palavrasEncontradas);
                    int* linhas = (int*)malloc(ocorrencias*sizeof(int));
                    char** lines = storeLines(nomeArquivo);
                    int i,j;
                    for (i=0; i<ocorrencias; i++) {
                        linhas[i] = palavrasEncontradas[i].line;
                    }
                    n2sortArray(linhas,ocorrencias);
                    printf("Existem %d ocorrências da palavra '%s' na(s) seguinte(s) linha(s):\n", ocorrencias, palavra);
                    
                    printf("%05d: %s\n", linhas[0]+1, lines[linhas[0]]);
                    for(j=1;j<ocorrencias;j++) {
                        if(linhas[j]!=linhas[j-1])
                            printf("%05d: %s\n", linhas[j]+1, lines[linhas[j]]);
                    }
                    tempoBusca = 1000*((double)t)/CLOCKS_PER_SEC;
                    printf("Tempo de busca: %.4lf ms\n", tempoBusca);
                }
                else {
                    printf("Palavra '%s' nao encontrada.\n", palavra);
                }
            }
            else {
                printf("Opcao invalida!\n");
            }
        }
    }
    else if (strcmp(estrutura,"arvore")==0) {
        clock_t t;
        t = clock();
        Arvore* arvore = createStructTree(text);
        t = clock() - t;
        double tempoConstruir,tempoBusca;
        tempoConstruir = 1000*((double)t)/CLOCKS_PER_SEC;
        printf("Tipo de indice: '%s'\n", estrutura);
        printf("Arquivo texto: '%s'\n", nomeArquivo);
        printf("Numero de linhas no arquivo: %d\n", linesNum);
        printf("Tempo para carregar o arquivo e construir o indice: %.4lf ms\n",tempoConstruir);
        while(1) {
            printf("> ");
            scanf("%s",comando);
            if (strcmp(comando,"fim")==0) {
                break;
            }
            else if (strcmp(comando,"busca")==0) {
                scanf("%s", palavra);
                t = clock();
                char* lowedPalavra = toLowerString(palavra);
                No* node = busca_bin(arvore,lowedPalavra);
                t = clock() - t; 
                if (node != NULL) {
                    char** lines = storeLines(nomeArquivo);
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
            }
            else {
                printf("Opcao invalida!\n");
            }
        }
    }
}

