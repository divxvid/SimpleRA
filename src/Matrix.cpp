#include "global.h"

Matrix::Matrix(): block_n(25), loaded(false) {}
Matrix::Matrix(int n): N(n), block_n(25), loaded(false) {}
map<string, Matrix> loadedMatrices ;

void Matrix::load_matrix(string file_name)
{
	matrix_name = file_name ;
	//file_name += ".csv" ; //extension 
	file_name = "../data/"+file_name+".csv" ;
	ifstream f(file_name, fstream::in) ;
	string row ;
	int row_number = 0 ;
	int block_id = 0 ;
	data_dim = 0;

	while(getline(f, row))
	{
		if(row.back() == '\r')
			row.pop_back() ;
		stringstream ss(row) ;
		string word ;
		while(getline(ss, word, ','))
		{
			++data_dim ;	
		}		
		num_blocks_per_row = data_dim/block_n + (data_dim % block_n != 0) ;
		assert(num_blocks_per_row < 1001);
		break ;
	}
	f.close();
	matrix_filename_prefix = "../data/temp/"+matrix_name+"_Block" ;
	temp_file_name = "../data/temp/TEMPFILE" ;

	f.open(file_name, fstream::in);

	vector<ofstream> out(num_blocks_per_row);
	while(getline(f, row))
	{
		if(row_number % block_n == 0)
		{
			for(int i = 0 ; i < num_blocks_per_row; ++i)
			{
				if(row_number != 0)
					out[i].close();
				string file_to_write = matrix_filename_prefix + to_string(block_id++) ;
				out[i].open(file_to_write, fstream::out);
			}
		}
		vector<string> values ;
		string word ;
		if(row.back() == '\r')
			row.pop_back() ;
		stringstream ss(row);
		while(getline(ss, word, ','))
		{
			values.push_back(word);
		}
		for(int i = 0; i < num_blocks_per_row; ++i)
		{
			int limit = min(i*block_n+block_n, data_dim);
			for(int j = i*block_n; j < limit; ++j)
			{
				out[i] << values[j] << ",\n"[j == limit-1] ;
			}
		}
		++row_number ;
	}

	for(ofstream& ff : out)
	{
		if(ff.is_open())
			ff.close();
	}
	f.close();

	total_blocks = block_id ;
	meta_data_file = "../data/temp/"+matrix_name+"_metadata.txt" ;
	ofstream meta(meta_data_file, fstream::out);
	meta << data_dim << '\n'; //outputs the dimension of the original matrix i.e. N
	meta << block_n << '\n' ; //outputs the dimension of a full block.
	meta << block_id << '\n'; //outputs the total number of blocks that the original matrix was divided into.
	meta.close();
	loaded = true ;
	cout << "LOAD SUCCESSFUL\n" ;
}

void Matrix::transpose()
{
	if(!loaded)
		return ;
	//ifstream f(meta_data_file, fstream::in) ;
	//f >> data_dim >> block_n >> number_of_blocks ;
	//f.close();

	for(int i = 0 ; i < total_blocks; ++i)
	{
		transpose_block(i) ;
	}

	num_blocks_per_row = data_dim/block_n + (data_dim % block_n != 0) ;
	for(int i = 0; i < num_blocks_per_row; ++i)
	{
		for(int j = i+1; j < num_blocks_per_row; ++j)
		{
			string fname1 = matrix_filename_prefix + to_string(i*num_blocks_per_row + j);
			string fname2 = matrix_filename_prefix + to_string(j*num_blocks_per_row + i);
			rename(fname1.c_str(), temp_file_name.c_str()) ;
			rename(fname2.c_str(), fname1.c_str()) ;
			rename(temp_file_name.c_str(), fname2.c_str()) ;
		}
	}
}

void Matrix::transpose_block(int block_id)
{
	string f_name = matrix_filename_prefix + to_string(block_id);
	ifstream f(f_name, fstream::in);
	string row ;
	string a[block_n][block_n] ;
	int x = 0, y = 0 ;
	while(getline(f, row))
	{
		if(row.back() == '\r')
			row.pop_back();
		stringstream ss(row) ;	
		string word ;
		y = 0 ;
		while(getline(ss, word, ','))
		{
			a[x][y] = word ;	
			++y ;
		}
		++x ;
	}
	f.close();
	
	ofstream out(f_name, fstream::out);
	for(int i = 0; i < y; ++i)
	{
		for(int j = 0; j < x; ++j)
		{
			out << a[j][i] << ",\n"[j == x-1];
		}
	}
	out.flush();
	out.close();
}
