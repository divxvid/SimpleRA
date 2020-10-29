#include <bits/stdc++.h>
using namespace std ;

struct BPlusNode
{
	bool is_leaf ; // determines whether the node is a leaf node or an internal node.	
	bool needs_split ; // determines whether the nodes requires a split or not ;

	vector<int> keys ; // stores the keys
	vector<BPlusNode*> children ; // to store the children of internal nodes.
	vector<int> values ; // to store the values in the leaf nodes.

	BPlusNode* next; // for the leaf doubly linked list
	BPlusNode* prev;
	/*
	union pointers
	{
		vector<BPlusNode*> children ; // to store the children of internal nodes.
		vector<int> values ; // to store the values in the leaf nodes.

		pointers() {}
		~pointers() {}
	} children_or_values ;
	*/

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
	BPlusNode* root ;

	/*
	 * This is a private function that performs the actual insert and 
	 * return a boolean value that decides whether the node where the insert
	 * is done has overflowed or not. If It has overflowed, then it needs to be split
	 * */
	BPlusNode* insert_p(BPlusNode*, int key, int value) ;
	void delete_nodes(BPlusNode*) ;
	void traverse_p(BPlusNode*) ;
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
	~BPlusTree()
	{
		delete_nodes(root) ;
	}
	/*
	 * This is a public function that takes in a key and a value, and uses the private
	 * insert function to insert value value. If the root has overflown, this function is
	 * responsible for creating a new root node.
	 * */
	void insert(int key, int value) ;
	void find(int key) ;
	void traverse() ;
	void print_content() ;
};

void BPlusTree::print_content()
{
	BPlusNode* trav = root ;
	while(trav != nullptr && !trav->is_leaf)
		trav = trav->children[0] ;
	if(trav == nullptr)
		return ;

	while(trav != nullptr)
	{
		for(int i = 0; i < trav->keys.size(); ++i)
			cout << trav->keys[i] << ' ' << trav->values[i] << '\n' ;

		trav = trav->next ;
	}
}

void BPlusTree::traverse_p(BPlusNode* node)
{
	if(node == nullptr)
		return ;
	cout << "<" ;
	for(int x : node->keys)
		cout << x << "," ;
	cout << "> " ;
	if(!node->is_leaf)
	{
		for(BPlusNode* child : node->children)
			traverse_p(child) ;
	}
}

void BPlusTree::traverse()
{
	traverse_p(root) ;	
	cout << '\n' ;
}

void BPlusTree::delete_nodes(BPlusNode* node)
{
	if(node == nullptr) return ;
	if(node->is_leaf)
	{
		delete node ;
		return ;
	}
	for(BPlusNode* child : node->children)
	{
		delete_nodes(child) ;
	}
	delete node ;
}

BPlusNode* BPlusTree::insert_p(BPlusNode* node, int key, int value)
{
	//If we need to allocate a new node.
	if(node == nullptr)
	{
		BPlusNode* new_node = new BPlusNode() ;		
		new_node->init_leaf_node() ;
		
		new_node->keys.push_back(key) ;
		new_node->values.push_back(value) ;
		return new_node ;
	}	
	
	if(node->is_leaf)
	{
		auto pos = upper_bound(node->keys.begin(), node->keys.end(), key) ;
		int offset = pos - node->keys.begin() ;
		node->keys.insert(node->keys.begin() + offset, key) ;	
		node->values.insert(node->values.begin() + offset, value) ;

		if(node->keys.size() > leaf_fanout) //no of values = no of keys for leaf nodes
		{
			//the leaf has now overflown, so we will mark the overflown flag and return back toit's parent node who will handle the splitting.
			node->needs_split = true ;
		}
		return node ;
	}	
	// TODO: write the handling of the internal nodes and check for the splits
	
	//At this stage, our current node is definitely an internal node.
	//Find the child where we have to descend.
	
	int index = upper_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin() ;
	cout << "keys : " ;
	for(int x : node->keys)
		cout << x << ' ';
	cout << '\n' ;
	cout << "Inserting into child : " << index << '\n' ;

	BPlusNode* &child = node->children[index] ;
	child = insert_p(child, key, value) ;

	//now we check if our child needed splitting.
	if(child->needs_split)
	{
		//turn off the flag.
		child->needs_split = false ;

		// NOTE: if the child that we need to split is a leaf node, then we have to change the next and prev pointers as well.

		const int limit = child->keys.size() ;
		int mid_index = limit/2;
		int mid_key = child->keys[mid_index] ;
		
		//creating a new node
		BPlusNode* new_node = new BPlusNode() ;

		//TODO : Merge both the conditions
		if(child->is_leaf)
		{
			new_node->init_leaf_node() ;
			new_node->prev = child ;
			child->next = new_node ;
			
			for(int i = mid_index; i < limit; ++i)
			{
				new_node->keys.push_back(child->keys[i]) ;
				new_node->values.push_back(child->values[i]) ;
			}
			for(int i = mid_index; i < limit; ++i)
			{
				child->keys.pop_back() ;
				child->values.pop_back() ;
			}
			
			//now the mid_key will be inserted into this node and pointers will be re-arranged accordingly.
			//TODO : Check the insertion of the child pointer once more. There could be a problem there.
			node->keys.insert(node->keys.begin()+index, mid_key);
			auto &children = node->children ;
			children.insert(children.begin()+index+1, new_node) ;

			if(node->keys.size() == internal_fanout)
			{
				node->needs_split = true ;
			}
		}
		else
		{
			new_node->init_internal_node() ;
			BPlusNode* left_mid = child->children[mid_index] ;

			for(int i = mid_index+1; i < limit; ++i)
			{
				new_node->keys.push_back(child->keys[i]) ;
				new_node->children.push_back(child->children[i]) ;
			}
			new_node->children.push_back(child->children.back());
			for(int i = mid_index; i < limit; ++i)
			{
				child->keys.pop_back() ;
				child->children.pop_back() ;
			}
			//child->children.push_back(left_mid) ;

			node->keys.insert(node->keys.begin()+index, mid_key);
			auto &children = node->children ;
			children.insert(children.begin()+index+1, new_node) ;

			if(node->keys.size() == internal_fanout)
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
	cout << "After insert details :\n" ;
	cout << "Key size in child : " << child->keys.size() << '\n' ;
	cout << "Needs split at root : " << (child->needs_split) << '\n' ;
	// TODO: Handle the logic for creating a new node here.
	if(child->needs_split)
	{
		//turn off the flag.
		child->needs_split = false ;
		
		cout << "leaf keys : " ;
		for(int x : child->keys)
			cout << x << ' ' ;
		cout << '\n' ;

		// NOTE: if the child that we need to split is a leaf node, then we have to change the next and prev pointers as well.

		const int limit = child->keys.size() ;
		int mid_index = limit/2;
		int mid_key = child->keys[mid_index] ;
		cout << "CREATING NEW ROOT\n" ;
		cout << "Mid Key : " << mid_key << '\n' ;
		
		//creating a new node
		BPlusNode* node = new BPlusNode() ; // This will be our new root.
		node->init_internal_node() ;

		BPlusNode* new_node = new BPlusNode() ;

		//TODO : Merge both the conditions
		if(child->is_leaf)
		{
			new_node->init_leaf_node() ;
			new_node->prev = child ;
			child->next = new_node ;
			
			for(int i = mid_index; i < limit; ++i)
			{
				new_node->keys.push_back(child->keys[i]) ;
				new_node->values.push_back(child->values[i]) ;
			}
			for(int i = mid_index; i < limit; ++i)
			{
				child->keys.pop_back() ;
				child->values.pop_back() ;
			}
			
			//now the mid_key will be inserted into this node and pointers will be re-arranged accordingly.
			//TODO : CAREFULLY OBSERVE THE ASSIGNMENT FOR new_node
			node->keys.push_back(mid_key) ;
			node->children.push_back(child) ;
			node->children.push_back(new_node) ;

			if(node->keys.size() == internal_fanout)
			{
				node->needs_split = true ;
			}
		}
		else
		{
			new_node->init_internal_node() ;

			BPlusNode* left_mid = child->children[mid_index] ;

			for(int i = mid_index+1; i < limit; ++i)
			{
				new_node->keys.push_back(child->keys[i]) ;
				new_node->children.push_back(child->children[i]) ;
			}
			new_node->children.push_back(child->children.back());
			for(int i = mid_index; i < limit; ++i)
			{
				child->keys.pop_back() ;
				child->children.pop_back() ;
			}
			//child->children.push_back(left_mid) ;

			node->keys.push_back(mid_key) ;
			node->children.push_back(child) ;
			node->children.push_back(new_node) ;

			if(node->keys.size() == internal_fanout)
			{
				node->needs_split = true ;
			}
		}
		this->root = node ;
	}
}

int main()
{
	BPlusTree tree(4, 3) ;
	tree.insert(1, 10) ;
	tree.insert(2, 20) ;
	tree.insert(3, 30) ;
	tree.insert(4, 40) ;
	tree.insert(5, 50) ;
	tree.insert(6, 60) ;
	tree.insert(7, 70) ;
	tree.insert(8, 80) ;
	tree.insert(9, 90) ;

	tree.traverse() ;
	tree.print_content() ;
}
