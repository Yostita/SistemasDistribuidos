#include "remotefile_stub.h"
#include "mpi_manager.h"
#include <string.h>

remotefile_stub::remotefile_stub(char*ip)
{
    MPI_Manager::Init();
    this->comm = MPI_Manager::Instanciate("rpc_remotefile",ip);   // En remoto se adapta al host
}

void remotefile_stub::readfile(char* filename,char** buff, unsigned long int* bufflen)
{
    *buff = nullptr;
    *bufflen = 0;
    // enviar tipo de operacion
    int op = OP_READFILE;
    MPI_Status status;
    MPI_Send(&op, 1, MPI_INT,0, 0, *(this->comm));
    //enviar parámetros
    unsigned long int filenameLen = strlen(filename) + 1;
    MPI_Send(&filenameLen, 1, MPI_LONG,0, 0, *(this->comm));
    MPI_Send(filename, (int)filenameLen, MPI_CHAR,0,0,*(this->comm));
    //esperar resultado
    //recibir tamaño
    MPI_Recv(bufflen, 1, MPI_LONG,0, 0, *(this->comm), &status);
    //reservar buffer
    *buff = new char[*bufflen];
    //recibir buffer
    MPI_Recv(*buff,*bufflen,MPI_LONG,0,0,*(this->comm),&status);
}

remotefile_stub::~remotefile_stub()
{
    int op = OP_EXIT;
    MPI_Send(&op, 1, MPI_INT,0, 0, *(this->comm));
}
