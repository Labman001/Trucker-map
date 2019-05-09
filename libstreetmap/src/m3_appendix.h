/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3_appendix.h
 * Author: yaoyinji
 *
 * Created on March 9, 2019, 11:16 AM
 */

#ifndef M3_APPENDIX_H
#define M3_APPENDIX_H
#include "graphics.hpp"
#include "application.hpp"
#include <vector>
#include <algorithm>

#define Initial_status -1
#define Distance_coefficient 10000
#define estimate_const (1/30)*3.6


//calculate the manhattan distance with coefficient
double estimate_time(unsigned p1,unsigned p2);

//this a combine of segment and intersection
class seg_inter{
public:
    unsigned segment_idx;
    unsigned intersection_idx;
    void operator=(seg_inter const &rhs){
        segment_idx = rhs.segment_idx;
        intersection_idx = rhs.intersection_idx;
    }
    bool operator==(seg_inter rhs){
        if(intersection_idx == rhs.intersection_idx) return true;
        else return false;
    }
    bool operator!=(seg_inter rhs){
        if(intersection_idx != rhs.intersection_idx) return true;
        else return false;
    }
};

class Node{
public:
   int reachingEdge;  // ID of edge used to reach this node
   double travelTime; // Total travel time to reach node 
   unsigned reachingPoint; //ID of the point used to reach the node
   unsigned intersection_Idx; //the intersectionID of this node
   
   unsigned dest_Idx; //ID of destination intersection
   double dest_distance; //distance from current point to destination

   
   Node(){
       travelTime = Initial_status;
       dest_distance = Initial_status;
   }
   
   Node(int inter_id)
   {
    travelTime = Initial_status;
    //estimate_time= Initial_status;
     intersection_Idx = inter_id;
     dest_distance=Initial_status;
   }
   
   //used to setup information for a node as initializing
   void setup(int edge,double cost_time,int point,unsigned destination){
       reachingEdge = edge;
       travelTime = cost_time;
       reachingPoint = point;
       dest_Idx = destination;
      dest_distance = estimate_time(intersection_Idx,destination);
   }
   
   //after initialized, this function is used to update some new information
   void update(int edge,double cost_time,int point){
       reachingEdge = edge;
       travelTime = cost_time;
       reachingPoint = point;
   }
   
   void reset(){
       travelTime = Initial_status;
       dest_distance=Initial_status;
   }
   
   bool visited(){
       if(travelTime != Initial_status) return true;
       else return false;
   }
   
   void print(){
       std::cout<<"reachingEdge is:"<<reachingEdge<<std::endl
               <<"reachingPoint is:"<<reachingPoint<<std::endl
               <<"travelTime is:"<<travelTime<<std::endl
               <<"intersection_Idx is:"<<intersection_Idx<<std::endl;
   }
   
   void operator=(Node const &rhs){
       reachingEdge = rhs.reachingEdge;
       travelTime = rhs.travelTime;
       reachingPoint = rhs.reachingPoint;
       intersection_Idx = rhs.intersection_Idx;
       dest_distance = rhs.dest_distance;
       dest_Idx = rhs.dest_Idx;
   }
   
   //operators here only means a better way, cause heap only put the max one to the top
   //so we choose the smallest travel time as the best one
   //a > b,indicates a is a better way, which means that a cost fewer time than b
   bool  operator< (Node rhs){
       if((travelTime+dest_distance) > (rhs.travelTime+rhs.dest_distance))
           return true;
       else return false;
   }
   bool  operator> (Node rhs){
       if((travelTime+dest_distance) < (rhs.travelTime+rhs.dest_distance))
           return true;
       else return false;
   }
   bool  operator<= (Node rhs){
       if((travelTime+dest_distance) >= (rhs.travelTime+rhs.dest_distance))
           return true;
       else return false;
   }
   bool  operator>= (Node rhs){
       if((travelTime+dest_distance) <= (rhs.travelTime+rhs.dest_distance))
           return true;
       else return false;
   }
   bool  operator== (Node rhs){
       if((travelTime+dest_distance) == (rhs.travelTime+rhs.dest_distance))
           return true;
       else return false;
   }
   
   
//      bool  operator< (Node rhs){
//       if((travelTime ) > (rhs.travelTime  ))
//           return true;
//       else return false;
//   }
//   bool  operator> (Node rhs){
//       if((travelTime ) < (rhs.travelTime  ))
//           return true;
//       else return false;
//   }
//   bool  operator<= (Node rhs){
//       if((travelTime ) >= (rhs.travelTime  ))
//           return true;
//       else return false;
//   }
//   bool  operator>= (Node rhs){
//       if((travelTime ) <= (rhs.travelTime  ))
//           return true;
//       else return false;
//   }
//   bool  operator== (Node rhs){
//       if((travelTime ) == (rhs.travelTime  ))
//           return true;
//       else return false;
//   }
//   
   
};


//decide whether a point is on the left of line
//whose direction is from--->to
bool point_on_left_of_line(ezgl::point2d point,ezgl::point2d line_from,
                        ezgl::point2d point_to);

//draw the path,indicating directions
void draw_route(ezgl::renderer &canvas_renderer,const std::vector<unsigned>& path
                                                    ,ezgl::rectangle screen_size);

//creates UI contains function to find route
void act_on_show_path_find_button(GtkWidget *widgets,ezgl::application *application);

void act_on_type_to_show_path_button(GtkWidget *widgets,ezgl::application *application);

void act_on_show_route_info(GtkWidget *widgets,ezgl::application *application);

//consider the situation of one-way
//return a list of point-line combination
std::vector<seg_inter> find_intersection_reachable_segments(int intersection_id);

//this function is used in find_path_between_intersections
//it decides whether should add this segment to the Node_list
bool edge_is_legal(seg_inter way, double shortest_time,const double right_turn_penalty, 
         const double left_turn_penalty,Node current_Node,unsigned intersect_id_end);

//this is basically the same as find_path_between_intersections
//however,it gives the time used by the shortest path
double find_path_shortest_time(
    const unsigned intersect_id_start, 
    const unsigned intersect_id_end,
    const double right_turn_penalty, 
    const double left_turn_penalty);
#endif /* M3_APPENDIX_H */

