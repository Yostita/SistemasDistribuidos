#ifndef MPI_MANAGER_H
#define MPI_MANAGER_H
#include <vector>
#include <openmpi/mpi.h>
using namespace std;
#include <string>
class MPI_Manager
{
    static bool init;
    static std::vector<MPI_Comm*> comms;
public:
    MPI_Manager();
    static void Init();
    static void Finalize();
    static MPI_Comm* Instanciate(char* processName, char *ip);
};

#endif // MPI_MANAGER_H
