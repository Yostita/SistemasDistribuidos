#include "utils.h"
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void readLine(int socket,std::vector<std::string*> *line)
{
    std::string* str=new std::string("");
    char c=' ';
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof (tv));
    while(c!='\n')
    {
        read(socket,&c,1);
        if((c==' ')||(c=='\n'))
        {
            if(str->length()>0)
            {
                line->push_back(str);
                str=new std::string("");
            }
        }else if(c!='\r')
        {
            str->append(1,c);
        }
    }
}

void readLines(int socket,std::vector<std::vector<std::string*> *> *lines)
{
    std::vector<std::string*>* line;
    do{
        line=new std::vector<std::string*>();
        readLine(socket,line);
        lines->push_back(line);
    }
    while(line->size()!=0);
}

void readPostLine(int socket,std::vector<std::string*> *line,int length)
{
    std::string* str=new std::string("");
    char c=' ';
    int pos=0;
    
    while(pos!=length)
    {
        read(socket,&c,1);
        
        if(c=='&')
        {
            line->push_back(str);
            str=new std::string("");
            pos++;
        }else if (pos == (length - 1))
        {
            str->append(1,c);
            line->push_back(str);
            pos = length;
        }
        else
        {
            str->append(1,c);
            pos++;
        }

    }
    
}

void printPostLines(std::vector<std::string*> *postLine)
{
    for(int i=0;i<postLine->size();i++)
    {
    std::cout<<*((*postLine)[i])<<"\n";
    }
}



void deleteLines(std::vector<std::vector<std::string*>*>* lines)
{
    for(unsigned long int i=0;i<lines->size();i++)
    {
        std::vector<std::string*> *v=(*lines)[i];

        for(unsigned long int j=0;j<v->size();j++)
        {
            delete (*v)[j];
        }
        delete v;
    }
}


void printLines(std::vector<std::vector<std::string*>*>* lines)
{
    for(unsigned long int i=0;i<lines->size();i++)
    {
        std::vector<std::string*> *v=(*lines)[i];

        for(unsigned long int j=0;j<v->size();j++)
        {
            std::cout<<(*((*v)[j]))<<" ";
        }
        std::cout<<"\n";
    }
}

httpServer::httpRequest_t getRequestType(std::vector<std::string*>* line)
{
    httpServer::httpRequest_t req=httpServer::GET;
    std::string *request=(*line)[0];

    if(!request->compare("GET"))
        {
            req=httpServer::GET;
        }
    else if(!request->compare("HEAD"))
        {
            req=httpServer::HEAD;
        }
    else if(!request->compare("POST"))
        {
            req=httpServer::POST;
        }
    else if(!request->compare("PUT"))
        {
            req=httpServer::PUT;
        }
    else if(!request->compare("DELETE"))
        {
            req=httpServer::DELETE;
        }
    else if(!request->compare("CONNECT"))
        {
            req=httpServer::CONNECT;
        }
    else if(!request->compare("OPTIONS"))
        {
            req=httpServer::OPTIONS;
        }
    else if(!request->compare("TRACE"))
        {
            req=httpServer::TRACE;
        }


    return req;

}


void createHeader(char** httpHeader,unsigned long int * headerLen,
             const char* responseCode,
             const char* contentType,
             unsigned long int contentLength)
{
    std::string msg="";
    msg+= "HTTP/1.1 "+std::string(responseCode)+"\r\n";
    msg+= "Content-type: "+std::string(contentType)+"\r\n";
    msg+= "Content-length: "+std::to_string(contentLength)+"\r\n";
    msg+= "Accept-Ranges: bytes\r\n";
    msg+= "\r\n";
    *headerLen=msg.length();
    *httpHeader=new char[(*headerLen)+1];
    memcpy(*httpHeader,msg.c_str(),(*headerLen)+1);
}

void readFile(char* file,char** buff,unsigned long int* fileLen)
{
    FILE* fin=fopen(file,"r");
    unsigned long int flen=0;
    if(fin!=NULL)
    {
        fseek(fin, 0, SEEK_END);
        flen=ftell(fin);
        fseek(fin, 0, SEEK_SET);
        char* content=new char[flen];
        fread(content,1,flen,fin);
        fclose(fin);
        *buff=content;
        *fileLen=flen;
    }else
    {
        *buff=NULL;
        *fileLen=0;
    }

}

std::vector<std::string*>* strCat(std::string* str,char sep)
{
    std::vector<std::string*>* result=new std::vector<std::string*>();
    std::string* strres=new std::string("");
    char c=' ';
    unsigned long int count=0;
    while(count<str->length())
    {
        c=str->at(count);
        if(c==sep)
        {
            if(strres->length()>0)
            {
                result->push_back(strres);
                strres=new std::string("");
            }
        }else
        {
            strres->append(1,c);
        }
        count++;
    }
    result->push_back(strres);
    return result;
}


char* getFromPost(std::vector<std::string*> &postLine, std::string param)
{

    char* result=NULL;
    for(unsigned long int i=0;i<postLine.size();i++)
    {
        std::vector<std::string*> *v=strCat(postLine[i],'=');

        if(((*v)[0])->compare(param)==0)
        {
            result=new char[((*v)[1]->size())+1];
            memcpy(result,((*v)[1]->c_str()),((*v)[1]->size())+1);
        }
        for(int i=0;i<v->size();i++){
            delete (*v)[i];
        }
        //delete (*v)[0];
        //delete (*v)[1];
        delete v;
    }

    return result;
}



void getFileFromPost(int socket,int postLen,char* &fileName,char* &data, int &dataLength )
{
    std::vector<std::string*>* boundary;
    std::vector<std::vector<std::string*>*> lines;
    std::vector<std::string*>* line;
    boundary=new std::vector<std::string*>();
    int filesize=postLen;
    readLine(socket,boundary);
    filesize-=boundary->at(0)->size()+2;// add \r\n
    do{
        line=new std::vector<std::string*>();
        readLine(socket,line);
        char* fname=getFromPost(*line,"filename");
        if(fname!=nullptr)
            fileName=fname;
        lines.push_back(line);
    }while(line->size()!=0);

    for(unsigned long int i=0;i<lines.size();i++)
    {
        line=lines.at(i);
        for(unsigned long int j=0;j<line->size();j++)
        {
            filesize-=line->at(j)->size();
            filesize--;
        }
        filesize--;
    }
    filesize--;//spurious \r
    filesize-=2;//\r\n after data
    filesize-=boundary->at(0)->size()+4;// adds --\r\n
    data=new char[filesize];
    dataLength=filesize;
    char c='\0';
    for(int i=0;i<filesize;i++)
    {
            read(socket,&c,1);
            data[i]=c;
    }
}

