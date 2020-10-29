#include "filemanager_imp.h"
#include "filemanager_stub.h"
#include "mpi.h"

filemanager_imp::filemanager_imp()
{
    MPI_Manager::Init();
    this->parent=new MPI_Comm[1];
    MPI_Comm_get_parent(this->parent);//Conseguimos el comunicador del padre para sustituirlo en las funciones de llamada y recepcion
    this->fileman=new FileManager("./prueba");
}

void filemanager_imp::run()
{
    bool salir = false;
    //mientras no salir
    while(!salir){
          //esperar tipo de operacion
        int op=-1;
        MPI_Status status;
        MPI_Recv(&op,1,MPI_INT,0,0,*(this->parent),&status);
        switch (op)
        {
        case OP_EXIT:
        {
            salir=true;
        }break;


        case OP_READFILE:
        {
            //recibo parametros
            unsigned  int filenamelen=0;
            //cout<<"OP_READFILE   1"<<"\n";
            MPI_Recv(&filenamelen,1,MPI_INT,0,0,*(this->parent),&status);
            //cout<<"OP_READFILE   2"<<"\n";
            char * filename=new char[filenamelen];
            MPI_Recv(filename,filenamelen,MPI_CHAR,0,0,*(this->parent),&status);
            //cout<<"OP_READFILE   3"<<"\n";
            //ejecuto operacion
            char * buff=NULL;
            unsigned  int bufflen=0;

            fileman->readFile(filename,buff,bufflen);

           //envio resultado
           MPI_Send(&bufflen,1,MPI_INT,0,0,*(this->parent));
           //cout<<"OP_READFILE   4"<<"\n";
           MPI_Send(buff,bufflen,MPI_CHAR,0,0,*(this->parent));
           //cout<<"OP_READFILE   5"<<"\n";

            delete [] buff;
            delete [] filename;
        }break;

        case OP_WRITEFILE:
        {
            unsigned int filenamelen=0;
            MPI_Recv(&filenamelen,1,MPI_INT,0,0,*(this->parent),&status);
            char * filename=new char[filenamelen];
            MPI_Recv(filename,filenamelen,MPI_CHAR,0,0,*(this->parent),&status);


            unsigned int dataLength=0;
            MPI_Recv(&dataLength,1,MPI_INT,0,0,*(this->parent),&status);

            char * data=NULL;
            data = new char[dataLength];
            MPI_Recv(data,dataLength,MPI_CHAR,0,0,*(this->parent),&status);

            fileman->writeFile(filename,data,dataLength);

            delete [] data;
            delete [] filename;

        }break;

        case OP_LISTFILE:
        {
            //listar los archivos
            vector<string*>* lista = fileman->listFiles();
            //enviar numero de ficheros
            int nFiles=lista->size();
            MPI_Send(&nFiles,1,MPI_INT,0,0,*(this->parent));
            //por cada fichero
            for(int i = 0; i < nFiles; i++)
            {
                //enviar su longitud de nombre
                int bufflen = lista->at(i)->length()+1;
                MPI_Send(&bufflen,1,MPI_INT,0,0,*(this->parent));
                //enviar su nombre
                string  *nombre=lista->at(i);
                MPI_Send(nombre->c_str(),bufflen,MPI_CHAR,0,0,*(this->parent));

            }

            fileman->freeListedFiles(lista);
        }break;

          default:
        {
            std::cout << "ERROR SLAVE OPERACION NO SOPORTADA"<<__LINE__<< __FILE__<<"\n";
        }
            break;
        }
    }
}

filemanager_imp::~filemanager_imp()
{
    MPI_Manager::Finalize();
    delete fileman;
    delete [] this->parent;
}
