#include "global.h"
/**
 * @brief 
 * SYNTAX: INSERT INTO <table name> VALUES <value1>,....
 */
bool syntacticParseINSERT()
{
    logger.log("syntacticParseINSERT");
    if (tokenizedQuery.size() < 5 || tokenizedQuery[1] != "INTO" || tokenizedQuery[3] != "VALUES")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = INSERT;
    parsedQuery.insertionRelationName = tokenizedQuery[2];
	parsedQuery.insertionValues.clear() ;
	for(int i = 4; i < tokenizedQuery.size(); ++i)
		parsedQuery.insertionValues.push_back(tokenizedQuery[i]) ;
    return true;
}

bool semanticParseINSERT()
{
    logger.log("semanticParseINSERT");

    if (!tableCatalogue.isTable(parsedQuery.insertionRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

	for(const string& s : parsedQuery.insertionValues)
	{
		try {
			stoi(s) ;
		} catch (...) {
			cout << "SEMANTIC ERROR: Values must be valid integers." << endl ;
			return false ;
		}
	}
	if(parsedQuery.insertionValues.size() != tableCatalogue.getTable(parsedQuery.insertionRelationName)->columnCount)
	{
		cout << "Number of values provided :" << parsedQuery.insertionValues.size() << endl; 
		cout << "Number of values expected :" << tableCatalogue.getTable(parsedQuery.insertionRelationName)->columnCount << endl;
		cout << "SEMANTIC ERROR: The number of values begin inserted is not equal to the number of columns in the Relation\n" ;
		return false ;
	}
    return true;
}

void executeINSERT()
{
    logger.log("executeINSERT");
	Table* table = tableCatalogue.getTable(parsedQuery.insertionRelationName) ;
	vector<int> row ;
	for(const string& s : parsedQuery.insertionValues)
	{
		row.push_back(stoi(s)) ;
	}
	table->insertRow(row) ;
    return;
}
