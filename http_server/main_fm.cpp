#include <iostream>
#include "filemanager_imp.h"

int main(int argc,char** argv)
{
    filemanager_imp*fm=new filemanager_imp();
    
    fm->run();
    delete[] fm;
    return 0;
}
