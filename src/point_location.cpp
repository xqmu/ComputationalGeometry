//
// Created by Xiaoqian Mu on 4/1/17.
//

#include <List>
#include "point_location.h"
#include <iostream>
using namespace geo;
using namespace std;


PointLocation::PointLocation(DoubleEdgeList* dcel){

    Create_edgeslist(dcel);
    Create_verticeslist(dcel);
    Create_PLTree();

}

//bool PointLocation::Find_point_location(Vector2 &p, geo::Triangle &triangle) {
//    double l=pl_tree.Find_location(p.x,p.y);
//
//    if(l!=-1) {
//        PointLocationVertex *ver = new PointLocationVertex(p);
//        PointLocationEdge *e = pl_tree.Search_point(l, ver);
//        if(e!= nullptr) {
//            triangle.point1 = e->v_l->point;
//            triangle.point2 = e->v_r->point;
//            triangle.point3 = (e->v_r->getEdgeTo(e->v_l))->next->twin->origin->point;
//            return true;
//        }
//        else{
//            //std::cout<<"Out of the polygon"<<endl;
//            return false;
//        }
//    }
//    else{
//        //std::cout<<"Out of the polygon"<<endl;
//        return false;
//    }
//}


bool PointLocation::Find_point_location(Vector2 &p, geo::Triangle &triangle) {
    int l=persistent_rbtree->Find_location(p.x,p.y);

    if(l!=-1) {
        PointLocationVertex *ver = new PointLocationVertex(p);

        int location=1;
        int temp=1;
        for(int i=1;i<=l;i++){
            int j;
            for(j=temp;j<vertices.size();j++) {
                if(vertices[j]->vertex->point.x==vertices[temp]->vertex->point.x) {
                    location += vertices[i]->edge_insert.size();
                    location += vertices[i]->edge_delete.size();
                }
                else break;
            }
            temp=j;
        }

        PointLocationEdge *e = persistent_rbtree->Search_point(location-1, ver);
        if(e!= nullptr) {
            triangle.point1 = e->v_l->point;
            triangle.point2 = e->v_r->point;
            triangle.point3 = (e->v_r->getEdgeTo(e->v_l))->next->twin->origin->point;

            if(e->v_r->getEdgeTo((e->v_r->getEdgeTo(e->v_l))->next->twin->origin)== nullptr)
                return false;
            return true;
        }
        else{
            //std::cout<<"Out of the polygon"<<endl;
            return false;
        }
    }
    else{
        //std::cout<<"Out of the polygon"<<endl;
        return false;
    }
}


void PointLocation::Create_edgeslist(DoubleEdgeList *dcel) {
    DoubleEdgeListHalfEdge* start;
    for(int i=0;i<dcel->edges.size();i++){
        start=dcel->edges.front();
        if(!start->visited) {
            PointLocationEdge* new_edge =new PointLocationEdge(start);
            edges.push_back(new_edge);
        }

        dcel->edges.pop_front();
        dcel->edges.push_back(start);

    }

}

void PointLocation::Create_verticeslist(DoubleEdgeList *dcel) {
    //DoubleEdgeListVertex* new_vertex=dcel->vertices.front();

    for(int i=0;i<dcel->vertices.size();i++){
        PointLocationVertex* new_vertex=new PointLocationVertex(dcel->vertices.front());
//        new_vertex.vertex=dcel->vertices.front();
//
        for(int j=0;j<edges.size();j++){
            if(edges[j]->v_l==new_vertex->vertex){
                new_vertex->edge_insert.push_back(edges[j]);
            }
            if(edges[j]->v_r==new_vertex->vertex){
                new_vertex->edge_delete.push_back(edges[j]);
            }
        }

        dcel->vertices.pop_front();
        dcel->vertices.push_back(new_vertex->vertex);
        vertices.push_back(new_vertex);

        int s=vertices.size();
        s=s+0;
    }
}

void PointLocation::Create_PLTree() {
    //pl_tree=PLRbTree(edges,vertices);

    PlCompare* cmp = new PlCompare();
    persistent_rbtree=new PerRBTree<PointLocationEdge*, PointLocationVertex*, PlCompare>(cmp);
    persistent_rbtree->get_roots(vertices);
   // delete cmp;
}



