/*
	NOTE : The column index for sorting will be 0 based.
*/
#include <bits/stdc++.h>
using namespace std ;

const string file_prefix = "EMPLOYEE_Page";
const string sorted_prefix = "sorted_" ;
const string output_file_prefix = "final_"+file_prefix ;
int num_files = 2 ;
vector<vector<int>> read_blocks(const string file_name)
{
	ifstream f ;
	f.open(file_name, ios::in);
	string line ;
	vector<vector<int>> ret ;
	while(getline(f, line))
	{
		string word ;
		vector<int> row ;
		stringstream ss(line);
		while(getline(ss, word, ' '))
		{
			row.push_back(stoi(word));	
		}
		ret.push_back(row) ;
	}
	f.close();
	return ret ;
}

void display_page_contents(vector<vector<int>>& stuff)
{
	for(const vector<int>& r : stuff)
	{
		for(const int x : r)
			cout << x << ' ' ;
		cout << '\n' ;
	}
	cout << '\n' ;
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
	cout << fname << " written\n" ;
}

void k_way_merge_sort(const string file_prefix, int num_files, int sort_column, int op_buff_size, bool is_ascending=true)
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
				const string op_file_name = output_file_prefix + to_string(file_number++) ;
				ofstream op_file(op_file_name, ios::out) ;
				for(const string& str : buffer)
				{
					op_file << str << '\n' ;
				}
				op_file.close() ;
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
				const string op_file_name = output_file_prefix + to_string(file_number++) ;
				ofstream op_file(op_file_name, ios::out) ;
				for(const string& str : buffer)
				{
					op_file << str << '\n' ;
				}
				op_file.close() ;
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
		const string op_file_name = output_file_prefix + to_string(file_number++) ;
		ofstream op_file(op_file_name, ios::out) ;
		for(const string& str : buffer)
		{
			op_file << str << '\n' ;
		}
		op_file.close() ;
		buffer.clear() ;
	}
	for(int i = 0 ; i < num_files; ++i)
	{
		chunk_files[i].close();
	}
	cout << "Sorting done\n" ;
}

int main()
{	
	int output_buffer_size = 0 ;
	int sorted_col = 1;
	for(int i = 0 ; i < num_files; ++i)	
	{
		const string f_name = file_prefix + to_string(i) ;
		vector<vector<int>> stuff = read_blocks(f_name) ;
		int n_rows = stuff.size() ;
		output_buffer_size = max(output_buffer_size, n_rows);
		cout << "Read file " << i << '\n' ;
		cout << "contents are : \n" ;
		display_page_contents(stuff) ;
		vector<vector<int>> sorted = sort_page_contents(stuff, sorted_col, false) ;
		cout << "sorted contents are : \n";
		display_page_contents(sorted) ;
		const string of_name = sorted_prefix + f_name ;
		write_to_file(sorted, of_name) ; 
	}
	const string file_name_prefix = sorted_prefix + file_prefix ;	
	k_way_merge_sort(file_name_prefix, num_files, sorted_col, output_buffer_size, false) ;
}
