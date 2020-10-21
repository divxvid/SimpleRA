#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
const int BUFF_SIZE = 16; 
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    if(tokenizedQuery.size()!= 8 || tokenizedQuery[4] != "BY" || tokenizedQuery[6] != "IN"){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortColumnName = tokenizedQuery[5];
    parsedQuery.sortRelationName = tokenizedQuery[3];
    string sortingStrateg = tokenizedQuery[7];
    if(sortingStrateg == "ASC")
        parsedQuery.sortingStrategy = ASC;
    else if(sortingStrateg == "DESC")
        parsedQuery.sortingStrategy = DESC;
    else{
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName, parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    return true;
}

vector<vector<int>> sort_page_contents(vector<vector<int>>& A, int col_idx, bool is_ascending=true)
{
	vector<pair<int, int>> tmp ;
	for(int i = 0 ; i < A.size(); ++i)
	{
		tmp.emplace_back(A[i][col_idx], i) ;
	}

	if(is_ascending)
		sort(tmp.begin(), tmp.end()) ;
	else
		sort(tmp.rbegin(), tmp.rend()) ;

	vector<vector<int>> sorted ;
	for(const pair<int, int>& p : tmp)
	{
		sorted.push_back(A[p.second]) ;
	}
	return sorted ;
}

void write_to_file(vector<vector<int>>& A, const string fname)
{
	ofstream f ;
	f.open(fname, ios::out) ;
	for(const vector<int>& row : A)
	{
		for(int i = 0 ; i < row.size(); ++i)
		{
			f << row[i] << " \n"[i == row.size()-1] ;
		}
	}
	f.close() ;
}

void k_way_merge_sort(Table* resultantTable, const string file_prefix, int num_files, int sort_column, int op_buff_size, bool is_ascending=true)
{
	vector<ifstream> chunk_files(num_files) ;
	for(int i = 0 ; i < num_files; ++i)
	{
		chunk_files[i].open(file_prefix + to_string(i), ios::in);
	}

	vector<string> buffer ;
	int file_number = 0 ;
	if(is_ascending)
	{
		priority_queue<tuple<int, int, string>, vector<tuple<int, int, string>>, greater<tuple<int, int, string>>> min_heap ;
		//init the heap with the first elements of all the files
		for(int i = 0 ; i < num_files; ++i)
		{
			string line, word;
			getline(chunk_files[i], line) ;
			stringstream ss(line) ;
			int idx = 0 ;
			while(getline(ss, word, ' ') && idx != sort_column)
			{
				++idx ;
			}
			int key = stoi(word) ;	
			min_heap.emplace(key, i, line);
		}

		//Now, we will perform the k way merge of all the different files
		while(!min_heap.empty())
		{
			tuple<int, int, string> t = min_heap.top(); min_heap.pop() ;
			buffer.push_back(get<2>(t)) ;
			if(buffer.size() == op_buff_size)
			{
				//flush the buffer
				for(const string& str : buffer)
				{
                    string word ;
                    vector<int> row ;
                    stringstream ss(str);
                    while(getline(ss, word, ' '))
                    {
                        row.push_back(stoi(word));	
                    }
                    resultantTable->writeRow<int>(row) ;
				}
				buffer.clear() ;
			}
			
			string line, word;
			if(getline(chunk_files[get<1>(t)], line))
			{
				stringstream ss(line) ;
				int idx = 0 ;
				while(getline(ss, word, ' ') && idx != sort_column)
				{
					++idx ;
				}
				int key = stoi(word) ;	
				min_heap.emplace(key, get<1>(t), line);
			}
		}
	} else
	{
		priority_queue<tuple<int, int, string>, vector<tuple<int, int, string>>> max_heap ;
		//init the heap with the first elements of all the files
		for(int i = 0 ; i < num_files; ++i)
		{
			string line, word;
			getline(chunk_files[i], line) ;
			stringstream ss(line) ;
			int idx = 0 ;
			while(getline(ss, word, ' ') && idx != sort_column)
			{
				++idx ;
			}
			int key = stoi(word) ;	
			max_heap.emplace(key, i, line);
		}

		//Now, we will perform the k way merge of all the different files
		while(!max_heap.empty())
		{
			tuple<int, int, string> t = max_heap.top(); max_heap.pop() ;
			buffer.push_back(get<2>(t)) ;
			if(buffer.size() == op_buff_size)
			{
				//flush the buffer
				
				for(const string& str : buffer)
				{
                    string word ;
                    vector<int> row ;
                    stringstream ss(str);
                    while(getline(ss, word, ' '))
                    {
                        row.push_back(stoi(word));	
                    }
                    resultantTable->writeRow<int>(row) ;
				}
				buffer.clear() ;
			}
			
			string line, word;
			if(getline(chunk_files[get<1>(t)], line))
			{
				stringstream ss(line) ;
				int idx = 0 ;
				while(getline(ss, word, ' ') && idx != sort_column)
				{
					++idx ;
				}
				int key = stoi(word) ;	
				max_heap.emplace(key, get<1>(t), line);
			}
		}
	}

	if(!buffer.empty())
	{
		//flush the buffer
		for(const string& str : buffer)
		{
            string word ;
            vector<int> row ;
            stringstream ss(str);
            while(getline(ss, word, ' '))
            {
                row.push_back(stoi(word));	
            }
            resultantTable->writeRow<int>(row) ;
		}
		buffer.clear() ;
	}
	for(int i = 0 ; i < num_files; ++i)
	{
		chunk_files[i].close();
	}
}

void executeSORT(){
    logger.log("executeSORT");
    const string temp_file_prefix = "../data/temp/SORTTEMP_" + parsedQuery.sortRelationName ;
    Table table = *tableCatalogue.getTable(parsedQuery.sortRelationName);
    vector<string> columns = table.columns ;
    bool is_ascending = parsedQuery.sortingStrategy == ASC ;

    Table* resultantTable = new Table(parsedQuery.sortResultRelationName, columns);

    Cursor cursor(parsedQuery.sortRelationName, 0) ;
    int column_number = -1 ;
    for(int i = 0; i < columns.size(); ++i)
    {
        if(columns[i] == parsedQuery.sortColumnName)
        {
            column_number = i ;
            break ;
        }
    }

    int block_number = 0 ;
    vector<vector<int>> rows_in_page ;
    for(int i = 0 ; i < table.rowCount; ++i)
    {
        vector<int> row = cursor.getNext() ;
        rows_in_page.push_back(row) ;
        if((i+1) % table.maxRowsPerBlock == 0)
        {
            const string f_name = temp_file_prefix + to_string(block_number);
            vector<vector<int>> sorted_contents = sort_page_contents(rows_in_page, column_number, is_ascending) ;
            write_to_file(sorted_contents, f_name) ;
            rows_in_page.clear() ;
            ++block_number ;
        }
    }
    if(!rows_in_page.empty())
    {
        const string f_name = temp_file_prefix + to_string(block_number);
        vector<vector<int>> sorted_contents = sort_page_contents(rows_in_page, column_number, is_ascending) ;
        write_to_file(sorted_contents, f_name) ;
        rows_in_page.clear() ;
        ++block_number ;
    }

    k_way_merge_sort(resultantTable, temp_file_prefix, block_number, column_number, BUFF_SIZE, is_ascending) ;
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable) ;

    system("rm -rf ../data/temp/SORTTEMP_*");
    cout << "TABLE SORTED\n" ;
    return;
}
