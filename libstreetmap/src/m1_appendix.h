/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m1_appendix.h
 * Author: yaoyinji
 *
 * Created on March 9, 2019, 11:30 AM
 */


#ifndef M1_APPENDIX_H
#define M1_APPENDIX_H
//constants will be used in this .cpp
extern int LETTER_SIZE;

#include "m3_appendix.h"

/*
 Naming: a_b_c
 * a:elements of vector
 * b:relation between element and item
 * c:all items and related to element
 */

//this class contains some structure that is useful for following functions
class map_elements{
public:
    
	//all data should be reset whenever declared or deleted
    map_elements(){
    	intersection_adjacent_segments.clear();
    	segment_adjacent_intersections.clear();
    	street_include_intersections.clear();
    	street_include_segments.clear();
    	segment_travel_time_chart.clear();
    	street_name_to_id.clear();
        path_finding_intersections.clear();
    	street_include_intersections_unique = false;
    	street_name_to_id_loaded = false;
        street_include_segments_ordered = false;
    }

    ~map_elements(){
    	intersection_adjacent_segments.clear();
    	segment_adjacent_intersections.clear();
    	street_include_intersections.clear();
    	street_include_segments.clear();
    	segment_travel_time_chart.clear();
    	street_name_to_id.clear();
        path_finding_intersections.clear();
    	street_include_intersections_unique = false;
    	street_name_to_id_loaded = false;
        street_include_segments_ordered = false;
    }
    
    void clear(){
    	intersection_adjacent_segments.clear();
    	segment_adjacent_intersections.clear();
    	street_include_intersections.clear();
    	street_include_segments.clear();
    	segment_travel_time_chart.clear();
    	street_name_to_id.clear();
        path_finding_intersections.clear();
    	street_include_intersections_unique = false;
    	street_name_to_id_loaded = false; 
        street_include_segments_ordered = false;
    }
        
	//INTERSECTION PART //
	//store all segments for each intersection
    std::vector<std::vector<unsigned>> intersection_adjacent_segments;
    
	//SEGMENTS PART//
	//store all intersections for all segment
    std::vector<std::vector<unsigned>> segment_adjacent_intersections;
    
	//STREET PART//
	//store all intersections in a street
    std::vector<std::vector<unsigned>> street_include_intersections;

	//store all segments in a street
	//used by calculating street length
    std::vector<std::vector<unsigned>> street_include_segments;

	//store all the segment travel time
    std::vector<double> segment_travel_time_chart;

	//store the first three letters of street name
    std::vector<std::vector<std::vector<std::vector<unsigned>>>> street_name_to_id;
    
    //this structure is used in m3 find_path.
    //it build the relation of Node and each intersection
    std::vector<Node> path_finding_intersections;
    
//For performance of load_map function, some of process will be done
//when first time calling corresponding functions, these bool variables
//are used to show process has finished    
 
	//decide need or not to check uniqueness
	bool street_include_intersections_unique;
    
	//decide need or not to load street name
	bool street_name_to_id_loaded;
    
        bool street_include_segments_ordered;
        
/////////////////////////////
//Graphics Related variable//
/////////////////////////////
    
      
};

//total amount
extern unsigned NumIntersections,NumStreetSegments;

//global data structure
extern map_elements m1_map;


#endif /* M1_APPENDIX_H */

