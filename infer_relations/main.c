//SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>

//INTERNAL INCLUDES
#include "ASNInferRelation.h"
#include "ASHashTable.h"
#include "ASDegreeLookup.h"
#include "ASFileOps.h"
#include "ASStubNodeList.h"
#include "ASFileOps.h"

#if 0
void verify_outputs(ASPair** t1hash)
{
    FILE *file = fopen ( "tier1_lu", "r" );

    if ( file )
    {
        char line [80] = {0};
        while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
        {
            int found = 0;
            const char* s = "|";
            char *curr_node = NULL;
            int fwd_cnt = 0;
            int bk_count = 0;
            ASNodeId nodeX, nodeY;
            ASNRelationships relation;
            char* key_tok = strtok(line, s);
            char* rel = strtok(NULL, s);
            char* cntstr = strtok(NULL, s);
            const char* s3 = " ";
            char* f = strtok(cntstr, s3);
            char* b = strtok(NULL, s3);

            //check if in hash table
            char* n1 = strtok(key_tok, s3);
            char* n2 = strtok(NULL, s3);
            ASPair* node = NULL;
            nodeX = atoi(n1);
            nodeY = atoi(n2);
            fwd_cnt = atoi(f);
            bk_count = atoi(b);
            relation = atoi(rel);

            node = getASNode(t1hash,nodeX,nodeY);
            if(node)
            {
                found = node->nodeX==nodeY && node->nodeY==nodeX && fwd_cnt==node->bk_count && bk_count==node->fw_count;
                if(!found)
                    found = node->nodeX==nodeX && node->nodeY==nodeY && fwd_cnt==node->fw_count && bk_count==node->bk_count;
            }
            if(!found)
                perror("Not found");

        }

        fclose ( file );
    }
}

void test_resolvePath(ASPair** hashTable,ASNode** lookupTable)
{

    //ASPair nodeList[] = {11537,22388,7660,2500,2497};
    //char relStack[]=">>?>";
    //ASPair nodeList[] = {11537,22388,7660};
    //char relStack[]=">?";
    ASPair nodeList[] = {22388,7660,2497,11537};
    char relStack[]=">?";
    int t1path = t1InPath(nodeList, relStack);
    printf("%d",t1path);
    //int i;
    //int relLen=strlen(relStack);
    //resPathLeastDist(hashTable,lookupTable,nodeList,relStack);
    //resPathLeastDist(hashTable,lookupTable,nodeList,relStack);

}

void test_detectValleyPath()
{
    int success = 0;
    char* relList = NULL;

    ASNodeId nodeList[] = {1221,4637,6453,7843,11427,62,4323,7018,32382};
    relList = "><>>>=->";
    success = detectValleyNode(nodeList,relList);

    ASNodeId nodeList2[] = {1221,4637,4809,7713,2914,45147,38154,38785};
    relList = "<>=<>>>";
    success = detectValleyNode(nodeList2,relList);


    ASNodeId nodeList3[] = {1221,4637,6453,7843,11427,62,4323,7018,32382};
    relList = "><>>>=->";
    success = detectValleyNode(nodeList3,relList);


    ASNodeId nodeList4[] = {1221,4637,1239,701,209,721,27064,6045,27138,27051};
    relList = "<<-->>>>>";
    success = detectValleyNode(nodeList4,relList);


    ASNodeId nodeList5[] = {1221,4637,1239,701,209,721,27064,6045,27138,27051};
    relList = "<>><>>>>>";
    success = detectValleyNode(nodeList5,relList);


    ASNodeId nodeList6[] = {2152,3356,1239,23520,262206,20299,3573};
    relList = "-<->>>";
    success = detectValleyNode(nodeList6,relList);

    ASNodeId nodeList7[] = {293,6939,21840,3257,3356,40374};
    relList = "<><->";
    success = detectValleyNode(nodeList7,relList);

    ASNodeId nodeList8[] = {1221,4637,1239,701,90,209,721,27064,6045,27138,27051};
    relList = "<<-=->>>>>";
    success = detectValleyNode(nodeList8,relList);

    ASNodeId nodeList9[] = {1221,4637,1239,701,90,209,721,27064,6045,27138,27051};
    relList = "<<-<->>>>>";
    success = detectValleyNode(nodeList9,relList);

    outputTriggerNodes();
    outputValleyNodeSummary();
}
#endif

void generate_compare_file(ASNode** lookupTable)
{
    ASNode* newNodeTable[200]={0};
    FILE* inFile = fopen("../output/VTN_compare.txt","r");

    int listLen=0;
    if ( inFile )
    {
        char* line = NULL;
        int len=0;
        while ( asn_fgetline(inFile,&line, &len)!= NULL ) /* read a line */
        {
            ASNode* node = getASNode(lookupTable,atoi(line));
            assert(node!=NULL);

            ASNode* new_node = (ASNode*) malloc(sizeof(ASNode));
            if(new_node)
            {
                new_node->nodeX = node->nodeX;
                new_node->degree = node->degree;
                new_node->distance = node->distance;
                new_node->next = NULL;
                newNodeTable[listLen++]=new_node;
            }
            free(line);
            line=NULL;
        }
        fclose(inFile);
    }
    FILE* outfile = fopen("../output/VTN_compare_full.txt","w+");
    int i=0;
    int dist=0;
    ASNode* node=NULL;
    for(dist=0;dist<7;++dist)
    {
        for(i=0; i<listLen; i++)
        {
            node =  newNodeTable[i];
            if(dist==node->distance)
                fprintf(outfile,"%d,%d:%d\n",node->distance,node->degree,node->nodeX);
            assert(node->distance<=6);
        }
    }
    fclose(outfile);
}

extern int num_v_paths;
extern int num_vf_paths;
extern int detVPaths;
extern int tot_lines;
extern int res_true;
extern int obsv_tru_paths;
void populateAllRelation(ASNode** lookupTable,ASPair** allRelTable,ASPair** resRelTable)
{
    FILE* inFileObs1 = fopen("../output/all_relations","r");
    FILE* inFileTruth1 = fopen("../output/obsvstruth1.txt","r");
    int true_lines=0;
    if ( inFileObs1 && inFileTruth1)
    {
        char* line = NULL;
        int len=0;
        int line_len=0;
        while ( asn_fgetline(inFileObs1,&line, &line_len)!= NULL ) /* read a line */
        {
            const char* s = "|";
            char *curr_node = NULL;
            int fwd_cnt = 0;
            int bk_count = 0;
            char* key_tok = strtok(line, s);
            char* rel = strtok(NULL, s);
            char* cntstr = strtok(NULL, s);
            const char* s3 = " ";
            char* f = strtok(cntstr, s3);
            char* b = strtok(NULL, s3);
            char* n1 = strtok(key_tok, s3);
            char* n2 = strtok(NULL, s3);
            ASPair* pr = insertExtraASPair(allRelTable,atoi(n1), atoi(n2),atoi(f),atoi(b),atoi(rel),1 );
            assert(pr!=NULL && pr->obsv_relation!=ASNUnknown);
            len++;
            free(line);
            line=NULL;
        }

        //replace with truth
        while ( asn_fgetline(inFileTruth1,&line, &line_len)!= NULL ) /* read a line */
        {
            const char* s = "|";
            char *curr_node = NULL;
            int fwd_cnt = 0;
            int bk_count = 0;
            char* key_tok = strtok(line, s);
            char* cntstr = strtok(NULL, s);
            const char* s3 = " ";
            char* obsv = strtok(cntstr, s3);
            char* tru = strtok(NULL, s3);
            char* n1 = strtok(key_tok, s3);
            char* n2 = strtok(NULL, s3);
            ASPair* pr = insertExtraASPair(allRelTable,atoi(n1), atoi(n2), 0, 0, atoi(tru),0 );
            assert(pr!=NULL && pr->true_relation!=ASNUnknown && pr->obsv_relation!=ASNUnknown && pr->true_relation!=pr->obsv_relation);
            ++true_lines;
            free(line);
            line=NULL;
        }

        fclose(inFileTruth1);
        fclose(inFileObs1);

        FILE* outFile1 = fopen("../output/true_all_relations","w+");
        int i=0;
        ASPair* node = NULL;
        ASNRelationships relation = ASNUnknown;
        for(i=0; i<TABLE_SIZE; i++)
        {
            node =  allRelTable[i];
            while(node)
            {
                relation = getExtraASNRelation(node, node->nodeX, node->nodeY, 0);
                if(relation==ASNUnknown)
                    relation = getExtraASNRelation(node, node->nodeX, node->nodeY, 1);
                fprintf(outFile1,"%d %d|%d|%d %d\n",node->nodeX, node->nodeY, relation, node->fw_count, node->bk_count);
                node = node->next;
            }
        }
        fclose(outFile1);
        printf("Wrote %d relations to true_all_realations\n",len);
    }

    FILE* inFileObs2 = fopen("../output/resolved_relation","r");
    FILE* inFileTruth2 = fopen("../output/obsvstruth2.txt","r");
    if ( inFileObs2 && inFileTruth2)
    {
        char* line = NULL;
        int len=0;
        int line_len=0;
        while ( asn_fgetline(inFileObs2,&line, &line_len)!= NULL ) /* read a line */
        {
            const char* s = "|";
            char *curr_node = NULL;
            char* key_tok = strtok(line, s);
            char* rel = strtok(NULL, s);
            char* cntstr = strtok(NULL, s);
            const char* s3 = " ";
            char* f = strtok(cntstr, s3);
            char* b = strtok(NULL, s3);
            char* n1 = strtok(key_tok, s3);
            char* n2 = strtok(NULL, s3);
            ASPair* pr = insertExtraASPair(resRelTable,atoi(n1), atoi(n2),atoi(f),atoi(b),atoi(rel),1 );
            assert(pr!=NULL && pr->obsv_relation!=ASNUnknown);
            len++;
            free(line);
            line=NULL;
        }

        while ( asn_fgetline(inFileTruth2,&line, &line_len)!= NULL ) /* read a line */
        {
            const char* s = "|";
            char *curr_node = NULL;
            char* key_tok = strtok(line, s);
            char* cntstr = strtok(NULL, s);
            const char* s3 = " ";
            char* obsv = strtok(cntstr, s3);
            char* tru = strtok(NULL, s3);
            char* n1 = strtok(key_tok, s3);
            char* n2 = strtok(NULL, s3);
            ASPair* pr = insertExtraASPair(resRelTable,atoi(n1), atoi(n2), 0, 0,atoi(tru),0 );
            assert(pr!=NULL && pr->true_relation!=ASNUnknown);
            ++true_lines;
            free(line);
            line=NULL;
        }

        fclose(inFileTruth2);

        fclose(inFileObs2);


        int j=0;
        FILE* outFile2 = fopen("../output/true_resolved_relation","w+");
        int i=0;
        ASPair* node = NULL;
        ASNRelationships relation = ASNUnknown;
        for(i=0; i<TABLE_SIZE; i++)
        {
            node =  resRelTable[i];
            while(node)
            {
                relation = getExtraASNRelation(node, node->nodeX, node->nodeY, 0);
                if(relation==ASNUnknown)
                    relation = getExtraASNRelation(node, node->nodeX, node->nodeY, 1);
                fprintf(outFile2,"%d %d|%d|%d %d\n",node->nodeX, node->nodeY, relation, node->fw_count, node->bk_count);
                node = node->next;
            }
        }
        fclose(outFile2);
        printf("Wrote %d relations to true_resolved_relations\n",len);
    }

    //infer relations
    DIR * d = NULL;
    const char* tier1_dir_name = "..//data//tier1_paths//";
    const char* non_tier1_dir_name = "..//data//non_tier1_paths//";
    char filepath[50] = {0};
    //resolve tier1 paths
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
            resolve_true_relations(lookupTable, allRelTable, resRelTable, filepath, 1);
        }
    }
    closedir (d);

    //resolve non-tier1 paths
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
            resolve_true_relations(lookupTable, allRelTable, resRelTable, filepath, 0);
        }
    }
    closedir (d);
    printf("\n-----------PHASE1------------------\n");
    outputTriggerNodes(0);
    outputValleyNodeSummary(0);
    cleanupTriggerList();

    strcpy(filepath, "_tmp_nr");
    resolve_true_relations(lookupTable, allRelTable, resRelTable, filepath , 2);

    printf("\n-----------PHASE2------------------\n");
    outputTriggerNodes(1);
    outputValleyNodeSummary(1);
    cleanupTriggerList();

#if 0
    //assert(res_true==true_lines);
    printf("%d:%d:%d\n",res_true,true_lines,num_hash_true);
    assert(detVPaths<=num_v_paths);
    assert((num_v_paths+num_vf_paths)==tot_lines);
    printf("Resolved %d paths\n",(num_v_paths+num_vf_paths));
#endif
    printf("Total valley Paths=%d Total paths with obs_tru_conflict=%d Total valley paths with obs_tru_conflict=%d Percent=%f\n",num_v_paths,obsv_tru_paths,detVPaths,(float)detVPaths/obsv_tru_paths*100);
}

void find_intersection_vtn_p2p(const char* fileName1, const char* fileName2, char* outfilename)
{
    FILE* inFile1 = fopen(fileName1,"r");
    FILE* inFile2 = fopen(fileName2,"r");
    FILE* outfile = fopen(outfilename,"w+");
    int line_len=0;
    int list1_len=0;
    ASNodeId list1[500] = {0};
    int i = 0;

    if ( inFile1)
    {
        char* line = NULL;
        while ( asn_fgetline(inFile1,&line, &line_len)!= NULL ) /* read a line */
        {
            const char* s = ":";
            char* vtn_node_str = strtok(line, s);
            list1[i++] = atoi(vtn_node_str);
            free(line);
            line=NULL;
        }
        fclose(inFile1);
    }
    list1_len = i;

    if ( inFile2)
    {
        char* line = NULL;
        int i = 0;
        line_len=0;
        while ( asn_fgetline(inFile2,&line, &line_len)!= NULL ) /* read a line */
        {
            const char* s = ":";
            char* vtn_node_str = strtok(line, s);
            //printf("%d\n",atoi(vtn_node_str));
            for(i=0;i<list1_len;++i)
            {
                if(atoi(vtn_node_str)==list1[i])
                    fprintf(outfile, "%d\n",atoi(vtn_node_str));
            }

            free(line);
            line=NULL;
        }
        fclose(inFile2);
    }
    fclose(outfile);

}
//=====================
// Entry point-main()
//=====================
#define REL_GEN 0
#define PHASE1 0
#define PHASE2 0
#define GEN_OUT 0
#define LOOKUP_GEN 1
#define EXTRA_VTN_INFO 0
#define USE_CORRECT_REL 0
#define INT_P2P 1

int main()
{
    ASPair* hashTable[TABLE_SIZE] = {0};
    ASPair* nrHashTable[TABLE_SIZE] = {0};

#if REL_GEN
    DIR * d = NULL;
    const char* tier1_dir_name = "..//data//tier1_paths//";
    const char* non_tier1_dir_name = "..//data//non_tier1_paths//";
    char filepath[50] = {0};

    remove("VFD");
    remove("VFR");
    remove("VD");
    remove("VR");
    remove("NR");
    remove("VTN");
    remove("_tmp");
    remove("tier1_relations");
    remove("all_relations");

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
            create_tier1_relations(hashTable, filepath);
        }
    }
    closedir (d);

    //Infer relation between as pairs with tier1 vantage point files
    //and write them into a file
    infer_tier1_relations(hashTable);

    //Infer valley/valley free paths from non-tier1 paths
    //and update the relations in the existing hash table of relations
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
            update_nont1_relations(hashTable, filepath);
        }
    }
    closedir (d);

    //Write all relationships to a fileb
    write_all_relations(hashTable);
#endif
#if LOOKUP_GEN
    //create degree lookup table
    ASNode* lookupTable[LOOKUP_TABLE_SIZE] = {0};
    populateLookupTable(lookupTable);
#endif
#if USE_CORRECT_REL
    populateAllRelation(lookupTable,hashTable,nrHashTable);
#endif
#if INT_P2P
    find_intersection_vtn_p2p("obsv_all_VTN_info_P2P_P2P","truth_all_VTN_info_P2P_P2P","obsv_truth_all_phase1_P2P_P2P_common");
    find_intersection_vtn_p2p("obsv_all_VTN_info_P2P_P2P_res","truth_all_VTN_info_P2P_P2P_res","obsv_truth_all_phase2_P2P_P2P_common");

#endif // INT_P2P
    //test_resolvePath(hashTable,lookupTable);
#if EXTRA_VTN_INFO
    //generate_compare_file(lookupTable);

#endif

#if GEN_OUT
    //write distances to a file
    writeNodeInfoToFile(lookupTable);
    //write stub list
    findStubNodes(lookupTable);
    outputStubListToFile();
#endif

#if PHASE1
    //Infer valley/valley free paths from tier1 paths
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
            infer_deterministic_paths(hashTable, nrHashTable, lookupTable, filepath, 1);
        }
    }
    closedir (d);

    //Infer valley/valley free paths from non-tier1 paths
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
            infer_deterministic_paths(hashTable, nrHashTable, lookupTable, filepath, 0);
        }
    }
    closedir (d);

    summarise_results_phase1(0);
    //outputTriggerNodes(0);
    //outputValleyNodeSummary();
    #ifndef EXTRA_VTN_INFO
    cleanupTriggerList();
    #endif
#endif
#if PHASE2
    //Write the new resolved relations
    write_resolved_relations(nrHashTable, lookupTable);

    //infer remaining unresolved relations using the resolved relations
    infer_unresolved_relations(lookupTable,nrHashTable, hashTable);
    printf("\n\n//-------PHASE2:Applying resolved relations to NR NT1 paths------------//\n");
    summarise_results_phase2();
    #ifndef EXTRA_VTN_INFO
    outputTriggerNodes(0);
    outputValleyNodeSummary(1);
    #else
    writeVTNInfoToFile(lookupTable);
    #endif
    cleanupTriggerList();
#endif
#if LOOKUP_GEN
    destroyNodeTable(lookupTable);
#endif
    destroyTable(nrHashTable);
    destroyTable(hashTable);

    return 0;
}
