/************************************************************************************************
* ASFileOps.h
* Description: An alternative to fgetline() function not available in ANSI C
*
*************************************************************************************************/

#ifndef ASFILEOPS_H_INCLUDED
#define ASFILEOPS_H_INCLUDED

//EXTERNAL INCLUDES
#include <stdio.h>

char* asn_fgetline(FILE* file,char** line, int* len);

#endif // ASFILEOPS_H_INCLUDED
