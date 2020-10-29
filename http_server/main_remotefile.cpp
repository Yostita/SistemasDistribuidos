#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "remotefile_imp.h"

int main(int argc, char** argv)
{
    remotefile_imp* rfile = new remotefile_imp();
    rfile->run();

    delete rfile;
    return 0;
}
