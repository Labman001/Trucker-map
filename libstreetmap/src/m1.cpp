/*
 * Copyright 2019 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated
 * documentation files (the "Software") in course work at the University
 * of Toronto, or for personal use. Other uses are prohibited, in
 * particular the distribution of the Software either publicly or to third
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "m1.h"
#include "convert.cpp"
#include "StreetsDatabaseAPI.h"
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <map>
#include "OSMDatabaseAPI.h"
#include "m1_appendix.h"



////////////These global variable initialized at the beginning, and make the search easier


//constants will be used in this .cpp
int LETTER_SIZE = 26;

/*
 Naming: a_b_c
 * a:elements of vector
 * b:relation between element and item
 * c:all items and related to element
 */

//total amount
unsigned NumIntersections,NumStreetSegments;

//global data structure
map_elements m1_map;


bool load_map(std::string map_path) {
	bool load_successful = loadStreetsDatabaseBIN(map_path); //Indicates whether the map has loaded successfully

	if(!load_successful) return false;//save time if it fails
    
	m1_map.clear();
	//total amount
	NumIntersections = getNumIntersections();
	NumStreetSegments = getNumStreetSegments();

        
           
        
	////index////
	//intersection
	unsigned Idx_intersection;
	unsigned Idx_inter_seg;
    
	//segment
	unsigned Idx_segment;
    
	//streets
	int Idx_street;
    
	//Load your map related data structures here

        
	//load data for intersections
	for (Idx_intersection = 0; Idx_intersection < NumIntersections;Idx_intersection++){
            //create a new vector for every intersection
            std::vector<unsigned> inter;
            //count how many segments do this intersection adjacent
            unsigned SegmentsAmount = getIntersectionStreetSegmentCount(Idx_intersection);
            for (Idx_inter_seg = 0; Idx_inter_seg< SegmentsAmount;Idx_inter_seg++){

        	//add the segment id of this intersection
        	inter.push_back(getIntersectionStreetSegment(Idx_inter_seg, Idx_intersection));
            }
            m1_map.intersection_adjacent_segments.push_back(inter);
       	//Remove all elements in vector
            inter.clear();
        
            Node path_node(Idx_intersection);
            m1_map.path_finding_intersections.push_back(path_node);
        }
    
    
    
//	//initialize the street vector
	for(Idx_street = 0; Idx_street < getNumStreets(); Idx_street++){
    	std::vector<unsigned> street;
    	m1_map.street_include_segments.push_back(street);
    	m1_map.street_include_intersections.push_back(street);
	}
 
//    
    
	Idx_street =0;
//	//load data for segments
//	//store the segments id on a street
	for(Idx_segment = 0; Idx_segment < NumStreetSegments;Idx_segment++){
    	////get intersection of segments
    	//get info of this seg
    	InfoStreetSegment seg_info = getInfoStreetSegment(Idx_segment);
   	 
    	//vector to hold two intersections for segement_adjacent_intersections
    	std::vector<unsigned> seg;
    	seg.push_back(seg_info.from);
    	seg.push_back(seg_info.to);
   	 
    	//store data in vector, first intersection is from and second one is to
    	m1_map.segment_adjacent_intersections.push_back(seg);
   	 
    	////add the segment to street
    	Idx_street = seg_info.streetID;
    	m1_map.street_include_segments[Idx_street].push_back(Idx_segment);
   	 
	}
//    
////	//load intersections on street
	for(Idx_street = 0;Idx_street < getNumStreets(); Idx_street++){
    	//these two are index of intersections for one segment
    	unsigned inter_Idx1,inter_Idx2;
   	 
    	//the id of segment
    	unsigned seg_Id;
   	 
    	//how many segments do this street contains
    	unsigned street_size = m1_map.street_include_segments[Idx_street].size();
    	for(unsigned i = 0; i<street_size;i++){
        	seg_Id = m1_map.street_include_segments[Idx_street][i];
        	inter_Idx1 = m1_map.segment_adjacent_intersections[seg_Id][0];
        	inter_Idx2 = m1_map.segment_adjacent_intersections[seg_Id][1];
       	 

        	m1_map.street_include_intersections[Idx_street].push_back(inter_Idx1);
        	m1_map.street_include_intersections[Idx_street].push_back(inter_Idx2);
       	 
    	}
   	 
    	std::sort(m1_map.street_include_intersections[Idx_street].begin(),
    	m1_map.street_include_intersections[Idx_street].end());
   	 
	}
    

	//start loading the length data into the chart
	//for distance
	for(int i=0;i<getNumStreetSegments();i++){
    	m1_map.segment_travel_time_chart.push_back(find_street_segment_length(i)/((getInfoStreetSegment(i).speedLimit)/3.6));
	}
        //load the OSM Database of the map
        std::string map_osm=map_path;
        for (int i=0;i<11;i++){
            map_osm.pop_back();
        }
        map_osm.push_back('o');
        map_osm.push_back('s');
        map_osm.push_back('m');
        map_osm.push_back('.');
        map_osm.push_back('b');
        map_osm.push_back('i');
        map_osm.push_back('n');
      
        bool load_osm_success = loadOSMDatabaseBIN(map_osm);
        if (!load_osm_success) {
            std::cerr <<"Failed to load osm '"<<map_osm<<"'\n";
            return 1;
        }
    
        std::cout<<"Successfully loaded osm '" << map_osm << "'\n";
        
        m1_map.street_include_segments.resize(m1_map.street_include_segments.size());
        
        
    
	return true;
}

void close_map() {
	m1_map.clear();
        
        std::cout<<"Closing osm\n";
        closeOSMDatabase();
	closeStreetDatabase();
        
	return ;
}

//Returns the street segments for the given intersection
std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id){
    return m1_map.intersection_adjacent_segments[intersection_id];
}

//Returns the street names at the given intersection (includes duplicate street
//names in returned vector)
std::vector<std::string> find_intersection_street_names(unsigned intersection_id){

	//variables to get information
	std::vector<unsigned> seg = find_intersection_street_segments(intersection_id);
	std::vector<std::string> street_name;
	unsigned Idx_seg,seg_id;
	InfoStreetSegment seg_info;
	std::string name;

	//search each segment
	for(Idx_seg = 0;Idx_seg < seg.size();Idx_seg++){
    	seg_id = seg[Idx_seg];
    	seg_info = getInfoStreetSegment(seg_id);
    	name = getStreetName(seg_info.streetID);
    	street_name.push_back(name);

	}
	return street_name;
}

//Returns true if you can get from intersection1 to intersection2 using a single
//street segment (hint: check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself
bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2){
	if(intersection_id1 == intersection_id2) return true;
    
	//check whether they share same segment
	std::vector<unsigned> seg1,seg2;
	std::vector<unsigned>::iterator ite_seg_share;
	bool bool_seg_share;
    
	seg1 = find_intersection_street_segments(intersection_id1);
	seg2 = find_intersection_street_segments(intersection_id2);
    
	//ite_seg_share points to the same element
	ite_seg_share = std::find_first_of(seg1.begin(),seg1.end(),
                               	seg2.begin(),seg2.end() );
    
	//if iterator points to seg1.end(), no segments are shared
	bool_seg_share = (ite_seg_share != seg1.end());
	if(!bool_seg_share) return false;
    
	//check the 1-way situation
	unsigned seg_id;
	InfoStreetSegment seg_info;
	seg_id = *ite_seg_share;//somehow it works.
	seg_info = getInfoStreetSegment(seg_id);
	if(!seg_info.oneWay) return true;
    
	int inter_id1=intersection_id1;
	//it's 1-way, check the direction
	if(seg_info.from == inter_id1) return true;
	else return 0;
}

//Returns all intersections reachable by traveling down one street segment
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections
std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id){
	std::vector<unsigned> seg;
	std::vector<unsigned> adj_inter;
	int seg_amount,index;
	InfoStreetSegment seg_info;
    
	seg = find_intersection_street_segments(intersection_id);
	//add the intersection itself in case duplication of this intersection
	//will delete at last
	
	seg_amount = seg.size();
        if(seg_amount == 0) return adj_inter;
        adj_inter.push_back(intersection_id);
	for(index = 0; index < seg_amount; index++){
    	seg_info = getInfoStreetSegment(seg[index]);
    	//check for duplicate
    	if(std::find(adj_inter.begin(),adj_inter.end(),seg_info.to) == adj_inter.end())
        	adj_inter.push_back(seg_info.to);
    	
        //if it's not 1-way
    	if(!seg_info.oneWay){
            if(std::find(adj_inter.begin(),adj_inter.end(),seg_info.from) == adj_inter.end())
                adj_inter.push_back(seg_info.from);
    	}
	}
	//erase the first intersection(itself)
	adj_inter.erase(adj_inter.begin());
	return adj_inter;
}

//Returns all street segments for the given street
std::vector<unsigned> find_street_street_segments(unsigned street_id){
//    if(!m1_map.street_include_segments_ordered){
//        //adjacent segment in this vector are connected in real road
//        std::vector<unsigned> ordered_segments;
//        
//    }
    
    return m1_map.street_include_segments[street_id];
}

//Returns all intersections along the a given street
std::vector<unsigned> find_all_street_intersections(unsigned street_id){
    	if(!m1_map.street_include_intersections_unique){
    	//varibles for all street
    	int Idx_street;
   	 
    	//varibles for a certain street
    	int inter_size;
    	int Idx_inter;
   	 
    	//check for each street
    	for (Idx_street = 0;Idx_street < getNumStreets();Idx_street++){
        	m1_map.street_include_intersections[Idx_street];
       	 
        	//get the size at beginning
        	inter_size = m1_map.street_include_intersections[Idx_street].size();
        	for(Idx_inter = 1; Idx_inter < inter_size;Idx_inter++){
            	if(m1_map.street_include_intersections[Idx_street][Idx_inter] == m1_map.street_include_intersections[Idx_street][Idx_inter-1]){
                	m1_map.street_include_intersections[Idx_street].erase(m1_map.street_include_intersections[Idx_street].begin() + Idx_inter);
                	Idx_inter--;//after ++ for next loop, it remains
                	inter_size = m1_map.street_include_intersections[Idx_street].size();//re-get a changed size
            	}
        	}
       	 
    	}

    	m1_map.street_include_intersections_unique = true;
	}
    
    
	return m1_map.street_include_intersections[street_id];
}

//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
std::vector<unsigned> find_intersection_ids_from_street_ids(unsigned street_id1, unsigned street_id2){
       if(!m1_map.street_include_intersections_unique){
    	//varibles for all street
    	int Idx_street;
   	 
    	//varibles for a certain street
    	int inter_size;
    	int Idx_inter;
   	 
    	//check for each street
    	for (Idx_street = 0;Idx_street < getNumStreets();Idx_street++){
        	m1_map.street_include_intersections[Idx_street];
       	 
        	//get the size at beginning
        	inter_size = m1_map.street_include_intersections[Idx_street].size();
        	for(Idx_inter = 1; Idx_inter < inter_size;Idx_inter++){
            	if(m1_map.street_include_intersections[Idx_street][Idx_inter] == m1_map.street_include_intersections[Idx_street][Idx_inter-1]){
                	m1_map.street_include_intersections[Idx_street].erase(m1_map.street_include_intersections[Idx_street].begin() + Idx_inter);
                	Idx_inter--;//after ++ for next loop, it remains
                	inter_size = m1_map.street_include_intersections[Idx_street].size();//re-get a changed size
            	}
        	}
       	 
    	}

    	m1_map.street_include_intersections_unique = true;
	}
    
    
    
	std::vector<unsigned> street1,street2;
	std::vector<unsigned> result;
	street1 = find_all_street_intersections(street_id1);
	street2 = find_all_street_intersections(street_id2);
    
	for(unsigned index = 0; index<street1.size(); index++){
    	unsigned counter = 0; //counters for street2
    	unsigned inter_id1,inter_id2;
    	//intersection that going to be tested
    	inter_id1 = street1[index];
    	//get the smallest intersection
    	inter_id2 = street2[0];
    	if(inter_id1 > street2[street2.size()-1])
        	break;
   	 
    	while(inter_id1 >= inter_id2 && counter < street2.size()){
        	if(inter_id1 == inter_id2){
            	result.push_back(inter_id1);
            	break;
        	}
        	counter ++;
        	inter_id2 = street2[counter];

    	}
   	 
	}
	return result;
}

//Returns the distance between two coordinates in meters
double find_distance_between_two_points(LatLon point1, LatLon point2){//
	double latavg=(point1.lat()+point2.lat())/2;//here is in degree, need to convert to radius
	return EARTH_RADIUS_IN_METERS*sqrt(pow((point2.lon()*DEG_TO_RAD*cos(latavg*DEG_TO_RAD)-point1.lon()*DEG_TO_RAD*cos(latavg*DEG_TO_RAD)),2.0)+
        	pow((point1.lat()*DEG_TO_RAD-point2.lat()*DEG_TO_RAD),2.0));
}

//Returns the length of the given street segment in meters
double find_street_segment_length(unsigned street_segment_id){
	InfoStreetSegment temp_segment_info=getInfoStreetSegment(street_segment_id);
	LatLon start=getIntersectionPosition(temp_segment_info.from);//latlon of the starting point
	LatLon end=getIntersectionPosition(temp_segment_info.to);//latlon of the ending point
	if (temp_segment_info.curvePointCount==0){//the segment is straight line, just find distance of the from point and end point
    	return find_distance_between_two_points(start,end);
	}
	else {//the segment isn't straight,need to find the curve length
    	double total_length;
    	//first we count the first curve and the last curve length
    	total_length=
        	find_distance_between_two_points(start,getStreetSegmentCurvePoint(0,street_segment_id))+
        	find_distance_between_two_points(end,getStreetSegmentCurvePoint(temp_segment_info.curvePointCount-1,street_segment_id));
    	//then we count the curve length inside
    	for (int i=0;i<temp_segment_info.curvePointCount-1;i++){
        	total_length=total_length+find_distance_between_two_points(getStreetSegmentCurvePoint(i,street_segment_id),getStreetSegmentCurvePoint(i+1,street_segment_id));
    	}
    	return total_length;  	 
   	 
	}
}

//Returns the length of the specified street in meters
double find_street_length(unsigned street_id){
	double total_length=0;
	for (unsigned i=0;i<m1_map.street_include_segments[street_id].size();i++){
            total_length=total_length+find_street_segment_length(m1_map.street_include_segments[street_id][i]);//need to check more times
	}
	return total_length;
}

//Returns the travel time to drive a street segment in seconds
//(time = distance/speed_limit)


double find_street_segment_travel_time(unsigned street_segment_id){
   
	return m1_map.segment_travel_time_chart[street_segment_id];
}

//Returns the nearest point of interest to the given position
unsigned find_closest_point_of_interest(LatLon my_position){
	unsigned cloest_length=find_distance_between_two_points(my_position,getPointOfInterestPosition(0)); 
        unsigned cloest_index=0;
        for (int i=0;i<getNumPointsOfInterest();i++){
            if (cloest_length>find_distance_between_two_points(my_position,getPointOfInterestPosition(i))){
                cloest_length=find_distance_between_two_points(my_position,getPointOfInterestPosition(i));
                cloest_index=i;
            }    
	}
	return cloest_index;
}

//Returns the nearest intersection to the given position
unsigned find_closest_intersection(LatLon my_position){
	unsigned cloest_length=find_distance_between_two_points(my_position,getIntersectionPosition(0)); 
        unsigned cloest_index=0;
        for (int i=0;i<getNumIntersections();i++){
            if (cloest_length>find_distance_between_two_points(my_position,getIntersectionPosition(i))){
                cloest_length=find_distance_between_two_points(my_position,getIntersectionPosition(i));
                cloest_index=i;
            }    
	}
	return cloest_index;
}

//Returns all street ids corresponding to street names that start with the given prefix
//The function should be case-insensitive to the street prefix. For example,
//both "bloo" and "BloO" are prefixes to "Bloor Street East".
//If no street names match the given prefix, this routine returns an empty (length 0)
//vector.
//You can choose what to return if the street prefix passed in is an empty (length 0)
//string, but your program must not crash if street_prefix is a length 0 string.
std::vector<unsigned> find_street_ids_from_partial_street_name(std::string street_prefix){
	std::vector<unsigned> result;
	//result.clear();
    
	if(street_prefix.size() == 0){
    	return result;
	}

	std::string str_pre = convert_to_low_case(street_prefix);
        //we only use first three letters as index
        int index_size = 3;
	int index1,index2,index3;
	int input_size = str_pre.size();
        
        //this is true if some input char is not a letter
        bool check_one_by_one = false;
    
	//load the data
	if(!m1_map.street_name_to_id_loaded){
    	m1_map.street_name_to_id_loaded = true;
   	 
   	 
 	//initialize the space
	for(int i = 0;i < LETTER_SIZE;i++){
    	std::vector<std::vector<std::vector<unsigned>>> second_letter;
    	for(int j = 0;j < LETTER_SIZE;j++){
        	std::vector<std::vector<unsigned>> third_letter;

        	for(int k = 0;k<LETTER_SIZE;k++){
            	std::vector<unsigned> a;
            	third_letter.push_back(a);
        	}
        	second_letter.push_back(third_letter);
    	}
    	m1_map.street_name_to_id.push_back(second_letter);

	}
 
   	 
    	for(int street_Idx = 0;street_Idx < getNumStreets();street_Idx ++){
        	std::string n = getStreetName(street_Idx);
        	//convert all name to low case
        	std::string street_name = convert_to_low_case(n);
        	//get the first three letters
                index1 = street_name[0] - 'a';
        	index2 = street_name[1] - 'a';
        	index3 = street_name[2] - 'a';
                
                //case for prefix are not letters
                //place them in 'x', which rarely shows in street name
                if(street_name[0]>'z'||street_name[0]<'a')
                    index1 = 'x'-'a'; 
                if(street_name[1]>'z'||street_name[1]<'a')
                    index2 = 'x'-'a';
                if(street_name[2]>'z'||street_name[2]<'a')
                    index3 = 'x'-'a';

        	m1_map.street_name_to_id[index1][index2][index3].push_back(street_Idx);
    	}
	}
    

    
//corner cases for few input elements   
	index1 = str_pre[0]-'a';
	if (str_pre[0]>'z'||str_pre[0]<'a'){
            check_one_by_one = true;
            index1 = 'x'-'a'; //the place where stores other letters
	}
   // if(m1_map.street_name_to_id[index1].size() == 0)return result;
    
	if(input_size == 1){
    	for(int i = 0;i<LETTER_SIZE;i++){
        	for(int j =0 ;j<LETTER_SIZE;j++){

            	for(unsigned k = 0;k<m1_map.street_name_to_id[index1][i][j].size();k++){
                    if(!check_one_by_one)
                        result.push_back(m1_map.street_name_to_id[index1][i][j][k]);
                    //in this case, we need to check one by one
                    else{
                         //the road name we are going to compare
                        std::string test_name = convert_to_low_case(getStreetName(
                    m1_map.street_name_to_id[index1][i][j][k]) );
                       
                        //if the letter matches, add to the result
                        if(test_name[0] == str_pre[0])
                            result.push_back(m1_map.street_name_to_id[index1][i][j][k]);
                    }
            	}
       	 
        	}
    	}
    	return result;
	}
    
	index2 = str_pre[1]-'a';
	if(str_pre[1]>'z'||str_pre[1]<'a'){
            check_one_by_one = true;
            index2 = 'x'-'a';
    	}
    
	//if(m1_map.street_name_to_id[index1][index2].size() == 0) return result;
	if(input_size == 2){

    	for(int i=0; i<LETTER_SIZE;i++){

        	for(unsigned j =0;j<m1_map.street_name_to_id[index1][index2][i].size();j++){
                    if(!check_one_by_one)
                        result.push_back(m1_map.street_name_to_id[index1][index2][i][j]);
                    else{
                        std::string test_name = convert_to_low_case(getStreetName(
                    m1_map.street_name_to_id[index1][index2][i][j]) );
                        if(test_name[0] == str_pre[0] && test_name[1] ==str_pre[1])
                            result.push_back(m1_map.street_name_to_id[index1][index2][i][j]);
                    }    
                }

       	 
    	}
    	return result;
	}
    
   //general case, input size >=3

	index3 = str_pre[2]-'a';
	if (str_pre[2]>'z'||str_pre[2]<'a'){
            index3 = 'x'-'a';
            check_one_by_one = true;
	}
	if(m1_map.street_name_to_id[index1][index2][index3].size() == 0 ) return result;
 
	if(input_size == 3) {
            if(!check_one_by_one)
                return result = m1_map.street_name_to_id[index1][index2][index3];
            else{
                for(unsigned i = 0; i<m1_map.street_name_to_id[index1][index2][index3].size();i++){
                    std::string test_name = convert_to_low_case(getStreetName(
                 m1_map.street_name_to_id[index1][index2][index3][i]) );
                    if( test_name.compare(0,input_size,str_pre) == 0)
                        result.push_back(m1_map.street_name_to_id[index1][index2][index3][i]);
                }
                return result;
            }
            
        }
        
	std::string input_remain;
	unsigned vector_size = m1_map.street_name_to_id[index1][index2][index3].size();
	input_remain = str_pre.substr(index_size,input_size -index_size);
        
	for(unsigned i = 0;i<vector_size;i++){
            std::string test_name = convert_to_low_case(getStreetName(
           	m1_map.street_name_to_id[index1][index2][index3][i]));
        
        //only check the sub string matches or not 
            if(!check_one_by_one){ 
             if(test_name.compare(3,input_size-3,input_remain) == 0){
        	result.push_back(
                	m1_map.street_name_to_id[index1][index2][index3][i]);
            }
         }else{
            if(test_name.compare(0,input_size,str_pre) == 0)
                result.push_back(
                        m1_map.street_name_to_id[index1][index2][index3][i]);
            }
        
	}
	return result;
}



