#include "filemanager.h"
/**
 * @brief FileManager::FileManager Constructor de la clase FileManager. Recibe por parámetros el directorio
 * que usará esta clase para indexar, almacenar y leer ficheros. Se aconseja usar una ruta completa al directorio,
 * desde la raiz del sistema de ficheros.
 *
 * @param path Ruta al directorio que se desa usar
 */

FileManager::FileManager(string path)
{
    FILE*f =fopen("hola.txt", "w");
    fclose(f);
    this->dirPath=path;
    //path="./Descargas";
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != nullptr) {

      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != nullptr) {
          if(ent->d_type==DT_REG) //Store only regular files
          {
           string* f=new string(ent->d_name);
           this->files[*f]=f;
          }
      }
      closedir (dir);
    } else {
      /* could not open directory */
        string* f=new string("ERROR: No existe el fichero o directorio");
        this->files[*f]=f;
        std::cout<<"ERROR: No existe el fichero o directorio\n";
    }
}

/**
 * @brief FileManager::listFiles Sirve para acceder a la lista de ficheros almacenados en la ruta
 * que se usó en el contructor de la clase
 * @return Una copia de la lista de ficheros almacenada en "files". Esta copia hay que liberarla
 * después de haber sido usada. Para ello se ofrece la función "freeListedFiles"
 */
vector<string*>* FileManager::listFiles(){

    vector<string*>* flist=new vector<string*>();
    for(map<string,string*>::iterator i=files.begin();i!= files.end();++i)
    {
        flist->push_back(new string(i->first));
    }
    return flist;
}
/**
 * @brief FileManager::freeListedFiles Función de apoyo, libera una lista de ficheros devuelta por la función "listFiles"
 * @param fileList Puntero a una lista de strings devuelta por la función "listFiles"
 */
void FileManager::freeListedFiles(vector<string*>* fileList)
{
    for(vector<string*>::iterator i=fileList->begin();i!= fileList->end();++i)
    {
        delete *i;
    }
    delete fileList;
}
/**
 * @brief FileManager::readFile Dado el nombre de un fichero almacenado en el directorio que se usó en el contructor,
 * se inicializará la variable "data" con un puntero al contenido del fichero, y la variable dataLength con el
 * tamaño del mismo en bytes.
 *
 * @param fileName Nombre del fichero a leer
 * @param data Datos del fichero. Deberá de liberarse después de ser usado
 * @param dataLength Longitud del fichero en bytes
 */

void FileManager::readFile(char* fileName, char* &data, unsigned int & dataLength)
{
    string path="./Descargas/"+string(fileName);
    //cout<<path<<"\n";
    FILE* f=fopen(path.c_str(),"r");
    fseek(f, 0L, SEEK_END);
    //cout<<"readFile   3"<<"\n";
    dataLength= ftell(f);
    //cout<<"readFile   3"<<"\n";
    fseek(f, 0L, SEEK_SET);
    //cout<<"readFile   3"<<"\n";
    data=new char[dataLength];
    //cout<<"readFile   3"<<"\n";
    fread(data,dataLength,1,f);
    fclose(f);
    //cout<<path<<"\n";
}


/**
 * @brief FileManager::readFile Dado un nuevo nombre de un fichero que se quiere almacenar  en el directorio que se usó en el contructor,
 * se escribirá el contenido del array de datos almacenado en "data", siendo dataLength el
 * tamaño del mismo en bytes. Sobreescribirá el fichero que hubiera en el directorio si tiene el mismo nombre.
 *
 * @param fileName Nombre del fichero a escribir
 * @param data Datos del fichero. Deberá de liberarse después de ser usado
 * @param dataLength Longitud del fichero en bytes
 */

void FileManager::writeFile(char* fileName, char* data, unsigned int dataLength)
{
    string path=this->dirPath+"/"+string(fileName);
    cout<<path<<"\n";
    FILE* f=fopen(path.c_str(),"w");
    fwrite(data,dataLength,1,f);
    fclose(f);
//añadir a la lista el nuevo fichero, si no existe ya
    if(files.find(string(fileName))==files.end())
        files[ string(fileName)]=new string(fileName);

}
