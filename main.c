#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>


char names[5][20];
char cities[5][10];
char ages[5][10];
struct record{
    char * name;
    char * city;
    char * age;
};
struct record recordlist[5];



pthread_mutex_t namemutex;
pthread_mutex_t citymutex;
pthread_mutex_t agemutex;
pthread_cond_t namesfinish;
pthread_cond_t citiesfinish;
pthread_cond_t agesfinish;

bool namesDone,citiesDone,agesDone;

void removeStringTrailingNewline(char *str) {
    if (str == NULL)
        return;
    int length = strlen(str);
    if (str[length-1] == '\n')
        str[length-1]  = '\0';
}

void *readNames(void *input){
    pthread_mutex_lock(&namemutex);

    printf("Thread Started for reading Names \n");
    int count = 0,charidx = 0;
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    stream = fopen("./Keywords.txt", "r");
    if (stream == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, stream)) != -1) {
        if (count < 5)
        {
            if(line != (char) 0){
                strcpy(names[charidx],line);
                charidx++;
            }
        }
        count++;
    }

    free(line);
    fclose(stream);
    for(int i = 0; i<5;i++){
        printf("Name Found %s", names[i]);
    }
    namesDone = true;
    pthread_mutex_unlock(&namemutex);
    pthread_cond_signal(&namesfinish);
}



void *readCities(void *input){
    pthread_mutex_lock(&citymutex);

    printf("Thread Started for reading Cities \n");
    int count = 0,charidx = 0;
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    stream = fopen("./Keywords.txt", "r");
    if (stream == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, stream)) != -1) {
        if (count >= 5 && count < 10)
        {
            if(line != (char) 0){
                strcpy(cities[charidx],line);
                charidx++;
            }
        }
        count++;
    }

    free(line);
    fclose(stream);
    for(int i = 0; i<5;i++){
        printf("City Found %s", cities[i]);
    }
    citiesDone = true;
    pthread_mutex_unlock(&citymutex);
    pthread_cond_signal(&citiesfinish);
}

void *readAges(void *input){
    pthread_mutex_lock(&agemutex);

    printf("Thread Started for reading Ages\n");
    int count = 0,charidx = 0;
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    stream = fopen("./Keywords.txt", "r");
    if (stream == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, stream)) != -1) {
        if (count >= 10 && count < 15)
        {
            if(line != (char) 0){


                strcpy(ages[charidx],line);
                charidx++;
            }
        }
        count++;
    }

    free(line);
    fclose(stream);
    for(int i = 0; i<5;i++){
        printf("Age Found %s", ages[i]);
    }
    agesDone = true;
    pthread_mutex_unlock(&agemutex);
    pthread_cond_signal(&agesfinish);
}

void writeToMemory(){
    pthread_mutex_lock(&namemutex);
    pthread_mutex_lock(&citymutex);
    pthread_mutex_lock(&agemutex);

    pthread_cond_wait(&namesfinish, &namemutex);
    pthread_cond_wait(&citiesfinish, &citymutex);
    pthread_cond_wait(&agesfinish, &agemutex);

    for(int i = 0; i < 5; i++){
        struct record tempr;
        tempr.name = names[i];
        tempr.city = cities[i];
        tempr.age = ages[i];

        recordlist[i] = tempr;
    }

    printf("\nAll Done");


    for(int i = 0; i < 5; i++){
        struct record rec = recordlist[i];
        printf("==New Record== \n");
        printf("Name: %sCity: %sAge: %s",rec.name,rec.city,rec.age);
    }
}




int main() {
    printf("Starting The Programme\n");


    pthread_t tid1,tid2,tid3,tid4;


    pthread_create(&tid1, NULL, readNames, NULL);
    pthread_join(tid1, NULL);
    pthread_create(&tid2, NULL, readCities, NULL);
    pthread_join(tid2, NULL);
    pthread_create(&tid3, NULL, readAges, NULL);
    pthread_join(tid3, NULL);

    bool flag = false;




    while (!flag){
        if(namesDone && citiesDone && agesDone){
            flag = true;
        }
        sleep(1);
    }

    writeToMemory();

    return 0;
}



//void readlinebyline(){
//    FILE * fp;
//    char * line = NULL;
//    size_t len = 0;
//    ssize_t read;
//
//    fp = fopen("./Keywords.txt", "r");
//    if (fp == NULL){
//        printf("File not found");
//        exit(EXIT_FAILURE);
//    }
//
//
//    while ((read = getline(&line, &len, fp)) != -1) {
//        printf("Retrieved line of length %zu:\n", read);
//        printf("%s", line);
//    }
//
//    fclose(fp);
//    if (line)
//        free(line);
//    exit(EXIT_SUCCESS);
//}

//char* readWholeFiletoBuffer(){
//    FILE *f = fopen("./Keywords.txt", "rb");
//    fseek(f, 0, SEEK_END);
//    long fsize = ftell(f);
//    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */
//
//    char *string = malloc(fsize + 1);
//    fread(string, 1, fsize, f);
//    fclose(f);
//
//    return string;
//
//
//}








