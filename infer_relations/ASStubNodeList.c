/************************************************************************************************
* ASStubNodeList.c
* Description: Manages list of nodes that triggers a valley in a valley path
*
*************************************************************************************************/
//EXTERNAL INCLUDES
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <dirent.h>

//INTERNAL INCLUDES
#include "ASStubNodeList.h"
#include "ASCommon.h"

//singleton instance of list node
static ASNode* vNodeList = NULL;
static int countNodes = 0;

/**
* Create a stub node
**/
static ASNode* createStubNode(ASNode* vnode)
{
    ASNode* node = ( ASNode*) malloc(sizeof( ASNode));
    node->degree=vnode->degree;
    node->distance=vnode->distance;
    node->nodeX=vnode->nodeX;
    node->next = NULL;
    ++countNodes;
    return node;
}

//===========================================================================
// Inserts a node into a list else updates existing node
//============================================================================
void addStubNode(ASNode* vnode)
{
    ASNode* node = vNodeList;
    int found = 0;
    while(node!=NULL)
    {
        if(node->nodeX==vnode->nodeX)
        {
            found=1;
        }
        node = node->next;
    }
    if(!found)
    {
        node = createStubNode(vnode);
        if(!vNodeList)
            vNodeList = node;
        else
        {
            node->next = vNodeList->next;
            vNodeList->next = node;
        }
    }
}

//===========================================================================
// Destroy node list
//============================================================================
void cleanupStubList()
{
    ASNode* node = vNodeList;
    ASNode* nextNode = NULL;
    while(node!=NULL)
    {
        nextNode = node->next;
        free(node);
        node = nextNode;
    }
}

//===========================================================================
// Destroy node list
//============================================================================
void findStubNodeInFile(ASNode** lookupTable, const char* infilename)
{
    FILE *file = fopen ( infilename, "r" );
    ASPair* node = NULL;

    int numlines = 0;
    if ( file )
    {
        char* line=NULL;
        int len;
        ASNodeId nodeX = 0;
        //Get the last node
        while ( asn_fgetline(file,&line, &len)!= NULL ) /* read a line */
        {
            const char* s = " ";
            char *curr_node = NULL;
            curr_node = strtok(line, s);
            while( curr_node != NULL )
            {
                nodeX = atoi(curr_node);
                curr_node = strtok(NULL, s);
            }
            free(line);
            line = NULL;
        }
        fclose ( file );
        ASNode* node = getASNode(lookupTable, nodeX);
        if(node->degree<=1)
            addStubNode(node);
    }

    return 0;
}

//===========================================================================
// Destroy node list
//============================================================================
void outputStubListToFile()
{
    ASNode* node = vNodeList;
    FILE* stubOut = fopen("../output/stublist.txt","w+");
    while(node!=NULL)
    {
        fprintf(stubOut,"%d,%d:%d\n",node->distance,node->degree,node->nodeX);
        node = node->next;
    }
    printf("Wrote %d nodes to stublist\n",countNodes);
    fclose(stubOut);
}

//===========================================================================
// Find Stub Nodes
//============================================================================
void findStubNodes(ASNode** lookupTable)
{
    const char* tier1_dir_name = "..//data//tier1_paths//";
    const char* non_tier1_dir_name = "..//data//non_tier1_paths//";
    char filepath[50] = {0};
    DIR * d = NULL;
    //Extract as pairs from tier1 paths
    d = opendir (tier1_dir_name);
    while (1) {
        struct dirent * entry;
        entry = readdir (d);
        if (! entry) {
            break;
        }
        if (!(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0))
        {
            strcpy(filepath, tier1_dir_name);
            strcat(filepath,entry->d_name);
            findStubNodeInFile(lookupTable,filepath);
        }
    }

    //Extract as pairs from non-tier1 paths
    d = opendir (non_tier1_dir_name);
    while (1) {
        struct dirent * entry;
        entry = readdir (d);
        if (! entry) {
            break;
        }
        if (!(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0))
        {
            strcpy(filepath, non_tier1_dir_name);
            strcat(filepath,entry->d_name);
            findStubNodeInFile(lookupTable,filepath);
        }
    }
}
