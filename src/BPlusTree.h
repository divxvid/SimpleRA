#include <bits/stdc++.h>
using namespace std ;

struct BPlusNode
{
	bool is_leaf ; // determines whether the node is a leaf node or an internal node.	
	bool needs_split ; // determines whether the nodes requires a split or not ;

	vector<int> keys ; // stores the keys
	vector<BPlusNode*> children ; // to store the children of internal nodes.
	//The values are of the form <page number> <row number> to locate a particular record
	//Pages can be accessed using the page number in the BufferManager class, and within the page class we can access each row using the row number.
	vector<pair<int, int>> values ; // to store the values in the leaf nodes.

	BPlusNode* next; // for the leaf doubly linked list
	BPlusNode* prev;

	void init_internal_node()
	{
		is_leaf = false ;
		needs_split = false ;
	}
	void init_leaf_node()
	{
		is_leaf = true ;
		needs_split = false ;
		next = nullptr ;
		prev = nullptr ;
	}
};

class BPlusTree
{
	int internal_fanout, leaf_fanout ;
	int number_of_elements ;
	int numbers_deleted ;
	int maximal_elements_reached ;
	BPlusNode* root ;

	/*
	 * This is a private function that performs the actual insert and 
	 * return a boolean value that decides whether the node where the insert
	 * is done has overflowed or not. If It has overflowed, then it needs to be split
	 * */
	BPlusNode* insert_p(BPlusNode*, int key, pair<int, int> value) ;
	void delete_nodes(BPlusNode*) ;
	void traverse_p(BPlusNode*) ;
	pair<int, int> get_p(BPlusNode*, int) ;
	void erase_p(BPlusNode*, int key) ;
	void reconstruct() ;
public:
	BPlusTree() 
	{
		root = nullptr ;
		internal_fanout = 10 ;
		leaf_fanout = 10 ;
		number_of_elements = 0 ;
		maximal_elements_reached = 0 ;
		numbers_deleted = 0 ;
	}
	BPlusTree(int internal_fo, int leaf_fo)
	{
		root = nullptr ;
		internal_fanout = internal_fo ;
		leaf_fanout = leaf_fo ;
		number_of_elements = 0 ;
		maximal_elements_reached = 0 ;
		numbers_deleted = 0 ;
	}
	~BPlusTree()
	{
		delete_nodes(root) ;
	}
	/*
	 * This is a public function that takes in a key and a value, and uses the private
	 * insert function to insert value value. If the root has overflown, this function is
	 * responsible for creating a new root node.
	 * */
	void insert(int key, pair<int, int> value) ;
	void traverse() ;
	void print_content() ;
	void print_content_reverse() ;
	pair<int, int> get(int key) ;
	void erase(int key) ;
	int size() ;
	BPlusNode* getReverseRecordIterator();
	BPlusNode* getForwardRecordIterator();
	BPlusNode* next(BPlusNode* iterator, bool forward);
};

//this map stores the pointer to the Index tree that was created on some <table name, column name>.
