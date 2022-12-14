#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}

void Table::insertRow(vector<int> row)
{
	logger.log("Table::insertRow");
	this->writeRow<int>(row) ;	

	if(this->rowsPerBlockCount[this->blockCount-1] == this->maxRowsPerBlock)
	{
		logger.log("Creating a new page!") ;
		vector<vector<int>> tbr ;
		tbr.push_back(row) ;
		bufferManager.writePage(this->tableName, blockCount, tbr, 1);	
		this->blockCount++ ;
		this->rowsPerBlockCount.push_back(1) ;
		this->updateStatistics(row) ;
		return ;
	}
	//inserting a new page here or appending a row to the last page.
	bufferManager.appendRowToPage(this->tableName, blockCount-1, row);
	//this->updateStatistics(row) ;
	this->rowCount++ ;
	this->rowsPerBlockCount[this->blockCount-1]++ ;
	
	if(this->indexed)
	{
		int col_idx = this->getColumnIndex(this->indexedColumn) ;
		BPlusTree* index = indexedColumns[this->tableName].second ;
		int key = row[col_idx] ;
		index->insert(key, row) ;
		logger.log("Table::insertRow : Row added to the index.") ;
	}
}

void Table::deleteRow(vector<int> row)
{
	logger.log("Table::deleteRow");
	//TODO : I have to manually delete it from the csv file.
	
	Cursor cursor = this->getCursor() ;	
	vector<int> page_row = cursor.getNext() ;
	int index = 0 ;
	while(!page_row.empty() && row != page_row)
	{
		page_row = cursor.getNext() ;
		++index ;
	}

	if(index == this->rowCount) return ;

	int pageIndex = -1 ;
	int s = 0 ;
	for(int i = 0 ; i < this->blockCount; ++i)
	{
		s += this->rowsPerBlockCount[i] ;		
		if(s > index)
		{
			pageIndex = i ;
			this->rowsPerBlockCount[i]-- ;
			this->rowCount-- ;
			break ;	
		}
	}
	bufferManager.deleteRowFromPage(this->tableName, pageIndex, row);
    ofstream fout(this->sourceFileName, ios::out);
    uint count = this->rowCount;

    //print headings
    this->writeRow(this->columns, fout);

    Cursor rewrite_cursor(this->tableName, 0);
    vector<int> r;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        r = rewrite_cursor.getNext();
        this->writeRow(r, fout);
    }
	fout.close();
	if(this->indexed)
	{
		int col_idx = this->getColumnIndex(this->indexedColumn) ;
		BPlusTree* index = indexedColumns[this->tableName].second ;
		int key = row[col_idx] ;
		index->erase(key, row) ;
		logger.log("Table::deleteRow : Row deleted from the index.") ;
	}
}


void Table::addColumn(string columnName)
{
	logger.log("Table::addColumn") ;

	ofstream trunc_file(this->sourceFileName, ofstream::out | ofstream::trunc) ;
	trunc_file.close() ;
	vector<string> temp_cols = this->columns ;
	temp_cols.push_back(columnName) ;

	this->writeRow(temp_cols) ;
	Cursor cursor = this->getCursor() ;
	vector<int> row = cursor.getNext() ;
	while(!row.empty())
	{
		row.push_back(0) ; // for the new column.
		this->writeRow(row) ;
		row = cursor.getNext() ;
	}

	logger.log("Table::addColumn : Rewritten original csv file.") ;
	
	//delete older pages.
	for(int i = 0; i < this->blockCount; ++i)
	{
		bufferManager.deleteFile(this->tableName, i) ;
	}	
	this->blockCount = 0 ;
	this->rowsPerBlockCount.clear() ;
	this->columns.push_back(columnName) ;
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * columnCount));
	this->rowCount = 0 ;
	logger.log("Table::addColumn : Deleted older Pages") ;
	this->blockify() ;
}

void Table::removeColumn(string columnName)
{
	logger.log("Table::removeColumn") ;

	ofstream trunc_file(this->sourceFileName, ofstream::out | ofstream::trunc) ;
	trunc_file.close() ;
	vector<string> temp_cols = this->columns ;
	auto it = find(temp_cols.begin(), temp_cols.end(), columnName) ;
	int col_index = it - temp_cols.begin() ;
	temp_cols.erase(it) ;

	this->writeRow(temp_cols) ;
	Cursor cursor = this->getCursor() ;
	vector<int> row = cursor.getNext() ;
	while(!row.empty())
	{
		vector<int> new_row ;
		for(int i = 0 ; i < row.size(); ++i)
		{
			if(i == col_index) continue ;
			new_row.push_back(row[i]) ;
		}
		this->writeRow(new_row) ;
		row = cursor.getNext() ;
	}

	logger.log("Table::removeColumn : Rewritten original csv file.") ;
	
	//delete older pages.
	for(int i = 0; i < this->blockCount; ++i)
	{
		bufferManager.deleteFile(this->tableName, i) ;
	}	
	this->blockCount = 0 ;
	this->rowsPerBlockCount.clear() ;
	this->columns = temp_cols ;
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * columnCount));
	this->rowCount = 0 ;
	logger.log("Table::removeColumn : Deleted older Pages") ;
	this->blockify() ;
}
