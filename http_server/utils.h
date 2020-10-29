#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "httpserver.h"

void readLine (int socket,   std::vector<std::string*> *line);
void readLines(int socket,std::vector<std::vector<std::string*> *> *lines);
void printPostLines(std::vector<std::string*> *postLine);
void readPostLine(int socket,std::vector<std::string*> *line,int length);
void deleteLines(std::vector<std::vector<std::string*>*>* lines);
void printLines(std::vector<std::vector<std::string*>*>* lines);

httpServer::httpRequest_t getRequestType(std::vector<std::string*>* line);

void createHeader(char** httpHeader, unsigned long int* headerLen,
             const char *responseCode,
             const char *contentType,
             unsigned long contentLength);

void readFile(char* file,char** buff,unsigned long int* fileLen);

char* getFromPost(std::vector<std::string*> &postLine, std::string param);
void getFileFromPost(int socket,int postLen,char* &fileName,char* &data, int &dataLength );

#endif // UTILS_H
