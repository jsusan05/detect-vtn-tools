/************************************************************************************************
* ASDegreeLookup.c
* Description: A lookup table for a node and its corresponding degree
*
*************************************************************************************************/
//SYSTEM INCLUDES
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

//INTERNAL INCLUDES
#include "ASDegreeLookup.h"

//=====================================================
// Returns a new ASN Node
//=====================================================
static ASNode* createASNode(ASNodeId nodeX)
{
    ASNode* node = (ASNode*) malloc(sizeof(ASNode));
    if(node)
    {
        node->nodeX = nodeX;
        node->degree = ZERO_DEGREE;
        node->distance = INVALID_DISTANCE;
        node->next = NULL;
    }
    return node;
}

//===========================================================================
// Inserts a new ASN Node if it doesn't exist else returns the existing one
//===========================================================================
ASNode* insertASNode(ASNode** ASNHashTable, ASNodeId nodeX, int degree, int parent_distance)
{
    assert(nodeX!=0);
    int exists = 0;
    const int index = (nodeX)%LOOKUP_TABLE_SIZE;
    ASNode* node = ASNHashTable[index];
    while(node && !(node->nodeX==nodeX))
    {
        node = node->next;
    }

    if(!node)
    {
        node = createASNode(nodeX);
        if(ASNHashTable[index])
        {
            node->next = ASNHashTable[index]->next;
            ASNHashTable[index]->next = node;
        }
        else
            ASNHashTable[index] = node;
    }
    else
    {
        exists = 1;
    }

    if(parent_distance==ZERO_DISTANCE)
        node->distance=0;
    else if(parent_distance!=INVALID_DISTANCE && parent_distance!=ZERO_DISTANCE)
    {
        if(node->distance==INVALID_DISTANCE || (node->distance!=INVALID_DISTANCE && node->distance>parent_distance))
            node->distance = parent_distance+1;
    }

    if(degree!=ZERO_DEGREE)
        node->degree = degree;

    return node;
}

//===========================================================================
// Gets the ASN Node if it exist else returns NULL
//===========================================================================
ASNode* getASNode(ASNode** ASNHashTable, ASNodeId nodeX)
{
    const int index = (nodeX)%LOOKUP_TABLE_SIZE;
    ASNode* node = ASNHashTable[index];

    while(node && !(node->nodeX==nodeX))
    {
        node = node->next;
    }
    return node;
}

//===========================================================================
// Destroys the nodes created in heap and resets the hash table
//===========================================================================
void destroyNodeTable(ASNode** hashTable)
{
    int i=0;
    ASNode* node = NULL;
    for(i=0; i<LOOKUP_TABLE_SIZE; i++)
    {
        node =  hashTable[i];
        while(node)
        {
            hashTable[i]=node->next;
            free(node);
            node = hashTable[i];
        }
    }
}

//===========================================================================
// Test if all nodes have a positive or zero_distance
//===========================================================================
int allNodesValidDistance(ASNode** hashTable)
{
    int i=0;
    ASNode* node = NULL;
    int validAll = 1;
    for(i=0; i<LOOKUP_TABLE_SIZE; i++)
    {
        node =  hashTable[i];
        while(node)
        {
            if(node->distance<0 || node->distance==INVALID_DISTANCE)
            {
                validAll=0;
            }
            node=node->next;
        }
    }
    return validAll;
}

//===========================================================================
// Populates the lookup table with degree and distance info
//===========================================================================
void populateLookupTable(ASNode** lookupTable)
{
    int i=0;
    for(i=0;i<TOT_TIER1;++i)
    {
        //add tier1 nodes to table with distance 0
        ASNode* parent = insertASNode(lookupTable, g_tier1_nodes[i], ZERO_DEGREE, ZERO_DISTANCE);
    }

    //Populate distance to tier one until all nodes have a valid distance
    FILE *file = NULL;
    do {
        file = fopen ( "../output/all_degree.txt", "r" );
        if(file)
        {
            char* line=NULL;
            int len;
            while ( asn_fgetline(file,&line, &len)!= NULL ) /* read a line */
            {
                //Get the node id and degree of parent node
                const char* s = ":";
                char* degree_str = strtok(line, s);
                char* node_str = strtok(NULL, s);
                ASNodeId as_id = atoi(node_str);
                int as_degree = atoi(degree_str);

                char* neighbor_str = strtok(NULL, s);
                assert(degree_str!=NULL && node_str!=NULL);
                //add the parent to the table
                ASNode* parent = insertASNode(lookupTable, as_id, as_degree, INVALID_DISTANCE);
                assert(parent!=NULL);
                int p_dist = parent->distance;

                s = "()";
                neighbor_str = strtok(neighbor_str, s);
                s = " ";
                char* neighbor_node = strtok(neighbor_str, s);
                while(neighbor_node)
                {
                    insertASNode(lookupTable, atoi(neighbor_node), ZERO_DEGREE, p_dist);
                    neighbor_node = strtok(NULL, s);
                }
                free(line);
                line = NULL;
            }
        }
    } while(!allNodesValidDistance(lookupTable));

    fclose(file);
}

//===========================================================================
// Write contents of lookup table to a file
//===========================================================================
void writeNodeInfoToFile(ASNode** lookupTable)
{
    FILE* outfile = fopen ( "../output/output_dist.txt", "w+" );
    ASNode* node=NULL;
    int num=0;
    int i=0;
    int dist=0;
    for(dist=0;dist<7;++dist)
    {
        for(i=0; i<LOOKUP_TABLE_SIZE; i++)
        {
            node =  lookupTable[i];
            while(node)
            {
                if(dist==node->distance)
                {
                    num++;
                    fprintf(outfile,"%d,%d:%d\n",node->distance,node->degree,node->nodeX);
                    assert(node->distance<=6);
                }
                node = node->next;
            }
        }
    }
    printf("Wrote No of nodes=%d to output_dist.txt\n",num);
    fclose(outfile);
}
