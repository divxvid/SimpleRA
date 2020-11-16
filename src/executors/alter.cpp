#include "global.h"
/**
 * @brief 
 * SYNTAX: ALTER TABLE <table_name> ADD|DELETE COLUMN <column_name>
 */
bool syntacticParseALTER()
{
    logger.log("syntacticParseALTER");
    if (tokenizedQuery.size() != 6 || tokenizedQuery[1] != "TABLE" || tokenizedQuery[4] != "COLUMN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = ALTER;
	parsedQuery.alterRelationName = tokenizedQuery[2] ;
	parsedQuery.alterColumnName = tokenizedQuery[5] ;
	parsedQuery.alterOperation = tokenizedQuery[3] ; // could be either "ADD" or "DELETE"
    return true;
}

bool semanticParseALTER()
{
    logger.log("semanticParseALTER");

    if (!tableCatalogue.isTable(parsedQuery.alterRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

	if(!tableCatalogue.isColumnFromTable(parsedQuery.alterColumnName, parsedQuery.alterRelationName) && parsedQuery.alterOperation == "DELETE")
	{
		cout << "SEMANTIC ERROR: The Column that you are trying to delete is not present in the given Relation." << endl ;	
		return false ;
	}
	if(tableCatalogue.isColumnFromTable(parsedQuery.alterColumnName, parsedQuery.alterRelationName) && parsedQuery.alterOperation == "ADD")
	{
		cout << "SEMANTIC ERROR: The column that you are trying to add is already present in the given Relation." << endl ;
		return false ;
	}
    return true;
}

void executeALTER()
{
    logger.log("executeALTER");
	Table* table = tableCatalogue.getTable(parsedQuery.alterRelationName) ;
	if(parsedQuery.alterOperation == "ADD")
	{
		//code for adding a column to the table.
		table->addColumn(parsedQuery.alterColumnName) ;	
	} else
	{
		//code for deleting a column from the table.	
		table->removeColumn(parsedQuery.alterColumnName) ;
	}
    return;
}
