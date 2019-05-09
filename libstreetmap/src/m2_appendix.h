/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2_appendix.h
 * Author: quyichen
 *
 * Created on February 16, 2019, 2:14 PM
 */

#ifndef M2_APPENDIX_H
#define M2_APPENDIX_H

#include "m1.h"
#include "m1_appendix.h"
#include "StreetsDatabaseAPI.h"
#include "graphics.hpp"
#include "application.hpp"
#include "math.h"

typedef double ZOOM_LEVEL;


//structs and classes
struct intersections{
    std::string name;
    LatLon position;
};

//Contains information used for drawing streets
struct street_graph_info{
    int width;
    std::string color;
    std::string type;
    ZOOM_LEVEL draw_level;
};

struct canvas_size{
    //in real world coordinates
    double lon_min;
    double lat_min;
    double lon_max;
    double lat_max;
    double lat_avg;
    double lon_avg;
    double lon_total;
    double lat_total;
    //in canvas coordinate
    double x_min;
    double y_min;
    double x_max;
    double y_max;
};

//contains information such as what things to draw in the current screen size
struct canvas_status{
   //show the detail of POI when clicking it 
    int draw_POI_Idx;
    bool draw_closeset_POI_name;
    
    unsigned parking_idx;
    bool draw_closest_parking;
    
    //Intersections used for path finding
    IntersectionIndex path_from,path_to;
    
    int intersections_choosed=0;
    
    int intersections_typed=0;
    
   //street id that matches the input 
    std::vector<unsigned> partial_name_street_Idx;
    
    //intersections that match the two streets
    std::vector<unsigned> intersection_Idx_between_streets;
    
    
    bool draw_segment_speed_limit;
    
    unsigned speed_limit_segment_Idx;
    
    ZOOM_LEVEL zoom;
    
    int max_label_show_name = 6;

    //whether showing two entry for user inputs
    bool two_input_flag = true;
    
    //following four flags are used to hide elements from the map
    bool draw_fuel_station_flag = false;

    bool draw_food_flag = false;

    bool draw_natural_feature_flag = true;

    bool draw_name_flag = false;
    
    bool draw_intersection_flag = false;
    
    bool draw_path=false;
    
    IntersectionIndex clicked_intersection ;
    
    //flag used to check if we now want to get the intersection for route finding
    bool start_find_path = false;
};

//contains user inputs 
struct entry_input{
    std::string down_entry_input;
    std::string up_entry_input;
};

//this class contains structures used for drawing 
class graphic_elements{
public:
    
    graphic_elements(){
        matching_name.clear();
        street_graph_info_list.clear();
        intersection_list.clear();
        street_type_chart.clear();
        POI_list.clear();
        parking_POI_Idx.clear();
        feature_list.clear();
        fuel_info.clear();
        status_reset();
    }
    
    ~graphic_elements(){
        matching_name.clear();
        street_graph_info_list.clear();
        intersection_list.clear();
        street_type_chart.clear();
        POI_list.clear();
        parking_POI_Idx.clear();
        feature_list.clear();
        fuel_info.clear();
        status_reset();
    }
    
    void clear(){
        matching_name.clear();
        street_graph_info_list.clear();
        intersection_list.clear();
        street_type_chart.clear();
        POI_list.clear();
        parking_POI_Idx.clear();
        feature_list.clear();
        fuel_info.clear();
        status_reset();
    }
    
    //turn penalty
    double left_penalty=0;
    double right_penalty=0;
    
    //name matched for single search
    std::vector<std::string> matching_name;
    
    //index to record location of scroll, will be reset everytime after search
    int scroll_index =0;

    //create a global variable contains the size information
    canvas_size main_size;

    //create a vector contains streets graphing information
    std::vector<street_graph_info> street_graph_info_list;

    //contains status(affected by keyboard,mouse..)
    canvas_status main_status;

    //create a vector contains intersection position and name information
    std::vector<intersections> intersection_list;

    //this map contains street type when OSMID act as the key
    std::map<OSMID,std::string> street_type_chart;

    //this map contains POI amenity type with key OSMID
    std::map<OSMID,std::string> POI_list;
    
    //contains all POIs that are parking lots
    std::vector<unsigned> parking_POI_Idx;
    
    std::map<double,FeatureIndex> feature_list;
    
    std::vector<std::pair<POIIndex,std::string>> fuel_info;
    
    //contains the path found by our map
    std::vector<unsigned> path_founded;
    
    //contains what user has input into the system
    entry_input main_input;
    
    //contains the area of each feature
    std::vector<double> feature_area_list;
    
void    status_reset(){
    main_status.two_input_flag = true;

    main_status.draw_fuel_station_flag = false;

    main_status.draw_food_flag = false;

    main_status.draw_natural_feature_flag = true;

    main_status.draw_name_flag = false;
    
    main_status.draw_intersection_flag = false;
    
    main_status.start_find_path = false;
    
    main_status.partial_name_street_Idx.clear();

    main_status.intersection_Idx_between_streets.clear();
    
    main_status.draw_path = false;
    }
};
//variables
extern std::string current_map_data;
extern graphic_elements m2_map;
extern double main_zoom_level[];
//CONSTANT
#define PI 3.14159265
#define RAD_TO_DEG 180/PI

#define UNUSED(x) (void)(x)

#define zoom_level_one 1.0108
#define zoom_level_two 0.6065
#define zoom_level_three 0.3640
#define zoom_level_four 0.2184
#define zoom_level_five 0.1310
#define zoom_level_six 0.07860
#define zoom_level_seven 0.04716
#define zoom_level_eight 0.02911
#define zoom_level_nine 0.01788
#define zoom_level_ten 0.01073
#define zoom_level_eleven 0.00643
#define zoom_level_twelve 0.00386
#define zoom_level_thirteen 0.00231

#define curve_tolerance 0.0001

#define radius_POI 0.00001

////////////////////
//load information//
////////////////////


//this function loads the area of each feature
void load_feature_area();

//this function load the POI amenity type into the POI_list
void load_OSM_POI_list();

//this function insert the intersection data in to the intersection_list
//and then find the left corner and right corner when traversing 
//to decide the size of the map
void load_intersection();

//this function load the OSMWay data and insert the corresponding OSMID and Street type into 
//"street_type_chart" for drawing function to decide which color it would use
void load_OSMWay();

//this function load the street type according to the OSM data stored in the
//street_type_chart
void load_street_graph_info_list();

//load POIs that have parking type
void load_parking();

//load feature from large to small area
void load_feature();
////////////////////
//Graphics///////////
////////////////////

void draw_main_canvas(ezgl::renderer &canvas_renderer);

//Draw a single circle with LatLon Coordinates
void draw_single_point(ezgl::renderer &canvas_renderer, LatLon position,
                     ezgl::color color, double radius_x,double radius_y);

void draw_POI(ezgl::renderer &canvas_renderer,POIIndex POI_Idx,ezgl::color color,
                bool show_name,int font_size);

void draw_intersection(ezgl::renderer &canvas_renderer,IntersectionIndex inter_idx);

void draw_intersection_color(ezgl::renderer &canvas_renderer,IntersectionIndex inter_idx,ezgl::color);

void draw_gas_station(ezgl::renderer &canvas_renderer,POIIndex POI_Idx,bool draw_flag);

void draw_food_location(ezgl::renderer &canvas_renderer,POIIndex POI_Idx,bool draw_flag);

void draw_parking_location(ezgl::renderer &canvas_renderer,POIIndex POI_Idx);

//draw the name of a street at the location of its first segment
void draw_street_name(ezgl::renderer &canvas_renderer,unsigned street_idx,
                    ezgl::color color,int font_size,ezgl::rectangle screen);

//draw the name of a single segment
void draw_segment_name(ezgl::renderer &canvas_renderer,unsigned segment_idx,
                        ezgl::color color,int font_size);

//draw a street according to a street id
void draw_street(ezgl::renderer &canvas_renderer,unsigned street_idx
                    ,ezgl::color color,int width,ezgl::rectangle screen);

//draw arrow which indicates the direction
void draw_direction(ezgl::renderer &canvas_renderer,LatLon from,LatLon to
                                        ,ezgl::color color,double radius);

//draw a curved segment according to a street segment id
void draw_full_segment(ezgl::renderer &canvas_renderer,unsigned street_segment_id,
                            ezgl::color color,int width,ezgl::rectangle screen);    

void draw_line(ezgl::renderer &canvas_renderer,LatLon from,LatLon to,
                ezgl::color color, int width);

void draw_feature(ezgl::renderer &canvas_renderer, FeatureIndex featureIdx,
                    ezgl::color color,ezgl::rectangle screen);

///////////////////
//UI part///////////
///////////////////

//add buttons and link them with callback function
void initial_setup(ezgl::application *application);

//left:show the information of the closest point of interest
//right:create a dialog to show the street name and speed limit of the closest street
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y);

//refresh the status of scroll bar
void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y);


//show the street name(include partial matched ones)
//when there are two entry, it shows the intersection
void act_on_search_button(GtkWidget *widgets,ezgl::application *application);

//show natural features
void act_on_show_feature_button(GtkWidget *widgets,ezgl::application *application);

//show street names
void act_on_show_name_button(GtkWidget *widgets,ezgl::application *application);

//show fuel station
void act_on_show_fuel_button(GtkWidget *widgets,ezgl::application *application);

//show restaurants
void act_on_show_food_button(GtkWidget *widgets,ezgl::application *application);

//display two entry for searching intersection
void act_on_dual_search(GtkWidget *widgets,ezgl::application *application);

//show the closest parking from a POI
void act_on_find_parking(GtkWidget *widgets,ezgl::application *application);

//reload the map
void act_on_reload(GtkWidget *widgets,ezgl::application *application);

//help window
void act_on_pop_help(GtkWidget *widgets,ezgl::application *application);

//help window
void act_on_show_position(GtkWidget *widgets,ezgl::application *application);

//adjust the camera for focusing
//this can be used after searching street or intersection
//also allocation the parking position after clicking find parking
gboolean act_on_allocation(GtkWidget *, gpointer data);

//actions to destroy dialog
//reset the flag of draw speed limit segment
void response_dialog_destroy_dialog(GtkDialog *dialog, gint response_id, gpointer user_data);

////////////////////
//functionality/////
////////////////////

//calculate the angle of a segment, return in 360 degrees
//used for draw direction of a single way street
double angle_of_segment(ezgl::point2d point_from,ezgl::point2d point_to);

//LUT for feature color
ezgl::color feature_color(FeatureType feature_type);

//convert LatLon to point2d
ezgl::point2d latlon_to_xy(LatLon point_latlon);

//convert point2d to LatLon
LatLon xy_to_latlon(ezgl::point2d point_xy);

//operator= for latlon
bool latlon_is_same(LatLon lhs,LatLon rhs);

//find the loaction of middle point of two points
ezgl::point2d middle_point_xy(ezgl::point2d p1,ezgl::point2d p2);

//calculate the distance between two points by latlon 
double distance_between_latlon(LatLon lhs,LatLon rhs);

//calculate the tangent of two points in degree
//range is from 75 to -105
double tangent_two_points_degree(ezgl::point2d p1,ezgl::point2d p2);

//if the feature is a line, it returns its length
//reference for calculating polygon:
//https://www.wikihow.com/Calculate-the-Area-of-a-Polygon
double feature_area(FeatureIndex feature_Idx);

//get the size of rectangle by its diagonal
double rectangle_length(ezgl::rectangle rectangle);

//returns true if the position given is inside the screen size
bool position_in_screen_flag(ezgl::rectangle current_screen,LatLon geo_position);

//Return true if the street is straight or curvature degree is 
//relatively low. 
//First connect a line between two terminal points, and
//use the distance between curve point and line to judge wether the street is 
//cureved enough
bool segment_nearly_straight(StreetSegmentIndex segment_Idx);

//convert current zoom_level into an integer,which is the index of main_zoom_level
int find_zoom_level(double zoom_factor);

//pass in a set of street id, and returns street name without duplication
std::vector<std::string> remove_duplicate_name_from_street_Idx(std::vector<unsigned> street_Idx_list);

//return pairs of street ids which have intersections, without duplication in street name
std::vector<std::pair<unsigned,unsigned> >possible_intersection_of_two_streets_Idx(
            std::vector<unsigned> street_Idx_list_1, std::vector<unsigned> street_Idx_list_2);

//return pairs of street names which have intersections, without duplication
std::vector<std::pair<std::string,std::string>> possible_intersection_of_two_streets_name(
        std::vector<unsigned> street_Idx_list_1, std::vector<unsigned> street_Idx_list_2); 

//a new camera focus on found intersection
ezgl::rectangle create_new_world_intersection(ezgl::point2d centre_point);

ezgl::rectangle create_new_world_from_and_to(std::vector<unsigned> segment_list);

//a new camera focus on found street
//make sure all segments are included
ezgl::rectangle creat_new_world_street(std::vector<unsigned> street_list);

//find the closest parking from given POI
unsigned find_closest_parking_of_POI(POIIndex POI_Idx);

//find the closest segment from given point (needs improve)
unsigned find_closest_street_segment(ezgl::point2d my_position);

std::pair<double,POIIndex> find_closest_fuel_station(ezgl::point2d my_position);

//calculate the distance from a point to a straight line
double distance_point_to_line(ezgl::point2d line_from,ezgl::point2d line_to,
                    ezgl::point2d point);
#endif /* M2_APPENDIX_H */
