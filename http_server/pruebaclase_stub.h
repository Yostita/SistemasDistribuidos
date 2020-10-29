#ifndef PRUEBACLASE_STUB_H
#define PRUEBACLASE_STUB_H
#include "mpi_manager.h"

#define OP_EXIT 0
#define OP_HOLAMUNDO 1
#define OP_SUMA 2

// Interfaz del cliente
class pruebaclase_stub
{
    MPI_Comm* comm;
public:
    pruebaclase_stub(char*ip);
    void holaMundo();
    int suma(int a, int b);
    ~pruebaclase_stub();
};

#endif // PRUEBACLASE_STUB_H
