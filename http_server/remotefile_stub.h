#ifndef REMOTEFILE_STUB_H
#define REMOTEFILE_STUB_H
#include "mpi_manager.h"

#define OP_EXIT 0
#define OP_READFILE 1
#define OP_SUMA 2


class remotefile_stub
{
    MPI_Comm* comm;
public:
    remotefile_stub(char*ip);
    void readfile(char* filename,char** buff, unsigned long int* bufflen);
    ~remotefile_stub();
};

#endif // REMOTEFILE_STUB_H
