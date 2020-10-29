#ifndef PRUEBACLASE_IMP_H
#define PRUEBACLASE_IMP_H
#include "pruebaclase.h"
#include "mpi_manager.h"

// Implementación
class pruebaClase_imp
{
    pruebaClase* pclase = nullptr;
    MPI_Comm * parent;
public:
    pruebaClase_imp();
    void run();
    ~pruebaClase_imp();
};

#endif // PRUEBACLASE_IMP_H
