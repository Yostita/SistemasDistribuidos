#include "pruebaclase_imp.h"
#include "pruebaclase_stub.h"
#include <mpi/mpi.h>
#include <iostream>

pruebaClase_imp::pruebaClase_imp()
{
    MPI_Manager::Init();
    this->parent = new MPI_Comm[1];
    MPI_Comm_get_parent(this->parent);
    this->pclase = new pruebaClase();
}

void pruebaClase_imp::run()
{
    bool salir = false;
    // mientras no salir
    while(!salir)
    {
        // esperar tipo de operacion
        int op = -1;
        MPI_Status status;
	
        MPI_Recv(&op, 1, MPI_INT, 0, 0, *(this->parent), &status);
        // dependiendo de operacion
        switch(op)
        {
            case OP_EXIT:
            {
                salir = true;
            }break;
            case OP_HOLAMUNDO:
            {
                // recibo parametros
                // ejecuto operacion
                pclase->holaMundo();
                // envio resultado
                int ack = 1;
                MPI_Send(&ack, 1, MPI_INT, 0, 0, *(this->parent));
            }break;
            case OP_SUMA:
            {
                // recibo parametros
                int* data = new int[2];
                MPI_Recv(data, 2, MPI_INT, 0, 0, *(this->parent), &status);
                // ejecuto operacion
                int result = pclase->suma(data[0],data[1]);
                // envio resultado

		
                MPI_Send(&result, 1, MPI_INT, 0, 0, *(this->parent));

                delete[] data;
            }break;
            default:
            {
                std::cout << "ERROR SLAVE OPERACION NO SOPORTADA\n" << __LINE__ << __FILE__ << "\n";
            }break;
        }

    }

}


pruebaClase_imp::~pruebaClase_imp()
{
    MPI_Manager::Finalize();
    delete pclase;
    delete[] this->parent;
}
