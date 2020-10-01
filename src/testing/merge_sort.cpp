#include <bits/stdc++.h>
using namespace std ;

const string file_prefix = "EMPLOYEE_Page";
const string sorted_prefix = "sorted_" ;
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

void k_way_merge_sort(const string file_prefix, int num_files, bool is_ascending=true)
{
	vector<ifstream> chunk_files(num_files) ;
	for(int i = 0 ; i < num_files; ++i)
	{
		chunk_files[i].open(file_prefix + to_string(i), ios::in);
	}

	if(is_ascending)
	{
		priority_queue<pair<int, int>, vector<pair<int, int>>> min_heap ;
		for(int i = 0 ; i < num_files; ++i)
		{
							
		}
	}

	for(int i = 0 ; i < num_files; ++i)
	{
		chunk_files[i].close();
	}
}

int main()
{	
	for(int i = 0 ; i < num_files; ++i)	
	{
		const string f_name = file_prefix + to_string(i) ;
		vector<vector<int>> stuff = read_blocks(f_name) ;
		cout << "Read file " << i << '\n' ;
		cout << "contents are : \n" ;
		display_page_contents(stuff) ;
		vector<vector<int>> sorted = sort_page_contents(stuff, 1) ;
		cout << "sorted contents are : \n";
		display_page_contents(sorted) ;
		const string of_name = sorted_prefix + f_name ;
		write_to_file(sorted, of_name) ; 
	}
	const string file_name_prefix = sorted_prefix + file_prefix ;	
	k_way_merge_sort(file_name_prefix, num_files) ;
}
