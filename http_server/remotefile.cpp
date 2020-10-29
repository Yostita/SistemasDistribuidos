#include "remotefile.h"
#include "stdio.h"
#include "stdlib.h"

remotefile::remotefile()
{

}

void remotefile::readfile(char* filename,char** buff, unsigned long int* bufflen)
{
    FILE* fin = fopen(filename,"r");

    if (fin == nullptr)
    {
        *buff = nullptr;
        *bufflen = 0;
    }
    else
    {
        unsigned long int flen = 0;
        fseek(fin,0,SEEK_END);  // Posicionarse al final del fichero
        flen = ftell(fin);      // Averiguar posicion
        fseek(fin,0,SEEK_SET);  // Posicionarse al inicio
        *buff = new char[flen];
        fread(*buff,1,flen,fin);
        *bufflen = flen + 2;
        (*buff)[flen]='\0';
        fclose(fin);
    }

}
