#ifndef REMOTEFILE_H
#define REMOTEFILE_H


class remotefile
{
public:
    remotefile();
    // Abre archivo "filename" y copia su contenido en "buff", inicia "bufflen" a su tamaño
    void readfile(char* filename,char** buff, unsigned long int* bufflen);
};

#endif // REMOTEFILE_H
