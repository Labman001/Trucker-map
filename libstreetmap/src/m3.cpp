/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m3.h"
#include "m3_appendix.h"
#include "m2.h"
#include "m2_appendix.h"
#include "m1.h"
#include "m1_appendix.h"
#include <algorithm>
#include <chrono>

// Returns the turn type between two given segments.
// street_segment1 is the incoming segment and street_segment2 is the outgoing
// one.
// If the two street segments do not intersect, turn type is NONE.
// Otherwise if the two segments have the same street ID, turn type is 
// STRAIGHT.  
// If the two segments have different street ids, turn type is LEFT if 
// going from street_segment1 to street_segment2 involves a LEFT turn 
// and RIGHT otherwise.  Note that this means that even a 0-degree turn
// (same direction) is considered a RIGHT turn when the two street segments
// have different street IDs.
TurnType find_turn_type(unsigned street_segment1, unsigned street_segment2){
    InfoStreetSegment segment_info1 = getInfoStreetSegment(street_segment1);
    InfoStreetSegment segment_info2 = getInfoStreetSegment(street_segment2);
    
    int intersection_list[3]={-1,-1,-1};
    intersection_list[0] = segment_info1.from;
    intersection_list[1] = segment_info1.to;
    
    //the intersection of two segments
    int common_intersection =-1;
    
    //check which point is the common point of both segments
    if(intersection_list[0] != segment_info2.to &&
         intersection_list[1] != segment_info2.to  ){
        intersection_list[2] = segment_info2.to;
    }else{
        common_intersection = segment_info2.to;
    }
    if((intersection_list[0] != segment_info2.from) &&
         intersection_list[1] != segment_info2.from  ){
        intersection_list[2] = segment_info2.from;
    }else {
        common_intersection = segment_info2.from;
    }
    
    //for no intersection,return none
    if(common_intersection == -1)
        return TurnType::NONE;
    
   //if street ids are same
   //don't to need test of straight 
    if(segment_info1.streetID != segment_info2.streetID){
        ezgl::point2d location_from;
        ezgl::point2d location_to;
        //in incoming segment
        //for curved situation, find the closest curve point to common intersection 
        if(segment_info1.curvePointCount != 0){
            if(common_intersection == segment_info1.from)
                location_from = latlon_to_xy(getStreetSegmentCurvePoint(0,street_segment1));
            else 
                location_from = latlon_to_xy(getStreetSegmentCurvePoint(segment_info1.curvePointCount-1,
                        street_segment1));
        }else{
            if(intersection_list[0] != common_intersection){
                location_from = latlon_to_xy(getIntersectionPosition(intersection_list[0]));
            } else{
                location_from = latlon_to_xy(getIntersectionPosition(intersection_list[1]));
            } 
        }
        //outgoing segment
        //straight situation
        if(segment_info2.curvePointCount == 0){
            if (intersection_list[2]!=-1)
                location_to = latlon_to_xy(getIntersectionPosition(intersection_list[2]));
        } else{
            if(common_intersection == segment_info2.from){
                location_to = latlon_to_xy(getStreetSegmentCurvePoint(0,street_segment2));
            }else{
               location_to = latlon_to_xy(getStreetSegmentCurvePoint(segment_info2.curvePointCount-1,
                        street_segment2)); 
            }
        }
        ezgl::point2d location_between = latlon_to_xy(getIntersectionPosition(common_intersection));    
        if(point_on_left_of_line(location_to,location_between,location_from))
            return TurnType::LEFT;
        else 
            return TurnType::RIGHT;
    }
    //it's straight if these two have same street ID
    else{
        return TurnType::STRAIGHT;
    }
   
}


// Returns the time required to travel along the path specified, in seconds.
// The path is given as a vector of street segment ids, and this function can
// assume the vector either forms a legal path or has size == 0.  The travel
// time is the sum of the length/speed-limit of each street segment, plus the
// given right_turn_penalty and left_turn_penalty (in seconds) per turn implied
// by the path.  If the turn type is STRAIGHT, then there is no penalty
double compute_path_travel_time(const std::vector<unsigned>& path, 
                                const double right_turn_penalty, 
                                const double left_turn_penalty){
    if(path.size() == 0) return 0;
    //calculate the travel time of first segment
    double travel_time = find_street_segment_travel_time(path[0]);
    for(unsigned i=1;i<path.size();i++){
        if(find_turn_type(path[i-1],path[i]) == TurnType::LEFT)
            travel_time = travel_time +left_turn_penalty + find_street_segment_travel_time(path[i]);
        else if(find_turn_type(path[i-1],path[i]) == TurnType::RIGHT)
            travel_time = travel_time + right_turn_penalty + find_street_segment_travel_time(path[i]);
        else 
            travel_time = travel_time + find_street_segment_travel_time(path[i]);
    }
    return travel_time;
}




// Returns a path (route) between the start intersection and the end
// intersection, if one exists. This routine should return the shortest path
// between the given intersections, where the time penalties to turn right and
// left are given by right_turn_penalty and left_turn_penalty, respectively (in
// seconds).  If no path exists, this routine returns an empty (size == 0)
// vector.  If more than one path exists, the path with the shortest travel
// time is returned. The path is returned as a vector of street segment ids;
// traversing these street segments, in the returned order, would take one from
// the start to the end intersection.
std::vector<unsigned> find_path_between_intersections(
    const unsigned intersect_id_start, 
    const unsigned intersect_id_end,
    const double right_turn_penalty, 
    const double left_turn_penalty){
    /////////////
   
    /////////////
//All found Nodes will be stored in wave_list,and always use the shortest one as next node that
//going to test.
//As long as once a path is found, there will be a shortest time. Each Node that exceed this
//time will be deleted, the function returns after all Node removed from wave_list
    
    //this list used stores visited intersections
    //for reset after search
    if (intersect_id_start==intersect_id_end){
        std::vector<unsigned> path;
        path.clear();
        return path;
    }
    std::vector<unsigned> intersection_LUT;
    std::vector<Node> Node_list;
    Node current_Node;
    double shortest_time = Initial_status;
    double start_end_distance = estimate_time(intersect_id_start,intersect_id_end);
    //initialize
    
    std::vector<seg_inter> reachable_segments = find_intersection_reachable_segments(intersect_id_start);
    for(unsigned i=0;i<reachable_segments.size();i++){
        double cost_time = find_street_segment_travel_time(reachable_segments[i].segment_idx);
        //check for duplication
        if(m1_map.path_finding_intersections[reachable_segments[i].intersection_idx].visited()){
            if(cost_time<m1_map.path_finding_intersections[reachable_segments[i].intersection_idx].travelTime){
                //need to renew
                m1_map.path_finding_intersections[reachable_segments[i].intersection_idx].update(
                    reachable_segments[i].segment_idx,cost_time,intersect_id_start);
                for(unsigned j=0;j<Node_list.size();j++){
                    if(Node_list[j].intersection_Idx == reachable_segments[i].intersection_idx){
                        Node_list.erase(Node_list.begin()+j);
                        break;
                    }
                }
                Node_list.push_back(m1_map.path_finding_intersections[reachable_segments[i].intersection_idx]);
            }
        }else{
        m1_map.path_finding_intersections[reachable_segments[i].intersection_idx].setup(
                reachable_segments[i].segment_idx,cost_time,intersect_id_start,intersect_id_end);
        Node_list.push_back(m1_map.path_finding_intersections[reachable_segments[i].intersection_idx]);
        intersection_LUT.push_back(reachable_segments[i].intersection_idx);
        }
    }
    //use the data structure heap
    std::make_heap(Node_list.begin(),Node_list.end());
    
    while(!Node_list.empty()){
        current_Node = Node_list.front();
        //delete the selected one from the list
        if(Node_list.size()==1) Node_list.clear();
        else{
            std::pop_heap(Node_list.begin(),Node_list.end());
            Node_list.pop_back(); 
        }

        //if reach destination
        if(current_Node.intersection_Idx == intersect_id_end){
            if(shortest_time == Initial_status){
                shortest_time = current_Node.travelTime;
            } else if(shortest_time > current_Node.travelTime){
                shortest_time = current_Node.travelTime;
               }
            if(Node_list.empty()) break;
            else continue;
        }
        
        if(shortest_time!= Initial_status){
            if(current_Node.travelTime >= shortest_time
                    ||current_Node.dest_distance > start_end_distance/4)
                continue;
        }
        //get all segments that the node can travel to 
        reachable_segments = find_intersection_reachable_segments(
                current_Node.intersection_Idx);
        
        for(unsigned i=0;i<reachable_segments.size();i++){
            //prevent to trace back
            if(reachable_segments[i].intersection_idx == current_Node.reachingPoint)
                continue;
            //check should add this segment to list for further search or not
            if(edge_is_legal(reachable_segments[i],shortest_time,right_turn_penalty,
                    left_turn_penalty,current_Node,intersect_id_end)){
                //add to the list
                Node_list.push_back(m1_map.path_finding_intersections[ 
                    reachable_segments[i].intersection_idx ]);
                std::push_heap(Node_list.begin(),Node_list.end());
                intersection_LUT.push_back(reachable_segments[i].intersection_idx);

            }
        }
    }

    //here use two vectors, one to record path in reverse order, and another one
    //will reverse it. The reason to do this:1.it asks to return as vector,which
    //doesn't provide push_front.  2.complexity of push_back is O(1), but for insert
    //it's O(n)
    std::vector<unsigned> reverse_path;
    std::vector<unsigned> result;
    //if no path is found between these two points
    if(shortest_time == Initial_status) return result;
    
    //get the path
    current_Node = m1_map.path_finding_intersections[intersect_id_end];
    while(current_Node.intersection_Idx != intersect_id_start){
        reverse_path.push_back(current_Node.reachingEdge);
        //get next node
        current_Node = m1_map.path_finding_intersections[current_Node.reachingPoint];
    }
    //reverse the order
    for(int i=reverse_path.size()-1;i>-1;i--){
        result.push_back(reverse_path[i]);
    }
    
    //reset the data in m1_map
    for(unsigned i=0;i<intersection_LUT.size();i++){
        m1_map.path_finding_intersections[intersection_LUT[i]].reset();
    }
    
    
    return result;
}

//additional functions


//decide whether a point is on the left of line
//whose direction is from--->to
bool point_on_left_of_line(ezgl::point2d point_to,ezgl::point2d point_middle,
                        ezgl::point2d point_from){

     point_from = point_middle - point_from;
     point_to = point_to - point_middle;
  
    //find determinant 
    //from X to(cross product order)
    double det = point_from.x*point_to.y - point_to.x*point_from.y;
    if(det > 0) return true;
    else return false;
}


//////////////////////////
//UI//////////////////////
//////////////////////////

//this function sets a flag to tell the function that controls mouse click
//that we should pick intersections to find path
//more detailed implementation in
//"act_on_mouse_press"
void act_on_show_path_find_button(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    m2_map.main_status.start_find_path = true;
    application->update_message("Start picking intersections");
}

void act_on_type_to_show_path_button(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    GtkEntry* text_entry_1 = (GtkEntry*) application->get_object("SearchInput_down");
    m2_map.main_input.down_entry_input = gtk_entry_get_text(text_entry_1);
    GtkEntry* text_entry_2 = (GtkEntry*) application->get_object("SearchInput_up");
    m2_map.main_input.up_entry_input = gtk_entry_get_text(text_entry_2);
    //If user doesn't type anything or missing a street name
    if ((m2_map.main_input.down_entry_input.empty())
            ||(m2_map.main_input.up_entry_input.empty())){
        application->update_message("Please enter both street names to find intersections!");
    }
    //both input has been filled 
    else if ((!m2_map.main_input.down_entry_input.empty())
            ||(!m2_map.main_input.up_entry_input.empty())){
        //creates a vector that holds the possible results
        std::vector<std::pair<unsigned,unsigned>> street_Idx_pair = possible_intersection_of_two_streets_Idx(
        find_street_ids_from_partial_street_name(m2_map.main_input.down_entry_input),
              find_street_ids_from_partial_street_name(m2_map.main_input.up_entry_input));
        if (street_Idx_pair.size()<1){
            application->update_message("No such intersection, try type another intersection!");
        }
        else if (street_Idx_pair.size()>3){
            application->update_message("Too many intersections, try again!");
        }
        //the number of intersections we find are good,let's input it
        else if (m2_map.main_status.intersections_typed==0){
            application->update_message("Good that's our start point");
            m2_map.main_status.path_from=*(find_intersection_ids_from_street_ids(
                 street_Idx_pair[0].first,street_Idx_pair[0].second).begin());
            m2_map.main_status.intersections_typed+=1;
            gtk_entry_set_text (text_entry_1,"");
            gtk_entry_set_text (text_entry_2,"");
        }
        else if (m2_map.main_status.intersections_typed==1){
            application->update_message("Good that's our destination");
            m2_map.main_status.path_to=*(find_intersection_ids_from_street_ids(
                 street_Idx_pair[0].first,street_Idx_pair[0].second).begin());
            m2_map.main_status.intersections_typed=0;
            m2_map.main_status.draw_path=true;
            //get penalty from user
            GtkSpinButton* left_p=(GtkSpinButton*)(application->get_object("left_penalty"));
            m2_map.left_penalty=gtk_spin_button_get_value (left_p);
            GtkSpinButton* right_p=(GtkSpinButton*)(application->get_object("right_penalty"));
            m2_map.right_penalty=gtk_spin_button_get_value (right_p);
            std::cout<<"penalty"<<m2_map.left_penalty;
            //call function to find path here
            m2_map.path_founded=find_path_between_intersections(m2_map.main_status.path_from,m2_map.main_status.path_to,m2_map.right_penalty,m2_map.left_penalty);
        }
        application->refresh_drawing();
    }
}

//this function is used to show detailed road directions
void act_on_show_route_info(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    //here we hardcode the path to test the functionality
    std::vector<unsigned> path=m2_map.path_founded;
    GObject *window; // the parent window over which to add the dialog
    window = application->get_object(application->get_main_window_id().c_str());
    GtkWidget *route_dialogue;
    route_dialogue=gtk_dialog_new_with_buttons(
            "Route",
            (GtkWindow*) window,
            GTK_DIALOG_MODAL,
            ("OK"),
            GTK_RESPONSE_ACCEPT,
            NULL,
            GTK_RESPONSE_REJECT,
            NULL);
    GtkWidget *content_area;
    GtkWidget *label; // the label we will create to display a message in the content area
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(route_dialogue));
    std::string route_string;
    double temp_travel_time=0;
    std::string temp_name;
    if (path.size()==0)
        route_string="Sorry we can't find the path for you./You are standing on your destination.\n";
    else
    for (unsigned i=0;i<path.size();i++){
        //this is the first segment in this path
        if (temp_name == ""){
            route_string = route_string+"Drive on "+getStreetName(getInfoStreetSegment(path[i]).streetID);
            temp_name = getStreetName(getInfoStreetSegment(path[i]).streetID);
            temp_travel_time = m1_map.segment_travel_time_chart[path[i]]; 
            //if there is only one segment in this path
            //we just print the string out
            if (i==path.size()-1)
                route_string += " for about "+std::to_string(temp_travel_time)+" seconds.\nThen you will see your desination!";
        }
        //this path has the same name with previous segment
        else if (temp_name==getStreetName(getInfoStreetSegment(path[i]).streetID)){
            temp_travel_time += m1_map.segment_travel_time_chart[path[i]];
            if (i==path.size()-1)
                route_string += " for about "+std::to_string(temp_travel_time)+" seconds.\nThen you will see your desination!";
        }
        //we have reached a new segment that doesn't have the same name
        else if (temp_name!=getStreetName(getInfoStreetSegment(path[i]).streetID)){
            route_string = route_string+" for about "+std::to_string(temp_travel_time)+" seconds.\n";
            temp_name = getStreetName(getInfoStreetSegment(path[i]).streetID);
            temp_travel_time = m1_map.segment_travel_time_chart[path[i]];
            if (find_turn_type(path[i-1],path[i])==TurnType::LEFT){
                route_string += "Turn left to "+getStreetName(getInfoStreetSegment(path[i]).streetID)+" and drive";
                temp_travel_time += m2_map.left_penalty;
            }
            if (find_turn_type(path[i-1],path[i])==TurnType::RIGHT){
                route_string += "Turn right to "+getStreetName(getInfoStreetSegment(path[i]).streetID)+" and drive";
                temp_travel_time += m2_map.right_penalty;
            }
            if (find_turn_type(path[i-1],path[i])==TurnType::STRAIGHT)
                route_string += "Drive on "+getStreetName(getInfoStreetSegment(path[i]).streetID);
            if (i==path.size()-1){
                route_string += " for about "+std::to_string(temp_travel_time)+" seconds.\nThen you will see your desination!";
            }
        }   
    }
    label = gtk_label_new(route_string.c_str());
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(route_dialogue);
    int result=gtk_dialog_run(GTK_DIALOG(route_dialogue));
     switch (result)
    {
    case GTK_RESPONSE_ACCEPT:
       // do_application_specific_something ();
       break;
    default:
       // do_nothing_since_dialog_was_cancelled ();
       break;
    }
    gtk_widget_destroy (route_dialogue);
    application->refresh_drawing();
}

//////////////////////////
//GRAPHING////////////////
//////////////////////////
void draw_route(ezgl::renderer &canvas_renderer,const std::vector<unsigned>& path
                    ,ezgl::rectangle screen_size){
    if (path.size()==0)
        return;//the path is empty, we do nothing
    for (unsigned i=0;i<path.size();i++){
        draw_full_segment(canvas_renderer,path[i],ezgl::BLUE,6,screen_size);
        
    }

}

//consider the situation of one-way
//return a list of point-line combination
//no rings will be returned
std::vector<seg_inter> find_intersection_reachable_segments(int intersection_id){
    std::vector<seg_inter> result;
    result.reserve(m1_map.intersection_adjacent_segments[intersection_id].size());
    for(unsigned i=0;i<m1_map.intersection_adjacent_segments[intersection_id].size();i++){
        InfoStreetSegment seg_info = getInfoStreetSegment(m1_map.intersection_adjacent_segments[intersection_id][i]);
        //we discard the rings
        if(seg_info.from == seg_info.to) continue;
        if(seg_info.oneWay){
            if(seg_info.from != intersection_id){
                continue;
            }
        }
        seg_inter elem;
        elem.segment_idx = m1_map.intersection_adjacent_segments[intersection_id][i];
        if(seg_info.from != intersection_id) elem.intersection_idx = seg_info.from;
        else elem.intersection_idx = seg_info.to;
        result.push_back(elem);
    }
    return result;
}

//this function is used in find_path_between_intersections
//it decides whether should add this segment to the Node_list
//!!!!it also change the data in m1_map,so after calling, the node in m1_map are already
//be renewed, and it won't change if result is false
//cost_time will be rewrite to new cost time, and can use it directly in find_path
bool edge_is_legal(seg_inter way, double shortest_time,const double right_turn_penalty, 
         const double left_turn_penalty,Node current_Node,unsigned intersect_id_end){

    double penalty =0;
    if(right_turn_penalty !=0 || left_turn_penalty != 0){
        TurnType turn =find_turn_type(current_Node.reachingEdge,way.segment_idx);
        if(turn == TurnType::LEFT) penalty = left_turn_penalty;
        else if(turn == TurnType::RIGHT) penalty = right_turn_penalty;
    }
    double cost_time = penalty + current_Node.travelTime + 
                find_street_segment_travel_time(way.segment_idx);
    //a path is found
    //when calling this function, it indicates that the travel time of current node is 
    //less than shortest time
    if(shortest_time != Initial_status){
        if(cost_time>=shortest_time){
            return false;
        }
    }
    //no path found so far
    if( !m1_map.path_finding_intersections[way.intersection_idx].visited()){
        m1_map.path_finding_intersections[way.intersection_idx].setup(way.segment_idx,
                        cost_time,current_Node.intersection_Idx,intersect_id_end);
        //if(shortest_time!= Initial_status) m1_map.path_finding_intersections[way.intersection_idx].v =true;
        return true;
    }
        
    else if(m1_map.path_finding_intersections[way.intersection_idx].travelTime > cost_time){
            //update the data in m1_map
            m1_map.path_finding_intersections[way.intersection_idx].update(way.segment_idx,
                        cost_time,current_Node.intersection_Idx);
            //if(shortest_time!= Initial_status) m1_map.path_finding_intersections[way.intersection_idx].v =true;
       
            return true;
        } else {
            return false;
        }
   
}


//calculate the manhattan distance with coefficient
//and use the time  of 60km/h as average speed. To match the cost time,
//convert the result to second
double estimate_time(unsigned p1,unsigned p2){
    LatLon p1_location = getIntersectionPosition(p1);
    LatLon p2_location = getIntersectionPosition(p2);

    return find_distance_between_two_points(p1_location,p2_location)*estimate_const;
}



//this is basically the same as find_path_between_intersections
//however,it gives the time used by the shortest path
double find_path_shortest_time(
    const unsigned intersect_id_start, 
    const unsigned intersect_id_end,
    const double right_turn_penalty, 
    const double left_turn_penalty){
    //All found Nodes will be stored in wave_list,and always use the shortest one as next node that
//going to test.
//As long as once a path is found, there will be a shortest time. Each Node that exceed this
//time will be deleted, the function returns after all Node removed from wave_list
    
    //this list used stores visited intersections
    //for reset after search
    if (intersect_id_start==intersect_id_end){
        return 0;
    }
    std::vector<unsigned> intersection_LUT;
    std::vector<Node> Node_list;
    Node current_Node;
    double shortest_time = Initial_status;
    double start_end_distance = estimate_time(intersect_id_start,intersect_id_end);
    //initialize
    
    std::vector<seg_inter> reachable_segments = find_intersection_reachable_segments(intersect_id_start);
    for(unsigned i=0;i<reachable_segments.size();i++){
        double cost_time = find_street_segment_travel_time(reachable_segments[i].segment_idx);
        //check for duplication
        if(m1_map.path_finding_intersections[reachable_segments[i].intersection_idx].visited()){
            if(cost_time<m1_map.path_finding_intersections[reachable_segments[i].intersection_idx].travelTime){
                //need to renew
                m1_map.path_finding_intersections[reachable_segments[i].intersection_idx].update(
                    reachable_segments[i].segment_idx,cost_time,intersect_id_start);
                for(unsigned j=0;j<Node_list.size();j++){
                    if(Node_list[j].intersection_Idx == reachable_segments[i].intersection_idx){
                        Node_list.erase(Node_list.begin()+j);
                        break;
                    }
                }
                Node_list.push_back(m1_map.path_finding_intersections[reachable_segments[i].intersection_idx]);
            }
        }else{
        m1_map.path_finding_intersections[reachable_segments[i].intersection_idx].setup(
                reachable_segments[i].segment_idx,cost_time,intersect_id_start,intersect_id_end);
        Node_list.push_back(m1_map.path_finding_intersections[reachable_segments[i].intersection_idx]);
        intersection_LUT.push_back(reachable_segments[i].intersection_idx);
        }
    }
    //use the data structure heap
    std::make_heap(Node_list.begin(),Node_list.end());
    
    while(!Node_list.empty()){
        current_Node = Node_list.front();
        //delete the selected one from the list
        if(Node_list.size()==1) Node_list.clear();
        else{
            std::pop_heap(Node_list.begin(),Node_list.end());
            Node_list.pop_back(); 
        }

        //if reach destination
        if(current_Node.intersection_Idx == intersect_id_end){
            if(shortest_time == Initial_status){
                shortest_time = current_Node.travelTime;
            } else if(shortest_time > current_Node.travelTime){
                shortest_time = current_Node.travelTime;
               }
            if(Node_list.empty()) break;
            else continue;
        }
        
        if(shortest_time!= Initial_status){
            if(current_Node.travelTime >= shortest_time
                    ||current_Node.dest_distance > start_end_distance/4)
                continue;
        }
        //get all segments that the node can travel to 
        reachable_segments = find_intersection_reachable_segments(
                current_Node.intersection_Idx);
        
        for(unsigned i=0;i<reachable_segments.size();i++){
            //prevent to trace back
            if(reachable_segments[i].intersection_idx == current_Node.reachingPoint)
                continue;
            //check should add this segment to list for further search or not
            if(edge_is_legal(reachable_segments[i],shortest_time,right_turn_penalty,
                    left_turn_penalty,current_Node,intersect_id_end)){
                //add to the list
                Node_list.push_back(m1_map.path_finding_intersections[ 
                    reachable_segments[i].intersection_idx ]);
                std::push_heap(Node_list.begin(),Node_list.end());
                intersection_LUT.push_back(reachable_segments[i].intersection_idx);

            }
        }
    }
    return shortest_time;
}