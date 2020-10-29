#ifndef REMOTEFILE_IMP_H
#define REMOTEFILE_IMP_H
#include"mpi_manager.h"
#include"remotefile.h"

class remotefile_imp
{
    remotefile* rfile=0x00;
    MPI_Comm * parent;

public:
    remotefile_imp();
    void run();
    ~remotefile_imp();
};

#endif // REMOTEFILE_IMP_H
