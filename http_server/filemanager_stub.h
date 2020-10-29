#ifndef FILEMANAGER_STUB_H
#ifndef FILEMANAGER_STUB_H
#define FILEMANAGER_STUB_H
#include "filemanager.h"
#include "mpi_manager.h"


#define OP_EXIT      0
#define OP_READFILE  1
#define OP_WRITEFILE 2
#define OP_LISTFILE  3

using namespace std;

class FileManager_stub
{
    MPI_Comm * comm;

   

public:
    FileManager_stub(char*ip);
    vector<string*>* listFiles();
    void readFile(char* fileName, char* &data, unsigned  int & dataLength);
    void writeFile(char* fileName, char* data, unsigned  int dataLength);
    void freeListedFiles(vector<string*>* fileList);
    ~FileManager_stub();
};


#endif // FILEMANAGER_STUB_H



#endif // FILEMANAGER_STUB_H
