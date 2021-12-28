/************************************************************************************************
* ASCommon.c
* Description: Function definitions for utility functions
*
*************************************************************************************************/
//EXTERNAL INCLUDE
#include <stdlib.h>
#include <assert.h>

//INTERNAL INCLUDES
#include "ASCommon.h"

//==================================
// Returns 1 if tier1 node else 0
//==================================
int isTier1(const ASNodeId nodeX)
{
    int i=0;
    for(i=0; i<TOT_TIER1; ++i)
    {
        if(nodeX == g_tier1_nodes[i])
            return 1;
    }
    return 0;
}

//==============================================
// Given node pair return the correct relation
//==============================================
ASNRelationships getASNRelation(ASPair* node, ASNodeId nodeX, ASNodeId nodeY)
{
    ASNRelationships relation = ASNUnknown;

    if((node && node->relOrigin==UNRESOLVED_REL)||!node)
    {
        return ASNUnknown;
    }

    /*if( (nodeX==3356 && nodeY==3549)||(nodeX==3549 && nodeY==3356))
    {
        relation = ASNS2S;
    }
    else*/
    if( node && node->bk_count>0 && node->fw_count>0 )
    {
        if(isTier1(nodeX) || isTier1(nodeY) || node->relOrigin==RESOLVED_REL)
            relation = ASNP2P;
        else
            relation = ASNS2S;
    }
    else if( node && node->nodeX==nodeX && node->nodeY==nodeY && node->fw_count>0 && node->bk_count==0)
    {
        relation = ASNP2C;
    }
    else if( node && node->nodeX==nodeY && node->nodeY==nodeX && node->fw_count>0 && node->bk_count==0)
    {
        relation = ASNC2P;
    }
    else
    {
        if(node && (isTier1(nodeX) || isTier1(nodeY)) )
            relation = ASNT1Unknown;
        else
            relation = ASNUnknown;
    }
    return relation;
}

ASNRelationships getExtraASNRelation(ASPair* node, ASNodeId nodeX, ASNodeId nodeY, int isObsv)
{
    ASNRelationships relation = ASNUnknown;
    assert(node && node->obsv_relation!=ASNUnknown);
    if(!node)
    {
        return ASNUnknown;
    }

    if(isObsv)
    {
        relation = node->obsv_relation;
        if( node && node->nodeX==nodeY && node->nodeY==nodeX)
        {
            if(relation==ASNP2C)
                relation = ASNC2P;
            else if(relation==ASNC2P)
                relation = ASNP2C;
        }
    }
    if(!isObsv)
    {
        relation = node->true_relation;
        if( node && node->nodeX==nodeY && node->nodeY==nodeX)
        {
            if(relation==ASNP2C)
                relation = ASNC2P;
            else if(relation==ASNC2P)
                relation = ASNP2C;
        }
    }
    return relation;
}
