#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "pruebaclase_imp.h"

int main(int argc, char** argv)
{
    pruebaClase_imp* pclase = new pruebaClase_imp();
    pclase->run();

    delete pclase;
    return 0;
}
