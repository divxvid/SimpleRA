#include <string>
#include <vector>
#include <sstream>

class Matrix
{
	int N ;
	int block_n ; // 25 x 25 values will pe present in the data block.
	int data_dim ; //the dimension of the current data.
	int num_blocks_per_row ; // stores the number of blocks that are present for each row of the data.
	int total_blocks ;
	string matrix_filename_prefix ;
	string meta_data_file ; 
	string temp_file_name ;
	void transpose_block(int block_id);
public:
	bool loaded ;
	string matrix_name ;
	Matrix() ;
	Matrix(int n);
	void load_matrix(string file_name) ;
	void transpose() ;
};

extern map<string, Matrix> loadedMatrices ;
