/************************************************************************************************
* ASInferRelation.c
* Description: Function definition of relation inference of tier1 and non-tier1 paths
*
*************************************************************************************************/
//SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//INTERNAL INCLUDES
#include "ASNInferRelation.h"
#include "ASHashTable.h"
#include "ASCommon.h"
#include "ASValleyTriggerList.h"
#include "ASDegreeLookup.h"
#include "ASFileOps.h"

//CONSTANTS
#define ASN_DEBUG 1

int t1InPath(ASNodeId* nodeStack, char* relStack)
{
    const int relLen = strlen(relStack);
    int t1InPath=0;
    int i=0;
    for(i=0;i<=relLen;++i)
    {
        if(isTier1(nodeStack[i]))
        {
            t1InPath=1;
            break;
        }
    }
    return t1InPath;
}
/**
* Resolves the relations in the path between the pair of nodes
* @param pathStack The stack that contains only nodes
* @param relStack The stack that contains only the relation characters
**/
int resPathLeastDist(ASPair** hashTable, ASNode* lookupTable, ASNodeId* nodeStack, char* relStack, char* line)
{
    int i=0,j=0,k=0;
    int same_dist=0;
    const int relLen = strlen(relStack);
    int leastDist = -1;

    //for(i=0;i<relLen;++i)
     //   printf("%d%c",nodeStack[i],relStack[i]);
    //printf("%d\n",nodeStack[i]);

    //Find the least distance
    for(i=0;i<=relLen;++i)
    {
        ASNode* nodeA = getASNode(lookupTable, nodeStack[i]);
        assert(nodeA!=NULL && nodeA->nodeX==nodeStack[i] && nodeA->distance>=0 && nodeA->distance<7 && nodeA->degree>0);
        if(i==0)
        {
            leastDist = nodeA->distance;
        }
        else
        {
            if(nodeA->distance<leastDist)
            {
                leastDist = nodeA->distance;
            }
        }
    }

    //choose consecutive nodes with least distance

    int prevIndex = -1;
    ASNodeId sameDistList[50] = {0};
    int sameListLen=0;
#if 0
    for(i=0;i<=relLen;++i)
    {
        ASNode* nodeA = getASNode(lookupTable, nodeStack[i]);
        assert(nodeA!=NULL && nodeA->nodeX==nodeStack[i] && nodeA->distance>=0 && nodeA->distance<7 && nodeA->degree>0);
        if(nodeA->distance==leastDist)
        {
            if( i-prevIndex==1 || sameListLen==0)
            {
                sameDistList[sameListLen++]=nodeA->nodeX;
                prevIndex=i;
            }
            else
            {
                break;
            }
        }
    }
#endif
    for(i=relLen;i>=0;--i)
    {
        ASNode* nodeA = getASNode(lookupTable, nodeStack[i]);
        assert(nodeA!=NULL && nodeA->nodeX==nodeStack[i] && nodeA->distance>=0 && nodeA->distance<7 && nodeA->degree>0);
        if(nodeA->distance==leastDist)
        {
            if( prevIndex-i==1 || sameListLen==0)
            {
                sameDistList[sameListLen++]=nodeA->nodeX;
                prevIndex=i;
            }
            else
            {
                break;
            }
        }
    }

    //find the first node with highest degree in the group
    ASNode top_provider = {0};
    ASNode* nodeA = getASNode(lookupTable, sameDistList[0]);
    top_provider.nodeX=nodeA->nodeX;
                top_provider.degree=nodeA->degree;
                top_provider.distance=nodeA->distance;

    if(sameListLen>0)
    {
        for(i=0;i<sameListLen;++i)
        {
            ASNode* nodeA = getASNode(lookupTable, sameDistList[i]);
            if(nodeA->degree>top_provider.degree)
            {
                top_provider.nodeX=nodeA->nodeX;
                top_provider.degree=nodeA->degree;
                top_provider.distance=nodeA->distance;
            }
        }
    }

    //printf("Top:%d\n",top_provider.nodeX);
    //find the top provider is the topmost in the array
    if( relLen>0 && !same_dist)
    {
        i=0;
        j=0;
        for(i=0; i<=relLen; ++i)
        {
            if(nodeStack[i]==top_provider.nodeX)
            {
                FILE* outfile = NULL;
                if(i==relLen)
                {
                    outfile = fopen("same_dist_paths","a+");
                    char aspath[100]={0};
                    k=0;
                    for(k=0;k<relLen;++k)
                    {
                        char nodeStr[30]={0};
                        sprintf(nodeStr,"%d",nodeStack[k]);
                        if(k==0)
                        {
                            strcpy(aspath,nodeStr);
                        }
                        else
                        {
                            strcat(aspath,nodeStr);
                        }
                        int aslen = strlen(aspath);
                        aspath[aslen]=relStack[k];
                        aspath[aslen+1]='\0';
                    }

                    if(k>0)
                    {
                        char nodeStr[30]={0};
                        sprintf(nodeStr,"%d",nodeStack[k]);
                        strcat(aspath,nodeStr);
                        //fprintf(outfile,"%d:%d:%s:%s:(",leastDist,top_provider.nodeX,line,aspath);
                        fprintf(outfile,"%d:%d:%s:(",leastDist,top_provider.nodeX,aspath);
                    }
                }

                //all relations to the left are c2p
                for(j=i;j>0;--j)
                {
                    if(relStack[j-1]=='?')
                    {
                        insertASPair(hashTable, nodeStack[j], nodeStack[j-1], RESOLVED_REL);
                        if(outfile)
                            fprintf(outfile,"%d<%d ",nodeStack[j-1],nodeStack[j]);
                        //printf("%d<%d ",nodeStack[j-1],nodeStack[j]);
                        //relStack[j-1]='<';
                    }
                }
                //all relations to the right are p2c
                for(j=i;j<relLen;++j)
                {
                    if(relStack[j]=='?')
                    {
                        insertASPair(hashTable, nodeStack[j], nodeStack[j+1], RESOLVED_REL);
                        if(outfile)
                            fprintf(outfile,"%d>%d ",nodeStack[j],nodeStack[j+1]);
                        //printf("%d>%d ",nodeStack[j],nodeStack[j+1]);
                        //relStack[j]='>';
                    }
                }
                if(outfile)
                    fprintf(outfile,")\n");
                //printf("\n");
                fclose(outfile);
                break;
            }
        }
    }
    //for(i=0;i<relLen;++i)
    //    printf("%d%c",nodeStack[i],relStack[i]);
    //printf("%d\n",nodeStack[i]);
    return 0;
}

/**
* Detects the node that triggers the valley in a valley path
* @param pathStack The stack that contains only nodes
* @param relStack The stack that contains only the relation characters
* @return 1 if valley triggering node detected else 0
**/
int num_violations = 0;
int nP2C_C2P_violation = 0;
int nP2C_P2P_violation = 0;
int nP2P_C2P_violation = 0;
int nP2P_P2P_violation = 0;
const int detectValleyNode(ASNode** lookupTable, ASNodeId* pathStack, char* relStack)
{
    int i=0;
    const int relLen = strlen(relStack);
    int detSuccess = 0;
    int j=0;
    int success = 0;
    int k=0;
    ASNValleyPattern vPattern = 0;

    for(i=relLen-1; i>=0; --i)
    {
        if (relStack[i]=='<' || relStack[i]=='-')
        {
            //recognize character on the left which must be '>' or '-'
            for(j=i-1;j>=0;--j)
            {
                if(relStack[j]=='<')
                {
                    i=j;
                }
                if(relStack[j]=='>' || relStack[j]=='-')
                {
                    if(relStack[j]=='>'&&relStack[i]=='<')
                    {
                        //printf("pattern violation %d: %d>%d<%d\n",pathStack[j+1], pathStack[j],pathStack[j+1],pathStack[i+1]);
                        vPattern=ASNP2C_C2P;
                        detSuccess = 1;
                        nP2C_C2P_violation++;
                    }
                    if(relStack[j]=='>'&&relStack[i]=='-')
                    {
                        //printf("pattern violation %d: %d>%d-%d\n",pathStack[j+1], pathStack[j],pathStack[j+1],pathStack[i+1]);
                        //printf("pattern violation >O- : %d\n\n",pathStack[j+1]);
                        vPattern=ASNP2C_P2P;
                        detSuccess = 1;
                        nP2C_P2P_violation++;
                    }
                    if(relStack[j]=='-'&&relStack[i]=='<')
                    {
                        //printf("%d\n",pathStack[i]);
                        //printf("pattern violation %d: %d-%d<%d\n",pathStack[j+1], pathStack[j],pathStack[j+1],pathStack[i+1]);
                        vPattern=ASNP2P_C2P;
                        detSuccess = 1;
                        nP2P_C2P_violation++;
                    }
                    if(relStack[j]=='-'&&relStack[i]=='-')
                    {
                        //printf("%d\n",pathStack[i]);
                        //printf("pattern violation %d: %d-%d-%d\n",pathStack[j+1], pathStack[j],pathStack[j+1],pathStack[i+1]);
                        vPattern=ASNP2P_P2P;
                        detSuccess = 1;
                        nP2P_P2P_violation++;
                    }
                    if(detSuccess)
                    {
                        num_violations++;
                        char vStr[80] = {0};
                        for(k=j;k<=i;++k)
                        {
                            sprintf(vStr, "%s%d%c",vStr, pathStack[k],relStack[k]);
                        }
                        sprintf(vStr, "%s%d", vStr, pathStack[k]);
//                        printf("Violation:%s\n", vStr);
                        ASNode* valleyNode = getASNode(lookupTable,pathStack[j+1]);
                        assert(valleyNode!=NULL && valleyNode->nodeX==pathStack[j+1]);
                        insertTriggerNode(valleyNode, vStr, vPattern);
                        detSuccess = 0;
                        success = 1;
                        i=j-1;
                        break;
                    }
                }
            }
            continue;
        }
    }
    return success;
}

/**
* Returns 1 if path is valley free else 0
* @param aRelStack A relation character stack
* @return 1 if valleyfree else 0
**/
static const int isValleyFree(char aRelStack[])
{
    int i=0;
    const int rlen = strlen(aRelStack);
    int is_vf = 1;
    int is_p2c = 0;
    //Find a P2C node and ensure that all the relations that follow are either P2C or S2S
    //Find a P2P node and ensure that all the relations that follow are either P2C or S2S
    for(i=0; i<rlen; ++i)
    {
        if(is_p2c)
        {
            if(!(aRelStack[i]=='>' || aRelStack[i]=='='))
            {
                return !is_vf;
            }
        }
        if(aRelStack[i]=='>' || aRelStack[i]=='-')
        {
            is_p2c = 1;
        }
    }
    return is_vf;
}

/**
* Returns the character that translates to a relation between node pair
* @param node Node reference
* @param nodeX node A
* @param nodeY node B
* @param Undecided relations are considered if 0 else mark them as decided
* @return -:P2P, =:S2S, >:P2C, <:C2P, ?:Unknown, !:T1Unknown
**/
char relCharASPair(ASPair* node, ASNodeId nodeX, ASNodeId nodeY)
{
    char rc = '?';
    ASNRelationships relation = ASNUnknown;
    relation = getASNRelation(node, nodeX, nodeY);
    if(ASNC2P == relation)
        rc = '<';
    if(ASNP2C == relation)
        rc = '>';
    if(ASNP2P == relation)
        rc = '-';
    if(ASNS2S == relation)
        rc = '=';
    if(ASNUnknown == relation)
        rc = '?';
    if(ASNT1Unknown==relation)
        rc = '!';
    return rc;
}

char relExtraCharASPair(ASPair* node, ASNodeId nodeX, ASNodeId nodeY, int obsv)
{
    char rc = '?';
    ASNRelationships relation = ASNUnknown;
    relation = getExtraASNRelation(node, nodeX, nodeY, obsv);
    if(ASNC2P == relation)
        rc = '<';
    if(ASNP2C == relation)
        rc = '>';
    if(ASNP2P == relation)
        rc = '-';
    if(ASNS2S == relation)
        rc = '=';
    if(ASNUnknown == relation)
        rc = '?';
    if(ASNT1Unknown==relation)
        rc = '!';
    return rc;
}

//FUNCTION DEFINITION
//=========================================================
// Populates hash table with AS pairs from input file
//=========================================================
const int create_tier1_relations(ASPair** hashTable, const char* infilename)
{
    FILE *file = fopen ( infilename, "r" );

    if ( file )
    {
        char line [80] = {0};
        while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
        {
            const char* s = " ";
            char *curr_node, *prev_node = NULL;
            curr_node = strtok(line, s);
            while( curr_node != NULL )
            {
                if(prev_node)
                {
                    //Find a pair in the path and add it to the pai list
                    //markUndecided is used to mark a tier1 relation
                    insertASPair(hashTable, atoi(prev_node), atoi(curr_node), TIER1_REL);
                }
                prev_node = curr_node;
                curr_node = strtok(NULL, s);
            }
        }
        fclose ( file );
    }
    else
    {
        perror (infilename);
        return ASNOUT_ERR;
    }
    return 0;
}

//============================================================
// Infer tier1 relations from hash table and write to a file
//============================================================
const int update_nont1_relations(ASPair** hashTable, const char* infilename)
{
    FILE *file = fopen ( infilename, "r" );
    ASPair* node = NULL;

    if ( file )
    {
        int isNT1_det = 0;
        char* line = NULL;
        int len=0;
        while ( asn_fgetline(file,&line, &len)!= NULL ) /* read a line */
        {
            int resSuccess = 0;
            const char* s = " ";
            char *curr_node, *prev_node = NULL;
            int rs_cnt=0;
            ASNodeId nPairStack[51]={0};
            ASNodeId nodeX, nodeY = 0;
            curr_node = strtok(line, s);
            while( curr_node != NULL )
            {
                if(prev_node)
                {
                    //Find a pair in the path and add it to the pair list
                    nodeX = atoi(prev_node);
                    nodeY = atoi(curr_node);
                    nPairStack[rs_cnt]=nodeX;
                    nPairStack[rs_cnt+1]=nodeY;
                    rs_cnt++;
                }
                prev_node = curr_node;
                curr_node = strtok(NULL, s);
            }
            //if path is a valley free deterministic path, then update the relation
            if( rs_cnt>0 )
            {
                int i=0;
                int j=0;
                for(i=0; i<=rs_cnt; ++i)
                {
                    if(isTier1(nPairStack[i]))
                    {
                        isNT1_det = 1;
                        //all relations to the left are c2p
                        for(j=i;j>0;--j)
                        {
                            //printf("%d<%d\n",nPairStack[j-1],nPairStack[j]);
                            insertASPair(hashTable, nPairStack[j], nPairStack[j-1], NON_TIER1_REL);
                        }
                        //all relations to the right are p2c
                        for(j=i;j<rs_cnt;++j)
                        {
                            //printf("%d>%d\n",nPairStack[j],nPairStack[j+1]);
                            insertASPair(hashTable, nPairStack[j], nPairStack[j+1], NON_TIER1_REL);
                        }
                        break;
                    }
                }
            }
            free(line);
            line = NULL;
        }
        fclose ( file );
    }
    else
    {
        perror (infilename);
        return ASNOUT_ERR;
    }

    return 0;
}

//============================================================
// Infer tier1 relations from hash table and write to a file
//============================================================
void infer_tier1_relations(ASPair** hashTable)
{
    const char* out_filename = "tier1_relations";
    FILE *outfile = fopen(out_filename, "w+");
    ASPair* node = NULL;
    int i=0;
    ASNRelationships relation = ASNUnknown;
    for(i=0; i<TABLE_SIZE; i++)
    {
        node =  hashTable[i];
        while(node)
        {
            relation = getASNRelation(node, node->nodeX, node->nodeY);
            fprintf(outfile,"%d %d|%d|%d %d\n",node->nodeX, node->nodeY, relation, node->fw_count, node->bk_count);
            node = node->next;
        }
    }
    fclose(outfile);
}

//============================================================
// Write relations from hash table and write to a file
//============================================================
extern int new_t1rel;
extern int new_nt1rel;
extern int changed_rel;
void write_all_relations(ASPair** hashTable)
{
    printf("new t1:%d new nt1:%d changed:%d\n",new_t1rel,new_nt1rel,changed_rel);
    const char* out_filename = "all_relations";
    const char* out_nt1_filename = "new_nt1_relations";
    const char* out_diff_filename = "changed_relations";
    FILE *outfile = fopen(out_filename, "w+");
    FILE *out_nt1_file = fopen(out_nt1_filename, "w+");
    FILE *out_diff_file = fopen(out_diff_filename, "w+");
    ASPair* node = NULL;
    int i=0;
    ASNRelationships relation = ASNUnknown;

    for(i=0; i<TABLE_SIZE; i++)
    {
        node =  hashTable[i];
        while(node)
        {
            relation = getASNRelation(node, node->nodeX, node->nodeY);
            if(node->relOrigin==TIER1_REL_CHANGED)
            {
                fprintf(out_diff_file,"%d %d|%d\n",node->nodeX, node->nodeY, relation);
            }

            if(node->relOrigin==NON_TIER1_REL)
            {
                fprintf(out_nt1_file,"%d %d|%d|%d %d\n",node->nodeX, node->nodeY, relation, node->fw_count, node->bk_count);
            }
            fprintf(outfile,"%d %d|%d|%d %d\n",node->nodeX, node->nodeY, relation, node->fw_count, node->bk_count);
            node = node->next;
        }
    }
    fclose(out_diff_file);
    fclose(out_nt1_file);
    fclose(outfile);
}

//============================================================
// Write resolved relations from hash table and write to a file
//============================================================
void write_resolved_relations(ASPair** hashTable, ASNode** lookupTable)
{
    //Write the new relations to a file
    FILE *outfile = fopen("resolved_relation", "w+");
    FILE *outviolafile = fopen("distance_resolved_violation", "w+");
    FILE *outdegviolafile = fopen("degree_resolved_violation", "w+");
    int i=0;
    ASPair* node = NULL;
    ASNRelationships relation = ASNUnknown;
    for(i=0; i<TABLE_SIZE; i++)
    {
        node =  hashTable[i];
        while(node)
        {
            relation = getASNRelation(node, node->nodeX, node->nodeY);
            fprintf(outfile,"%d %d|%d|%d %d\n",node->nodeX, node->nodeY, relation, node->fw_count, node->bk_count);
            ASNode* nodeA = getASNode(lookupTable,node->nodeX);
            ASNode* nodeB = getASNode(lookupTable,node->nodeY);
            if(relation==ASNP2C && nodeA->distance>nodeB->distance)
                fprintf(outviolafile,"%d %d|%d|%d %d|%d %d|\n", node->nodeX, node->nodeY, relation, node->fw_count, node->bk_count, nodeA->distance, nodeB->distance);
            if(relation==ASNP2C && nodeA->degree<nodeB->degree)
                fprintf(outdegviolafile,"%d %d|%d|%d %d|%d %d|\n", node->nodeX, node->nodeY, relation, node->fw_count, node->bk_count, nodeA->distance, nodeB->distance);
            node = node->next;
        }
    }
    fclose(outviolafile);
    fclose(outdegviolafile);
    fclose(outfile);
}

//============================================================
// Interpret path relations
//============================================================
static int vf_tier1_lines = 0;
static int vf_non_tier1_lines = 0;
int v_tier1_lines = 0;
int v_non_tier1_lines = 0;
static int nr_lines = 0;
static int tot_tier1_lines = 0;
static int tot_non_tier1_lines = 0;
int detVPaths = 0;
int detnt1path_withnot1=0;
const int infer_deterministic_paths(ASPair** hashTable, ASPair** nrHashTable, ASNode** lookupTable, const char* infilename, int isTier1File)
{
    FILE *file = fopen ( infilename, "r" );

    FILE * outfile_t1_det_vf = fopen(out_t1_det_vf, "a+");
    FILE * outfile_t1_det_nvf = fopen(out_t1_det_nvf, "a+");
    FILE * outfile_nt1_det_vf = fopen(out_nt1_det_vf, "a+");
    FILE * outfile_nt1_det_nvf = fopen(out_nt1_det_nvf, "a+");
    FILE * outfile_tmp = fopen(out_temp_nr, "a+");
    FILE * outfile_notres = fopen(out_nr, "a+");
    ASPair* node = NULL;

    int numlines = 0;
    if ( file )
    {
        char* c_line=NULL;
        char* line = NULL;
        int len=0;
        while ( asn_fgetline(file,&line, &len)!= NULL ) /* read a line */
        {
            //if(strchr(line,'\n'))
            numlines++;
            int resSuccess = 0;
            const char* s = " ";
            char aspath[1024] = {0};
            char *curr_node, *prev_node = NULL;
            char relStack[50]={0}; int rs_cnt=0;
            ASNodeId nPairStack[51]={0};
            ASNodeId nodeX, nodeY = 0;

            c_line = (char*) calloc((len+1),sizeof(char));
            strcpy(c_line,line);

            curr_node = strtok(line, s);
            while( curr_node != NULL )
            {
                if(prev_node)
                {
                    //Find a pair in the path and add it to the pair list
                    nodeX = atoi(prev_node);
                    nodeY = atoi(curr_node);
                    node = getASPair(hashTable, nodeX, nodeY);
                    if(strlen(aspath)==0)
                        strcpy(aspath,prev_node);
                    char rc = relCharASPair(node, nodeX, nodeY);
                    nPairStack[rs_cnt]=nodeX;
                    nPairStack[rs_cnt+1]=nodeY;
                    relStack[rs_cnt++]=rc;
                    const int pathlen=strlen(aspath);
                    aspath[pathlen+1]='\0';
                    aspath[pathlen]=rc;
                    strcat(aspath, curr_node);
                }
                prev_node = curr_node;
                curr_node = strtok(NULL, s);
            }

            if(aspath[strlen(aspath)-1]=='\n')
                aspath[strlen(aspath)-1]='\0';

            //If path has only one node
            if(strlen(relStack)==0)
            {
                if(isTier1File)
                {
                    vf_tier1_lines++;
                    fprintf(outfile_t1_det_vf,"%s\n",prev_node);
                }
                else
                {
                    vf_non_tier1_lines++;
                    fprintf(outfile_nt1_det_vf,"%s\n",prev_node);
                }
            }
            //if path contains not ? and !
            else if(!strchr(aspath,'?') && !strchr(aspath,'!'))
            {
                if(isValleyFree(relStack))
                {
                    if(isTier1File)
                    {
                        vf_tier1_lines++;
                        fprintf(outfile_t1_det_vf,"%s\n",aspath);
                    }
                    else
                    {
                        if(!t1InPath(nPairStack,relStack))
                        {
                            detnt1path_withnot1++;
                        }
                        vf_non_tier1_lines++;
                        fprintf(outfile_nt1_det_vf,"%s\n",aspath);
                    }
                }
                else
                {
                    if(isTier1File)
                    {
                        v_tier1_lines++;
                        fprintf(outfile_t1_det_nvf,"%s\n",aspath);
                    }
                    else
                    {
                        if(!t1InPath(nPairStack,relStack))
                        {
                            detnt1path_withnot1++;
                        }
                        v_non_tier1_lines++;
                        fprintf(outfile_nt1_det_nvf,"%s\n",aspath);
                    }

                    if(detectValleyNode(lookupTable,nPairStack, relStack)) detVPaths++;
                }

            }
            else
            {
                //try to resolve
                assert(!isTier1File);
                ++nr_lines;
                resPathLeastDist(nrHashTable, lookupTable, nPairStack, relStack,c_line);
                fprintf(outfile_tmp,"%s\n",c_line);
            }
            free(c_line);
            c_line = NULL;
            free(line);
            line = NULL;
        }
        fclose(outfile_notres);
        fclose ( outfile_tmp );
        fclose ( outfile_nt1_det_nvf );
        fclose ( outfile_nt1_det_vf );
        fclose ( outfile_t1_det_nvf );
        fclose ( outfile_t1_det_vf );
        fclose ( file );
    }
    else
    {
        perror (infilename);
        return ASNOUT_ERR;
    }

    if(isTier1File)
        tot_tier1_lines+=numlines;
    else
        tot_non_tier1_lines+=numlines;

    //printf("%s:lines:%d\n",infilename,numlines);

    return 0;
}

//===========================================================================
// Summarise the phase 1 results
//============================================================================
void summarise_results_phase1()
{
    printf("\n\n//-------PHASE1:Applying all relations to T1 and NT1 paths------------//\n");
    assert(tot_tier1_lines+tot_non_tier1_lines==(vf_tier1_lines+vf_non_tier1_lines+v_tier1_lines+v_non_tier1_lines+nr_lines));
    assert(detVPaths==v_tier1_lines+v_non_tier1_lines);
    assert(tot_tier1_lines==v_tier1_lines+vf_tier1_lines);
    assert(tot_non_tier1_lines=v_non_tier1_lines+vf_non_tier1_lines+nr_lines);
    printf("\nlines:%d+%d=%d|VFD_t1:%d|VFD_nt1:%d|VD_t1:%d|VD_nt1:%d|NR:%d\n",tot_tier1_lines,tot_non_tier1_lines,tot_tier1_lines+tot_non_tier1_lines,vf_tier1_lines,vf_non_tier1_lines,v_tier1_lines,v_non_tier1_lines, nr_lines);
    printf("Non-T1 determined paths that does not contain a T1 node=%d\n",detnt1path_withnot1);
    printf("Percent of tier1 determined valley paths = %f\n", (float) v_tier1_lines/tot_tier1_lines*100);
    printf("Percent of tier1 determined valley free paths = %f\n", (float) vf_tier1_lines/tot_tier1_lines*100);
    printf("Percent of non-tier1 determined valley paths = %f\n", (float) v_non_tier1_lines/tot_non_tier1_lines*100);
    printf("Percent of non-tier1 determined valley free paths = %f\n", (float) vf_non_tier1_lines/tot_non_tier1_lines*100);
    printf("Percent of non-tier1 not resolved paths = %f\n", (float) nr_lines/tot_non_tier1_lines*100);
}

//===========================================================================
// Infer the pair that had unresolved relations and write them to output file
//============================================================================
static int vfr_lines = 0;
int vr_lines = 0;
void infer_unresolved_relations(ASNode** lookupTable, ASPair** resHashTable, ASPair** allHashTable)
{
    FILE *file = fopen ( out_temp_nr, "r" );
    FILE *outfile_res_vf = fopen ( out_res_vf, "a+" );
    FILE *outfile_res_nvf = fopen ( out_res_nvf, "a+" );
    FILE *outfile_notres = NULL;
    outfile_notres = fopen ( out_nr, "w+" );
    nr_lines = 0;

    if(file)
    {
        char* line = NULL;
        int len=0;
        while ( asn_fgetline(file,&line, &len)!= NULL ) /* read a line */
        {
            const char* s = " ";
            char aspath[1024] = {0};
            char *curr_node, *prev_node = NULL;
            char relStack[50]={0}; int rs_cnt=0;
            ASNodeId nPairStack[51]={0};
            ASPair* node = NULL;
            ASNodeId nodeX, nodeY = 0;
            curr_node = strtok(line, s);
            while( curr_node != NULL )
            {
                if(prev_node)
                {
                    //Find a pair in the path and add it to the pair list
                    nodeX = atoi(prev_node);
                    nodeY = atoi(curr_node);
                    node = getASPair(resHashTable, nodeX, nodeY);
                    if(node)
                        assert(!getASPair(allHashTable, nodeX, nodeY));
                    if(!node)
                        node = getASPair(allHashTable, nodeX, nodeY);

                    if(strlen(aspath)==0)
                        strcpy(aspath,prev_node);
                    char rc = relCharASPair(node, nodeX, nodeY);
                    nPairStack[rs_cnt]=nodeX;
                    nPairStack[rs_cnt+1]=nodeY;
                    relStack[rs_cnt++]=rc;
                    const int pathlen=strlen(aspath);
                    aspath[pathlen+1]='\0';
                    aspath[pathlen]=rc;
                    strcat(aspath, curr_node);
                }
                prev_node = curr_node;
                curr_node = strtok(NULL, s);
            }

            if(aspath[strlen(aspath)-1]=='\n')
                aspath[strlen(aspath)-1]='\0';

            //if path contains not ? and !
            if(!strchr(aspath,'?') && !strchr(aspath,'!'))
            {
                if(isValleyFree(relStack))
                {
                    vfr_lines++;
                    fprintf(outfile_res_vf,"%s\n",aspath);
                }
                else
                {
                    vr_lines++;
                    fprintf(outfile_res_nvf,"%s\n",aspath);
                    if(detectValleyNode(lookupTable,nPairStack, relStack)) detVPaths++;
                }
            }
            else
            {
                //cannot be resolved
                nr_lines++;
                fprintf(outfile_notres,"%s\n",aspath);
            }
            free(line);
            line = NULL;
        }
    }
    fclose ( file );
    fclose(outfile_notres);
    fclose ( outfile_res_nvf );
    fclose ( outfile_res_vf );

    //remove(out_temp);
}

//===========================================================================
// Summarise the phase 2 results
//============================================================================
void summarise_results_phase2()
{
    assert(tot_tier1_lines+tot_non_tier1_lines==(vf_tier1_lines+vf_non_tier1_lines+v_tier1_lines+v_non_tier1_lines+vfr_lines+vr_lines+nr_lines));
    assert(tot_non_tier1_lines=v_non_tier1_lines+vf_non_tier1_lines+vfr_lines+vr_lines+nr_lines);
    printf("\nlines:%d+%d=%d|VFD_t1:%d|VFD_nt1:%d|VD_t1:%d|VD_nt1:%d|VFR:%d|VR:%d|NR:%d\n",tot_tier1_lines,tot_non_tier1_lines,tot_tier1_lines+tot_non_tier1_lines,vf_tier1_lines,vf_non_tier1_lines,v_tier1_lines,v_non_tier1_lines,vfr_lines,vr_lines,nr_lines);
    printf("Percent of non-tier1 determined valley paths = %f\n", (float) v_non_tier1_lines/tot_non_tier1_lines*100);
    printf("Percent of non-tier1 determined valley free paths = %f\n", (float) vf_non_tier1_lines/tot_non_tier1_lines*100);
    printf("Percent of non-tier1 valley-free resolved paths = %f\n", (float) vfr_lines/tot_non_tier1_lines*100);
    printf("Percent of non-tier1 valley resolved paths = %f\n", (float) vr_lines/tot_non_tier1_lines*100);
    printf("Percent of non-tier1 not resolved paths = %f\n", (float) nr_lines/tot_non_tier1_lines*100);
}
int num_vf_paths=0;
int num_v_paths=0;
int tot_lines=0;
int res_true=0;
int obsv_tru_paths=0;
void resolve_true_relations(ASNode** lookupTable, ASPair* allRelTable, ASPair* resRelTable, char* infilename, int isTier1File)
{
    FILE *file = fopen ( infilename, "r" );
    FILE * outfile_t1_det_nvf = fopen("../output/true_t1_vd_paths", "a+");
    FILE * outfile_t1_det_vf = fopen("../output/true_t1_vfd_paths", "a+");
    FILE * outfile_nt1_det_nvf = fopen("../output/true_nt1_vd_paths", "a+");
    FILE * outfile_nt1_det_vf = fopen("../output/true_nt1_vfd_paths", "a+");
    FILE * outfile_res_nvf = fopen("../output/true_res_vd_paths", "a+");
    FILE * outfile_res_vf = fopen("../output/true_res_vfd_paths", "a+");
    ASPair* node = NULL;

    int numlines = 0;
    if ( file )
    {
        char* line = NULL;
        int len=0;
        while ( asn_fgetline(file,&line, &len)!= NULL ) /* read a line */
        {
            int obsv_tru_exists = 0;
            const char* s = " ";
            char aspath[1024] = {0};
            char *curr_node, *prev_node = NULL;
            char relStack[50]={0}; int rs_cnt=0;
            ASNodeId nPairStack[51]={0};
            ASNodeId nodeX, nodeY = 0;
            curr_node = strtok(line, s);
            ++tot_lines;
            int inRes=0;
            while( curr_node != NULL )
            {
                if(prev_node)
                {
                    //Find a pair in the path and add it to the pair list
                    nodeX = atoi(prev_node);
                    nodeY = atoi(curr_node);
                    int inResHash=0;
                    ASPair* pr = getASPair(allRelTable,nodeX,nodeY);
                    if(pr==NULL)
                    {
                        inRes=1;
                        pr = getASPair(resRelTable,nodeX,nodeY);
                    }
                    assert(pr!=NULL);
                    ASNRelationships relation = ASNUnknown;

                    relation = pr->true_relation;
                    char rc = '?';
                    rc = relExtraCharASPair(pr,nodeX,nodeY,1);

                    //Consider tryth first
                    if(relation==ASNUnknown)
                    {
                        //if truth is unknown, then consider observed
                        rc = relExtraCharASPair(pr,nodeX,nodeY,1);
                    }
                    else
                    {
                        //if truth is known, then consider truth or observed
                        obsv_tru_exists = 1;
                        rc = relExtraCharASPair(pr,nodeX,nodeY,1);
                        assert(pr->obsv_relation!=ASNUnknown && pr->true_relation!=ASNUnknown);
                        assert(pr->obsv_relation!=pr->true_relation);
                    }

                    assert(rc!='?');
                    if(strlen(aspath)==0)
                        strcpy(aspath,prev_node);

                    nPairStack[rs_cnt]=nodeX;
                    nPairStack[rs_cnt+1]=nodeY;
                    relStack[rs_cnt++]=rc;
                    const int pathlen=strlen(aspath);
                    aspath[pathlen+1]='\0';
                    aspath[pathlen]=rc;
                    strcat(aspath, curr_node);
                }
                prev_node = curr_node;
                curr_node = strtok(NULL, s);
            }

            if(aspath[strlen(aspath)-1]=='\n')
                aspath[strlen(aspath)-1]='\0';

            //If path has only one node
            if(strlen(relStack)==0)
            {
                num_vf_paths++;
                if(isTier1File)
                {
                    if(strchr(prev_node,'\n'))
                        fprintf(outfile_t1_det_vf,"%s",prev_node);
                    else
                        fprintf(outfile_t1_det_vf,"%s\n",prev_node);
                }
                else
                {
                    if(strchr(prev_node,'\n'))
                        fprintf(outfile_nt1_det_vf,"%s",prev_node);
                    else
                        fprintf(outfile_nt1_det_vf,"%s\n",prev_node);
                }

            }
            //if path contains not ? and !
            assert(!strchr(aspath,'?'));
            if(!strchr(aspath,'?') && strlen(relStack)>0)
            {
                if(obsv_tru_exists)
                    obsv_tru_paths++;

                if(isValleyFree(relStack))
                {
                    num_vf_paths++;
                    if(isTier1File==1)
                        fprintf(outfile_t1_det_vf,"%s\n",aspath);
                    else if(inRes && isTier1File==2)
                        fprintf(outfile_res_vf,"%s\n",aspath);
                    else if(!inRes && isTier1File==0)
                        fprintf(outfile_nt1_det_vf,"%s\n",aspath);
                }
                else
                {
                    if(isTier1File==1)
                        fprintf(outfile_t1_det_nvf,"%s\n",aspath);
                    else if(inRes && isTier1File==2)
                        fprintf(outfile_res_nvf,"%s\n",aspath);
                    else if(!inRes && isTier1File==0)
                        fprintf(outfile_nt1_det_nvf,"%s\n",aspath);
                    num_v_paths++;
                    //if(obsv_tru_exists)
                    if((isTier1File==1) || (!inRes && isTier1File==0) || (inRes && isTier1File==2) )
                    {
                        if(detectValleyNode(lookupTable,nPairStack, relStack)) detVPaths++;
                    }
                }
            }

            free(line);
            line = NULL;
        }
        fclose ( outfile_res_nvf );
        fclose ( outfile_res_vf );
        fclose ( outfile_t1_det_nvf );
        fclose ( outfile_nt1_det_nvf );
        fclose ( outfile_t1_det_vf );
        fclose ( outfile_nt1_det_vf );
        fclose ( file );
    }
    return 0;
}
