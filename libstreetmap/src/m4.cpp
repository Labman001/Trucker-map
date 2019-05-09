/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m4_appendix.h"
#include "m3.h"
#include "m3_appendix.h"
#include "m2.h"
#include "m2_appendix.h"
#include "m1.h"
#include "m1_appendix.h"
#include <list>
#include <chrono>
#include <thread>


vector<vector<vector<vector<PathInfo>>>> each_point_cost;

std::vector<CourierSubpath> traveling_courier(
    const std::vector<DeliveryInfo>& deliveries,
    const std::vector<unsigned>& depots,
    const float right_turn_penalty,
    const float left_turn_penalty,
    const float truck_capacity
){
    auto startTime= std::chrono::high_resolution_clock::now();
    
    //std::map<double,std::vector<CourierSubpath>> path_different_depot;
    std::map<double,std::vector<PointInfo>> path_different_depot;
    
    
    
    //setup the cost between each intersections
    each_point_cost.clear();
    each_point_cost = cost_matrix(deliveries,right_turn_penalty,left_turn_penalty);
    unsigned solution_number = depots.size()+deliveries.size();
    if(solution_number >= INITIAL_SOLUTION)
        solution_number = INITIAL_SOLUTION;

       //////////////multi-thread part
        //store all results from different start point
        std::vector<std::vector<PointInfo>> results_list(solution_number);
        //force to have 10 solutions
        #pragma omp parallel for 
        for(unsigned initial_counter =0;initial_counter<solution_number;initial_counter++){
            //this sequence is used to store some temporary sub path 
            std::vector<PointInfo> temp_sequence;
            //this sequence stores a valid result
            std::vector<PointInfo> real_sequence;
            std::vector<CourierSubpath> legal_path;

            std::vector<int> new_delivery;
            unsigned index =0;
            PointInfo start_depot;
            unsigned start_intersection;
            //get start point via depots
            if(initial_counter<depots.size()){
               start_depot.intersection_Idx = depots[initial_counter];
               start_depot.is_pick_up = true;
               start_depot.deliver_indices = Initial_status;
               start_depot.cost = 0;
               start_intersection = find_closest_pickup(depots[initial_counter],index,deliveries);
               
            } else{ //choose random pickUp to start
                index = initial_counter - depots.size();
                start_intersection = deliveries[index].pickUp;
                start_depot.intersection_Idx = find_closest_depot(start_intersection,depots);
                start_depot.is_pick_up = true;
                start_depot.deliver_indices = Initial_status;
                start_depot.cost = 0;
            }

            real_sequence.push_back(start_depot);
            
            PointInfo startPoint(start_intersection,index,true);
            //initialize the look up table for deliveries
            for(unsigned i=0;i<deliveries.size();i++){
                new_delivery.push_back(i);
            }
            //update for start point
            //this point has already been picked up
            new_delivery[index]=Initial_status;

            real_sequence.push_back(startPoint);
            temp_sequence.push_back(startPoint);
            //initialize the temp
            //add all pickUp points               
            for(unsigned i=0;i<deliveries.size();i++){
                if(i!=index){
                    PointInfo newPoint(deliveries[i].pickUp,i,true);
                    temp_sequence.push_back(newPoint);
                }
            }

            //reset the capacity remains
            float current_capacity = truck_capacity - deliveries[index].itemWeight;

            while (find_legal_point(temp_sequence,new_delivery,deliveries,current_capacity)){
                real_sequence.push_back(temp_sequence[0]);
                float weight = deliveries[temp_sequence[0].deliver_indices].itemWeight;
                if(temp_sequence[0].is_pick_up){
                    current_capacity =current_capacity - weight;
                } else {
                    current_capacity =current_capacity  + weight;
                }
            }

            results_list[initial_counter] = real_sequence;
        }


        double time_cost =0;
        for(unsigned i=0;i<results_list.size();i++){
            //find the time cost of this path
            time_cost = find_path_time_without_depot(results_list[i]);
            path_different_depot.insert(std::make_pair(time_cost,results_list[i]));
        } 
    
    //////////////////////start optimizing///////////
    double best_time;
    std::vector<PointInfo> current_best_sequence=path_different_depot.begin()->second;
    bool no_better=false;
    while (!no_better){   
        TWO_OPT:
        best_time=find_path_time_without_depot(current_best_sequence);
        for (unsigned i=1;i<current_best_sequence.size()-1;i++){
            for (unsigned k=i+1;k<current_best_sequence.size();k++){
                std::vector<PointInfo> new_sequence=two_opt_swap(current_best_sequence,i,k);
                if (check_legalty_of_sequence(new_sequence,deliveries,truck_capacity)){//the new sequence is valid
                    double new_time=find_path_time_without_depot(new_sequence);
                    if(best_time>new_time){
                        best_time=new_time;
                        current_best_sequence=new_sequence;
                        auto currentTime= std::chrono::high_resolution_clock::now();
                        auto wallClock=std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
                        if (wallClock.count()> 0.9 * 45) {
                            std::cout<<"rua time out"<<std::endl;
                            goto build_path;
                        }
                        goto TWO_OPT;
                    }
                }
                auto currentTime= std::chrono::high_resolution_clock::now();
                auto wallClock=std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
                if (wallClock.count()> 0.9 * 45) {
                    std::cout<<"rua time out"<<std::endl;
                    goto build_path;
                }
            }
        }
        //we have tried all the combinations
        no_better=true;
    }
    
    
    //////////////////////////////for 3-opt  ////
    if (current_best_sequence.size()>7){
        no_better=false;
        while(!no_better){
            Three_OPT:
            best_time=find_path_time_without_depot(current_best_sequence);
            for (unsigned i=1;i<current_best_sequence.size()-2;i++){
                for (unsigned j=i+1;j<current_best_sequence.size()-1;j++){
                    for (unsigned k=j+1;k<current_best_sequence.size();k++){
                        std::vector<PointInfo> new_sequence=three_opt_swap(current_best_sequence,i,j,k,deliveries,truck_capacity);
                        if (find_path_time_without_depot(new_sequence)<best_time){
                            best_time=find_path_time_without_depot(new_sequence);
                            current_best_sequence=new_sequence;
                            auto currentTime= std::chrono::high_resolution_clock::now();
                auto wallClock=std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
                if (wallClock.count()> 0.9 * 45) {
                    std::cout<<"rua time out"<<std::endl;
                    goto build_path;
                }
                goto Three_OPT;
                        }
                            auto currentTime= std::chrono::high_resolution_clock::now();
                auto wallClock=std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
                if (wallClock.count()> 0.9 * 45) {
                    std::cout<<"rua time out"<<std::endl;
                    goto build_path;
                }
                    }                  
                }
            }
            no_better=true;
        }
    }
    
    build_path:
    ////////////////////////////////////////////////////////////
    return build_path(current_best_sequence,depots,right_turn_penalty,left_turn_penalty);
    //return path_different_depot.begin()->second;
    //return get_actual_path(real_sequence,depots,right_turn_penalty,left_turn_penalty);
}


unsigned find_closest_depot(unsigned current_intersection,
                                        const std::vector<unsigned>& depots){
    unsigned closest_depot=depots[0];
    double min_distance=find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(depots[0]));
    for (unsigned i=1;i<depots.size();i++){
        if (min_distance>find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(depots[i]))){
            min_distance=find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(depots[i]));
            closest_depot=depots[i];
        }
    } 
    return closest_depot;   

}

unsigned find_closest_start(unsigned current_intersection,
                                        const std::vector<unsigned>& depots){
    unsigned closest_depot=depots[0];
    double min_distance=find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(depots[0]));
    for (unsigned i=1;i<depots.size();i++){
        if (min_distance>find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(depots[i]))){
            if (!find_path_between_intersections(depots[i],current_intersection,0,0).empty())
            min_distance=find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(depots[i]));
            closest_depot=depots[i];
        }
    } 
    return closest_depot;   

}

//this function is used to find the point can go to 
//if there are still some points, it will return true
//point_list store the points that we can goto, will be modified in this function.
//new_delivery stores the indices of deliveries that we still haven't picked-up,will be modified
//current_capacity is the capacity remains, which should include the capacity change for the 
//first point in the list
//the first point in point_list is regarded same as current_point,if it is a pick-up
//new_delivery will be modified
bool find_legal_point(std::vector<PointInfo>& point_list, std::vector<int> &new_delivery,
               const std::vector<DeliveryInfo>& deliveries,float current_capacity){
    //if there is only one element in the vector, it means no more any other possible 
    //point to go

    for(unsigned i=0;i<new_delivery.size();i++){
        if(new_delivery[i] != -1) goto search_more;
    }
    if(point_list.size() == 1 && !point_list[0].is_pick_up){
            return false;
    }

    search_more:
    std::vector<PointInfo> result;
    vector<vector<PathInfo>> path_cost;
    int index = point_list[0].deliver_indices;
    //at most, number of legal point is the number of deliveries
    //for current point is a pick up point, the new legal point can only be the subset of current 
    //point list, cause the capacity can only decrease
    if(point_list[0].is_pick_up){
        
        path_cost = each_point_cost[index][0];
        //delete the corresponding id from new_delivery
        new_delivery[index] = Initial_status;
        //add the corresponding drop-off
        PointInfo new_point(deliveries[index].dropOff,index,false);
        new_point.set_cost(path_cost);
        result.push_back(new_point);
        //result_counter++;
        
        for(unsigned i =1;i<point_list.size();i++){
            if(point_list[i].is_pick_up){
                if(deliveries[point_list[i].deliver_indices].itemWeight < current_capacity){
                    point_list[i].set_cost(path_cost);
                    result.push_back(point_list[i]);
                }                
            } else{
                 point_list[i].set_cost(path_cost);
                 result.push_back(point_list[i]);
            }
        }
    } else{
       //first get all drop-off points in point_list
        //except the first one, which is the current point
        path_cost = each_point_cost[index][1];
        for(unsigned i=1;i<point_list.size();i++){
            if(!point_list[i].is_pick_up){
                point_list[i].set_cost(path_cost);
                result.push_back(point_list[i]);
            }
        }
        for(unsigned i=0;i<new_delivery.size();i++){
            if(new_delivery[i] != Initial_status){
                if(deliveries[i].itemWeight < current_capacity){
                    PointInfo new_point(deliveries[i].pickUp ,i,true);
                    new_point.set_cost(path_cost);
                    result.push_back(new_point);
                }
            }
        }
                
    }
    point_list.clear();     
    std::make_heap(result.begin(),result.end());
    point_list = result;
    return true;
    
}
//calculate the manhattan distance of two latlon points
double distance_latlon(LatLon p1,LatLon p2){
    return (abs(p1.lat() - p2.lat()) + abs(p1.lon() - p2.lon()));
}



int get_point_intersection(PointInfo courier_sub_point){
    return courier_sub_point.intersection_Idx;
}

std::vector<CourierSubpath> build_path(std::vector<PointInfo> point_list,
                                const std::vector<unsigned>& depots,
                                const double right_turn_penalty, 
                                const double left_turn_penalty){
    std::vector<CourierSubpath> actual_path;
    std::vector<unsigned> current_items_on_car;
    //this is used for each part of the subpath
    CourierSubpath sub_path;
    //for path from depot to first pickUp 
    sub_path.start_intersection = point_list[0].intersection_Idx;
    sub_path.end_intersection = point_list[1].intersection_Idx;
    sub_path.subpath = find_path_between_intersections(sub_path.start_intersection,
            sub_path.end_intersection,right_turn_penalty,left_turn_penalty);
    //we have nothing to pick at a depo start
    current_items_on_car.clear();
    sub_path.pickUp_indices = current_items_on_car;
    actual_path.push_back(sub_path);
    for(unsigned index =1;index<point_list.size()-1;index++){
        //time to complete is sub_path
        sub_path.start_intersection = point_list[index].intersection_Idx;
        sub_path.end_intersection = point_list[index+1].intersection_Idx;
        //if the start_intersection and end_intersection are same point, we 
        //allow empty vector to be the actual path
        int start_type,end_type;
        if(point_list[index].is_pick_up) {
            start_type = 0;
            current_items_on_car.clear();
            current_items_on_car.push_back(point_list[index].deliver_indices);
        }
        else {
            start_type = 1;
            current_items_on_car.clear();
        }
        if(point_list[index+1].is_pick_up) end_type =0;
        else end_type = 1;
        sub_path.subpath = each_point_cost[point_list[index].deliver_indices][start_type][point_list[index+1].deliver_indices][end_type].route;

        sub_path.pickUp_indices = current_items_on_car;
        actual_path.push_back(sub_path);
    }
    
    //to find the last drop off to closest depot
    int closest_depot = find_closest_depot(point_list.back().intersection_Idx,depots);

    sub_path.start_intersection = point_list.back().intersection_Idx;
    sub_path.end_intersection = closest_depot;

    current_items_on_car.clear();
    sub_path.pickUp_indices = current_items_on_car; 
    sub_path.subpath = find_path_between_intersections(sub_path.start_intersection,sub_path.end_intersection,right_turn_penalty,left_turn_penalty);
    if (!sub_path.subpath.empty()){
 
        actual_path.push_back(sub_path);
    }
    return actual_path;    
}


unsigned find_closest_pickup(unsigned current_intersection,unsigned& indices,
                                        const std::vector<DeliveryInfo>& deliveries){
    unsigned closest_pickup=deliveries[0].pickUp;
    indices =0;
    double min_distance=100000;
    for (unsigned index=0;index<deliveries.size();index++){
        if (current_intersection==deliveries[index].pickUp)
            min_distance=find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(deliveries[index].pickUp));
        else if (min_distance>find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(deliveries[index].pickUp))){
            min_distance=find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(deliveries[index].pickUp));
            closest_pickup=deliveries[index].pickUp;
            indices = index;
        }
    }
    return closest_pickup;
}

unsigned find_closest_dropoff(unsigned current_intersection,
                                        const std::vector<DeliveryInfo>& deliveries){
    unsigned closest_dropoff=deliveries[0].dropOff;
    double min_distance=find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(deliveries[0].dropOff));
    for (unsigned i=0;i<deliveries.size();i++){
        if (current_intersection==deliveries[i].dropOff)
            continue;
        else if (min_distance>find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(deliveries[i].dropOff))){
            min_distance=find_distance_between_two_points
                    (getIntersectionPosition(current_intersection),getIntersectionPosition(deliveries[i].dropOff));
            closest_dropoff=deliveries[i].dropOff;
        }
    }
    return closest_dropoff;
}



//this one is use the Dijkstra to find one source 
//to multi destinations
//includes the situation for same point
//the outer vector is arranged by the sequence of deliveries
//the second one stands for pickUP(0) and dropOff(1))
std::vector<std::vector<PathInfo>> find_path_multi_dest(unsigned start_point,
            std::vector<DeliveryInfo> deliveries,
            const double right_turn_penalty, 
            const double left_turn_penalty){
    //initialize all the intersections
    std::vector<path_node> intersection_list;
    for(int i=0;i<getNumIntersections();i++){
        path_node temp(i);
        intersection_list.push_back(temp);
    }
    //get all destinations 
    //the first point is always the point we are going to find
    std::vector<unsigned>dest_list;
    for(unsigned i=0;i<deliveries.size();i++){
        unsigned idx;
        idx = deliveries[i].pickUp;
        if(idx != start_point)
            dest_list.push_back(deliveries[i].pickUp);
        idx = deliveries[i].dropOff;
        if(idx != start_point)
            dest_list.push_back(deliveries[i].dropOff);
    }
    path_node current_point;
    
    intersection_list[start_point].travelTime = 0;
    
    //points in it are all visited
    std::vector<path_node> next_togo_list;
    double time_limit = Initial_status;
    //initialize from the start point
    std::vector<seg_inter> adjacent_seg = find_intersection_reachable_segments(start_point);
    for(unsigned i=0;i<adjacent_seg.size();i++){
        double cost_time = find_street_segment_travel_time(adjacent_seg[i].segment_idx);
        if(!intersection_list[adjacent_seg[i].intersection_idx].visited()){
            intersection_list[adjacent_seg[i].intersection_idx].setup(adjacent_seg[i].segment_idx,cost_time,start_point);
            next_togo_list.push_back(intersection_list[adjacent_seg[i].intersection_idx]);
        }
        else{
            if(cost_time<intersection_list[adjacent_seg[i].intersection_idx].travelTime){
                intersection_list[adjacent_seg[i].intersection_idx].setup(adjacent_seg[i].segment_idx,cost_time,start_point);
                for(unsigned j=0;j<next_togo_list.size();j++){
                    if(next_togo_list[j].intersection_Idx == adjacent_seg[i].intersection_idx){
                        next_togo_list.erase(next_togo_list.begin()+j);
                        break;
                    }
                }
                next_togo_list.push_back(intersection_list[adjacent_seg[i].intersection_idx]);
            }
        }
    }
    std::make_heap(next_togo_list.begin(),next_togo_list.end());
    
    while(!next_togo_list.empty()){
        current_point = intersection_list[next_togo_list[0].intersection_Idx];
        //delete the current point from the list
        if(next_togo_list.size()==1) next_togo_list.clear();
        else{
            std::pop_heap(next_togo_list.begin(),next_togo_list.end());
            next_togo_list.pop_back(); 
        }
        
        //if current is the destination we are going to find
        if(!dest_list.empty()){
          if(current_point.intersection_Idx == dest_list[0]){
             dest_list.erase(dest_list.begin());
             //check if the points in list have already visited
             for(unsigned i=0;i<dest_list.size();i++){
                 if(intersection_list[dest_list[i]].visited()){
                     dest_list.erase(dest_list.begin()+i);
                     i--;
                 }
             }
             if(dest_list.empty()) time_limit = current_point.travelTime;
         } 
        }

        unsigned current_index = current_point.intersection_Idx;
        std::vector<seg_inter> reachable_segments = find_intersection_reachable_segments(current_index);
        for(unsigned i=0;i<reachable_segments.size();i++){
            seg_inter togo = reachable_segments[i];
            //prevent to trace back
            unsigned cast = current_point.reachingPoint;
            if(togo.intersection_idx == cast)
                continue;
            double penalty;
            if(find_turn_type(current_point.reachingEdge,togo.segment_idx) == TurnType::LEFT)
                penalty = left_turn_penalty;
            else if(find_turn_type(current_point.reachingEdge,togo.segment_idx) == TurnType::RIGHT)
                penalty = right_turn_penalty;
            else
                penalty =0;
            double cost_time = current_point.travelTime + penalty + find_street_segment_travel_time(togo.segment_idx);
            if(time_limit == Initial_status){
                if(!intersection_list[togo.intersection_idx].visited()){
                    intersection_list[togo.intersection_idx].setup(togo.segment_idx,cost_time,current_index);
                    next_togo_list.push_back(intersection_list[togo.intersection_idx]);
                    std::push_heap(next_togo_list.begin(),next_togo_list.end());
                }  else{
                    if(cost_time<intersection_list[togo.intersection_idx].travelTime){
                        intersection_list[togo.intersection_idx].setup(togo.segment_idx,cost_time,current_index);
                    }
                } 
            }
            //all destinations have at least one route
            //current travel time must less than the time limit
            else if(cost_time<time_limit){
                if(!intersection_list[togo.intersection_idx].visited()){
                    next_togo_list.push_back(intersection_list[togo.intersection_idx]);
                    std::push_heap(next_togo_list.begin(),next_togo_list.end());
                    intersection_list[togo.intersection_idx].setup(togo.segment_idx,cost_time,current_index);
                }  else{
                    if(cost_time<intersection_list[togo.intersection_idx].travelTime){
                        intersection_list[togo.intersection_idx].setup(togo.segment_idx,cost_time,current_index);
                    }
                } 
            }
        }
    }
    
    std::vector<std::vector<PathInfo>> result;
    for(unsigned index =0;index<deliveries.size();index++){
        std::vector<PathInfo> path_for_delivery;
        PathInfo path_info(start_point);
        //for pickup 
        unsigned trace_idx = deliveries[index].pickUp;
        path_info.intersection_end = trace_idx;
        std::vector<unsigned> path;
        while(trace_idx != start_point){
            path.insert(path.begin(),intersection_list[trace_idx].reachingEdge);
            trace_idx = intersection_list[trace_idx].reachingPoint;
        }
        path_info.route = path;
        
        if(path_info.intersection_start == path_info.intersection_end){
            path_info.cost_time =0;
            path_info.route.clear();
        }
        else path_info.cost_time = compute_path_travel_time(path,right_turn_penalty,left_turn_penalty);
        path_for_delivery.push_back(path_info);
        
        //for drop off
        trace_idx = deliveries[index].dropOff;
        path_info.intersection_end = trace_idx;
        path.clear();
        while(trace_idx != start_point){
            path.insert(path.begin(),intersection_list[trace_idx].reachingEdge);
            trace_idx = intersection_list[trace_idx].reachingPoint;
        }
        path_info.route = path;
        if(path_info.intersection_start == path_info.intersection_end){
            path_info.cost_time =0;
            path_info.route.clear();
        }
        else path_info.cost_time = compute_path_travel_time(path,right_turn_penalty,left_turn_penalty);
        path_for_delivery.push_back(path_info);
        
        result.push_back(path_for_delivery);
    }
    
    return result;
}

////return a matrix of cost time between each points
//first two for row indices, and last two for col
vector<vector<vector<vector<PathInfo>>>> cost_matrix(const vector<DeliveryInfo>& deliveries,
           const float right_turn_penalty,const float left_turn_penalty ){
    vector<vector<vector<vector<PathInfo>>>> result(deliveries.size());
    //the list contains all destinations
    double right_p=static_cast<double>(right_turn_penalty);
    double left_p=static_cast<double>(left_turn_penalty);
    
    auto start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for
    for(unsigned index = 0;index<deliveries.size();index++){
        vector<vector<vector<PathInfo>>> delivery_info(2);
        vector<vector<PathInfo>> delivery_path;
        delivery_path = find_path_multi_dest(deliveries[index].pickUp,
                deliveries,right_p,left_p);
        delivery_info[0] = delivery_path;
        delivery_path = find_path_multi_dest(deliveries[index].dropOff,
                deliveries,right_p,left_p);
        delivery_info[1] = delivery_path;
        result[index] = delivery_info;
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    return result;
}



bool check_legalty_of_sequence(std::vector<PointInfo> delivery_sequence,
                               const std::vector<DeliveryInfo>& deliveries,
                                float capacity){
    float current_capacity=capacity;
    std::vector <PointInfo> work_todo;
    for (unsigned i=1;i<delivery_sequence.size();i++){
        if (delivery_sequence[i].is_pick_up){
            work_todo.push_back(delivery_sequence[i]);
            current_capacity-=deliveries[delivery_sequence[i].deliver_indices].itemWeight;
            if (current_capacity<0) 
                return false;//the truck exploded
        }
        else if (!delivery_sequence[i].is_pick_up){
            //we first check if the item has been picked 
            bool picked=false;
            for (unsigned k=0;k<work_todo.size();k++){
                if (work_todo[k].deliver_indices==delivery_sequence[i].deliver_indices){
                    picked=true;
                    work_todo.erase(work_todo.begin()+k);
                    current_capacity+=deliveries[delivery_sequence[i].deliver_indices].itemWeight;
                    break;
                }
            }
            if (!picked)
                return false;
        }
    }
    return true;
}

std::vector<PointInfo> two_opt_swap(std::vector<PointInfo> origin_point_list,unsigned i,unsigned k){
    std::vector<PointInfo> new_result;
    new_result.push_back(origin_point_list[0]);//push back the start depot
    for (unsigned j=1;j<i;j++){
        new_result.push_back(origin_point_list[j]);//from 1 to i-1
    }
    for (unsigned j=k;j>i-1;j--){
        new_result.push_back(origin_point_list[j]);//reverse from i to k
    }
    for (unsigned j=k+1;j<origin_point_list.size();j++){
        new_result.push_back(origin_point_list[j]);
    }//push pack the rest
    return new_result;
}

//pass a legal path to find the cost time,without the cost to depot
//the first element in the path is a depot
double find_path_time_without_depot(std::vector<PointInfo> path){
    double result =0;
    for(unsigned i=1;i<path.size()-1;i++){
        int first_index = path[i].deliver_indices;
        int second_index = path[i+1].deliver_indices;
        int type_first,type_second;
        if(path[i].is_pick_up) type_first = 0;
        else type_first =1;
        if(path[i+1].is_pick_up) type_second = 0;
        else type_second =1;
        result +=each_point_cost[first_index][type_first][second_index][type_second].cost_time;
    }
    return result;
}

std::vector<PointInfo> three_opt_swap(std::vector<PointInfo> origin_point_list,
        unsigned i,unsigned j,unsigned k,
        const std::vector<DeliveryInfo>& deliveries,
                                float capacity){
    std::vector<std::vector<PointInfo>> new_result;
    //fisrt we consider the 2 opt case
    std::vector<PointInfo> new_result1;//A->,<-B,C->
    new_result1=two_opt_swap(origin_point_list,i,j);
    new_result.push_back(new_result1);
    std::vector<PointInfo> new_result2;//A->,B->,<-C
    new_result2=two_opt_swap(origin_point_list,j,k);
    new_result.push_back(new_result2);
    ///////////////////////////////////////////////////////
    std::vector<PointInfo> new_result3;//A->,<-C,<-B
    for (unsigned z=0;z<i;z++){
        new_result3.push_back(origin_point_list[z]);
    }
    for (unsigned z=k;z>j-1;z--){
        new_result3.push_back(origin_point_list[z]);
    }
    for (unsigned z=j-1;z>i-1;z--){
        new_result3.push_back(origin_point_list[z]);
    }
    for (unsigned z=k+1;z<origin_point_list.size();z++){
        new_result3.push_back(origin_point_list[z]);
    }
    new_result.push_back(new_result3);
    ///////////////////////////////////////////////////////
    std::vector<PointInfo> new_result4;//A->,<-B,<-C
    for (unsigned z=0;z<i;z++){
        new_result4.push_back(origin_point_list[z]);
    }
    for (unsigned z=j-1;z>i-1;z--){
        new_result4.push_back(origin_point_list[z]);
    }
    for (unsigned z=k;z>j-1;z--){
        new_result4.push_back(origin_point_list[z]);
    }
    for (unsigned z=k+1;z<origin_point_list.size();z++){
        new_result4.push_back(origin_point_list[z]);
    }
    new_result.push_back(new_result4);
    ////////////////////////////////////////////////////
    std::vector<PointInfo> new_result5;//A->,C->,B->
    for (unsigned z=0;z<i;z++){
        new_result5.push_back(origin_point_list[z]);
    }
    for (unsigned z=j;z<k+1;z++){
        new_result5.push_back(origin_point_list[z]);
    }
    for (unsigned z=i;z<j;z++){
        new_result5.push_back(origin_point_list[z]);
    }
    for (unsigned z=k+1;z<origin_point_list.size();z++){
        new_result5.push_back(origin_point_list[z]);
    }
    new_result.push_back(new_result5);
    ///////////////////////////////////////////////////
    std::vector<PointInfo> new_result6;//A->,C->,<-B
    for (unsigned z=0;z<i;z++){
        new_result6.push_back(origin_point_list[z]);
    }
    for (unsigned z=j;z<k+1;z++){
        new_result6.push_back(origin_point_list[z]);
    }
    for (unsigned z=j-1;z>i-1;z--){
        new_result6.push_back(origin_point_list[z]);
    }
    for (unsigned z=k+1;z<origin_point_list.size();z++){
        new_result6.push_back(origin_point_list[z]);
    }
    new_result.push_back(new_result6);
    ///////////////////////////////////////////////////
    std::vector<PointInfo> new_result7;//A->,<-C,B->
    for (unsigned z=0;z<i;z++){
        new_result7.push_back(origin_point_list[z]);
    }
    for (unsigned z=k;z>j-1;z--){
        new_result7.push_back(origin_point_list[z]);
    }
    for (unsigned z=i;z<j;z++){
        new_result7.push_back(origin_point_list[z]);
    }
    for (unsigned z=k+1;z<origin_point_list.size();z++){
        new_result7.push_back(origin_point_list[z]);
    }
    new_result.push_back(new_result7);
    std::vector<PointInfo> local_best=origin_point_list;
    double best_result=find_path_time_without_depot(origin_point_list);
    for (unsigned z=0;z<new_result.size();z++){
        if (check_legalty_of_sequence(new_result[z],deliveries,capacity)){
            if (find_path_time_without_depot(new_result[z])<best_result){
                best_result=find_path_time_without_depot(new_result[z]);
                local_best=new_result[z];
            }
        }
    }
    return local_best;
}