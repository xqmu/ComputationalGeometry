//
// Created by Xiaoqian Mu on 4/6/17.
//

#ifndef COMPUTATIONALGEOMETRY_GEO_PERSISTENT_RBTREE_H
#define COMPUTATIONALGEOMETRY_GEO_PERSISTENT_RBTREE_H

#include <iostream>
#include <vector>

template<typename ValueT, typename SearchT, class Compare>
class PerRBTree
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

//        bool is_null(){
//            return(this->color==BLACK&&this->parent==NULL&&this->rightTree==NULL&&this->leftTree==NULL);
//        }

    };


private:

    Compare* comp;

    void rotate_right(Node* &t_root,Node* p) {
        Node *gp = p->grandparent();
        Node *fa = p->parent;
        Node *y = p->rightTree;

        fa->leftTree = y;

        if (y != NIL) {
            y->parent = fa;
        }
        p->rightTree = fa;
        fa->parent = p;

        if (t_root == fa) {
            t_root = p;
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

    void rotate_left(Node* &t_root,Node *p) {
        if (p->parent == nullptr) {
            t_root = p;
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

        if (t_root == fa) {
            t_root = p;
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
            //if (p == nullptr) {
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
        if(p->leftTree==NIL){
            return p;
        }
        return get_smallest_child(p->leftTree);
    }

    void insert(Node* &t_root,Node *p, ValueT& data) {
        if (!comp->less(p->value, data)) {
            if (p->leftTree != NIL)
                insert(t_root,p->leftTree, data);
            else {
                Node *tmp = new Node();
                tmp->value = data;
                tmp->leftTree = tmp->rightTree = NIL;
                tmp->parent = p;
                p->leftTree = tmp;
                insert_case(t_root,tmp);
            }
        }
        else {
            if (p->rightTree != NIL)
                insert(t_root,p->rightTree, data);
            else {
                Node *tmp = new Node();
                tmp->value = data;
                tmp->leftTree = tmp->rightTree = NIL;
                tmp->parent = p;
                p->rightTree = tmp;
                insert_case(t_root,tmp);
            }
        }
    }

    void insert_case(Node* &t_root,Node *p) {
        if (p->parent == nullptr) {
            t_root = p;
            p->color = BLACK;
            return;
        }
        if (p->parent->color == RED) {
            if (p->uncle()!= nullptr)
                if (p->uncle()->color == RED){
                    p->parent->color = p->uncle()->color = BLACK;
                    p->grandparent()->color = RED;
                    insert_case(t_root,p->grandparent());
                }
                else {
                    if (p->parent->rightTree == p && p->grandparent()->leftTree == p->parent) {
                        rotate_left(t_root,p);
                        rotate_right(t_root,p);
                        p->color = BLACK;
                        p->leftTree->color = p->rightTree->color = RED;
                    }
                    else if (p->parent->leftTree == p && p->grandparent()->rightTree == p->parent) {
                        rotate_right(t_root,p);
                        rotate_left(t_root,p);
                        p->color = BLACK;
                        p->leftTree->color = p->rightTree->color = RED;
                    }
                    else if (p->parent->leftTree == p && p->grandparent()->leftTree == p->parent) {
                        p->parent->color = BLACK;
                        p->grandparent()->color = RED;
                        rotate_right(t_root,p->parent);
                    }
                    else if (p->parent->rightTree == p && p->grandparent()->rightTree == p->parent) {
                        p->parent->color = BLACK;
                        p->grandparent()->color = RED;
                        rotate_left(t_root, p->parent);
                    }
                }
        }
    }

    bool delete_child_p(Node* &t_root,Node *p, Node* &q, const ValueT& data)
    {
        if(p->parent== nullptr)
            q=new Node(p);
        if (comp->less(data, p->value)) {
            if (p->leftTree == NIL) {
                return false;
            }
            Node* new_node=new Node(p->leftTree);
            new_node->parent=q;
            q->leftTree=new_node;
            return delete_child_p(t_root,p->leftTree,new_node, data);
        }

        if (comp->less(p->value, data)) {
            if (p->rightTree == NIL) {
                return false;
            }

            Node* new_node=new Node(p->rightTree);
            new_node->parent=q;
            q->rightTree=new_node;
            return delete_child_p(t_root,p->rightTree,new_node, data);

            //return delete_child(p->rightTree, data);
        }

        if (p->rightTree == NIL) {
            delete_one_child_p(t_root,q);
            //delete_one_child(p);
            return true;
        }
        //Node *smallest = get_smallest_child(p->rightTree);

        Node *smallest = get_smallest_child(q->rightTree);

        Node* new_smallest = new Node(smallest);
        q->rightTree = new_smallest;
        new_smallest->parent = q;
        ValueT temp = q->value;
        q->value = new_smallest->value;
        new_smallest->value = temp;


        delete_one_child_p(t_root,new_smallest);

        return true;
    }

    void delete_one_child_p(Node* &t_root,Node *p) {
        Node *child = p->leftTree == NIL ? p->rightTree : p->leftTree;     //********************changed
        if (p->parent == nullptr && p->leftTree==NIL && p->rightTree == NIL) {
            p = nullptr;
            t_root = p;
            return;
        }

        if (p->parent == nullptr) {
            delete  p;
            child->parent = nullptr;
            t_root = child;
            t_root->color = BLACK;
            return;
        }

        if (child != NIL) {
            Node* new_child = new Node(child);
            if (p->parent->leftTree == p) {
                p->parent->leftTree = new_child;
            }
            else {
                p->parent->rightTree = new_child;
            }
            //child->parent = p->parent;
            new_child->parent = p->parent;

            if (p->color == BLACK) {
                if (child->color == RED) {
                    new_child->color = BLACK;
                }
                else
                    delete_case_p(t_root, new_child);
            }
        }
        else {
            if (p->parent->leftTree == p) {
                p->parent->leftTree = child;
            }
            else {
                p->parent->rightTree = child;
            }
            delete p;
            return;
        }

        delete p;
    }

    void delete_case_p(Node* &t_root,Node *p)
    {
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
                rotate_left(t_root,p->sibling());
            else
                rotate_right(t_root,p->sibling());
        }
        if (p->parent->color == BLACK && p->sibling()->color == BLACK
            && p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK)
        {
            Node* new_sibling=new Node(p->sibling());
            if(p->parent->leftTree==p->sibling())
                p->parent->leftTree=new_sibling;
            else p->parent->rightTree=new_sibling;

            //p->sibling()->color = RED;
            new_sibling->color=RED;
            delete_case_p(t_root,p->parent);
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

                    rotate_right(t_root,p->sibling()->leftTree);
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

                    rotate_left(t_root,p->sibling()->rightTree);
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
                rotate_left(t_root,p->sibling());
            }
            else {
                if(p->sibling()->leftTree->color != BLACK){
                    Node* new_sibling_lt=new Node(p->sibling()->leftTree);
                    p->sibling()->sibling()->leftTree=new_sibling_lt;
                    new_sibling_lt->parent=p->sibling();
                    new_sibling_lt->color=BLACK;
                }
                //p->sibling()->leftTree->color = BLACK;
                rotate_right(t_root,p->sibling());
            }
        }
    }

    Node* insert_p(Node* &t_root, Node* const p, Node* &q, ValueT& data){
        // check root
        if(p->parent == nullptr)
        {
            q = new Node(p);
        }

        if (!comp->less(p->value, data)) {
            if (p->leftTree != NIL)
            {
                Node* new_node=new Node(p->leftTree);
                q->leftTree=new_node;
                new_node->parent=q;
                insert_p(t_root, p->leftTree, q->leftTree, data);

            }
            else
            {
                // insert node
                Node *tmp = new Node();
                tmp->value = data;
                tmp->leftTree = tmp->rightTree = NIL;
                tmp->parent = q;
                q->leftTree=tmp;
                insert_case_p(t_root,tmp);
            }
        }
        else {
            if (p->rightTree != NIL)
            {
                Node* new_node=new Node(p->rightTree);
                q->rightTree=new_node;
                new_node->parent=q;
                insert_p(t_root, p->rightTree, q->rightTree, data);

            }
            else {
                Node *tmp = new Node();
                tmp->value = data;
                tmp->leftTree = tmp->rightTree = NIL;

                tmp->parent=q;
                q->rightTree=tmp;
                insert_case_p(t_root,tmp);
            }
        }

        return nullptr;
    }

    void insert_case_p(Node* &t_root,Node *p)
    {
        if (p->parent == nullptr) {
            t_root = p;
            p->color = BLACK;
            return;
        }

        if (p->parent->color == RED) {
            if (p->uncle()->color == RED)
            {
                Node* new_uncle=new Node(p->uncle());
                p->parent->color = new_uncle->color = BLACK;

                new_uncle->parent=p->grandparent();
                if(p->grandparent()->rightTree==p->uncle())
                    p->grandparent()->rightTree=new_uncle;
                else p->grandparent()->leftTree=new_uncle;

                p->grandparent()->color = RED;
                insert_case_p(t_root,p->grandparent());
            }
            else {
                if (p->parent->rightTree == p && p->grandparent()->leftTree == p->parent) {
                    rotate_left(t_root,p);
                    rotate_right(t_root,p);
                    p->color = BLACK;

                    if(p->leftTree->color==BLACK&&p->leftTree!= nullptr){
                        Node* new_lt=new Node(p->leftTree);
                        new_lt->color=RED;
                        new_lt->parent=p;
                        p->leftTree=new_lt;
                    }


                    if(p->rightTree->color==BLACK&&p->rightTree!= nullptr){
                        Node* new_rt=new Node(p->rightTree);
                        new_rt->color=RED;
                        new_rt->parent=p;
                        p->rightTree=new_rt;
                    }

                }
                else if (p->parent->leftTree == p && p->grandparent()->rightTree == p->parent) {
                    rotate_right(t_root,p);
                    rotate_left(t_root,p);
                    p->color = BLACK;

                    if(p->leftTree->color==BLACK&&p->leftTree!= nullptr){
                        Node* new_lt=new Node(p->leftTree);
                        new_lt->color=RED;
                        new_lt->parent=p;
                        p->leftTree=new_lt;
                    }


                    if(p->rightTree->color==BLACK&&p->rightTree!= nullptr){
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
                    rotate_right(t_root,p->parent);
                }
                else if (p->parent->rightTree == p && p->grandparent()->rightTree == p->parent) {
                    p->parent->color = BLACK;
                    p->grandparent()->color = RED;
                    rotate_left(t_root,p->parent);
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


    Node *root;
    Node *NIL;
public:


    std::vector<double> location;

    std::vector<Node*> roots;

    PerRBTree(Compare* cmp = nullptr)
    {
        NIL = new Node();
        NIL->color = BLACK;
        root = nullptr;
        this->comp = cmp;
    }

    ~PerRBTree()
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

    void insert(Node* &p,ValueT& x) {
        if (p == nullptr) {
            p = new Node();
            p->color = BLACK;
            p->leftTree = p->rightTree = NIL;
            p->value = x;
        }
        else {
            insert(p,p, x);
        }
    }


    void insert_p(Node* p,ValueT& x,Node* &q)
    {
        if (p == nullptr) {
            p = new Node();
            p->color = BLACK;
            p->leftTree = p->rightTree = NIL;
            p->value = x;
            //q=root;
        }
        else {
            insert_p(q, p, q, x);
        }
    }

    bool delete_value_p(Node* p,const ValueT& data,Node* &q) {
        return delete_child_p(q, p, q, data);
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

    void get_roots(const std::vector<SearchT>& vers)
    {
        int total=0;
        NIL=new Node();
        NIL->color=BLACK;

        for(int i=0;i<vers.size();i++)
        {
            if (i== 0) {
                for(int p=i+1;p<vers.size();p++)
                {
                    if(vers[p]->vertex->point.x==vers[i]->vertex->point.x)
                        i++;
                    else break;
                }

                for(int k=0;k<vers[i]->edge_insert.size();k++){
                    insert(root,vers[i]->edge_insert[k]);
                }
                roots.push_back(root);
                total++;
            }
            else{
                int p;
                for(p = i; p < vers.size(); p++){
                    if(vers[p]->vertex->point.x==vers[i]->vertex->point.x){
                        for(int j=0;j<vers[p]->edge_delete.size();j++){
                            // std::shared_ptr<PLRBTree> rbtree = std::make_shared<PLRBTree>(cmp);
                            Node* new_root;
                            delete_value_p(roots[total-1],vers[p]->edge_delete[j],new_root);
                            roots.push_back(new_root);
                            total++;
                        }

                        for(int k=0;k<vers[p]->edge_insert.size();k++){
                            Node* new_root;
                            insert_p(roots[total-1],vers[p]->edge_insert[k],new_root);
                            roots.push_back(new_root);
                            total++;
                        }
                    }
                    else break;
                }
                if(i>=vers.size())
                    i=vers.size()-1;
                else
                   i=p-1;
            }

            double x_coor=vers[i]->vertex->point.x;
            location.push_back(x_coor);
        }
    }

    int Find_location(double x, double y)
    {
        int low=0;
        int high=location.size()-2;
        int mid=0;
        int loc=-1;
        if(x<location[low]||x>location[high])
            return -1;

        if(x>=location[low]&&x<location[low+1]) {
            loc = low;
        }
        if(x>=location[high]&&x<location[high+1]) {
            loc = high;
        }

        while(loc<0) {
            mid=(low+high)/2;
            if(x>=location[mid]&&x<location[mid+1])
                loc=mid;
            if(x<location[mid]){
                high=mid;
            }
            if(x>location[mid+1])
                low=mid;
        }
        return loc;
    }

    ValueT Search_point(int loc, SearchT ver)
    {

        Node* below=get_smaller(roots[loc],ver);
        Node* above=get_larger(roots[loc],ver);
        if(below!= nullptr&&above!= nullptr)
            return below->value;
        else
            return nullptr;

    }
};


#endif //COMPUTATIONALGEOMETRY_GEO_PERSISTENT_RBTREE_H
