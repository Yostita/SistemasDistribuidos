#include "httpserver.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"
#include <iostream>
#include <openmpi/mpi.h>
#include <string.h>
#include "pruebaclase_stub.h"
#include "remotefile_stub.h"
#include "mpi_manager.h"
#include <fstream>
#include <vector>
using namespace std;

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "mysql_driver.h"


httpServer::httpServer(unsigned short port)
{

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
     if (sock_fd < 0)
     {
        printf("Error creating socket\n");
     }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    int option = 1;
    setsockopt(sock_fd,SOL_SOCKET,
               (SO_REUSEPORT | SO_REUSEADDR),
               &option,sizeof(option));

    if (bind(sock_fd,(struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0)
          printf("ERROR on binding");


    listen(sock_fd,5);

    buildMimetypeTable();
}


std::string httpServer::getmimeType(char* file)
{
    std::string strfile=std::string(file);
    std::string ext="";
    std::string result="application/octet-stream";

    if(strfile[strfile.length()-4]=='.')
    {
        ext=std::string(&strfile[strfile.length()-3]);
    }
    else
        if(strfile[strfile.length()-5]=='.')
        {
            ext=std::string(&strfile[strfile.length()-4]);
        }


    std::map<std::string,std::string>::iterator mimetype= this->mimeTypes.find(ext);
    if(mimetype!=mimeTypes.end())
    {
        result= mimetype->second;
    }

    return result;
}
void httpServer::buildMimetypeTable()
{
    this->mimeTypes["aac"]="audio/aac";
    this->mimeTypes["abw"]="application/x-abiword";
    this->mimeTypes["arc"]="application/octet-stream";
    this->mimeTypes["avi"]="video/x-msvideo";
    this->mimeTypes["azw"]="application/vnd.amazon.ebook";
    this->mimeTypes["bin"]="application/octet-stream";
    this->mimeTypes["bz"]="application/x-bzip";
    this->mimeTypes["bz2"]="application/x-bzip2";
    this->mimeTypes["csh"]="application/x-csh";
    this->mimeTypes["css"]="text/css";
    this->mimeTypes["csv"]="text/csv";
    this->mimeTypes["doc"]="application/msword";
    this->mimeTypes["epub"]="application/epub+zip";
    this->mimeTypes["gif"]="image/gif";
    this->mimeTypes["htm"]="text/html";
    this->mimeTypes["html"]="text/html";
    this->mimeTypes["ico"]="image/x-icon";
    this->mimeTypes["ics"]="text/calendar";
    this->mimeTypes["jar"]="application/java-archive";
    this->mimeTypes["jpeg"]="image/jpeg";
    this->mimeTypes["jpg"]="image/jpeg";
    this->mimeTypes["js"]="application/javascript";
    this->mimeTypes["json"]="application/json";
    this->mimeTypes["mid"]="audio/midi";
    this->mimeTypes["midi"]="audio/midi";
    this->mimeTypes["mpeg"]="video/mpeg";
    this->mimeTypes["mpkg"]="application/vnd.apple.installer+xml";
    this->mimeTypes["odp"]="application/vnd.oasis.opendocument.presentation";
    this->mimeTypes["ods"]="application/vnd.oasis.opendocument.spreadsheet";
    this->mimeTypes["odt"]="application/vnd.oasis.opendocument.text";
    this->mimeTypes["oga"]="audio/ogg";
    this->mimeTypes["ogv"]="video/ogg";
    this->mimeTypes["ogx"]="application/ogg";
    this->mimeTypes["pdf"]="application/pdf";
    this->mimeTypes["png"]="image/png";
    this->mimeTypes["ppt"]="application/vnd.ms-powerpoint";
    this->mimeTypes["rar"]="application/x-rar-compressed";
    this->mimeTypes["rtf"]="application/rtf";
    this->mimeTypes["sh"]="application/x-sh";
    this->mimeTypes["svg"]="image/svg+xml";
    this->mimeTypes["swf"]="application/x-shockwave-flash";
    this->mimeTypes["tar"]="application/x-tar";
    this->mimeTypes["tif"]="image/tiff";
    this->mimeTypes["tiff"]="image/tiff";
    this->mimeTypes["ttf"]="font/ttf";
    this->mimeTypes["vsd"]="application/vnd.visio";
    this->mimeTypes["wav"]="audio/x-wav";
    this->mimeTypes["weba"]="audio/webm";
    this->mimeTypes["webm"]="video/webm";
    this->mimeTypes["webp"]="image/webp";
    this->mimeTypes["woff"]="font/woff";
    this->mimeTypes["woff2"]="font/woff2";
    this->mimeTypes["xhtml"]="application/xhtml+xml";
    this->mimeTypes["xls"]="application/vnd.ms-excel";
    this->mimeTypes["xml"]="application/xml";
    this->mimeTypes["xul"]="application/vnd.mozilla.xul+xml";
    this->mimeTypes["zip"]="application/zip";
    this->mimeTypes["3gp"]="video/3gpp";
    this->mimeTypes["3g2"]="video/3gpp2";
    this->mimeTypes["7z"]="application/x-7z-compressed";

}

int httpServer::waitForConnections()
{
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsock_fd = accept(sock_fd,
                            (struct sockaddr *) &cli_addr,
                            &clilen);
    return newsock_fd;
}


void httpServer::sendContent(int newsock_fd,char* httpHeader,unsigned long int headerLen, char* content, unsigned long int contentLen)
{
    char* msg=new char[headerLen+contentLen+2];
    memcpy(msg,httpHeader,headerLen);
    msg[headerLen]='\r';
    msg[headerLen+1]='\n';
    memcpy(&(msg[headerLen]),content,contentLen);

    send(newsock_fd, msg,headerLen+contentLen+2,0);
}

void httpServer::sendFile(int newsock_fd,char* file)
{
    cout << "la recontra" << endl;
    std::string mimetype=getmimeType(file);
    std::string filepath=this->files_path+std::string(file);
    char* fileContent=NULL;
    unsigned long int filelen=0;
    char* httpHeader;
    unsigned long int headerLen;
    readFile(&filepath[0],&fileContent,&filelen);
    createHeader(&httpHeader,&headerLen,
                 "200 OK",
                 &(mimetype[0]),
                 filelen);
    sendContent(newsock_fd,httpHeader,headerLen,fileContent,filelen);

}


int httpServer::getHTTPParameter(std::vector<std::vector<std::string*>*> *lines,std::string parameter)
{

    for(unsigned long int i=0;i<lines->size();i++)
    {
        std::vector<std::string*> *v=(*lines)[i];

        if(((*v)[0])->compare(parameter)==0)
        {
            return atoi((*v)[1]->c_str());
        }
    }
    return 0;
}

void httpServer::resolveRequests(int newsock_fd)
{
    int resulpclase;


    std::vector<std::vector<std::string*>*> lines;
    readLines(newsock_fd,&lines);
    printLines(&lines);
    std::cout<<"\n";
    std::vector<std::string*> *v=(lines)[0];
    httpRequest_t req=getRequestType(v);
        switch(req)
        {
         case GET:
            {
                std::string *s2=(*v)[1];
                std::string subString = s2->substr(0,20);

                std::cout << subString << std::endl;
                std::cout<<*s2<<"\n";

                if(subString.compare("/html_dir/Descargas/")==0)
                {

                    std::string nombre = s2->substr(20,s2->length());
                    cout<<nombre<<"\n";

                    char * data=NULL;
                    unsigned int datalen=0;

                    char * name = new char [nombre.length()+1];
                    memcpy(name,nombre.c_str(),nombre.length()+1);
                    cout<<name<<"\n";

                    fmanager->readFile(name,data,datalen);
                    string ruta=files_path+"/prueba/"+nombre;
                    char * ruta2 = new char [ruta.length()+1];
                    memcpy(ruta2,ruta.c_str(),ruta.length()+1);

                    FILE* recvFile=fopen(ruta2,"w");
                    fwrite(data,sizeof(char),datalen,recvFile);
                    fclose(recvFile);

                    string rutafinal = "/prueba/"+nombre;
                    char * rutafinal2 = new char [rutafinal.length()+1];
                    memcpy(rutafinal2,rutafinal.c_str(),rutafinal.length()+1);
                    sendFile(newsock_fd,rutafinal2);
                    delete [] name;

                }
                if(s2->compare("/")==0)
                {
                    sendFile(newsock_fd,"/index.html");
                }

                else
                {
                    sendFile(newsock_fd,&((*s2)[0]));
                }
            }
            break;
        case POST:
           {
                std::vector<std::string*> postLine;
                int length=getHTTPParameter(&lines,"Content-Length:");

                std::string *s2=(*v)[1];
                if(s2->compare("/login.php")==0)
                {
                    /*
                    int ifl = 0;
                    char hostname[256];
                    gethostname(hostname, sizeof(hostname));
                    printf("PID %d on %s ready for attach\n", getpid(), hostname);
                    fflush(stdout);
                    while (0 == ifl){
                          sleep(5);
                    }

                    */
                    readPostLine(newsock_fd,&postLine,length);

                    char* user= getFromPost(postLine,"uname");
                    char* pass= getFromPost(postLine,"psw");

                    std::cout<<"user: "<<user<<"\n";
                    std::cout<<"pass: "<<pass<<"\n";
                    if(validatePassword(user,pass))
                    {
                        getIps();
                        getProcesos();
                        crearServicesHTML();
                        sendFile(newsock_fd,"/services.html");
                    }
                    
                    delete[] user;
                    delete[] pass;
                }
                if(s2->compare("/services.html")==0)
                {

                    getIps();
                    getProcesos();
                    crearServicesHTML();
                }

                if(s2->compare("/services.php")==0)
                {
                    readPostLine(newsock_fd,&postLine,length);


                    char * proceso = getFromPost(postLine,"procesos");
                    char * ip = getFromPost(postLine,"IPs");


                    string resultados="";

                    if(strcmp(proceso,"remotefile")==0)
                    {
                        copiarProc(proceso,ip);
                        copyText(ip);
                        if(this->rfile==nullptr)
                        {
                            rfile= new remotefile_stub(ip);
                        }
                        char* buff;
                        unsigned long int bufflen;
                        rfile->readfile("prueba.txt",&buff,&bufflen);
                        resultados+="<p>"+string(buff)+"</p>";

                        createHTML(resultados);
                        sendFile(newsock_fd,"/resultado.html");

                    }
                    if(strcmp(proceso,"pruebaclase")==0)
                    {

                        copiarProc(proceso,ip);
                        if(this->pclase==nullptr)
                        {
                            pclase = new pruebaclase_stub(ip);
                        }
                        int resulsuma = pclase->suma(5,5);
                        resultados+="<p>"+to_string(resulsuma)+"</p>";

                        createHTML(resultados);
                        sendFile(newsock_fd,"/resultado.html");
                    }
                    if(strcmp(proceso,"store_service")==0){
                        copiarProc(proceso,ip);

                        if(this->fmanager==nullptr)
                        {
                            printf("Creando FMANAGER");
                            fmanager= new FileManager_stub(ip);
                            cout << "111" << endl;
                        }
                        crearStoreHtml();
                        cout << "2" << endl;
                        sendFile(newsock_fd,"/store_service.html");
                        cout << "3" << endl;

                    }
                    if(strcmp(proceso,"download_service")==0){
                         copiarProc(proceso,ip);
                        if(this->fmanager==nullptr)
                        {
                            fmanager= new FileManager_stub(ip);
                        }
                        crearDownloadHtml();
                        sendFile(newsock_fd,"/download_service.html");
                    }

                }

                if(s2->compare("/download_service.html")==0){
                    /*
                    readPostLine(newsock_fd,&postLine,length);
                    char* fname=NULL;
                    char* data=NULL;
                    int datalen=0;
                    fname = getFromPost(postLine,"fichero");
                    std::cout<<"Fichero: "<<fname<<" subido\n";

                    std::cout<<"Tamaño: "<<datalen<<" bytes\n";
                    std::cout<<"Contenido: "<<data<<"\n";
                    this->fmanager->readFile(fname,data,datalen);
                    free(data);
                    free(fname);
                    sendFile(newsock_fd,"/download_service.html");
                    */
                    
                    cout<<"download_service.html" << endl;
                    sendFile(newsock_fd,"/download_service.html.html");

                }
                if(s2->compare("/store_service.php")==0){
                    std::cout<<"STORE_SERVICE recien lanzado \n";
                    char* fname=NULL;
                    char* data=NULL;
                    int fileLen=0;
                    getFileFromPost(newsock_fd,length,fname,data,fileLen);
                    std::cout<<"Fichero: "<<fname<<" recibido\n";
                    std::cout<<"Tamaño: "<<fileLen<<" bytes\n";
                    std::cout<<"Lanzando writeFile\n";
                    this->fmanager->writeFile(fname, data, fileLen);
                    std::cout<<"WriteFile ha finalizado\n";
                    free(fname);
                    free(data);
                    sendFile(newsock_fd, "/descargas.html");
                }

          }
           break;
        default:
            break;
        }
    close(newsock_fd);
    deleteLines(&lines);
}


void httpServer:: createHTML(string &resultados)
{
    string header = "<html> <body>";
    string piePagina="</body> </html>";
    string final=header+resultados+piePagina;
    string resultado_fin=files_path+string("/resultado.html");

    FILE* html=fopen(resultado_fin.c_str(), "w");
    fwrite(final.c_str(), sizeof(char), final.length(),html);
    fclose(html);

}


void httpServer::iniciarSQL()
{
    /* Create a connection */
    driver = get_driver_instance();
    con = driver->connect("localhost:3306", "username", "password");
    /* Connect to the MySQL test database */
    con->setSchema("p4");
}


bool httpServer::validatePassword(char* username,char* password)
{
    try{
        iniciarSQL();
        user = con->createStatement();
        res = user->executeQuery("SELECT * FROM usuarios");

        while(res->next())
        {
            if(strcmp(res->getString("user").c_str(), username)==0)
            {
                cout << "user exist" << endl;
                if(strcmp(res->getString("pwd").c_str(), password)==0)
                {
                    cout << "\t... MySQL procesos: " << endl;
                    cout << "User: " << res->getString("user") << endl;
                    cout << "Pwd: " << res->getString("pwd") << endl;
                    return true;
                }
            }
        }

        return false;

    }catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}


void httpServer::getIps()
{
    try{
        iniciarSQL();
        user = con->createStatement();
        res = user->executeQuery("SELECT * FROM servidores");

        while(res->next())
        {
            string ipName = res->getString("ip");
            cout << "\t... MySQL servidores: " << endl;
            cout << "Servidores: " << ipName << endl;
            ips.push_back(ipName);
        }
    }
    catch (sql::SQLException &e) {
          cout << "# ERR: SQLException in " << __FILE__;
          cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
          cout << "# ERR: " << e.what();
          cout << " (MySQL error code: " << e.getErrorCode();
          cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}


void httpServer::getProcesos()
{
    try{
        iniciarSQL();
        user = con->createStatement();
        res = user->executeQuery("SELECT * FROM procesos");

        while(res->next())
        {
            string processName = res->getString("proceso");
            cout << "\t... MySQL procesos: " << endl;
            cout << "Proceso: " << processName << endl;
            proc.push_back(processName);
        }

    }catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
}


void httpServer::crearServicesHTML(){

    int contadorProcesos=0;
    int contadorIps=0;
    string header = "<html><head><title>Ejecucion de procesos</title><link rel='shortcut icon' href='./icon.png'><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'><link rel='stylesheet' href='styles.css'><link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css'integrity='sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T' crossorigin='anonymous'></head><body><div class='d-flex w-100 h-100 mx-auto flex-column'><header class='mb-auto bg-dark p-1 w-100'><a class='navbar-brand' href='index.html'><img src='https://www.u-tad.com/wp-content/uploads/2016/02/logo-utad-1.png' class='d-inline-block align-top' height='40rem'></a></header><form  class=\"mb-auto ml-1 mr-1\"  action=\"services.php\" method=\"post\">";
    string inicioTarjeta = "<div class=\"card rounded-lg\" style=\"width: 30rem;\"><div class=\"card-body mb-0\"><h5 class=\"card-title\">Procesos</h5><h6 class=\"card-subtitle mb-2 text-muted font-weight-light\">Seleccione el proceso y la ip donde desee realizarlo.</h6><div class=\"container\">";
    string selectIP = "<div class=\"form-group\"><select class=\"custom-select\" name=\"IPs\" required>";

    while(contadorIps < ips.size())
    {
        selectIP+="<option name=ip"+to_string(contadorIps)+" value="+ips[contadorIps]+">"+ips[contadorIps]+"</option>";

        contadorIps++;
    }
    selectIP+="</select></div>";

    string selectProcesos = "<div class=\"form-group\"><select class=\"custom-select\" name=\"procesos\" required>";

    while(contadorProcesos < proc.size())
    {
        selectProcesos+="<option name=ip"+to_string(contadorProcesos)+" value="+proc[contadorProcesos]+">"+proc[contadorProcesos]+"</option>";
        contadorProcesos++;
    }
    selectProcesos+="</select></div>";
    string boton ="<div class=\"form-group mt-3 mb-0\"><button type=\"submit\" class=\"btn btn-success\">Lanzar</button></div></div></div></div>";
    string piePagina = "</form><section  class='copyright text-center text-white' style='background-color:rgb(22, 22, 22);'><div class='container'><small>Copyright &copy; <a href='https://www.instagram.com/luisct99/' class='text-white'>Luis Crespo</a> &<a href='https://www.instagram.com/_yosta_/' class='text-white'>Jose Luis Bellosta</a> 2019</small></div></section></div></body></html>";
    string final = header+inicioTarjeta+selectIP+selectProcesos+boton+piePagina;
    string resultado_html=files_path+string("/services.html");

    FILE * html=fopen(resultado_html.c_str(),"w");
    fwrite(final.c_str(),sizeof(char),final.length(),html);
    fclose(html);
}


void httpServer::crearDownloadHtml(){
    std::vector<std::string*>* vfiles = fmanager->listFiles();
    std::cout<<"Lista de ficheros en el directorio de prueba:\n";
    std::ofstream result_file;
    result_file.open("./html_dir/download_service.html");
    result_file << "<html><head></head><body> <form enctype=\"multipart/form-data\" action=\"download_service.php\">Choose a file to download:<br>";
    for(unsigned int i=0;i<vfiles->size();++i)
    {
        result_file << "<a href=\"html_dir/Descargas/" ;
        result_file << vfiles->at(i)->c_str();
        result_file <<"\" class=\"btn\" download=\"";
        result_file << vfiles->at(i)->c_str();
        result_file << "\">";
        result_file << vfiles->at(i)->c_str();
        result_file << "</a><br>";
        std::cout<<"Fichero: "<<vfiles->at(i)->c_str()<<"\n";
    }
    result_file << "</form></body></html>";
    result_file.close();
}


void httpServer::crearStoreHtml(){
    string storeHtml="<html><head></head><body><form enctype=\"multipart/form-data\" action=\"store_service.php\" method=\"POST\">Choose a file to upload: <input name=\"uploadedfile\" type=\"file\" /><br /> <input type=\"submit\" value=\"Upload File\" /></form></body></html>";
    string htmlStore=files_path+string("/store_service.html");
    FILE *storesHtml=fopen(htmlStore.c_str(),"w");
    fwrite(storeHtml.c_str(),sizeof(char), storeHtml.length(),storesHtml);
    fclose(storesHtml);

}


void httpServer::copyText(string ip)
{
    std::string comando="scp prueba.txt lluspel@"+ip+":/home/lluspel/Documentos/T4/ResultT4";
    FILE * texto = popen(comando.c_str(),"w");
    pclose(texto);
}


void httpServer::copiarProc(string proceso, string ip)
{

    cout<<proceso<<"\n";
    cout<<ip<<"\n";
    if(proceso=="pruebaclase"){
        std::string comando="scp -r ./build/rpc_"+proceso+" lluspel@"+ip+":/home/lluspel/Documentos/T4/ResultT4";
        FILE * texto = popen(comando.c_str(),"w");
        pclose(texto);
        
    }
    else if(proceso=="remotefile"){
        std::string comando="scp -r ./build/rpc_"+proceso+" lluspel@"+ip+":/home/lluspel/Documentos/T4/ResultT4";
        FILE * texto = popen(comando.c_str(),"w");
        pclose(texto);
        
    }
    else{
        std::string comando="scp -r ./build/rpc_filemanager lluspel@"+ip+":/home/lluspel/Documentos/T4/ResultT4";
        FILE * texto = popen(comando.c_str(),"w");
        pclose(texto);
    }
}


void httpServer::closeServer()
{

    close(this->sock_fd);
}
