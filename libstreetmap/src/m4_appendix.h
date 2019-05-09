/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m4_appendix.h
 * Author: yaoyinji
 *
 * Created on March 20, 2019, 9:34 AM
 */

#ifndef M4_APPENDIX_H
#define M4_APPENDIX_H
#pragma once
#include <vector>
#include <list>
#include "m3.h"
#include "m3_appendix.h"
#include "m2.h"
#include "m2_appendix.h"
#include "m1.h"
#include "m1_appendix.h"
using namespace std;


#define INITIAL_SOLUTION 12

struct DeliveryInfo {
//Specifies a delivery order (input to your algorithm).
//
//To satisfy the order the item-to-be-delivered must have been picked-up
//from the pickUp intersection before visiting the dropOff intersection.

DeliveryInfo(unsigned pick_up, unsigned drop_off, float weight)
    : pickUp(pick_up), dropOff(drop_off), itemWeight(weight) {}

//The intersection id where the item-to-be-delivered is picked-up.
    unsigned pickUp;

//The intersection id where the item-to-be-delivered is dropped-off.
    unsigned dropOff;

// Weight of the item in pounds (lb)
    float itemWeight;
};

struct CourierSubpath {

// Specifies one subpath of the courier truck route
// The intersection id where a start depot, pick-up intersection or drop-off intersection
// is located
    unsigned start_intersection;
// The intersection id where this subpath ends. This must be the
// start_intersection of the next subpath or the intersection of an end depot
    unsigned end_intersection;
// Street segment ids of the path between start_intersection and end_intersection
// They form a connected path (see m3.h)
    std::vector<unsigned> subpath;
// Specifies the indices from the deliveries vector of the picked up
// delivery items at the start_intersection (if a pick up is to be made)
// Will be length zero if no delivery item is picked up at the start intersection
    std::vector<unsigned> pickUp_indices;
};



class PathInfo{
public:
    double cost_time;
    unsigned intersection_start;
    unsigned intersection_end;
    std::vector<unsigned> route;
    PathInfo(){}
    
    PathInfo(unsigned start){
        intersection_start = start;
    }
    
    void print(){
        cout<<"cost_time is "<<cost_time<<'\n'
                <<"intersection_start is"<<intersection_start<<'\n'
                <<"intersection_end is"<<intersection_end<<'\n';
        cout<<"route are:\n";
        for(unsigned i=0;i<route.size();i++){
            cout<<route[i]<<' ';
        }        
    }
    
    
    void operator=(const PathInfo &rhs){
        cost_time = rhs.cost_time;
        intersection_start = rhs.intersection_start;
        intersection_end = rhs.intersection_end;
        route = rhs.route;
        return ;
    }
    
    //closer distance, higher priority
    bool operator==(PathInfo rhs){
        if(cost_time == rhs.cost_time) return true;
        else return false;
    }
    bool operator>(PathInfo rhs){
        if(cost_time > rhs.cost_time) return false;
        else return true;
    }
    
    bool operator<(PathInfo rhs){
        if(cost_time < rhs.cost_time) return false;
        else return true;
    }
        
    bool operator>=(PathInfo rhs){
        if(cost_time <= rhs.cost_time) return true;
        else return false;
    }
    
    bool operator<=(PathInfo rhs){
        if(cost_time >= rhs.cost_time) return true;
        else return false;
    }
};

class PointInfo{
public:
    unsigned intersection_Idx;
    unsigned deliver_indices; //the indices in delivery vector
    bool is_pick_up;
    double cost;
    PointInfo(){}
    
    PointInfo(unsigned inter,unsigned idx,bool pick_up){
        intersection_Idx = inter;
        deliver_indices = idx;
        is_pick_up = pick_up;
        cost = 0;
    }
    

    //closer distance, higher priority
    bool operator>(PointInfo rhs){
        if(cost > rhs.cost) return false;
        else return true;
    }
    
    bool operator<(PointInfo rhs){
        if(cost < rhs.cost) return false;
        else return true;
    }
        
    bool operator>=(PointInfo rhs){
        if(cost <= rhs.cost) return true;
        else return false;
    }
    
    bool operator<=(PointInfo rhs){
        if(cost >= rhs.cost) return true;
        else return false;
    }
    
    void operator=(PointInfo const &rhs){
        intersection_Idx = rhs.intersection_Idx;
        deliver_indices = rhs.deliver_indices;
        is_pick_up = rhs.is_pick_up;
        cost = rhs.cost;
    }
    
    void set_cost(vector<vector<PathInfo>> path_cost){
        int p;
        if(is_pick_up) p = 0;
        else p=1;
        cost = path_cost[deliver_indices][p].cost_time;
    }

};




class path_node{
public:
   int reachingEdge;  // ID of edge used to reach this node
   double travelTime; // Total travel time to reach node 
   int reachingPoint; //ID of the point used to reach the node
   unsigned intersection_Idx;

   
   path_node(){
       travelTime = Initial_status;
   }
   
   path_node(int inter_id)
   {
    travelTime = Initial_status;
    reachingPoint = Initial_status;
    reachingEdge = Initial_status;
     intersection_Idx = inter_id;
   }
   
   //used to setup information for a node as initializing
   void setup(int edge,double cost_time,int point){
       reachingEdge = edge;
       travelTime = cost_time;
       reachingPoint = point;

   }
   
   
   void reset(){
       travelTime = Initial_status;
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
   
   void operator=(path_node const &rhs){
       reachingEdge = rhs.reachingEdge;
       travelTime = rhs.travelTime;
       reachingPoint = rhs.reachingPoint;
       intersection_Idx = rhs.intersection_Idx;

   }
   
   //operators here only means a better way, cause heap only put the max one to the top
   //so we choose the smallest travel time as the best one
   //a > b,indicates a is a better way, which means that a cost fewer time than b
   bool  operator< (path_node rhs){
       if((travelTime) > (rhs.travelTime))
           return true;
       else return false;
   }
   bool  operator> (path_node rhs){
       if((travelTime) < (rhs.travelTime))
           return true;
       else return false;
   }
   bool  operator<= (path_node rhs){
       if((travelTime) >= (rhs.travelTime))
           return true;
       else return false;
   }
   bool  operator>= (path_node rhs){
       if((travelTime) <= (rhs.travelTime))
           return true;
       else return false;
   }
   bool  operator== (path_node rhs){
       if((travelTime) == (rhs.travelTime))
           return true;
       else return false;
   }

   
};


std::vector<CourierSubpath> traveling_courier(
    const std::vector<DeliveryInfo>& deliveries,
    const std::vector<unsigned>& depots,
    const float right_turn_penalty,
    const float left_turn_penalty,
    const float truck_capacity
);

unsigned find_closest_pickup(unsigned current_intersection,unsigned& indices,
                                        const std::vector<DeliveryInfo>& deliveries);

unsigned find_closest_dropoff(unsigned current_intersection,
                                        const std::vector<DeliveryInfo>& deliveries);

unsigned find_closest_depot(unsigned current_intersection,
                                        const std::vector<unsigned> &depots);

unsigned find_closest_start(unsigned current_intersection,
                                        const std::vector<unsigned>& depots);

//get the path from a depot to the last dropOff
// it will find the closest depot for the last dropOff
std::vector<CourierSubpath> build_path(std::vector<PointInfo> point_list,
                                const std::vector<unsigned>& depots,
                                const double right_turn_penalty, 
                                const double left_turn_penalty);


std::vector<CourierSubpath> get_actual_path(std::vector<PointInfo> point_list,
                                const std::vector<unsigned>& depots,
                                const double right_turn_penalty, 
                                const double left_turn_penalty);

//this function is used to find the point can go to 
//if there are still some points, it will return true
//point_list store the points that we can goto, will be modified in this function.
//new_delivery stores the indices of deliveries that we still haven't picked-up,will be modified
//current_capacity is the capacity remains, which should include the capacity change for the 
//first point in the list
//the first point in point_list is regarded same as current_point,if it is a pick-up
//new_delivery will be modified
bool find_legal_point(std::vector<PointInfo>& point_list, 
                        std::vector<int> &new_delivery,
     const std::vector<DeliveryInfo>& deliveries,
        float current_capacity);

//this one is use the Dijkstra to find one source 
//to multi destinations
//includes the situation for same point
//the outer vector is arranged by the sequence of deliveries
//the second one stands for pickUP(0) and dropOff(1))
std::vector<std::vector<PathInfo>> find_path_multi_dest(unsigned start_point,
            std::vector<DeliveryInfo> deliveries,
            const double right_turn_penalty, 
            const double left_turn_penalty);

//return a matrix of cost time between each points
//first two for row indices, and last two for col
vector<vector<vector<vector<PathInfo>>>> cost_matrix(const vector<DeliveryInfo>& deliveries,
           const float right_turn_penalty,const float left_turn_penalty );

//return the deliver_list
std::vector<PointInfo> find_deliver_sequence
        (std::vector<std::vector<PathInfo>>,
        const std::vector<DeliveryInfo>& deliveries,
        float current_capacity);

int get_point_intersection(PointInfo courier_sub_point);

bool check_legalty_of_sequence(std::vector<PointInfo> delivery_sequence,
                               const std::vector<DeliveryInfo>& deliveries,
                                float capacity);

std::vector<PointInfo> two_opt_swap(std::vector<PointInfo> origin_point_list,unsigned i,unsigned k);

//pass a legal path to find the cost time,without the cost to depot
//the first element in the path is a depot
double find_path_time_without_depot(std::vector<PointInfo> path);
//calculate the manhattan distance of two latlon points
double distance_latlon(LatLon p1, LatLon p2);

std::vector<PointInfo> three_opt_swap(std::vector<PointInfo> origin_point_list,
        unsigned i,unsigned j,unsigned k,
        const std::vector<DeliveryInfo>& deliveries,float capacity);
#endif /* M4_APPENDIX_H */

