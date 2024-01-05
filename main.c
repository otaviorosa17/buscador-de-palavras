#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

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
        text[j] = (char*) malloc((max+1)*sizeof(char*));
        strcpy(text[j],line);
        j++;
    }
    text[j] = NULL;
    fclose(fptr);
    return text;
}

char*** indexWords(char ** text) { // recebe um array de strings e devolve um array de linhas, onde cada linha é uma array de palavras
    int i,j,k;
    char*** result = (char***)malloc((sizeof(char**)));
    for (i=0; text[i]; i++) {
        char* word = strtok(text[i], " \t");
        char** words = (char**)malloc(sizeof(char*));
        words[0] = NULL;
        for (j=0; word; j++) {
            words = (char**)realloc(words, (j+2)*sizeof(char*));
            words[j] = strdup(word);
            word = strtok(NULL, " \t");
        }
        words[j] = NULL;
        result = (char***)realloc(result,(i+2)*sizeof(char**));
        result[i] = (char**)malloc((j+1)*sizeof(char*));
        for (k=0; k<=j; k++) {
            result[i][k] = strdup(words[k]);
            free(words[k]);
        }
        free(words);
    }
    result[i] = NULL;
    return result;
}

char*** formatText(char* filename) { // recebe o nome de um arquivo .txt como parâmetro 
    char** buffer = storeLines(filename); // e devolve e devolve um array de linhas, onde cada linha é uma array de palavras
    char *** text = indexWords(buffer); // onde cada linha é uma array de palavras 
    return text;
}

// criar função que organize as palavras em ordem alfabética

char** createList(char*** text) { // cria uma lista de palavras com letras minúsculas e sem pontuanção no início/meio das palavras
    int i,j,k,l,index;

    for (i=0;text[i]!=NULL;i++) {
        for (j=0;text[i][j]!=NULL;j++) {
            for (k=0;text[i][j][k]!='\0';k++) {
                text[i][j][k] = tolower((unsigned char)(text[i][j][k]));
                if (text[i][j][k]<'a' || text[i][j][k]>'z') {
                    for(l=k; text[i][j][l]!='\0'; l++) {
                        text[i][j][l] = text[i][j][l+1];
                    }
                }
                text[i][j][k] = tolower((unsigned char)(text[i][j][k]));
            }
        }
    }

    char** list = (char**)malloc((((i-1)*(j-1))+1)*sizeof(char*));
    
    index = 0;
    for (i=0;text[i]!=NULL;i++) {
        for (j=0;text[i][j]!=NULL;j++) {
            list[index] = text[i][j];
            index++;
        }
    }
    list[index] = NULL;
    return list;
}

int listLength(char** list) {
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



char** listSort(char** list) {

    int len = listLength(list);
    char* aux;
    for (int i = 0; i<len; i++) {
        for (int j = i+1; j<len; j++) {
            if (strcmp(list[i],list[j])>0) {
                aux = list[i];
                list[i] = list[j];
                list[j] = aux;
            }
        }
    }
    return list;
}

struct lista {
    int line;
    int order;
    char* word;
};

struct lista * indexList(char*** text) {
    char** list = createList(text);
    int len = listLength(list);
    struct lista* data = (struct lista*)malloc(len*sizeof(struct lista));
    int i,j,index;
    index = 0;
    for (i=0;text[i]!=NULL;i++) {
        for (j=0;text[i][j]!=NULL;j++) {
            data[index].line = i;
            data[index].word = text[i][j];
            index++;
        }
    }

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
    
    return data;
}

int main() {
    char*** text = formatText("test.txt");
    struct lista* data = indexList(text);

    printf("%s %d", data[3].word, data[3].line);
    return 0;
}