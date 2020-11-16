#include "global.h"
/**
 * @brief 
 * SYNTAX: INDEX ON column_name FROM relation_name USING indexing_strategy
 * indexing_strategy: HASH | BTREE | NOTHING
 */

// TODO : If the table is already indexed, then do something about it .

int default_fanout = 10 ;
bool syntacticParseINDEX()
{
    logger.log("syntacticParseINDEX");
    if (tokenizedQuery.size() != 7 || tokenizedQuery[1] != "ON" || tokenizedQuery[3] != "FROM" || tokenizedQuery[5] != "USING")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = INDEX;
    parsedQuery.indexColumnName = tokenizedQuery[2];
    parsedQuery.indexRelationName = tokenizedQuery[4];
    string indexingStrategy = tokenizedQuery[6];
    if (indexingStrategy == "BTREE")
        parsedQuery.indexingStrategy = BTREE;
    else if (indexingStrategy == "HASH")
        parsedQuery.indexingStrategy = HASH;
    else if (indexingStrategy == "NOTHING")
        parsedQuery.indexingStrategy = NOTHING;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseINDEX()
{
    logger.log("semanticParseINDEX");
    if (!tableCatalogue.isTable(parsedQuery.indexRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if (!tableCatalogue.isColumnFromTable(parsedQuery.indexColumnName, parsedQuery.indexRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.indexRelationName);
    if(table->indexed){
        cout << "SEMANTIC ERROR: Table already indexed" << endl;
        return false;
    }
    return true;
}

void executeINDEX()
{
    logger.log("executeINDEX");
	if(parsedQuery.indexingStrategy == NOTHING)
		return ;
	if(parsedQuery.indexingStrategy == HASH)
	{
		cout << "INDEX USING HASHING IS NOT IMPLEMENTED YET... FALLING BACK TO B+ TREE INDEXING" << endl ;
	}

	//this is the domain of the B+ Tree indexing.
	BPlusTree* index = new BPlusTree(default_fanout, default_fanout);
	Table* table = tableCatalogue.getTable(parsedQuery.indexRelationName) ;			
	const int num_pages = table->blockCount ;
	int col_idx = -1 ;
	for(string s : table->columns)
	{
		++col_idx ;
		if(s == parsedQuery.indexColumnName)
		{
			break ;
		}
	}
	/*
	for(int page_number = 0; page_number < num_pages; ++page_number)
	{
		Page page = bufferManager.getPage(parsedQuery.indexRelationName, page_number);
		int i = 0 ;
		while(true)
		{
			vector<int> row = page.getRow(i) ;
			if(row.empty()) break ;
			int key = row[col_idx] ;		
			index->insert(key, make_pair(page_number, i));
			++i ;
		}	
	}
	*/

	Cursor cursor = table->getCursor() ;
	vector<int> row = cursor.getNext() ;
	while(!row.empty())
	{
		int key = row[col_idx] ;
		index->insert(key, row) ;

		row = cursor.getNext() ;
	}

	table->indexed = true ;
	table->indexedColumn = parsedQuery.indexColumnName ;	
	table->indexingStrategy = BTREE ;
	//indexedColumns[make_pair(parsedQuery.indexRelationName, parsedQuery.indexColumnName)] = index ;
	indexedColumns[parsedQuery.indexRelationName] = make_pair(parsedQuery.indexColumnName, index) ;
	cout << "INDEX CREATION SUCCESSFUL" << endl ;
	default_fanout = 10 ; //resetting it back to it's default.
    return;
}
