/************************************************************************************************
* ASValleyTriggerList.c
* Description: Manages list of nodes that triggers a valley in a valley path
*
*************************************************************************************************/
//EXTERNAL INCLUDES
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

//INTERNAL INCLUDES
#include "ASValleyTriggerList.h"
#include "ASCommon.h"

/**
* ASNVPair
* Stores the left and right node
* lnode Left node in violation
* rnode Right node in violation
* vPattern Associated violation pattern
* pCount Number of times the pattern repeats
**/
typedef struct ASNVPair
{
    char vStr[80];
    ASNValleyPattern vPattern;
    int pCount;
    struct ASNVPair* next;
} ASNVPair;

/**
* ASNVTriggerNode
* Stores the node that causes the valley to form in a valley path
* vnode the trigger node
* p2c_p2p_cnt # of occurences of trigger node in a pattern like >O-
* p2c_c2p_cnt # of occurences of trigger node in a pattern like >O<
* p2p_c2p_cnt # of occurences of trigger node in a pattern like -O<
* p2p_p2p_cnt # of occurences of trigger node in a pattern like -O-
* pList Head pointer to list of neighboring nodes and the associated pattern
* next Pointer to next valley triggering node in the list of valley triggering nodes
**/
typedef struct ASNVTriggerNode
{
    ASNodeId vnode;
    int degree;
    int distance;
    int p2c_p2p_cnt; //>O-
    int p2c_c2p_cnt; //>O<
    int p2p_c2p_cnt; //-O<
    int p2p_p2p_cnt; //-O-
    struct ASNVPair* pList;
    struct ASNVTriggerNode* next;
}ASNVTriggerNode;

//singleton instance of list node
static ASNVTriggerNode* vNodeList = NULL;
static int countNodes = 0;
/**
* Create a violation pair for a node
**/
static ASNVPair* createVPair(char vStr[], ASNValleyPattern vPattern)
{
    ASNVPair* vpair = (ASNVPair*) malloc(sizeof(ASNVPair));
    strcpy(vpair->vStr,vStr);
    vpair->pCount = 1;
    vpair->vPattern = vPattern;
    vpair->next = NULL;
    return vpair;
}

/**
* Create a valley triggering node
**/
static ASNVTriggerNode* createTriggerNode(ASNode* vnode, ASNValleyPattern vPattern)
{
    ASNVTriggerNode* node = (ASNVTriggerNode*) malloc(sizeof(ASNVTriggerNode));
    node->vnode = vnode->nodeX;
    node->degree = vnode->degree;
    node->distance = vnode->distance;
    node->p2c_p2p_cnt = 0;
    node->p2c_c2p_cnt = 0;
    node->p2p_c2p_cnt = 0;
    node->p2p_p2p_cnt = 0;
    node->pList = NULL;
    node->next = NULL;
    ++countNodes;
    return node;
}

/**
* Create a valley pattern for node
**/
static void addViolationPair(ASNVTriggerNode* vNode, char vStr[], ASNValleyPattern vPattern)
{
    int found = 0;

    if(!vNode->pList)
    {
        vNode->pList = createVPair(vStr, vPattern);
        return;
    }

    ASNVPair* vpair = vNode->pList;
    while(vpair)
    {
        if(strcmp(vStr,vpair->vStr)==0 && vpair->vPattern==vPattern)
        {
            found = 1;
            vpair->pCount++;
            break;
        }
        if(vpair->next==NULL && !found)
        {
            vpair->next = createVPair(vStr, vPattern);
        }
        vpair = vpair->next;
    }
}

/**
* Create a valley pattern for node
**/
static void updateVPattern(ASNVTriggerNode* vNode, ASNValleyPattern vPattern)
{
    if(ASNP2C_P2P==vPattern) //>O-
        vNode->p2c_p2p_cnt++;
    if(ASNP2C_C2P==vPattern) //>O<
        vNode->p2c_c2p_cnt++;
    if(ASNP2P_C2P==vPattern) //-O<
        vNode->p2p_c2p_cnt++;
    if(ASNP2P_P2P==vPattern) //-O-
        vNode->p2p_p2p_cnt++;
}

//===========================================================================
// Inserts a node into a list else updates existing node
//============================================================================
void insertTriggerNode(ASNode* vnode, char vStr[], ASNValleyPattern vPattern)
{
    ASNVTriggerNode* node = vNodeList;
    int found = 0;
    while(node!=NULL)
    {
        if(node->vnode==vnode->nodeX)
        {
            //Update the count for this pattern
            updateVPattern(node, vPattern);
            found = 1;
            break;
        }
        node = node->next;
    }
    if(!found)
    {
        node = createTriggerNode(vnode,vPattern);
        updateVPattern(node, vPattern);
        if(!vNodeList)
            vNodeList = node;
        else
        {
            node->next = vNodeList->next;
            vNodeList->next = node;
        }
    }
    //Add it to the violation pair list
    addViolationPair(node, vStr, vPattern);
}

//===========================================================================
// Writes the list of nodes to a file and frees all nodes in the list
//============================================================================
extern int detVPaths;
extern int v_tier1_lines;
extern int v_non_tier1_lines;
extern int vr_lines;
extern int num_violations;
extern int nP2C_C2P_violation;
extern int nP2C_P2P_violation;
extern int nP2P_C2P_violation;
extern int nP2P_P2P_violation;
void outputValleyNodeSummary(int run2)
{

    ASNVTriggerNode* node = vNodeList;
    FILE *ofile_P2C_C2P_summ = NULL;
    FILE *ofile_P2P_P2P_summ = NULL;
    FILE *ofile_P2C_P2P_summ = NULL;
    FILE *ofile_P2P_C2P_summ = NULL;

    if(!run2)
    {
        ofile_P2C_C2P_summ = fopen ( out_P2C_C2P_summ, "a+" );
        ofile_P2P_P2P_summ = fopen ( out_P2P_P2P_summ, "a+" );
        ofile_P2C_P2P_summ = fopen ( out_P2C_P2P_summ, "a+" );
        ofile_P2P_C2P_summ = fopen ( out_P2P_C2P_summ, "a+" );
    }
    else
    {
        ofile_P2C_C2P_summ = fopen ( out_P2C_C2P_summ_res, "a+" );
        ofile_P2P_P2P_summ = fopen ( out_P2P_P2P_summ_res, "a+" );
        ofile_P2C_P2P_summ = fopen ( out_P2C_P2P_summ_res, "a+" );
        ofile_P2P_C2P_summ = fopen ( out_P2P_C2P_summ_res, "a+" );
    }

    int p2c_c2p_nodes = 0;
    int p2p_p2p_nodes = 0;
    int p2c_p2p_nodes = 0;
    int p2p_c2p_nodes = 0;

    int p2c_c2p_patterns = 0;
    int p2p_p2p_patterns = 0;
    int p2c_p2p_patterns = 0;
    int p2p_c2p_patterns = 0;

    while(node!=NULL)
    {
        if(node->p2c_c2p_cnt>0)
        {
            fprintf(ofile_P2C_C2P_summ,"%d:", node->vnode);
            ASNVPair* vpair = node->pList;
            while(vpair)
            {
                if(ASNP2C_C2P==vpair->vPattern)
                {
                    fprintf(ofile_P2C_C2P_summ," %s", vpair->vStr);
                    p2c_c2p_patterns++;
                }
                if(vpair && vpair->next && ASNP2C_C2P==vpair->vPattern && ASNP2C_C2P==vpair->next->vPattern)
                    fprintf(ofile_P2C_C2P_summ,",");
                vpair = vpair->next;
            }
            p2c_c2p_nodes++;
            fprintf(ofile_P2C_C2P_summ,"\n");
        }
        if(node->p2p_p2p_cnt>0)
        {
            fprintf(ofile_P2P_P2P_summ,"%d:", node->vnode);
            ASNVPair* vpair = node->pList;
            while(vpair)
            {
                if(ASNP2P_P2P==vpair->vPattern)
                {
                    fprintf(ofile_P2P_P2P_summ," %s", vpair->vStr);
                    p2p_p2p_patterns++;
                }
                if(vpair && vpair->next && ASNP2P_P2P==vpair->vPattern && ASNP2P_P2P==vpair->next->vPattern)
                    fprintf(ofile_P2P_P2P_summ,",");
                vpair = vpair->next;
            }
            p2p_p2p_nodes++;
            fprintf(ofile_P2P_P2P_summ,"\n");
        }
        if(node->p2c_p2p_cnt>0)
        {
            fprintf(ofile_P2C_P2P_summ,"%d:", node->vnode);
            ASNVPair* vpair = node->pList;
            while(vpair)
            {
                if(ASNP2C_P2P==vpair->vPattern)
                {
                    fprintf(ofile_P2C_P2P_summ," %s", vpair->vStr);
                    p2c_p2p_patterns++;
                }
                if(vpair && vpair->next && ASNP2C_P2P==vpair->vPattern && ASNP2C_P2P==vpair->next->vPattern)
                    fprintf(ofile_P2C_P2P_summ,",");
                vpair = vpair->next;
            }
            p2c_p2p_nodes++;
            fprintf(ofile_P2C_P2P_summ,"\n");
        }
        if(node->p2p_c2p_cnt>0)
        {
            fprintf(ofile_P2P_C2P_summ,"%d:", node->vnode);
            ASNVPair* vpair = node->pList;
            while(vpair)
            {
                if(ASNP2P_C2P==vpair->vPattern)
                {
                    fprintf(ofile_P2P_C2P_summ," %s", vpair->vStr);
                    p2p_c2p_patterns++;
                }
                if(vpair && vpair->next && ASNP2P_C2P==vpair->vPattern && ASNP2P_C2P==vpair->next->vPattern)
                    fprintf(ofile_P2P_C2P_summ,",");
                vpair = vpair->next;
            }
            p2p_c2p_nodes++;
            fprintf(ofile_P2P_C2P_summ,"\n");
        }

        node = node->next;
    }
    //assert(countNodes==p2c_c2p_nodes+p2p_p2p_nodes+p2c_p2p_nodes+p2p_c2p_nodes);
    //printf("Total:node: %d", x);
    printf("Violation >o< : %d nodes %d patterns\n",p2c_c2p_nodes,p2c_c2p_patterns);
    printf("Violation >o- : %d nodes %d patterns\n",p2c_p2p_nodes,p2c_p2p_patterns);
    printf("Violation -o< : %d nodes %d patterns\n",p2p_c2p_nodes,p2p_c2p_patterns);
    printf("Violation -o- : %d nodes %d patterns\n",p2p_p2p_nodes,p2p_p2p_patterns);

    fclose(ofile_P2P_C2P_summ);
    fclose(ofile_P2C_P2P_summ);
    fclose(ofile_P2P_P2P_summ);
    fclose(ofile_P2C_C2P_summ);
}

void outputTriggerNodes(int run2)
{
    ASNVTriggerNode* node = vNodeList;
    FILE *outfile_vtd = NULL;
    if(!run2)
        outfile_vtd = fopen ( out_vtd, "a+" );
    else
        outfile_vtd = fopen ( out_vtd_res, "a+" );

    printf("Wrote %d Valley triggering nodes and %d violations[(><:%d)+(>-:%d)+(-<:%d)+(--:%d)] from %d valley paths\nFormat<ASN:#p2c_c2p #p2c_p2p_cnt #p2p_c2p_cnt #p2p_p2p_cnt>\n\n",
           countNodes, num_violations, nP2C_C2P_violation, nP2C_P2P_violation, nP2P_C2P_violation, nP2P_P2P_violation, detVPaths);

    printf("Percentage of violation >o< : %lf nodes\n",(float)nP2C_C2P_violation/num_violations*100);
    printf("Percentage of violation >o- : %f nodes\n",(float)nP2C_P2P_violation/num_violations*100);
    printf("Percentage of violation -o< : %f nodes\n",(float)nP2P_C2P_violation/num_violations*100);
    printf("Percentage of violation -o- : %f nodes\n",(float)nP2P_P2P_violation/num_violations*100);
    //if(!run2)
    //    assert(detVPaths==(v_tier1_lines+v_non_tier1_lines));
   // else
    //    assert(detVPaths==vr_lines);
    assert(num_violations==(nP2C_C2P_violation+nP2C_P2P_violation+nP2P_C2P_violation+nP2P_P2P_violation));
    while(node!=NULL)
    {
        fprintf(outfile_vtd,"%d:%d %d %d %d\n", node->vnode, node->p2c_c2p_cnt, node->p2c_p2p_cnt, node->p2p_c2p_cnt, node->p2p_p2p_cnt);
        node = node->next;
    }
    fclose(outfile_vtd);
}

//===========================================================================
// Destroy node list
//============================================================================
void cleanupTriggerList()
{
    ASNVTriggerNode* node = vNodeList;
    ASNVTriggerNode* next = NULL;
    while(node!=NULL)
    {
        ASNVPair* vpair = node->pList;
        ASNVPair* vnext = NULL;
        while(vpair)
        {
            vnext = vpair->next;
            free(vpair);
            vpair = NULL;
            vpair = vnext;

        }
        next = node->next;
        free(node);
        node = NULL;
        node = next;
    }
    vNodeList = NULL;
    num_violations=0;
    countNodes = 0;
    vr_lines = 0;
    nP2C_C2P_violation=0;
    nP2C_P2P_violation=0;
    nP2P_C2P_violation=0;
    nP2P_P2P_violation=0;
    detVPaths = 0;
}

//===========================================================================
// Write contents of lookup table to a file
//===========================================================================
void writeVTNInfoToFile(ASNode** lookupTable)
{
    FILE* outfile = fopen ( "VTN_all_info", "w+" );
    FILE* outfile_P2C_C2P = fopen ( "VTN_all_P2C_C2P_info", "w+" );
    FILE* outfile_P2C_P2P = fopen ( "VTN_all_P2C_P2P_info", "w+" );
    FILE* outfile_P2P_C2P = fopen ( "VTN_all_P2P_C2P_info", "w+" );
    FILE* outfile_P2P_P2P = fopen ( "VTN_all_P2P_P2P_info", "w+" );
    int i=0;
    int dist=0;
    for(dist=0;dist<7;++dist)
    {
        ASNVTriggerNode* node = vNodeList;
        while(node!=NULL)
        {
            if(dist==node->distance)
            {
                if(node->p2c_c2p_cnt>0)
                    fprintf(outfile_P2C_C2P,"%d, %d:%d\n", node->distance, node->degree, node->vnode);
                if(node->p2c_p2p_cnt>0)
                    fprintf(outfile_P2C_P2P,"%d, %d:%d\n", node->distance, node->degree, node->vnode);
                if(node->p2p_c2p_cnt>0)
                    fprintf(outfile_P2P_C2P,"%d, %d:%d\n", node->distance, node->degree, node->vnode);
                if(node->p2p_p2p_cnt>0)
                    fprintf(outfile_P2P_P2P,"%d, %d:%d\n", node->distance, node->degree, node->vnode);

                fprintf(outfile,"%d, %d:%d\n", node->distance, node->degree, node->vnode);
            }
            node = node->next;
        }
    }

    fclose(outfile_P2P_P2P);
    fclose(outfile_P2P_C2P);
    fclose(outfile_P2C_P2P);
    fclose(outfile_P2C_C2P);
    fclose(outfile);
}
