#include"executor.h"
#include "Matrix.h"
#include "BPlusTree.h"

extern bool is_matrix;
extern float BLOCK_SIZE;
extern uint BLOCK_COUNT;
extern uint PRINT_COUNT;
extern vector<string> tokenizedQuery;
extern ParsedQuery parsedQuery;
extern TableCatalogue tableCatalogue;
extern BufferManager bufferManager;
extern map<string, pair<string, BPlusTree*>> indexedColumns ;
