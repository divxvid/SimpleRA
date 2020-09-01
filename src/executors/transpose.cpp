#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE MATRIXNAME
 */
bool syntacticParseTRANSPOSE()
{
    logger.log("syntacticParseTRANSPOSE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE;
    return true;
}

bool semanticParseTRANSPOSE()
{
   	if(loadedMatrices.count(tokenizedQuery[1]) == 0)
	{
		//Matrix not found
		cout << "SEMANTIC ERROR: Matrix Not Found." << endl;
		return false ;
	}
    return true;
}

void executeTRANSPOSE()
{
    logger.log("executeTRANSPOSE");
   	Matrix& m = loadedMatrices[tokenizedQuery[1]];
	m.transpose();
	cout << "TRANSPOSE SUCCESSFUL\n" ;
}
