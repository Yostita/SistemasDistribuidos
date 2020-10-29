
#include "remotefile_imp.h"
#include "mpi.h"
#include "remotefile_stub.h"

remotefile_imp::remotefile_imp()
{
    MPI_Manager::Init();
    this->parent=new MPI_Comm[1];
    MPI_Comm_get_parent(this->parent);//Conseguimos el comunicador del padre para sustituirlo en las funciones de llamada y recepcion
    this->rfile=new remotefile();
}

void remotefile_imp::run()
{
    bool salir = false;
    //mientras no salir
    while(!salir){
          //esperar tipo de operacion
        int op=-1;
        MPI_Status status;
        MPI_Recv(&op,1,MPI_INT,0,0,*(this->parent),&status);//Aqui tambien cambiamos MPI_COMM_WORLD por el PADRE que lo creo
            //dependiendo de operacion
        switch (op)
        {
        case OP_EXIT:
        {
            salir=true;
        }break;


        case OP_READFILE:
        {
            //recibo parametros
            unsigned long int filenamelen=0;
            MPI_Recv(&filenamelen,1,MPI_LONG,0,0,*(this->parent),&status);
            char * filename=new char[filenamelen];
            MPI_Recv(filename,filenamelen,MPI_CHAR,0,0,*(this->parent),&status);

            //ejecuto operacion
            char * buff=NULL;
            unsigned long int bufflen=0;
            rfile->readfile(filename,&buff,&bufflen);

           //envio resultado
           MPI_Send(&bufflen,1,MPI_LONG,0,0,*(this->parent));
           MPI_Send(buff,bufflen,MPI_CHAR,0,0,*(this->parent));

            delete [] buff;
            delete [] filename;
        }break;

          default:
        {
            std::cout << "ERROR SLAVE OPERACION NO SOPORTADA"<<__LINE__<< __FILE__<<"\n";
        }
            break;
        }
            //recibo parametros y ejecuto operacion
            //envio resultado
    }
}

remotefile_imp::~remotefile_imp()
{
    MPI_Manager::Finalize();
    delete rfile;
    delete [] this->parent;
}
