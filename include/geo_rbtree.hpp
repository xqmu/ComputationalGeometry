#ifndef GEO_RBTREE_HPP
#define GEO_RBTREE_HPP

#include <iostream>

template<typename ValueT, typename SearchT, class Compare>
class RBTree
{
	typedef enum { RED, BLACK } Color;
	typedef bool(*comp_func)(const ValueT&, const ValueT&);

public:
	struct Node {
		ValueT				value;
		Color				color;
		Node				*leftTree, *rightTree, *parent;

		Node()
			: color(RED)
			, leftTree(nullptr)
			, rightTree(nullptr)
			, parent(nullptr)
		{}
		Node(Node* p){
			value=p->value;
			color=p->color;
			leftTree=p->leftTree;
			rightTree=p->rightTree;
			parent=p->parent;
		}

		Node* grandparent() const
		{
			if (parent == nullptr) {
				return nullptr;
			}
			return parent->parent;
		}

		Node* uncle() const
		{
			if (grandparent() == nullptr) {
				return nullptr;
			}

			if (parent == grandparent()->rightTree) {
				return grandparent()->leftTree;
			}
			return grandparent()->rightTree;
		}

		Node* sibling() {
			if (parent->leftTree == this) {
				return parent->rightTree;
			}
			return parent->leftTree;
		}

		Node* copy_path(Node* p){
			Node* new_node=new Node(p);

			if(p->parent!= nullptr){
				new_node->parent=copy_path(p);
				if(p->parent->leftTree==p){
					new_node->parent->leftTree=new_node;
				}
				else new_node->parent->rightTree=new_node;

			}
			else
				return new_node;

		}

        bool is_null(){
			if(this->color==BLACK&&this->value==NULL&&/*this->parent==NULL&&*/this->rightTree==NULL&&this->leftTree==NULL)
              return true;
			if(this->color==BLACK&&this->parent==NULL&&this->rightTree==NULL&&this->leftTree==NULL)
				return true;
			return false;
        }

	};


private:
//public:

	//Node *root, *NIL;
	Compare* comp;

	//bool less(ValueT& left, ValueT& right)
	//{
	//	return comp->less(left, right);
	//}

	void rotate_right(Node* p) {
		Node *gp = p->grandparent();
		Node *fa = p->parent;
		Node *y = p->rightTree;

		fa->leftTree = y;

		if (y != NIL) {
			y->parent = fa;
		}
		p->rightTree = fa;
		fa->parent = p;

		if (root == fa) {
			root = p;
		}
		p->parent = gp;

		if (gp != nullptr) {
			if (gp->leftTree == fa) {
				gp->leftTree = p;
			}
			else {
				gp->rightTree = p;
			}
		}
	}

	void rotate_left(Node *p) {
		if (p->parent == nullptr) {
			root = p;
			return;
		}
		Node *gp = p->grandparent();
		Node *fa = p->parent;
		Node *y = p->leftTree;

		fa->rightTree = y;

		if (y != NIL) {
			y->parent = fa;
		}
		p->leftTree = fa;
		fa->parent = p;

		if (root == fa) {
			root = p;
		}
		p->parent = gp;

		if (gp != nullptr) {
			if (gp->leftTree == fa)
				gp->leftTree = p;
			else
				gp->rightTree = p;
		}
	}

	void inorder(Node *p) const {
		if (p == NIL) {
			return;
		}

		if (p->leftTree) {
			inorder(p->leftTree);
		}

		std::cout << p->value << " ";

		if (p->rightTree) {
			inorder(p->rightTree);
		}
	}

	Node* get_smallest_child(Node *p) const {
		//if (p->leftTree == NIL) {     //*******************changed
		if (p->leftTree->is_null()) {
			return p;
		}
		return get_smallest_child(p->leftTree);
	}

	bool delete_child(Node *p, const ValueT& data) {
		if (comp->less(data, p->value)) {
			if (p->leftTree == NIL) {
				return false;
			}
			return delete_child(p->leftTree, data);
		}

		if (comp->less(p->value, data)) {
			if (p->rightTree == NIL) {
				return false;
			}
			return delete_child(p->rightTree, data);
		}

		if (p->rightTree == NIL) {
			delete_one_child(p);
			return true;
		}
		Node *smallest = get_smallest_child(p->rightTree);
		//std::swap(p->value, smallest->value);
		ValueT temp=p->value;
		p->value=smallest->value;
		smallest->value=temp;
		delete_one_child(smallest);

		return true;
	}

	void delete_one_child(Node *p) {
		Node *child = p->leftTree == NIL ? p->rightTree : p->leftTree;
		if (p->parent == nullptr && p->leftTree == NIL && p->rightTree == NIL) {
			p = nullptr;
			root = p;
			return;
		}

		if (p->parent == nullptr) {
			delete  p;
			child->parent = nullptr;
			root = child;
			root->color = BLACK;
			return;
		}

		if (p->parent->leftTree == p) {
			p->parent->leftTree = child;
		}
		else {
			p->parent->rightTree = child;
		}
		child->parent = p->parent;

		if (p->color == BLACK) {
			if (child->color == RED) {
				child->color = BLACK;
			}
			else
				delete_case(child);
		}

		delete p;
	}

	void delete_case(Node *p) {
		if (p->parent == nullptr) {
			p->color = BLACK;
			return;
		}
		if (p->sibling()->color == RED) {
			p->parent->color = RED;
			p->sibling()->color = BLACK;
			if (p == p->parent->leftTree)
				rotate_left(p->sibling());
			else
				rotate_right(p->sibling());
		}
		if (p->parent->color == BLACK && p->sibling()->color == BLACK
			&& p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) {
			p->sibling()->color = RED;
			delete_case(p->parent);
		}
		else if (p->parent->color == RED && p->sibling()->color == BLACK
			&& p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) {
			p->sibling()->color = RED;
			p->parent->color = BLACK;
		}
		else {
			if (p->sibling()->color == BLACK) {
				if (p == p->parent->leftTree && p->sibling()->leftTree->color == RED
					&& p->sibling()->rightTree->color == BLACK) {
					p->sibling()->color = RED;
					p->sibling()->leftTree->color = BLACK;
					rotate_right(p->sibling()->leftTree);
				}
				else if (p == p->parent->rightTree && p->sibling()->leftTree->color == BLACK
					&& p->sibling()->rightTree->color == RED) {
					p->sibling()->color = RED;
					p->sibling()->rightTree->color = BLACK;
					rotate_left(p->sibling()->rightTree);
				}
			}
			p->sibling()->color = p->parent->color;
			p->parent->color = BLACK;
			if (p == p->parent->leftTree) {
				p->sibling()->rightTree->color = BLACK;
				rotate_left(p->sibling());
			}
			else {
				p->sibling()->leftTree->color = BLACK;
				rotate_right(p->sibling());
			}
		}
	}

	void insert(Node *p, ValueT& data) {
		if (!comp->less(p->value, data)) {
			if (p->leftTree != NIL)
				insert(p->leftTree, data);
			else {
				Node *tmp = new Node();
				tmp->value = data;
				tmp->leftTree = tmp->rightTree = NIL;
				tmp->parent = p;
				p->leftTree = tmp;
				insert_case(tmp);
			}
		}
		else {
			if (p->rightTree != NIL)
				insert(p->rightTree, data);
			else {
				Node *tmp = new Node();
				tmp->value = data;
				tmp->leftTree = tmp->rightTree = NIL;
				tmp->parent = p;
				p->rightTree = tmp;
				insert_case(tmp);
			}
		}
	}

	void insert_case(Node *p) {
		if (p->parent == nullptr) {
			root = p;
			p->color = BLACK;
			return;
		}
		if (p->parent->color == RED) {
			if (p->uncle()->color == RED) {
				p->parent->color = p->uncle()->color = BLACK;
				p->grandparent()->color = RED;
				insert_case(p->grandparent());
			}
			else {
				if (p->parent->rightTree == p && p->grandparent()->leftTree == p->parent) {
					rotate_left(p);
					rotate_right(p);
					p->color = BLACK;
					p->leftTree->color = p->rightTree->color = RED;
				}
				else if (p->parent->leftTree == p && p->grandparent()->rightTree == p->parent) {
					rotate_right(p);
					rotate_left(p);
					p->color = BLACK;
					p->leftTree->color = p->rightTree->color = RED;
				}
				else if (p->parent->leftTree == p && p->grandparent()->leftTree == p->parent) {
					p->parent->color = BLACK;
					p->grandparent()->color = RED;
					rotate_right(p->parent);
				}
				else if (p->parent->rightTree == p && p->grandparent()->rightTree == p->parent) {
					p->parent->color = BLACK;
					p->grandparent()->color = RED;
					rotate_left(p->parent);
				}
			}
		}
	}

	//for the persistent tree part
	bool delete_child_p(Node *p, Node* &q, const ValueT& data) {
		if(p->parent== nullptr)
			q=new Node(p);
		if (comp->less(data, p->value)) {
			//if (p->leftTree == NIL) {    //********************changed
            if (p->leftTree->is_null()) {
				return false;
			}
			Node* new_node=new Node(p->leftTree);
			new_node->parent=q;
			q->leftTree=new_node;
			return delete_child_p(p->leftTree,new_node, data);
		}

		if (comp->less(p->value, data)) {
			//if (p->rightTree == NIL) {  //********************changed
            if (p->rightTree->is_null()) {
				return false;
			}

			Node* new_node=new Node(p->rightTree);
			new_node->parent=q;
			q->rightTree=new_node;
			return delete_child_p(p->rightTree,new_node, data);

			//return delete_child(p->rightTree, data);
		}

    	//if (p->rightTree == NIL) {     //********************changed
        if (p->rightTree->is_null()) {
			delete_one_child_p(q);
			//delete_one_child(p);
			return true;
		}
		//Node *smallest = get_smallest_child(p->rightTree);

		Node *smallest = get_smallest_child(q->rightTree);
		//std::swap(p->value, smallest->value);
		ValueT temp=p->value;
		p->value=smallest->value;
		smallest->value=temp;

		delete_one_child_p(smallest);

		return true;
	}

	void delete_one_child_p(Node *p) {
		//Node *child = p->leftTree == NIL ? p->rightTree : p->leftTree;     //********************changed
        Node *child = p->leftTree->is_null() ? p->rightTree : p->leftTree;
		//if (p->parent == nullptr && p->leftTree==NIL && p->rightTree == NIL) {
        if (p->parent == nullptr && p->leftTree->is_null() && p->rightTree->is_null()) {
			p = nullptr;
			root = p;
			return;
		}

		if (p->parent == nullptr) {
			delete  p;
			child->parent = nullptr;
			root = child;
			root->color = BLACK;
			return;
		}

		Node* new_child=new Node(child);
		if (p->parent->leftTree == p) {
			p->parent->leftTree = new_child;
		}
		else {
			p->parent->rightTree = new_child;
		}
		//child->parent = p->parent;
		new_child->parent=p->parent;

		if (p->color == BLACK) {
			if (child->color == RED) {
				new_child->color = BLACK;
			}
			else
				delete_case_p(new_child);
		}

		delete p;
	}

	void delete_case_p(Node *p) {
		if (p->parent == nullptr) {
			p->color = BLACK;
			return;
		}
		if (p->sibling()->color == RED) {
			p->parent->color = RED;

			Node* new_sibling=new Node(p->sibling());
			new_sibling->parent=p->parent;
			if(p->parent->leftTree==p->sibling())
				p->parent->leftTree=new_sibling;
			else p->parent->rightTree=new_sibling;

			//p->sibling()->color = BLACK;
			new_sibling->color=BLACK;
			if (p == p->parent->leftTree)
				rotate_left(p->sibling());
			else
				rotate_right(p->sibling());
		}
		if (p->parent->color == BLACK && p->sibling()->color == BLACK
			&& p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) {
			Node* new_sibling=new Node(p->sibling());
			if(p->parent->leftTree==p->sibling())
				p->parent->leftTree=new_sibling;
			else p->parent->rightTree=new_sibling;

			//p->sibling()->color = RED;
			new_sibling->color=RED;
			delete_case_p(p->parent);
		}
		else if (p->parent->color == RED && p->sibling()->color == BLACK
				 && p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) {
			Node* new_sibling=new Node(p->sibling());
			if(p->parent->leftTree==p->sibling())
				p->parent->leftTree=new_sibling;
			else p->parent->rightTree=new_sibling;

			//p->sibling()->color = RED;
			new_sibling->color=RED;
			p->parent->color = BLACK;
		}
		else {
			if (p->sibling()->color == BLACK) {
				if (p == p->parent->leftTree && p->sibling()->leftTree->color == RED
					&& p->sibling()->rightTree->color == BLACK) {

					Node* new_sibling=new Node(p->sibling());
					if(p->parent->leftTree==p->sibling())
						p->parent->leftTree=new_sibling;
					else p->parent->rightTree=new_sibling;
					//p->sibling()->color = RED;
					new_sibling->color=RED;

					Node* new_sibling_lt=new Node(p->sibling()->leftTree);
					new_sibling->leftTree=new_sibling_lt;
					new_sibling_lt->parent=new_sibling;
					//p->sibling()->leftTree->color = BLACK;
					new_sibling_lt->color=BLACK;

					rotate_right(p->sibling()->leftTree);
				}
				else if (p == p->parent->rightTree && p->sibling()->leftTree->color == BLACK
						 && p->sibling()->rightTree->color == RED) {

					Node* new_sibling=new Node(p->sibling());
					if(p->parent->leftTree==p->sibling())
						p->parent->leftTree=new_sibling;
					else p->parent->rightTree=new_sibling;
					//p->sibling()->color = RED;
					new_sibling->color=RED;

					Node* new_sibling_rt=new Node(p->sibling()->rightTree);
					new_sibling->rightTree=new_sibling_rt;
					new_sibling_rt->parent=new_sibling;
					new_sibling_rt->color=BLACK;
					//p->sibling()->rightTree->color = BLACK;

					rotate_left(p->sibling()->rightTree);
				}
			}

			if(p->sibling()->color!= p->parent->color){
				Node* new_sibling= new Node(p->sibling());
				new_sibling->parent=p->parent;
				if(p->sibling()==p->parent->leftTree)
					p->parent->leftTree=new_sibling;
				else p->parent->rightTree=new_sibling;
				new_sibling->color=p->parent->color;
			}
			else
			    p->sibling()->color = p->parent->color;


			p->parent->color = BLACK;
			if (p == p->parent->leftTree) {
				if(p->sibling()->rightTree->color != BLACK){
					Node* new_sibling_rt=new Node(p->sibling()->rightTree);
					p->sibling()->sibling()->rightTree=new_sibling_rt;
					new_sibling_rt->parent=p->sibling();
					new_sibling_rt->color=BLACK;
				}
				//p->sibling()->rightTree->color = BLACK;
				rotate_left(p->sibling());
			}
			else {
				if(p->sibling()->leftTree->color != BLACK){
					Node* new_sibling_lt=new Node(p->sibling()->leftTree);
					p->sibling()->sibling()->leftTree=new_sibling_lt;
					new_sibling_lt->parent=p->sibling();
					new_sibling_lt->color=BLACK;
				}
				//p->sibling()->leftTree->color = BLACK;
				rotate_right(p->sibling());
			}
		}
	}

	Node* insert_p(Node *p, Node* &q, ValueT& data){
		if(p->parent== nullptr)
			q=new Node(p);
		if (!comp->less(p->value, data)) {
			//if (p->leftTree != NIL) {
			if (!p->leftTree->is_null()) {
				Node* new_node=new Node(p->leftTree);
				q->leftTree=new_node;
				new_node->parent=q;
				insert_p(p->leftTree,new_node, data);
//                Node* new_node=new Node(insert_p(p->leftTree,new_node, data));
//                q->leftTree=new_node;
//                new_node->parent=q;
//                //insert_p(p->leftTree,new_node, data);
			}
			else {
				Node *tmp = new Node();
				tmp->value = data;
				tmp->leftTree = tmp->rightTree = NIL;
				//tmp->parent = p;
				//p->leftTree = tmp;
				tmp->parent = q;
				q->leftTree=tmp;
				insert_case_p(tmp);
			}
		}
		else {
			//if (p->rightTree != NIL) {
            if (!p->rightTree->is_null()) {
				Node* new_node=new Node(p->rightTree);
				q->rightTree=new_node;
				new_node->parent=q;
				insert_p(p->rightTree,new_node, data);

//                Node* new_node=new Node(insert_p(p->rightTree,new_node, data));
//                q->rightTree=new_node;
//                new_node->parent=q;
//                //insert_p(p->rightTree,new_node, data);
			}
			else {
				Node *tmp = new Node();
				tmp->value = data;
				tmp->leftTree = tmp->rightTree = NIL;
				tmp->parent=q;
				q->rightTree=tmp;
				insert_case_p(tmp);
			}
		}
		//return q;
	}

	void insert_case_p(Node *p) {
		if (p->parent == nullptr) {
			root = p;
			p->color = BLACK;
			return;
		}
		if (p->parent->color == RED) {
			if (p->uncle()->color == RED) {
				Node* new_uncle=new Node(p->uncle());
				//p->parent->color = p->uncle()->color = BLACK;
				p->parent->color = new_uncle->color = BLACK;
				new_uncle->parent=p->grandparent();
				if(p->grandparent()->rightTree==p->uncle())
					p->grandparent()->rightTree=new_uncle;
				else p->grandparent()->leftTree=new_uncle;

				p->grandparent()->color = RED;
				insert_case_p(p->grandparent());
			}
			else {
				if (p->parent->rightTree == p && p->grandparent()->leftTree == p->parent) {
					rotate_left(p);
					rotate_right(p);
					p->color = BLACK;
					if(p->leftTree->color==BLACK&&!p->leftTree->is_null()){
						Node* new_lt=new Node(p->leftTree);
						new_lt->color=RED;
						new_lt->parent=p;
						p->leftTree=new_lt;
					}
					if(p->rightTree->color==BLACK&&!p->rightTree->is_null()){
						Node* new_rt=new Node(p->rightTree);
						new_rt->color=RED;
						new_rt->parent=p;
						p->rightTree=new_rt;
					}
					//p->leftTree->color = p->rightTree->color = RED;
				}
				else if (p->parent->leftTree == p && p->grandparent()->rightTree == p->parent) {
					rotate_right(p);
					rotate_left(p);
					p->color = BLACK;

					if(p->leftTree->color==BLACK&&!p->leftTree->is_null()){
						Node* new_lt=new Node(p->leftTree);
						new_lt->color=RED;
						new_lt->parent=p;
						p->leftTree=new_lt;
					}
					if(p->rightTree->color==BLACK&&!p->rightTree->is_null()){
						Node* new_rt=new Node(p->rightTree);
						new_rt->color=RED;
						new_rt->parent=p;
						p->rightTree=new_rt;
					}
					//p->leftTree->color = p->rightTree->color = RED;
				}
				else if (p->parent->leftTree == p && p->grandparent()->leftTree == p->parent) {
					p->parent->color = BLACK;
					p->grandparent()->color = RED;
					rotate_right(p->parent);
				}
				else if (p->parent->rightTree == p && p->grandparent()->rightTree == p->parent) {
					p->parent->color = BLACK;
					p->grandparent()->color = RED;
					rotate_left(p->parent);
				}
			}
		}
	}


	void delete_tree(Node *p) {
		if (!p || p == NIL) {
			return;
		}
		delete_tree(p->leftTree);
		delete_tree(p->rightTree);
		delete p;
	}


	Node* get_smaller(Node* p, const SearchT& value)
	{
		if (comp->less(value, p->value))
		{
			if (p->leftTree == NIL)
			{
				return nullptr;
			}

			return get_smaller(p->leftTree, value);
		}

		if (comp->less(p->value, value))
		{
			if (p->rightTree == NIL)
			{
				return p;
			}

			Node* ps = get_smaller(p->rightTree, value);
			if (ps == nullptr)
			{
				return p;
			}

			return ps;
		}

		return p;
	}

	Node* get_larger(Node* p, const SearchT& value)
	{
		if (comp->less(value, p->value))
		{
			if (p->leftTree == NIL)
			{
				return p;
			}

			Node* pl = get_larger(p->leftTree, value);
			if (pl == nullptr)
			{
				return p;
			}

			return pl;
		}

		if (comp->less(p->value, value))
		{
			if (p->rightTree == NIL)
			{
				return nullptr;
			}

			return get_larger(p->rightTree, value);
		}

		return p;
	}



public:
	Node *root, *NIL;
	RBTree(Compare* cmp = nullptr)
	{
		NIL = new Node();
		NIL->color = BLACK;
		root = nullptr;
		this->comp = cmp;
	}

	~RBTree()
	{
		if (root) {
			delete_tree(root);
		}
		delete NIL;
	}

	void inorder()
	{
		if (root == nullptr) {
			return;
		}
		inorder(root);
	}

	void insert(ValueT& x) {
		if (root == nullptr) {
			root = new Node();
			root->color = BLACK;
			root->leftTree = root->rightTree = NIL;
			root->value = x;
		}
		else {
			insert(root, x);
		}
	}

	void insert_p(ValueT& x,Node* &q) {
        //q=new Node();
		if (root == nullptr) {
			root = new Node();
			root->color = BLACK;
			root->leftTree = root->rightTree = NIL;
			root->value = x;
            q=root;
		}
		else {
			insert_p(root,q, x);
		}
        //return q;
	}

	bool delete_value(const ValueT& data) {
		return delete_child(root, data);
	}

	bool delete_value_p(const ValueT& data,Node* &q) {
		return delete_child_p(root,q, data);
	}

	bool get_smaller(const SearchT& value, ValueT& smaller)
	{
		Node* p = get_smaller(root, value);

		if (p == nullptr)
		{
			return false;
		}

		smaller = p->value;
		return true;
	}

	bool get_larger(const SearchT& value, ValueT& larger)
	{
		Node* p = get_larger(root, value);

		if (p == nullptr)
		{
			return false;
		}

		larger = p->value;
		return true;
	}

};

#endif