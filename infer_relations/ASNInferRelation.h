/************************************************************************************************
* ASInferRelation.h
* Description: Function prototypes of relation inference of tier1 and non-tier1 paths
*
*************************************************************************************************/

#ifndef ASNINFERRELATION_H_INCLUDED
#define ASNINFERRELATION_H_INCLUDED

//SYSTEM INCLUDE
#include "ASCommon.h"

//FUNCTION DECLARATIONS
/**
* Reads the asn pairs from tier1 file and updates a map with fwd and backward counters
* @param hashTable Map that holds the asn pairs and the fwd and backward counter
* @param infilename Tier1 filename
* @return 0 if no error else error
**/
const int create_tier1_relations(ASPair** hashTable, const char* infilename);

/**
* Updates the asn pairs by updating the fwd and backward counters by navigating through the
* valley free determined paths in non-tier1 files
* @param hashTable Map that holds the asn pairs and the fwd and backward counter
* @param infilename NonTier1 filename
* @return 0 if no error else error
**/
const int update_nont1_relations(ASPair** hashTable, const char* infilename);

/**
* Output the relationships between asn pairs and the counters to a file
* @param hashTable Map that holds the asn pairs and backward and fwd counters and
*                   relation between nodes in a pair which have been inferred from tier1 files
*/
void infer_tier1_relations(ASPair** hashTable);

/**
* Output the relationships between asn pairs and the counters to a file
* @param hashTable Map that holds the asn pairs and backward and fwd counters and
*                   relation between nodes in a pair which have been inferred from tier1 files
*/
void write_all_relations(ASPair** hashTable);

/**
* Infer the relationships from non-tier1 vp paths
* @param hashTable Hash table
* @param infilename Filename of nont1 vp file
*/
const int infer_deterministic_paths(ASPair** hashTable, ASPair** nrHashTable, ASNode** lookupTable, const char* infilename, int isTier1File);

/**
* Infer the unresolved relationships
* @param hashTable Hash table
*/
void infer_unresolved_relations(ASNode** lookupTable, ASPair** resHashTable, ASPair** allHashTable);

/**
* Resolve the unresolved relations using degree
**/
void resolve_nr_with_degree();

/**
* Output the relationships between asn pairs and the counters to a file
* @param hashTable Map that holds the resolved asn pairs and backward and fwd counters from the non t1 unresolved pairs
*/
void write_resolved_relations(ASPair** hashTable, ASNode** lookupTable);

/**
* Summarise the results after inferring all the deterministic relations from t1 and nt1 paths
*/
void summarise_results_phase1();

/**
* Summarise the results after inferring unresolved non-t1 relations using all relations hash table
*/
void summarise_results_phase2();

void resolve_true_relations(ASNode** lookupTable, ASPair* allRelTable, ASPair* resHashTable, char* infilename,int isTier1File);

#endif // ASNINFERRELATION_H_INCLUDED
