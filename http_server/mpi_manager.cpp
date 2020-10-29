#include "mpi_manager.h"

std::vector<MPI_Comm*> MPI_Manager::comms;
bool MPI_Manager::init = false;

MPI_Manager::MPI_Manager()
{

}

void MPI_Manager::Init()
{
    if (!MPI_Manager::init)
    {
        int argc = 0;
        char** argv = nullptr;
        MPI_Init(&argc,&argv);
        /*
        bool salir=false;
        while(!salir){
            ;
        }*/
        MPI_Manager::init = true;
    }

}

void MPI_Manager::Finalize()
{
    if (MPI_Manager::init)
    {
        MPI_Finalize();
        MPI_Manager::init = false;
    }

}

MPI_Comm* MPI_Manager::Instanciate(char *processName, char *ip)
{
    //printf("MPI_MANAGER Instanciate\n");
    MPI_Comm* newComm = new MPI_Comm[1];
    MPI_Info info;
    MPI_Info_create(&info);
    if(strcmp(processName, "rpc_remotefile")==0)
    {
        MPI_Info_set(info,"host", ip);
    }

    else if(strcmp(processName, "rpc_pruebaclase")==0)
    {
        MPI_Info_set(info,"host", ip);
    }

    else if(strcmp(processName,"rpc_filemanager")==0)
    {
        MPI_Info_set(info,"host", ip);
    }

    std::string procname="/home/lluspel/Documentos/T4/ResultT4/"+std::string(processName);
    cout << procname << endl;
    MPI_Comm_spawn(procname.c_str(), MPI_ARGV_NULL, 1, info, 0, MPI_COMM_SELF, newComm, MPI_ERRCODES_IGNORE);
    comms.push_back(newComm);
    return newComm;
}
