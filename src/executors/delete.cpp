#include "global.h"
/**
 * @brief 
 * SYNTAX: DELETE FROM <table name> VALUES <value1>,....
 */
bool syntacticParseDELETE()
{
    logger.log("syntacticParseDELETE");
    if (tokenizedQuery.size() < 5 || tokenizedQuery[1] != "FROM" || tokenizedQuery[3] != "VALUES")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = DELETE;
    parsedQuery.deletionRelationName = tokenizedQuery[2];
	for(int i = 4; i < tokenizedQuery.size(); ++i)
		parsedQuery.deletionValues.push_back(tokenizedQuery[i]) ;
    return true;
}

bool semanticParseDELETE()
{
    logger.log("semanticParseDELETE");

    if (!tableCatalogue.isTable(parsedQuery.deletionRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

	for(const string& s : parsedQuery.deletionValues)
	{
		try {
			stoi(s) ;
		} catch (...) {
			cout << "SEMANTIC ERROR: Values must be valid integers." << endl ;
			return false ;
		}
	}
	if(parsedQuery.deletionValues.size() != tableCatalogue.getTable(parsedQuery.deletionRelationName)->columnCount)
	{
		cout << "Number of values provided :" << parsedQuery.deletionValues.size() << endl; 
		cout << "Number of values expected :" << tableCatalogue.getTable(parsedQuery.deletionRelationName)->columnCount << endl;
		cout << "SEMANTIC ERROR: The number of values being deleted is not equal to the number of columns in the Relation\n" ;
		return false ;
	}
    return true;
}

void executeDELETE()
{
    logger.log("executeDELETE");
	Table* table = tableCatalogue.getTable(parsedQuery.deletionRelationName) ;
	vector<int> row ;
	for(const string& s : parsedQuery.deletionValues)
	{
		row.push_back(stoi(s)) ;
	}
	table->deleteRow(row) ;
    return;
}
