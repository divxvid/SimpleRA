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
		if(table->indexed && table->indexedColumn == parsedQuery.alterColumnName)
		{
			table->indexed = false ;
			table->indexedColumn = "" ;
			table->indexingStrategy = NOTHING ;
			auto it = indexedColumns.find(table->tableName) ;	
			delete (it->second).second ;
			indexedColumns.erase(it) ;
			cout << "REMOVED INDEX ON " << parsedQuery.alterColumnName << endl ;
		}
	}

	if(table->indexed)
	{
		//need to recreate the index sadly.	
		auto it = indexedColumns.find(table->tableName) ;	
		int fanout = (it->second).second->get_current_fanout() ;
		delete (it->second).second ;
		indexedColumns.erase(it) ;

		
		BPlusTree* index = new BPlusTree(fanout, fanout);
		const int num_pages = table->blockCount ;
		int col_idx = -1 ;
		for(string s : table->columns)
		{
			++col_idx ;
			if(s == table->indexedColumn)
			{
				break ;
			}
		}
		Cursor cursor = table->getCursor() ;
		vector<int> row = cursor.getNext() ;
		while(!row.empty())
		{
			int key = row[col_idx] ;
			index->insert(key, row) ;

			row = cursor.getNext() ;
		}

		indexedColumns[table->tableName] = make_pair(table->indexedColumn, index) ;
	}

    return;
}
