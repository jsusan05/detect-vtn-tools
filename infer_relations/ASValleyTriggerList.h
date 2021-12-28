#ifndef ASVALLEYTRIGGERLIST_H_INCLUDED
#define ASVALLEYTRIGGERLIST_H_INCLUDED

//FORWARD DECLARATION
#include "ASCommon.h"

//CONSTANTS

/**
* ASNValleyPattern
**/
typedef enum ASNValleyPattern
{
    ASNP2C_P2P=1,
    ASNP2C_C2P,
    ASNP2P_C2P,
    ASNP2P_P2P
}ASNValleyPattern;

//FUNCTION DECLARATIONS
/**
* Inserts a node into a list else updates existing node
* @param vnode Node that is to be inserted
* @param vPattern The valley pattern observed for this node
**/
void insertTriggerNode(ASNode* vnode, char vStr[], ASNValleyPattern vPattern);

/**
* Writes the list of nodes to a file that represents the pattern it violates
**/
void outputValleyNodeSummary();

/**
* Writes the list of nodes to a file
**/
void outputTriggerNodes();

/**
* Write the VTN info to a single file and separate violation file of format distance, degree:nodeId
* @param lookupTable Look up table
**/
void writeVTNInfoToFile(ASNode** lookupTable);

/**
* Deletes nodes in the list of trigger nodes
**/
void cleanupTriggerList();

char* process_nr_line(char* line, ASNode** lookupTable, ASPair** hashTable);
int t1InPath(ASNodeId* nodeStack, char* relStack);
#endif // ASVALLEYTRIGGERLIST_H_INCLUDED
