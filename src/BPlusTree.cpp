#include "global.h" 

map<pair<string, string>, BPlusTree*> indexedColumns ;
int BPlusTree::size()
{
	return number_of_elements ;
}

void BPlusTree::reconstruct()
{
	BPlusNode* old_root = this->root ;	
	BPlusNode* trav = this->root ;
	this->root = nullptr ; 
	this->number_of_elements = 0 ;
	this->maximal_elements_reached = 0 ;
	while(trav != nullptr && !trav->is_leaf)
		trav = trav->children[0] ;
	if(trav == nullptr)
		return ;

	while(trav != nullptr)
	{
		for(int i = 0; i < trav->keys.size(); ++i)
			this->insert(trav->keys[i], trav->values[i]) ;
		trav = trav->next ;
	}
	delete_nodes(old_root) ;
	cout << "RECONSTRUCTION\n" ;
}

void BPlusTree::erase_p(BPlusNode* node, int key)
{
	if(node == nullptr)
		return ;

	if(node->is_leaf)
	{
		auto pos = lower_bound(node->keys.begin(), node->keys.end(), key) ;
		if(pos == node->keys.end() || *pos != key)
			return ;
		int index = pos - node->keys.begin() ;
		node->keys.erase(node->keys.begin()+index);
		node->values.erase(node->values.begin()+index);
		this->number_of_elements-- ;
		this->numbers_deleted++ ;
		return ;
	}

	int pos = upper_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin() ;
	erase_p(node->children[pos], key) ;
}

void BPlusTree::erase(int key)
{
	erase_p(root, key) ;

	/*
	if(number_of_elements < maximal_elements_reached/2)
	{
		//reconstrct the tree.
		reconstruct() ;
		this->maximal_elements_reached = this->number_of_elements ;
	}
	*/

	if(numbers_deleted > maximal_elements_reached/2)
	{
		reconstruct() ;
		this->maximal_elements_reached = max(this->maximal_elements_reached, this->number_of_elements) ;
		this->numbers_deleted = 0 ;
	}
}

pair<int, int> BPlusTree::get_p(BPlusNode* node, int key)
{
	if(node == nullptr)
		return {-1, -1} ;

	if(node->is_leaf)
	{
		auto pos = lower_bound(node->keys.begin(), node->keys.end(), key) ;
		if(pos == node->keys.end() || *pos != key)
			return {-1, -1} ;
		int index = pos - node->keys.begin() ;
		return node->values[index] ;
	}
	int pos = upper_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin() ;
	return get_p(node->children[pos], key) ;
}

pair<int ,int> BPlusTree::get(int key)
{
	return get_p(root, key) ;
}

void BPlusTree::print_content_reverse()
{
	BPlusNode* trav = root ;
	while(trav != nullptr && !trav->is_leaf)
		trav = trav->children.back() ;
	if(trav == nullptr)
		return ;

	while(trav != nullptr)
	{
		for(int i = trav->keys.size()-1; i >= 0; --i)
			cout << trav->keys[i] << " <" << trav->values[i].first << ' ' << trav->values[i].second << ">\n" ;

		trav = trav->prev ;
	}
}

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
			cout << trav->keys[i] << " <" << trav->values[i].first << ' ' << trav->values[i].second << ">\n" ;

		trav = trav->next ;
	}
}

BPlusNode* BPlusTree::getForwardRecordIterator()
{
	BPlusNode* trav = root ;
	while(trav != nullptr && !trav->is_leaf)
		trav = trav->children[0] ;
	return trav ; 
}

BPlusNode* BPlusTree::getReverseRecordIterator()
{
	BPlusNode* trav = root ;
	while(trav != nullptr && !trav->is_leaf)
		trav = trav->children.back() ;
	return trav ;
}

BPlusNode* BPlusTree::next(BPlusNode* iterator, bool forward)
{
	if(iterator == nullptr) return nullptr ;	
	if(forward)
		return iterator->next ;
	else
		return iterator->prev ;
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

BPlusNode* BPlusTree::insert_p(BPlusNode* node, int key, pair<int, int> value)
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
			BPlusNode* next_to_curr_leaf = child->next ;
			if(next_to_curr_leaf != nullptr)
			{
				new_node->next = next_to_curr_leaf ;
				next_to_curr_leaf->prev= new_node ;
			}
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

void BPlusTree::insert(int key, pair<int ,int> value)
{
	BPlusNode* child = insert_p(root, key, value) ;		
	this->root = child ;
	// TODO: Handle the logic for creating a new node here.
	if(child->needs_split)
	{
		//turn off the flag.
		child->needs_split = false ;

		// NOTE: if the child that we need to split is a leaf node, then we have to change the next and prev pointers as well.

		const int limit = child->keys.size() ;
		int mid_index = limit/2;
		int mid_key = child->keys[mid_index] ;
		
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
	++number_of_elements ;
	maximal_elements_reached = max(maximal_elements_reached, number_of_elements) ;
}
