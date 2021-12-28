/************************************************************************************************
* ASHashTable.h
* Description: Function prototypes of hash table that stores AS pairs and relations
*
*************************************************************************************************/

#ifndef AS_HASHTABLE_H_INCLUDED
#define AS_HASHTABLE_H_INCLUDED

//FORWARD DECLARATION
#include "ASCommon.h"

//CONSTANTS
#define TABLE_SIZE 64000

//FUNCTION DECLARATIONS
/**
* Inserts a node into a hash table else returns existing node
* @param nodeX NodeX to be inserted
* @param nodeY NodeY to be inserted
* @param relOrigin which path the relationship originated from
* @return The ASNNode
**/
ASPair* insertASPair(ASPair** ASNHashTable, ASNodeId nodeX, ASNodeId nodeY, ASNRelationOrigin relOrigin);

ASPair* insertExtraASPair(ASPair** ASNHashTable, ASNodeId nodeX, ASNodeId nodeY, int fwd_cnt, int bk_cnt, ASNRelationships relation, int isObsv);
/**
* Retrieves an ASNNode from the hash table
* @param nodeX NodeX part of key
* @param nodeY NodeY part of key
**/
ASPair* getASPair(ASPair** ASNHashTable, ASNodeId nodeX, ASNodeId nodeY);

/**
* Frees all nodes in the hash table and resets it
* @param hashTable Hash table
**/
void destroyTable(ASPair** hashTable);

#endif // ASHASHTABLE_H_INCLUDED
