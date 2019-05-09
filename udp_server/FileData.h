//
// Created by Max Pokidaylo on 2019-05-09.
//

#ifndef UDPSERVER_FILEDATA_H
#define UDPSERVER_FILEDATA_H

#endif //UDPSERVER_FILEDATA_H

#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <strings.h>
#include <zconf.h>
#include <stdio.h>
#include <sys/socket.h>

void option2(int newsd, char command[1024], struct sockaddr_in clientaddr, int clientlen);
void reaction1(int newsd,char command[1024], struct sockaddr_in clientaddr, int clientlen);
void reaction3(int newsd,char command[1024], struct sockaddr_in clientaddr, int clientlen);
void reaction4(int newsd,char command[1024], struct sockaddr_in clientaddr, int clientlen);
void reaction5(int newsd,char command[1024], struct sockaddr_in clientaddr, int clientlen);
bool valueCheck(char value[]);
char* removeWord(char str[100],char key[100]);

void option2(int newsd, char command[1024], struct sockaddr_in clientaddr, int clientlen) {
    command[strlen(command) - 1] = '\0';
    switch (command[0]) {
        case '.': /* reaction 1 and 2 */
            reaction1(newsd,command,clientaddr,clientlen);
            break;
        case '+': /* reaction 3 */
            reaction3(newsd,command,clientaddr,clientlen);
            break;
        case '*': /* reaction 4 */
            reaction4(newsd,command,clientaddr,clientlen);
            break;
        case '-': /* reaction 5 */
            reaction5(newsd,command,clientaddr,clientlen);
            break;

        default:
            sendto(newsd, "The command don't exits, try again.\n", strlen("The command don't exits, try again.\n"), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
            break;
    }
}


void reaction1(int newsd,char command[1024], struct sockaddr_in clientaddr, int clientlen) {
    char *token;
    char *path = malloc(256 * sizeof(*path)) , *fileName = malloc(256 * sizeof(*fileName));
    char buffer[256];
    bzero(buffer, sizeof(buffer));
    char directoryName[100];
    bzero(directoryName, sizeof(directoryName));
    int chdirCount = 0;

    token = strtok(command, " "); /* returns the path */
    strcpy(path, token);
    token = strtok(NULL,"\0"); /* returns the value */
    path = strtok(path, ".");
    if (token != NULL) { /* file input into the path */
        while (1) {
            if (path == NULL) {
                chdir("..");
                rmdir(fileName);
                strcat(fileName, ".txt"); /* creates <filename.txt> string */
                FILE *file = fopen(fileName, "w"); /* if txt file don't exits - creates it, if exist - overwrites it */
                fprintf(file, "%s\n", token); /* writes the value into the txt file */
                fclose(file);
                break;
            } else {
                /*
                 * TODO: if text file exist change the folder name to: 'folderName_'.
                 */
                mkdir(path, 0755); /* creates the directory */
                chdir(strcat(strcat(getcwd(directoryName, 100), "/"), path)); /* steps into the directory */
                chdirCount++;
            }
            strcpy(fileName, path);
            path = strtok(NULL, ".");
        }
    } else { /* file output */
        while (1) {
            if (path == NULL) {
                chdir("..");
                rmdir(fileName);
                strcat(fileName, ".txt"); /* creates <filename.txt> string */
                FILE *file = fopen(fileName, "r");
                if (file == NULL) {
                    sendto(newsd, "The file don't exist.\n", strlen("The file don't exist.\n"), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
                    break;
                }
                fgets(buffer, 256, file);
                fclose(file);
                sendto(newsd, buffer, strlen(buffer), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
                break;
            } else {
                mkdir(path, 0755); /* creates the directory */
                chdir(strcat(strcat(getcwd(directoryName, 100), "/"), path)); /* steps into the directory */
                chdirCount++;
            }
            strcpy(fileName, path);
            path = strtok(NULL, ".");
        }
    }

    /* returns to the main directory */
    for (int i = 0 ; i < chdirCount - 1 ; i++) {
        chdir("..");
    }
}

void reaction3(int newsd, char command[1024], struct sockaddr_in clientaddr, int clientlen) {
    char *token, *path, *fileName = malloc(256 * sizeof(*fileName));
    char buffer[256];
    bzero(buffer, sizeof(buffer));
    char directoryName[100];
    bzero(directoryName, sizeof(directoryName));
    int chdirCount = 0;

    token = strtok(command,"+");
    path = strtok(token, ".");
    while (1) {
        if (path == NULL) {
            chdir("..");
            rmdir(fileName);
            strcat(fileName, ".txt"); /* creates <filename.txt> string */
            FILE *file = fopen(fileName, "r");
            if (file == NULL) {
                sendto(newsd, "The file don't exist.\n", strlen("The file don't exist.\n"), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
                break;
            }
            fgets(buffer, 256, file);
            fclose(file);
            if (valueCheck(buffer)) {
                int num = atoi(buffer);
                num += 1;
                file = fopen(fileName, "w");
                fprintf(file, "%d\n", num);
                fclose(file);
            }
            break;
        } else {
            mkdir(path, 0755); /* creates the directory */
            chdir(strcat(strcat(getcwd(directoryName, 100), "/"), path)); /* steps into the directory */
            chdirCount++;
        }
        strcpy(fileName, path);
        path = strtok(NULL, ".");
    }

    /* returns to the main directory */
    for (int i = 0; i < chdirCount - 1; i++) {
        chdir("..");
    }
}

void reaction4(int newsd,char command[1024], struct sockaddr_in clientaddr, int clientlen){
    char *token, *path, *folderName = malloc(256 * sizeof(*folderName));;
    char directoryName[100];
    bzero(directoryName, sizeof(directoryName));
    char buffer[256];
    bzero(buffer, sizeof(buffer));
    DIR *d;
    struct dirent *dir;
    int chdirCount = 0;
    int sum = 0;

    token = strtok(command, "*"); /* returns the path */
    path = strtok(token, "."); /* returns the first token in the path */
    while (1) {
        if (path == NULL) {
            d = opendir(".");
            if (d) {
                while ((dir = readdir(d)) != NULL) {
                    FILE *file = fopen(dir->d_name, "r");
                    fgets(buffer, 256, file);
                    fclose(file);
                    if (valueCheck(buffer)) sum += atoi(buffer);
                }
                closedir(d);
                sprintf(buffer, "%d\n", sum);
                strcat(buffer,"\0");
                sendto(newsd, buffer, strlen(buffer), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
            } else {
                sendto(newsd, "0\n", strlen("0\n"), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
            }
            break;
        }
        else {
            int react = chdir(strcat(strcat(getcwd(directoryName, 100), "/"), path));
            if (react != 0) {
                sendto(newsd, "Folder don't exist.\n", strlen("Folder don't exist.\n"), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
                break;
            }
            chdirCount++;
        }
        strcpy(folderName,path);
        path = strtok(NULL, ".");
    }

    /* returns to the main directory */
    for (int i = 0; i < chdirCount; i++) {
        chdir("..");
    }
}

void reaction5(int newsd,char command[1024], struct sockaddr_in clientaddr, int clientlen) {
    char *originalCommand = malloc(1024 * sizeof(*originalCommand));;
    printf("comand: %s\n", command);
    char *token, *path, *folderName = malloc(256 * sizeof(*folderName));
    char directoryName[100];
    bzero(directoryName, sizeof(directoryName));
    char buffer[256], secBuffer[256];
    bzero(buffer, sizeof(buffer));
    DIR *d;
    struct dirent *dir;
    int chdirCount = 0;
    long max = 0;
    char *lastFile = malloc(1024 * sizeof(*lastFile));

    strcpy(originalCommand,command);
    token = strtok(command, "-"); /* returns the path */
    path = strtok(token, "."); /* returns the first token in the path */
    while (1) {
        if (path == NULL) {
            struct stat filesStat; /* TODO: change on linux */
            d = opendir(".");
            if (d) { /* finds the last modified file in the directory */
                while ((dir = readdir(d)) != NULL) {
                    if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,".DS_Store") != 0) {
                        stat(dir->d_name, &filesStat);
                        if (filesStat.st_mtime > max) {
                            max = filesStat.st_mtime;
                            strcpy(lastFile, dir->d_name);
                        }
                    }
                }
                FILE *file = fopen(lastFile, "r");
                fgets(buffer, 256, file);
                fclose(file);
                strcpy(secBuffer,removeWord(lastFile,".txt"));
                originalCommand[0] = '.';
                strcat(originalCommand, ".");
                strcat(originalCommand,secBuffer);
                strcat(originalCommand," ");
                strcat(originalCommand, buffer);
                sendto(newsd, originalCommand, strlen(originalCommand), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
                break;
            }
            else break;
        }
        else {
            int react = chdir(strcat(strcat(getcwd(directoryName, 100), "/"), path));
            if (react != 0) {
                sendto(newsd, "Folder don't exist.\n", strlen("Folder don't exist.\n"), 0, (struct sockaddr *) &clientaddr, (socklen_t) clientlen);
                break;
            }
            chdirCount++;
        }
        strcpy(folderName,path);
        path = strtok(NULL, ".");
    }

    /* returns to the main directory */
    for (int i = 0; i < chdirCount; i++) {
        chdir("..");
    }

}


/**
 *  returns true if the value is a number, false if not.
 */
bool valueCheck(char value[1024]) {
    for (int i = 0 ; i < strlen(value) - 1 ; i++) {
        if (!(value[i] >= '0' && value[i] <= '9')) {
            return false;
        }
    }
    return true;
}

char* removeWord(char str[100],char key[100]) {
    int i = 0,j = 0, k;
    for(i = 0 ; i < strlen(str) ; i++) {
        while( str[i] == key[j] && key[j] != '\0') {
            for(k = i ; str[k] != '\0' ; k++) {
                str[k] = str[k + 1]; //to override the word by next words
            }
            j++;
        }
    }
    return str;
}


//token = strtok(command, "."); /* returns the first token */
//mkdir(token, 0755); /* creates the directory */
//chdir(strcat(strcat(getcwd(s, 100), "/"), token)); /* steps into the directory */
//s[strlen(s)] = '\0';
//token = strtok(NULL, " "); /* returns the second token */
//strcat(txtFile,token);
//strcat(txtFile,".txt"); /* creates <filename.txt> string */
//FILE* file = fopen(txtFile, "w"); /* if txt file don't exits - creates it, if exist - overwrites it */
//token = strtok(NULL, "\0"); /* returns the last token */
//fprintf(file,"%s\n",token); /* writes the value into the txt file */
//fclose(file);
//chdir(".."); /* returns to the main directory */