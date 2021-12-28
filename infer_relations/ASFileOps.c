/************************************************************************************************
* ASFileOps.c
* Description: An alternative to fgetline() function not available in ANSI C
*
*************************************************************************************************/
//EXTERNAL INCLUDES
#include <string.h>
#include <stdlib.h>

//INTERNAL INCLUDES
#include "ASFileOps.h"

//CONSTANTS
#define MAX_TEMP_LINE   100

char* asn_fgetline(FILE* file,char** line, int* len)
{
    char temp_line[MAX_TEMP_LINE] = {0};
    *line = NULL;
    char t='\0';
    int i=0;
    *len=0;
    while((t=fgetc(file))!='\n')
    {
        if(t==EOF)
            return NULL;

        temp_line[i++]=t;
        if(i==MAX_TEMP_LINE)
        {
            //realloc if needed
            temp_line[i]='\0';
            *len = *len+i;
            int str_len = *len;
            char* tmp_ptr = (char*) malloc(sizeof(char)*(str_len+1));
            if(*line)
            {
                strncpy(tmp_ptr,*line,strlen(*line)+1);
                strncpy(tmp_ptr+strlen(*line), temp_line, strlen(temp_line)+1);
            }
            else
            {
                strncpy(tmp_ptr, temp_line, strlen(temp_line)+1);
            }
            i=0;
            *line = tmp_ptr;
        }
    }
    temp_line[i]='\0';
    *len = *len+i;
    int str_len = *len;
    char* tmp_ptr = (char*) malloc(sizeof(char)*(str_len+1));
    if(*line)
        strncpy(tmp_ptr,*line,strlen(*line)+1);
     if(*line)
        strncpy(tmp_ptr+strlen(*line), temp_line, strlen(temp_line)+1);
    else
        strncpy(tmp_ptr, temp_line, strlen(temp_line)+1);
    *line = tmp_ptr;

    return *line;
}
