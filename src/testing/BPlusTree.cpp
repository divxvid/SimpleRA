#include <bits/stdc++.h>
using namespace std ;

struct BPlusNode
{
	bool is_leaf ; // determines whether the node is a leaf node or an internal node.	
	bool needs_split ; // determines whether the nodes requires a split or not ;
	int fill_ptr ;
	vector<int> keys ; // stores the keys

	BPlusNode* next; // for the leaf doubly linked list
	BPlusNode* prev;
	union pointers
	{
		vector<BPlusNode*> children ; // to store the children of internal nodes.
		vector<int> values ; // to store the values in the leaf nodes.

		pointers() {}
		~pointers() {}
	} children_or_values ;

	void init_internal_node(int internal_fanout)
	{
		is_leaf = false ;
		needs_split = false ;
		fill_ptr = 0 ;
		keys.assign(internal_fanout, -1);
		children_or_values.children.assign(internal_fanout+1, nullptr);
	}
	void init_leaf_node(int leaf_fanout)
	{
		is_leaf = true ;
		needs_split = false ;
		fill_ptr = 0 ;
		keys.assign(leaf_fanout+1, -1);
		children_or_values.values.assign(leaf_fanout+1, -1) ;
		next = nullptr ;
		prev = nullptr ;
	}

	~BPlusNode()
	{
		keys.clear() ;
		if(is_leaf)
			children_or_values.values.clear() ;
		else
			children_or_values.children.clear() ;
	}
};

class BPlusTree
{

	int internal_fanout, leaf_fanout ;
	BPlusNode* root ;

	/*
	 * This is a private function that performs the actual insert and 
	 * return a boolean value that decides whether the node where the insert
	 * is done has overflowed or not. If It has overflowed, then it needs to be split
	 * */
	BPlusNode* insert_p(BPlusNode*, int key, int value) ;

public:
	BPlusTree() 
	{
		root = nullptr ;
		internal_fanout = 10 ;
		leaf_fanout = 10 ;
	}
	BPlusTree(int internal_fo, int leaf_fo)
	{
		root = nullptr ;
		internal_fanout = internal_fo ;
		leaf_fanout = leaf_fo ;
	}
	/*
	 * This is a public function that takes in a key and a value, and uses the private
	 * insert function to insert value value. If the root has overflown, this function is
	 * responsible for creating a new root node.
	 * */
	void insert(int key, int value) ;
	void find(int key) ;
};


BPlusNode* BPlusTree::insert_p(BPlusNode* node, int key, int value)
{
	//If we need to allocate a new node.
	if(node == nullptr)
	{
		BPlusNode* new_node = new BPlusNode() ;		
		new_node->init_leaf_node(leaf_fanout) ;
		
		new_node->keys[new_node->fill_ptr] = key ;
		new_node->children_or_values.values[new_node->fill_ptr] = value ;
		new_node->fill_ptr++ ;
		return new_node ;
	}	
	
	if(node->is_leaf)
	{
		//find the position where the key needs to be inserted.
		auto pos = upper_bound(node->keys.begin(), node->keys.begin()+node->fill_ptr, key) ;
		node->keys.insert(pos, key) ;	
		node->children_or_values.values.insert(pos, value) ;
		node->fill_ptr++ ;

		if(node->fill_ptr > leaf_fanout)
		{
			//the leaf has now overflown, so we will mark the overflown flag and return back toit's parent node who will handle the splitting.
			node->needs_split = true ;
		}
		return node ;
	}	
	// TODO: write the handling of the internal nodes and check for the splits
	
	//At this stage, our current node is definitely an internal node.
	//Find the child where we have to descend.
	
	auto end = node->keys.begin() + node->fill_ptr ;
	int index = upper_bound(node->keys.begin(), end, key) - end ;

	BPlusNode* &child = node->children_or_values.children[index] ;
	child = insert_p(child, key, value) ;

	//now we check if our child needed splitting.
	if(child->needs_split)
	{
		//turn off the flag.
		child->needs_split = false ;

		// NOTE: if the child that we need to split is a leaf node, then we have to change the next and prev pointers as well.

		int mid_index = child->fill_ptr / 2 ;	
		const int limit = child->fill_ptr ;
		int mid_key = child->keys[mid_index] ;
		
		//creating a new node
		BPlusNode* new_node = new BPlusNode() ;

		//TODO : Merge both the conditions
		if(child->is_leaf)
		{
			new_node->init_leaf_node(leaf_fanout) ;
			new_node->prev = child ;
			child->next = new_node ;
			
			for(int i = mid_index; i < limit; ++i)
			{
				new_node->keys[new_node->fill_ptr] = child->keys[i] ;
				new_node->children_or_values.values[new_node->fill_ptr] = child->children_or_values.values[i] ;
				new_node->fill_ptr++ ;
			}
			child->fill_ptr = mid_index ; //made it empty.
			
			//now the mid_key will be inserted into this node and pointers will be re-arranged accordingly.
			//TODO : Check the insertion of the child pointer once more. There could be a problem there.
			node->keys.insert(node->keys.begin()+index, mid_key);
			auto &children = node->children_or_values.children ;
			children.insert(children.begin()+index+1, new_node) ;
			node->fill_ptr++ ;

			if(node->fill_ptr == internal_fanout)
			{
				node->needs_split = true ;
			}
		}
		else
		{
			new_node->init_internal_node(internal_fanout) ;

			for(int i = mid_index; i < limit; ++i)
			{
				new_node->keys[new_node->fill_ptr] = child->keys[i] ;
				new_node->children_or_values.children[new_node->fill_ptr] = child->children_or_values.children[i] ; // the only difference lies in the selection of children or values
				new_node->fill_ptr++ ;
			}
			child->fill_ptr = mid_index ;

			node->keys.insert(node->keys.begin()+index, mid_key);
			auto &children = node->children_or_values.children ;
			children.insert(children.begin()+index+1, new_node) ;
			node->fill_ptr++ ;

			if(node->fill_ptr == internal_fanout)
			{
				node->needs_split = true ;
			}
		}
	}
	return node ;
}

void BPlusTree::insert(int key, int value)
{
	BPlusNode* child = insert_p(root, key, value) ;		
	this->root = child ;
	cout << (child->needs_split) << '\n' ;
	// TODO: Handle the logic for creating a new node here.
	if(child->needs_split)
	{
		//turn off the flag.
		child->needs_split = false ;

		// NOTE: if the child that we need to split is a leaf node, then we have to change the next and prev pointers as well.

		int mid_index = child->fill_ptr / 2 ;	
		const int limit = child->fill_ptr ;
		int mid_key = child->keys[mid_index] ;
		
		//creating a new node
		BPlusNode* node = new BPlusNode() ; // This will be our new root.
		node->init_internal_node(internal_fanout) ;

		BPlusNode* new_node = new BPlusNode() ;

		//TODO : Merge both the conditions
		if(child->is_leaf)
		{
			new_node->init_leaf_node(leaf_fanout) ;
			new_node->prev = child ;
			child->next = new_node ;
			
			for(int i = mid_index; i < limit; ++i)
			{
				new_node->keys[new_node->fill_ptr] = child->keys[i] ;
				new_node->children_or_values.values[new_node->fill_ptr] = child->children_or_values.values[i] ;
				new_node->fill_ptr++ ;
			}
			child->fill_ptr = mid_index ; //made it empty.
			
			//now the mid_key will be inserted into this node and pointers will be re-arranged accordingly.
			//TODO : CAREFULLY OBSERVE THE ASSIGNMENT FOR new_node
			node->keys[node->fill_ptr] = mid_key ;
			node->children_or_values.children[node->fill_ptr] = child ;
			node->fill_ptr++ ;
			node->children_or_values.children[node->fill_ptr] = new_node ;

			if(node->fill_ptr == internal_fanout)
			{
				node->needs_split = true ;
			}
		}
		else
		{
			new_node->init_internal_node(internal_fanout) ;

			for(int i = mid_index; i < limit; ++i)
			{
				new_node->keys[new_node->fill_ptr] = child->keys[i] ;
				new_node->children_or_values.children[new_node->fill_ptr] = child->children_or_values.children[i] ; // the only difference lies in the selection of children or values
				new_node->fill_ptr++ ;
			}
			child->fill_ptr = mid_index ;

			node->keys[node->fill_ptr] = mid_key ;
			node->children_or_values.children[node->fill_ptr] = child ;
			node->fill_ptr++ ;
			node->children_or_values.children[node->fill_ptr] = new_node ;

			if(node->fill_ptr == internal_fanout)
			{
				node->needs_split = true ;
			}
		}
		this->root = node ;
	}
}

int main()
{
	BPlusTree tree(3, 3) ;
	tree.insert(1, 10) ;
	tree.insert(2, 20) ;
	tree.insert(3, 30) ;
	cout << "Done\n" ;
	tree.insert(4, 40) ;
	tree.insert(5, 50) ;
	tree.insert(6, 60) ;
	cout << "done2\n" ;
}
