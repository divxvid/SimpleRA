#include "global.h"
/**
 * @brief 
 * SYNTAX: BULK_INSERT <csv_file_name> INTO <table_name>
 */
bool syntacticParseBULK_INSERT()
{
    logger.log("syntacticParseBULK_INSERT");
    if (tokenizedQuery.size() != 4 || tokenizedQuery[2] != "INTO")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = BULK_INSERT;
	parsedQuery.bulkInsertRelationName = tokenizedQuery[3] ;
	parsedQuery.bulkInsertFileName = tokenizedQuery[1] ;
    return true;
}

bool semanticParseBULK_INSERT()
{
    logger.log("semanticParseBULK_INSERT");

    if (!tableCatalogue.isTable(parsedQuery.bulkInsertRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

	if(!isFileExists(parsedQuery.bulkInsertFileName))
	{
		cout << "SEMANTIC ERROR: Data file doesn't exist." << endl ;
		return false ;
	}

	string fname = "../data/" + parsedQuery.bulkInsertFileName + ".csv" ;
	Table* table = tableCatalogue.getTable(parsedQuery.bulkInsertRelationName) ;

	ifstream file(fname, ios::in) ;
	string col_names ;
	getline(file, col_names) ;
	file.close() ;

	vector<string> cols ;
	string word ;
	stringstream ss(col_names) ;
	while(getline(ss, word, ','))
	{
		cols.push_back(word) ;
	}

	if(cols != table->columns)
	{
		cout << "SEMANTIC ERROR: Columns do not match." << endl ;
		return false ;
	}

    return true;
}

void executeBULK_INSERT()
{
    logger.log("executeBULK_INSERT");
	Table* table = tableCatalogue.getTable(parsedQuery.bulkInsertRelationName) ;

	string fname = "../data/" + parsedQuery.bulkInsertFileName + ".csv" ;
	ifstream file(fname, ios::in) ;
	string line ;
	int line_number = 0 ;
	int row_size = -1 ;
	while(getline(file, line))
	{
		++line_number ;
		if(line_number == 1)	
			continue ;
		vector<int> row ;
		string word ;
		stringstream ss(line) ;
		while(getline(ss, word, ','))
		{
			row.push_back(stoi(word)) ;	
		}
		if(row_size == -1)
			row_size = row.size() ;
		assert(row_size == row.size()) ;
		table->insertRow(row) ;
	}
	file.close() ;
    return;
}
