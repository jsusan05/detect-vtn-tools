#ifndef ASSTUBNODELIST_H_INCLUDED
#define ASSTUBNODELIST_H_INCLUDED

//FORWARD DECLARATION
#include "ASCommon.h"

//CONSTANTS

//FUNCTION DECLARATIONS
/**
* Inserts a node into a list else updates existing node
* @param lookupTable Table to lookup the node information
**/
void findStubNodes(ASNode** lookupTable);

/**
* Writes the list of nodes to a file that represents the pattern it violates
**/
void outputStubListToFile();

#endif // ASSTUBNODELIST_H_INCLUDED
