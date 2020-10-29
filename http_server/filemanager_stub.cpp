#include "filemanager_stub.h"

FileManager_stub::FileManager_stub(char* ip)
{

    MPI_Manager::Init();
    cout<<"\n"<<ip<<"\n";
    this->comm=MPI_Manager::Instanciate("rpc_filemanager", ip);
    printf("CONTRUCTOR FILEMANAGER_STUB\n");

}

void FileManager_stub::readFile(char* filename, char* &data, unsigned int &dataLength)
{
    char *buff=NULL;
    int buffLength=0;
    cout<<"hola desde ReadFile_Stub\n";
//enviar tipo de operacion
    MPI_Status status;
       //enviar tipo de operacion readfile
       int op=OP_READFILE;

       MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));

//enviar parametros
       unsigned int filenameLen=strlen(filename)+1;//+1 para enviar el carecter de final de cadena
       MPI_Send(&filenameLen,1,MPI_INT,0,0,*(this->comm));
       cout<<"hola desde ReadFile_Stub\n";
       MPI_Send(filename,(int)filenameLen,MPI_CHAR,0,0,*(this->comm));
       cout<<"hola desde ReadFile_Stub\n";
//esperar resultado
      //recibir tamaño
       MPI_Recv(&buffLength,1,MPI_INT,0,0,*(this->comm),&status);

     //reservar buffer
      buff=new char[buffLength];
    //recibir buffer
       MPI_Recv(buff,buffLength,MPI_INT,0,0,*(this->comm),&status);
       data= buff;
       dataLength=buffLength;

}

void FileManager_stub::writeFile(char* filename, char* data, unsigned int dataLength)
{
    int op = OP_WRITEFILE;
    MPI_Send(&op, 1, MPI_INT, 0, 0, *(this->comm));
    // Enviamos parametros
    unsigned int fileLen = strlen(filename) + 1;
    MPI_Send(&fileLen, 1, MPI_INT, 0, 0, *(this->comm));
    MPI_Send(filename, fileLen, MPI_CHAR, 0, 0, *(this->comm));

    // Enviamos el tamaño de los datos
    MPI_Send(&dataLength, 1, MPI_INT, 0, 0, *(this->comm));
    // Enviamos los datos
    MPI_Send(data, dataLength, MPI_CHAR, 0, 0, *(this->comm));
}

void FileManager_stub::freeListedFiles(vector<string*>* fileList)
{
    for(vector<string*>::iterator i=fileList->begin();i!= fileList->end();++i)
    {
        delete *i;
    }
    delete fileList;
}

vector<string*>* FileManager_stub::listFiles()
{
    vector<string*>* lista;
    lista= new vector<string*>();

    
    MPI_Status status;
    int op=OP_LISTFILE;
    std::cout<<"creando download service4\n";
    MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));
    std::cout<<"creando download service5\n";
    int nFiles=-1;
    std::cout<<"creando download service6\n";
    MPI_Recv(&nFiles,1,MPI_INT,0,0,*(this->comm),&status);

    //por cada fichero
    for(int i = 0 ; i < nFiles; ++i)
    {
	std::cout<<i+"\n";
        //recibir longitud de nombre
        int bufflen=0;
        MPI_Recv(&bufflen,1,MPI_INT,0,0,*(this->comm),&status);
        //reservar buffer
        char * nombre = new char[bufflen];

        //recibir nombre
        MPI_Recv(nombre,bufflen,MPI_CHAR,0,0,*(this->comm),&status);
        lista->push_back(new string (nombre));
        delete [] nombre;
    }

    return lista;
}

FileManager_stub::~FileManager_stub()
{
    int op=OP_EXIT;
    MPI_Send(&op,1,MPI_INT,0,0,*(this->comm));
}
