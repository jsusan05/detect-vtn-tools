/************************************************************************************************
* ASHashTable.c
* Description: Function definitions of hash table that stores AS pairs and relations
*
*************************************************************************************************/
//SYSTEM INCLUDES
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

//INTERNAL INCLUDES
#include "ASHashTable.h"
#include "ASCommon.h"

//=====================================================
// Returns a new ASN Node
//=====================================================
static ASPair* createASPair(ASNodeId nodeX, ASNodeId nodeY)
{
    ASPair* node = (ASPair*) malloc(sizeof(ASPair));
    if(node)
    {
        node->nodeX = nodeX;
        node->nodeY = nodeY;
        node->obsv_relation = ASNUnknown;
        node->true_relation = ASNUnknown;
        node->fw_count = 0;
        node->bk_count = 0;
        node->next = NULL;
    }
    return node;
}

int num_hash_true=0;
ASPair* insertExtraASPair(ASPair** ASNHashTable, ASNodeId nodeX, ASNodeId nodeY, int fwd_cnt, int bk_cnt, ASNRelationships relation, int isObsv)
{
    assert(nodeX!=0 && nodeY!=0 && relation!=ASNUnknown);
    const int index = (nodeX+nodeY)%TABLE_SIZE;
    ASPair* node = ASNHashTable[index];
    while(node && !((node->nodeX==nodeX && node->nodeY==nodeY)||(node->nodeX==nodeY && node->nodeY==nodeX)))
    {
        node = node->next;
    }
    if(isObsv)
        assert(!node);
    else
        assert(node);
    if(!node)
    {
        node = createASPair(nodeX,nodeY);
        if(ASNHashTable[index])
        {
            node->next = ASNHashTable[index]->next;
            ASNHashTable[index]->next = node;
        }
        else
            ASNHashTable[index] = node;
    }

    if(node)
    {
        if(node->nodeX==nodeX && node->nodeY==nodeY)
        {
            if(fwd_cnt>0)
                node->fw_count = fwd_cnt;
            if(bk_cnt>0)
                node->bk_count = bk_cnt;
        }
        if(node->nodeX==nodeY && node->nodeY==nodeX)
        {
            if(fwd_cnt>0)
                node->fw_count = bk_cnt;
            if(bk_cnt>0)
                node->bk_count = fwd_cnt;
            if(relation==ASNP2C)
               relation = ASNC2P;
            else if(relation==ASNC2P)
               relation = ASNP2C;

        }

        if(isObsv)
        {
            node->obsv_relation = relation;
        }
        else
        {
            node->true_relation = relation;
        }
    }
    return node;
}

//===========================================================================
// Inserts a new ASN Node if it doesn't exist else returns the existing one
//===========================================================================
int new_t1rel=0;
int new_nt1rel=0;
int changed_rel=0;
ASPair* insertASPair(ASPair** ASNHashTable, ASNodeId nodeX, ASNodeId nodeY, ASNRelationOrigin relOrigin)
{
    assert(nodeX!=0 && nodeY!=0);
    const int index = (nodeX+nodeY)%TABLE_SIZE;
    ASPair* node = ASNHashTable[index];
    while(node && !((node->nodeX==nodeX && node->nodeY==nodeY)||(node->nodeX==nodeY && node->nodeY==nodeX)))
    {
        node = node->next;
    }

    ASNRelationships beforeRel = getASNRelation(node, nodeX, nodeY);


    if(!node)
    {
        node = createASPair(nodeX,nodeY);
        if(ASNHashTable[index])
        {
            node->next = ASNHashTable[index]->next;
            ASNHashTable[index]->next = node;
        }
        else
            ASNHashTable[index] = node;
    }

    if(node)
    {
        if(node->nodeX==nodeX && node->nodeY==nodeY)
            node->fw_count++;
        else if(node->nodeX==nodeY && node->nodeY==nodeX)
            node->bk_count++;
    }

    //If the node was newly created, assign the origin
    if(ORIGIN_UNKNOWN==node->relOrigin)
    {
        node->relOrigin = relOrigin;
        if(TIER1_REL==relOrigin)
            new_t1rel++;
        if(NON_TIER1_REL==relOrigin)
            new_nt1rel++;
    }

    ASNRelationships afterRel = getASNRelation(node, nodeX, nodeY);

    if(node->relOrigin==TIER1_REL && relOrigin==NON_TIER1_REL && beforeRel!=afterRel)
    {
        node->relOrigin = TIER1_REL_CHANGED;
        changed_rel++;
    }
    if(node->relOrigin==TIER1_REL_CHANGED)
    {
        node->relOrigin = TIER1_REL_CHANGED;
    }
    return node;
}

//===========================================================================
// Gets the ASN Node if it exist else returns NULL
//===========================================================================
ASPair* getASPair(ASPair** ASNHashTable, ASNodeId nodeX, ASNodeId nodeY)
{
    const int index = (nodeX+nodeY)%TABLE_SIZE;
    ASPair* node = ASNHashTable[index];

    while(node && !((node->nodeX==nodeX && node->nodeY==nodeY)||(node->nodeX==nodeY && node->nodeY==nodeX)))
    {
        node = node->next;
    }
    return node;
}

//===========================================================================
// Destroys the nodes created in heap and resets the hash table
//===========================================================================
void destroyTable(ASPair** hashTable)
{
    int i=0;
    ASPair* node = NULL;
    for(i=0; i<TABLE_SIZE; i++)
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

