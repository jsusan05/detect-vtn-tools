/************************************************************************************************
* ASCommon.h
* Description: Function prototypes and constants for common
*
*************************************************************************************************/
#ifndef ASCOMMON_H_INCLUDED
#define ASCOMMON_H_INCLUDED

//defines
#define ASN_DEBUG 1

typedef unsigned int ASNodeId;

//CONSTANTS
static const ASNodeId g_tier1_nodes[] ={3356, 7018, 701, 3549, 2914, 3257, 1299, 1239, 3561, 6762, 209, 3320, 2828, 6453, 6461, 174};
#define TOT_TIER1   16

static const char* out_t1_det_vf = "T1_VFD"; //Valley Free Determined
static const char* out_t1_det_nvf = "T1_VD"; //Valley Determined
static const char* out_nt1_det_vf = "NT1_VFD"; //Valley Free Determined
static const char* out_nt1_det_nvf = "NT1_VD"; //Valley Determined
static const char* out_res_vf = "VFR"; //Valley Free Resolved
static const char* out_res_nvf = "VR"; //Valley Resolved
static const char* out_nr = "NR";      //Not resolved
static const char* out_vtd = "VTN";     //Valley triggering nodes
static const char* out_vtd_res = "VTN_res";     //Valley triggering nodes
static const char* out_P2C_C2P_summ = "VTN_info_P2C_C2P";     //Valley triggering nodes nP2C_C2P
static const char* out_P2P_P2P_summ = "VTN_info_P2P_P2P";     //Valley triggering nodes nP2P_P2P
static const char* out_P2C_P2P_summ = "VTN_info_P2C_P2P";     //Valley triggering nodes nP2C_P2P
static const char* out_P2P_C2P_summ = "VTN_info_P2P_C2P";     //Valley triggering nodes nP2P_C2P
static const char* out_P2C_C2P_summ_res = "VTN_info_P2C_C2P_res";     //Valley triggering nodes nP2C_C2P
static const char* out_P2P_P2P_summ_res = "VTN_info_P2P_P2P_res";     //Valley triggering nodes nP2P_P2P
static const char* out_P2C_P2P_summ_res = "VTN_info_P2C_P2P_res";     //Valley triggering nodes nP2C_P2P
static const char* out_P2P_C2P_summ_res = "VTN_info_P2P_C2P_res";     //Valley triggering nodes nP2P_C2P
static const char* out_temp_nr = "_tmp_nr";

//ENUMS
/**
* ASNRelationships
**/
typedef enum ASNRelationships
{
    ASNP2P=0, //Peer to Peer
    ASNP2C=1, //Provider to Customer
    ASNC2P=-1, //Customer to Provider
    ASNS2S=2, //Sibling to Sibling
    ASNUnknown=4, //Unkown
    ASNT1Unknown=3 //Relationship Unknown in Tier1
}ASNRelationships;

/**
* ASNErrors
**/
typedef enum ASNErrors
{
    ASNOUT_ERR=-1
}ASNErrors;

typedef enum ASNRelationOrigin
{
    TIER1_REL=1,
    NON_TIER1_REL,
    TIER1_REL_CHANGED,
    ORIGIN_UNKNOWN,
    UNRESOLVED_REL,
    RESOLVED_REL
}ASNRelationOrigin;

//STRUCTURES


/**
* ASPair
* Stores a pair of nodes and it relation
* nodeX NodeA
* nodeX NodeB
* markUndecided is used to mark a tier1 relation or if relation is undecided when resolving a path
* fw_count # of A->B pairs
* bk_count # of B->A pairs
**/
typedef struct ASPair
{
    ASNodeId nodeX;
    ASNodeId nodeY;
    ASNRelationOrigin relOrigin;
    ASNRelationships true_relation;
    ASNRelationships obsv_relation;
    int fw_count;
    int bk_count;
    struct ASPair* next;
} ASPair;

/**
* ASNode
* Stores a Node and its distance from tier1 and degree
* nodeX Node Id
* degree The degree of the node
**/
typedef struct ASNode
{
    ASNodeId nodeX;
    int degree;
    int distance;
    struct ASNode* next;
} ASNode;

//FUNCTION DECLARATIONS
/**
* Returns 1 if node is a Tier1 node else return 0
* @param nodeX Node Id
* @return 1 if node is tier1 else 0
**/
int isTier1(const ASNodeId nodeX);

/**
* Gets the correct AS relation given a pair of nodes
* @param node Reference to AS node pair
* @param nodeX Node A
* @param nodeY Node B
* @return The relationship between nodeX and nodeY
*/
ASNRelationships getASNRelation(ASPair* node, ASNodeId nodeX, ASNodeId nodeY);

ASNRelationships getExtraASNRelation(ASPair* node, ASNodeId nodeX, ASNodeId nodeY, int isObsv);

#endif // ASCOMMON_H_INCLUDED
