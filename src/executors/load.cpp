#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 * OR
 * SYNTAX: LOAD MATRIX file_name
 */
bool is_matrix ;
//extern vector<Matrix> loadedMatrices;
bool syntacticParseLOAD()
{
    logger.log("syntacticParseLOAD");
	is_matrix = false ;
    if (tokenizedQuery.size() < 2 || tokenizedQuery.size() > 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
	if(tokenizedQuery.size() == 2)
	{
		parsedQuery.queryType = LOAD;
		parsedQuery.loadRelationName = tokenizedQuery[1];
	} else if(tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX")
	{
		is_matrix = true ;
		parsedQuery.queryType = LOAD;
		cout << "loading matrix " << tokenizedQuery[2] << '\n' ;	
	} else
	{
		cout << "SYNTAX ERROR" << endl;
		return false ;
	}
    return true;
}

bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");
	if(is_matrix)
	{
		//checking for the existence of file.
		if(loadedMatrices.count(tokenizedQuery[2]))
		{
			cout << "SEMANTIC ERROR: Matrix is already Loaded." << endl ;
			is_matrix = false ;
			return false ;
		}
		struct stat buffer ;
		string file_name = "../data/"+tokenizedQuery[2]+".csv" ;
		if(stat(file_name.c_str(), &buffer))
		{
			cout << "SEMANTIC ERROR: Matrix filename not found" << endl ;
			is_matrix = false ;
			return false ;
		}
		return true ;
	}
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");
	if(is_matrix)
	{
		is_matrix = false ;
		Matrix new_matrix;
		new_matrix.load_matrix(tokenizedQuery[2]) ;
		loadedMatrices[tokenizedQuery[2]] = new_matrix;
		return ;
	}
    Table *table = new Table(parsedQuery.loadRelationName);
    if (table->load())
    {
        tableCatalogue.insertTable(table);
        cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    }
    return;
}
