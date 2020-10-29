#include <openmpi/mpi.h>
#include <iostream>
#include "mpi_manager.h"
#include "pruebaclase_stub.h"

pruebaclase_stub::pruebaclase_stub(char *ip)
{
    MPI_Manager::Init();
    this->comm = MPI_Manager::Instanciate("rpc_pruebaclase", ip);   // En remoto se adapta al host
}


void pruebaclase_stub::holaMundo()
{
 //enviar el tipo de operacion: holamundo
    int op = OP_HOLAMUNDO;
    MPI_Send(&op, 1, MPI_INT, 0, 0, *(this->comm));
    //recibo ack
    int ack = 0;
    MPI_Status status;
    MPI_Recv(&ack,1,MPI_INT, 0, 0, *(this->comm), &status);
    if (ack != 1)
    {
       std::cout << "ERROR MASTER HOLAMUNDO\n" << __LINE__ << __FILE__ << "\n";
    }


}

int pruebaclase_stub::suma(int a, int b)
{
 //enviar tipo operacion: suma
    int op = OP_SUMA;
    int result = 0;
    MPI_Status status;
    MPI_Send(&op, 1, MPI_INT,0, 0, *(this->comm));
 // enviar parametros
    int* data = new int[2];
    data[0] = a;
    data[1] = b;
    MPI_Send(data,2,MPI_INT,0, 0, *(this->comm));
 // esperar resultados
    MPI_Recv(&result, 1, MPI_INT,0, 0, *(this->comm), &status);

    delete[] data;

    return result;
}

pruebaclase_stub::~pruebaclase_stub()
{
    int op = OP_EXIT;
    MPI_Send(&op, 1, MPI_INT,0, 0, *(this->comm));
}
