#ifndef FILEMANAGER_IMP_H
#define FILEMANAGER_IMP_H
#include"mpi_manager.h"
#include"filemanager.h"

class filemanager_imp
{
    FileManager* fileman=0x00;
    MPI_Comm * parent;

public:
    filemanager_imp();
    void run();
    ~filemanager_imp();
};

#endif // FILEMANAGER_IMP_H
