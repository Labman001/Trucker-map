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
//Draws the map which has been loaded with load_map().
//Your main() program should do the same.

#include "m3.h"
#include "m3_appendix.h"
#include "m1.h"
#include "m1_appendix.h"
#include "m2.h"
#include "m2_appendix.h"
#include "OSMDatabaseAPI.h"
#include "point.hpp"
#include "graphics.hpp"
#include "application.hpp"
#include <algorithm>
#include "camera.hpp"

#define CORE_NUMBER 4
std::string current_map_data;

//used to return current screen zoom level 
double main_zoom_level[] = {zoom_level_one,
zoom_level_two,zoom_level_three,zoom_level_four,
zoom_level_five,zoom_level_six,zoom_level_seven,
zoom_level_eight,zoom_level_nine,zoom_level_ten,
zoom_level_eleven,zoom_level_twelve,zoom_level_thirteen};

//////////////////
//Gobal Variable//
//////////////////
graphic_elements m2_map;

////////////////
////Load Map////
////////////////

//this function loads the area of each feature
void load_feature_area(){
    for (int i=0;i<getNumFeatures();i++){
        m2_map.feature_area_list.push_back(feature_area(i));
    }
}

//this function load the POI amenity type into the POI_list
void load_OSM_POI_list(){
    std::map<OSMID,POIIndex> OSM_to_POI;
    for(int i=0;i<getNumPointsOfInterest();i++){
        OSM_to_POI.insert(std::make_pair(getPointOfInterestOSMNodeID(i),i));
        std::pair<POIIndex,std::string> a;
        //initialize
        m2_map.fuel_info.push_back(a);
    }

    
    for (unsigned i=0;i<getNumberOfNodes();i++){
        for (unsigned j=0;j<getTagCount(getNodeByIndex(i));j++){
            if (getTagPair(getNodeByIndex(i),j).first=="amenity"){
               m2_map.POI_list.insert(std::make_pair((*(getNodeByIndex(i))).id()
               ,getTagPair(getNodeByIndex(i),j).second));  
               if (getTagPair(getNodeByIndex(i),j).second=="fuel"){
                   //get corresponding POIIndex
               std::map<OSMID,POIIndex> ::iterator it;
               it = OSM_to_POI.find( (*(getNodeByIndex(i))).id() );
               if(it != OSM_to_POI.end()){
                POIIndex POI_Idx =it->second;
                std::string info;
                for(unsigned k = 0;k<getTagCount(getNodeByIndex(i));k++){
                    if(getTagPair(getNodeByIndex(i),k).first == "amenity"){
                        continue;
                    }
                    info = info+getTagPair(getNodeByIndex(i),k).first;
                    info.push_back('\t');
                    info = info+getTagPair(getNodeByIndex(i),k).second;
                    info.push_back('\n');
                }
                m2_map.fuel_info.at(POI_Idx) = std::make_pair(POI_Idx,info);
               }
               }
            }
        }
    }
}

//this function insert the intersection data in to the intersection_list
//and then find the left corner and right corner when traversing 
//to decide the size of the map
void load_intersection(){
    m2_map.intersection_list.resize(getNumIntersections());
    //initialize the max and min position with the first intersection, will modify later
    m2_map.main_size.lon_max=getIntersectionPosition(0).lon();
    m2_map.main_size.lat_max=getIntersectionPosition(0).lat();
    m2_map.main_size.lon_min=getIntersectionPosition(0).lon();
    m2_map.main_size.lat_min=getIntersectionPosition(0).lat();
    //first create a vector contains all intersections 
    for (int i=0;i<getNumIntersections();i++){
        m2_map.intersection_list[i].position = getIntersectionPosition(i);
        m2_map.intersection_list[i].name = getIntersectionName(i);
        //find the max and min position
        if (m2_map.main_size.lat_max<m2_map.intersection_list[i].position.lat())
            m2_map.main_size.lat_max=m2_map.intersection_list[i].position.lat();
        
        if (m2_map.main_size.lon_max<m2_map.intersection_list[i].position.lon())
            m2_map.main_size.lon_max=m2_map.intersection_list[i].position.lon();
        
        if (m2_map.main_size.lat_min>m2_map.intersection_list[i].position.lat())
            m2_map.main_size.lat_min=m2_map.intersection_list[i].position.lat();
        
        if (m2_map.main_size.lon_min>m2_map.intersection_list[i].position.lon())
            m2_map.main_size.lon_min=m2_map.intersection_list[i].position.lon();
        m2_map.main_size.lat_total+=m2_map.intersection_list[i].position.lat();
        m2_map.main_size.lon_total+=m2_map.intersection_list[i].position.lon();
    } 
    m2_map.main_size.lat_avg = (m2_map.main_size.lat_max+m2_map.main_size.lat_min)/2;
    //m2_map.main_size.lon_avg = m2_map.main_size.lon_total/getNumIntersections();
    //convert lat/lon max/min into xy
    m2_map.main_size.y_max = m2_map.main_size.lat_max;
    m2_map.main_size.y_min = m2_map.main_size.lat_min;
    m2_map.main_size.x_min = m2_map.main_size.lon_min*cos(DEG_TO_RAD*m2_map.main_size.lat_avg);
    m2_map.main_size.x_max = m2_map.main_size.lon_max*cos(DEG_TO_RAD*m2_map.main_size.lat_avg);
}

//this function load the OSMWay data and insert the corresponding OSMID and Street type into 
//"street_type_chart" for drawing function to decide which color it would use
void load_OSMWay(){
    for (unsigned i=0;i<getNumberOfWays();i++){    
            if (getTagCount(getWayByIndex(i))>1){
                if ((getTagPair(getWayByIndex(i),1).first=="highway")){//only pick out the motorway here
                    //starts type chart insertion
                    if ((getTagPair(getWayByIndex(i),1).second=="motorway")
                            || (getTagPair(getWayByIndex(i),1).second=="motorway_link"))
                    m2_map.street_type_chart.insert(std::make_pair((*(getWayByIndex(i))).id(),"motorway"));
                     if ((getTagPair(getWayByIndex(i),1).second=="trunk")||
                        (getTagPair(getWayByIndex(i),1).second=="trunk_link"))
                    m2_map.street_type_chart.insert(std::make_pair((*(getWayByIndex(i))).id(),"trunk"));
                    if ((getTagPair(getWayByIndex(i),1).second=="primary")||
                        (getTagPair(getWayByIndex(i),1).second=="primary_link"))
                    m2_map.street_type_chart.insert(std::make_pair((*(getWayByIndex(i))).id(),"primary"));
                    if ((getTagPair(getWayByIndex(i),1).second=="secondary")||
                        (getTagPair(getWayByIndex(i),1).second=="secondary_link"))
                    m2_map.street_type_chart.insert(std::make_pair((*(getWayByIndex(i))).id(),"secondary"));
                    if ((getTagPair(getWayByIndex(i),1).second=="tertiary")||
                        (getTagPair(getWayByIndex(i),1).second=="tertiary_link"))
                    m2_map.street_type_chart.insert(std::make_pair((*(getWayByIndex(i))).id(),"tertiary"));
                }
            }
    }  
}

//this function load the street type according to the OSM data stored in the
//street_type_chart
void load_street_graph_info_list(){
    m2_map.street_graph_info_list.resize(getNumStreets());
    for (int i=0;i<getNumStreets();i++){
        const OSMID temp_id=getInfoStreetSegment(m1_map.street_include_segments[i][0]).wayOSMID;
        //first we load the color of a street 
        if ((m2_map.street_type_chart.find(temp_id)->second=="motorway")){
           m2_map.street_graph_info_list[i].color="DARKORANGE";
           m2_map.street_graph_info_list[i].width=3.5;
        }
        else if ((m2_map.street_type_chart.find(temp_id)->second=="trunk")){
           m2_map.street_graph_info_list[i].color="DARKPINK";
           m2_map.street_graph_info_list[i].width=3;
        }
        else if ((m2_map.street_type_chart.find(temp_id)->second=="primary")){
           m2_map.street_graph_info_list[i].color="ORANGE";
           m2_map.street_graph_info_list[i].width=2.7;
        }
        else if ((m2_map.street_type_chart.find(temp_id)->second=="secondary")){
           m2_map.street_graph_info_list[i].color="LIGHTYELLOW";
           m2_map.street_graph_info_list[i].width=2.5;
        }
        else if ((m2_map.street_type_chart.find(temp_id)->second=="tertiary")){
           m2_map.street_graph_info_list[i].color="DARKGREY";
           m2_map.street_graph_info_list[i].width=2.5;
        } 
        else{
            m2_map.street_graph_info_list[i].color="GREY";
            m2_map.street_graph_info_list[i].width=2.5;
        }
        //then we assign the level to draw based on their length
        if (find_street_length(i)>7500){
            m2_map.street_graph_info_list[i].draw_level=0;
        }
        else if ((find_street_length(i)<=7500)&&(find_street_length(i)>3750)){
            m2_map.street_graph_info_list[i].draw_level=1;
        }
        else if ((find_street_length(i)<=3750)&&(find_street_length(i)>2000)){
            m2_map.street_graph_info_list[i].draw_level=2;
        }
        else if ((find_street_length(i)<=2000)&&(find_street_length(i)>1000)){
            m2_map.street_graph_info_list[i].draw_level=3;
        }
        else if ((find_street_length(i)<=1000)&&(find_street_length(i)>500)){
            m2_map.street_graph_info_list[i].draw_level=4;
        }
        else if ((find_street_length(i)<=500)&&(find_street_length(i)>250)){
            m2_map.street_graph_info_list[i].draw_level=5;
        }
        else if ((find_street_length(i)<=250)&&(find_street_length(i)>125)){
            m2_map.street_graph_info_list[i].draw_level=6;
        }
        else if (find_street_length(i)<=125){
            m2_map.street_graph_info_list[i].draw_level=7;
        }
    }
}

//load POIs that have parking type
void load_parking(){
    for(int i=0;i<getNumPointsOfInterest();i++){
        if(getPointOfInterestType(i) == "parking"){
            m2_map.parking_POI_Idx.push_back(i);
        }
    }
}

//with the sequence of 
void load_feature(){
    for(int i =0;i<getNumFeatures();i++){
        //order from large area to small ones
        std::pair<double,FeatureIndex> index__area(1000/feature_area(i),i);
        m2_map.feature_list.insert(index__area);
    }
}

///////////////////
//Graphics/////////
///////////////////
void draw_main_canvas(ezgl::renderer &canvas_renderer){
    ezgl::rectangle current_size=canvas_renderer.get_visible_world();
    ezgl::rectangle initial_size({m2_map.main_size.x_min,m2_map.main_size.y_min},{m2_map.main_size.x_max,m2_map.main_size.y_max});
    double zoom_factor=rectangle_length(current_size)/rectangle_length(initial_size);
    m2_map.main_status.zoom = find_zoom_level(zoom_factor);
    canvas_renderer.draw_rectangle({m2_map.main_size.x_min,m2_map.main_size.y_min},{m2_map.main_size.x_max,m2_map.main_size.y_max});    
    
    //draw all features
    std::map<double,FeatureIndex>::iterator it;
    for (it=m2_map.feature_list.begin(); it!=m2_map.feature_list.end(); ++it) {
    //for (unsigned feature_Idx=0;feature_Idx<getNumFeatures();feature_Idx++){
       FeatureIndex feature_Idx = it->second;
        if (getFeatureType(feature_Idx)==Island){
            draw_feature(canvas_renderer,feature_Idx,feature_color( getFeatureType(feature_Idx)),current_size);
        }
        else if ((getFeatureType(feature_Idx)==Building)&&(zoom_factor<zoom_level_four)){
            draw_feature(canvas_renderer,feature_Idx,feature_color( getFeatureType(feature_Idx)),current_size );
        }
        else if (getFeatureType(feature_Idx)==Lake){
            draw_feature(canvas_renderer,feature_Idx,feature_color( getFeatureType(feature_Idx)) ,current_size);
        }
        else if (m2_map.main_status.draw_natural_feature_flag){
            if (m2_map.feature_area_list[feature_Idx]>=0.0003){//if the feature is big enough, always draw it
                draw_feature(canvas_renderer,feature_Idx,feature_color( getFeatureType(feature_Idx)),current_size );
            }
            else if ((m2_map.feature_area_list[feature_Idx]<0.0003)&&(m2_map.feature_area_list[feature_Idx]>0.0001)){
                if (m2_map.main_status.zoom>=2)
                    draw_feature(canvas_renderer,feature_Idx,feature_color( getFeatureType(feature_Idx) ),current_size);
            }
            else if ((m2_map.feature_area_list[feature_Idx]<0.0001)&&(m2_map.feature_area_list[feature_Idx]>0.00005)){
                if (m2_map.main_status.zoom>=3)
                    draw_feature(canvas_renderer,feature_Idx,feature_color( getFeatureType(feature_Idx) ),current_size);
            }
            else if ((m2_map.feature_area_list[feature_Idx]<0.00005)){
                if (m2_map.main_status.zoom>=5)
                    draw_feature(canvas_renderer,feature_Idx,feature_color( getFeatureType(feature_Idx) ),current_size);
            }
        }
    }
        
    //draw streets
    for (unsigned i=0;i<(unsigned)getNumStreets();i++){
        //first we check if a street has been searched
        ezgl::color street_color(184, 203, 211,255);
        if(!m2_map.main_status.partial_name_street_Idx.empty() ){
            std::vector<unsigned>::iterator matched_Idx = std::find(m2_map.main_status.partial_name_street_Idx.begin(),
                 m2_map.main_status.partial_name_street_Idx.end(),i);
            //find a matched index
            if(matched_Idx != m2_map.main_status.partial_name_street_Idx.end()){
                draw_street(canvas_renderer,i,ezgl::RED,4,current_size);
                continue;
            }
        }   
        //then draw other streets
        if (m2_map.street_graph_info_list[i].draw_level<=m2_map.main_status.zoom){
            if (m2_map.street_graph_info_list[i].color=="DARKORANGE")
                draw_street(canvas_renderer,i,ezgl::color(243, 177, 126,255),m2_map.street_graph_info_list[i].width,current_size);
            else if (m2_map.street_graph_info_list[i].color=="DARKPINK")
                draw_street(canvas_renderer,i,ezgl::color(217, 164, 49,255),m2_map.street_graph_info_list[i].width,current_size);
            else if ((m2_map.street_graph_info_list[i].color=="ORANGE"))
                draw_street(canvas_renderer,i,ezgl::color(232, 164, 88,255),m2_map.street_graph_info_list[i].width,current_size);
            else if ((m2_map.street_graph_info_list[i].color=="LIGHTYELLOW"))
                draw_street(canvas_renderer,i,ezgl::color(232, 208, 88,255),m2_map.street_graph_info_list[i].width,current_size);
            else if ((m2_map.street_graph_info_list[i].color=="DARKGREY"))
                draw_street(canvas_renderer,i,ezgl::color(192, 189, 183,255),m2_map.street_graph_info_list[i].width,current_size);
            else 
                draw_street(canvas_renderer,i,ezgl::color(192, 189, 183,255),m2_map.street_graph_info_list[i].width,current_size); 
        }
    }
        
    //draw all Points of Interest
    for(int i=0;i<getNumPointsOfInterest();i++){
        ezgl::color POI_color(255,0,0,100);
        int POI_font_size = 15;
        if(i != m2_map.main_status.draw_POI_Idx){
            if (position_in_screen_flag(current_size,getPointOfInterestPosition(i)))
            draw_POI(canvas_renderer,i,POI_color,false,POI_font_size);         
        } else{
            draw_POI(canvas_renderer,i,POI_color,m2_map.main_status.draw_closeset_POI_name,POI_font_size);
        }
        
    }  
    
    //draw intersection between two required streets
    m2_map.main_status.intersection_Idx_between_streets.resize(m2_map.main_status.intersection_Idx_between_streets.size());
    for (unsigned i=0;i<m2_map.main_status.intersection_Idx_between_streets.size();i++){
        draw_intersection(canvas_renderer,m2_map.main_status.intersection_Idx_between_streets[i]);
        std::cout<<getIntersectionName(m2_map.main_status.intersection_Idx_between_streets[i])<<"\n";
    }
    
    //draw parking spots
    if(m2_map.main_status.draw_closest_parking){
        draw_parking_location(canvas_renderer,m2_map.main_status.parking_idx);
    }
    
    //draw segment for showing speed limit
    if(m2_map.main_status.draw_segment_speed_limit){
        draw_full_segment(canvas_renderer,m2_map.main_status.speed_limit_segment_Idx,ezgl::YELLOW,4,current_size);
    } 
    if (m2_map.main_status.draw_intersection_flag){
    //draw the selected intersection
        draw_intersection(canvas_renderer,m2_map.main_status.clicked_intersection);
        canvas_renderer.set_color(ezgl::BLUE);
        canvas_renderer.format_font("serif",ezgl::font_slant::normal,ezgl::font_weight::normal,15);
        canvas_renderer.draw_text(latlon_to_xy(getIntersectionPosition(m2_map.main_status.clicked_intersection))
        ,getIntersectionName(m2_map.main_status.clicked_intersection));
    }
    
    if (m2_map.main_status.draw_path){
         draw_single_point(canvas_renderer, getIntersectionPosition(m2_map.main_status.path_from)
                        ,ezgl::RED,0.0001,0.0001);
         draw_single_point(canvas_renderer, getIntersectionPosition(m2_map.main_status.path_to)
                        ,ezgl::GREEN,0.0001,0.0001);
    }
   
 

    
    if (m2_map.main_status.draw_path){
            LatLon from=getIntersectionPosition(m2_map.main_status.path_from);
            LatLon to=getIntersectionPosition(m2_map.main_status.path_to);
            std::cout<<distance_between_latlon(from,to)<<std::endl;
        draw_route(canvas_renderer,m2_map.path_founded,current_size);
    }
    
    //draw street names here
    if (m2_map.main_status.draw_name_flag){
        for(int i =0; i<getNumStreets();i++){
            if (m2_map.street_graph_info_list[i].draw_level<=m2_map.main_status.zoom)
                draw_street_name(canvas_renderer,i,ezgl::PURPLE,12,current_size);
        }
    }
    
    
}

//Draw a single circle with LatLon Coordinates
void draw_single_point(ezgl::renderer &canvas_renderer, LatLon position
                        ,ezgl::color color,double radius_x,double radius_y){
        ezgl::point2d point_xy = latlon_to_xy(position);
        canvas_renderer.set_color(color);
        canvas_renderer.fill_elliptic_arc(point_xy,radius_x,radius_y,0,360);
}

void draw_POI(ezgl::renderer &canvas_renderer,POIIndex POI_Idx,ezgl::color color,bool show_name_flag,int font_size){
    ezgl::point2d point_xy = latlon_to_xy(getPointOfInterestPosition(POI_Idx));
    
    canvas_renderer.set_color(color);
    canvas_renderer.fill_elliptic_arc(point_xy,radius_POI,radius_POI,0,360); 
    draw_gas_station(canvas_renderer,POI_Idx,m2_map.main_status.draw_fuel_station_flag);
    draw_food_location(canvas_renderer,POI_Idx,m2_map.main_status.draw_food_flag);
    //TEXT PART
    if(show_name_flag){
        canvas_renderer.set_color(ezgl::BLUE);
        canvas_renderer.format_font("serif",ezgl::font_slant::normal,ezgl::font_weight::normal,font_size);
        canvas_renderer.draw_text(point_xy,getPointOfInterestName(POI_Idx));
    }
}

void draw_intersection(ezgl::renderer &canvas_renderer,IntersectionIndex inter_idx){
    canvas_renderer.set_color(ezgl::LIGHT_PINK);
    canvas_renderer.fill_arc(latlon_to_xy(getIntersectionPosition(inter_idx)),0.00005,0,360);
}

void draw_intersection_color(ezgl::renderer &canvas_renderer,IntersectionIndex inter_idx,ezgl::color color){
    canvas_renderer.set_color(color);
    canvas_renderer.fill_arc(latlon_to_xy(getIntersectionPosition(inter_idx)),0.0001,0,360);
}

void draw_gas_station(ezgl::renderer &canvas_renderer,POIIndex POI_Idx,bool draw_flag){
    if ((m2_map.POI_list.find(getPointOfInterestOSMNodeID(POI_Idx))->second=="fuel")
            &&draw_flag){
        
        ezgl::point2d point_xy = latlon_to_xy(getPointOfInterestPosition(POI_Idx));
        ezgl::surface *png_surface = ezgl::renderer::load_png("libstreetmap/resources/gas_station.png");
        canvas_renderer.draw_surface(png_surface, point_xy);
        ezgl::renderer::free_surface(png_surface);
    }
}

void draw_food_location(ezgl::renderer &canvas_renderer,POIIndex POI_Idx,bool draw_flag){
    if (((m2_map.POI_list.find(getPointOfInterestOSMNodeID(POI_Idx))->second=="restaurant")||
        (m2_map.POI_list.find(getPointOfInterestOSMNodeID(POI_Idx))->second=="food_court")||
        (m2_map.POI_list.find(getPointOfInterestOSMNodeID(POI_Idx))->second=="fast_food"))
            &&draw_flag){
        ezgl::point2d point_xy = latlon_to_xy(getPointOfInterestPosition(POI_Idx));
        ezgl::surface *png_surface = ezgl::renderer::load_png("libstreetmap/resources/oriangefood.png");
        canvas_renderer.draw_surface(png_surface, point_xy);
        ezgl::renderer::free_surface(png_surface);
    }
}
void draw_parking_location(ezgl::renderer &canvas_renderer,POIIndex POI_Idx){
    ezgl::point2d point_xy = latlon_to_xy(getPointOfInterestPosition(POI_Idx));
    //can change the picture
    ezgl::surface *png_surface = ezgl::renderer::load_png("libstreetmap/resources/parking_lot.png");
    canvas_renderer.draw_surface(png_surface, point_xy);
    ezgl::renderer::free_surface(png_surface);
    std::cout<<"Parking Id"<<m2_map.main_status.parking_idx<<std::endl;
}

//draw the name of a street at the location of its first segment
void draw_street_name(ezgl::renderer &canvas_renderer,unsigned street_idx,ezgl::color color
                        ,int font_size,ezgl::rectangle screen){
    InfoStreetSegment segment_info = getInfoStreetSegment(*(m1_map.street_include_segments[street_idx].begin()));    
    //first checks if the street is within the screen range
    if ((position_in_screen_flag(screen,getIntersectionPosition(segment_info.from)))
            &&(position_in_screen_flag(screen,getIntersectionPosition(segment_info.to)))){
        if(segment_nearly_straight(*(m1_map.street_include_segments[street_idx].begin()))){
            draw_segment_name(canvas_renderer,*(m1_map.street_include_segments[street_idx].begin()),
                        color,font_size);
        }
        else{
            draw_segment_name(canvas_renderer,*(m1_map.street_include_segments[street_idx].begin()),
                        color,font_size);
        }
    }
}

//draw the name of a single segment
void draw_segment_name(ezgl::renderer &canvas_renderer,unsigned segment_idx,
                                            ezgl::color color,int font_size){
    InfoStreetSegment temp_info=getInfoStreetSegment(segment_idx);
    ezgl::point2d p1(latlon_to_xy(getIntersectionPosition(temp_info.from)));
    ezgl::point2d p2(latlon_to_xy(getIntersectionPosition(temp_info.to)));
    //if the segment is straight, just draw its name
    if (temp_info.curvePointCount==0){
        double rotate_angle = tangent_two_points_degree(p1,p2);
        canvas_renderer.set_text_rotation(rotate_angle);
        canvas_renderer.set_color(color);
        canvas_renderer.format_font("monospace",ezgl::font_slant::italic,ezgl::font_weight::bold,font_size);
        canvas_renderer.draw_text(middle_point_xy(p1,p2),getStreetName(temp_info.streetID));
        //reset text configuration to horizontal
        canvas_renderer.set_text_rotation(0);
    }
    //if the segment is curved, draw its name at the middle point
    else 
        if (temp_info.curvePointCount>0){
            int half_value = floor(temp_info.curvePointCount/2);
            ezgl::point2d middle_point(latlon_to_xy(getStreetSegmentCurvePoint(half_value,segment_idx)));
            //find the tangent using first point and curve point
            double rotate_angle = tangent_two_points_degree(p1,p2);
            canvas_renderer.set_text_rotation(rotate_angle);
            canvas_renderer.set_color(color);   
            canvas_renderer.format_font("monospace",ezgl::font_slant::italic,ezgl::font_weight::bold,font_size);             
            //locate the text on middle point
            canvas_renderer.draw_text(middle_point,getStreetName(temp_info.streetID));
            //reset text configuration to horizontal
            canvas_renderer.set_text_rotation(0);                
        }
}

//draw a street according to a street id
void draw_street(ezgl::renderer &canvas_renderer,unsigned street_id,ezgl::color color,
        int width,ezgl::rectangle screen){
    //traverse all segments of a given street id
    for (unsigned i=0;i<m1_map.street_include_segments[street_id].size();i++){
        draw_full_segment(canvas_renderer,m1_map.street_include_segments[street_id][i],color,width,screen);
    } 
}

//draw arrow which indicates the direction
void draw_direction(ezgl::renderer &canvas_renderer,LatLon from,LatLon to,ezgl::color color,double radius){
    canvas_renderer.set_color(color);
    ezgl::point2d to_point=latlon_to_xy(to); 
    ezgl::point2d from_point=latlon_to_xy(from);
    const double radius_to_extent=30;
    double angle=angle_of_segment(from_point,to_point);
    //draw an arc to act as the arrow to show direction
    canvas_renderer.fill_arc(to_point,radius,angle-15,radius_to_extent);
}

//draw a curved segment according to a street segment id
void draw_full_segment(ezgl::renderer &canvas_renderer,unsigned street_segment_id,
                            ezgl::color color,int width,ezgl::rectangle screen){   
    //create a temp variable contains the street_seg info
    InfoStreetSegment temp_segment_info=getInfoStreetSegment(street_segment_id);
    //first we check if the segment is in the screen
    if ((position_in_screen_flag(screen,getIntersectionPosition(temp_segment_info.from)))
            ||(position_in_screen_flag(screen,getIntersectionPosition(temp_segment_info.to)))){
    //the segment is straight line, just draw a line
    if (temp_segment_info.curvePointCount==0){
        if (temp_segment_info.oneWay){
            draw_direction(canvas_renderer,getIntersectionPosition(temp_segment_info.from),
                                   getIntersectionPosition(temp_segment_info.to),color,0.00015);
        }
        draw_line(canvas_renderer,getIntersectionPosition(temp_segment_info.from)
                                     ,getIntersectionPosition(temp_segment_info.to),color,width);
    }
    //the segment is curved
    else {
        //we first draw the first and last segment in the segments list
        draw_line(canvas_renderer,getIntersectionPosition(temp_segment_info.from),
                                        getStreetSegmentCurvePoint(0,street_segment_id),color,width);
        draw_line(canvas_renderer,getIntersectionPosition(temp_segment_info.to),
                                        getStreetSegmentCurvePoint(temp_segment_info.curvePointCount-1,
                                        street_segment_id),color,width);
        //then we draw the segments inside the list
        for (int i=0;i<temp_segment_info.curvePointCount-1;i++){
            draw_line(canvas_renderer, getStreetSegmentCurvePoint(i,street_segment_id),
                      getStreetSegmentCurvePoint(i+1,street_segment_id),color,width);
        }
        //check direction
        if (temp_segment_info.oneWay){
            draw_direction(canvas_renderer,getStreetSegmentCurvePoint(temp_segment_info.curvePointCount-1,street_segment_id),
                                    getIntersectionPosition(temp_segment_info.to),color,0.0001);
        }
    }  
    }
}  

void draw_line(ezgl::renderer &canvas_renderer,LatLon from,LatLon to,ezgl::color color,int width){
    canvas_renderer.set_color(color);
    canvas_renderer.set_line_width(width);
    canvas_renderer.draw_line(latlon_to_xy(from),latlon_to_xy(to));
}

void draw_feature(ezgl::renderer &canvas_renderer, FeatureIndex featureIdx
                    ,ezgl::color color,ezgl::rectangle screen){
    int point_amount = getFeaturePointCount(featureIdx);
    bool draw_this_feature=false;
    //checks if the feature has a point in the screen range
    //if not we will not draw it
    for (int i=0;i<point_amount;i++){
        if (position_in_screen_flag(screen,getFeaturePoint(i,featureIdx))){
            draw_this_feature=true;
            break;
        }
    }
    if (draw_this_feature){
        //if first and last are the same point, feature is a poly
        bool is_poly = latlon_is_same(getFeaturePoint(0,featureIdx),
                        getFeaturePoint(point_amount-1,featureIdx));      
        //if feature has multiple points and is a poly
        if(point_amount>1 && is_poly){
            std::vector<ezgl::point2d> feature_vertex;
            for(int i =0; i<point_amount;i++){
                ezgl::point2d vertex = latlon_to_xy(getFeaturePoint(i,featureIdx) );
                feature_vertex.push_back(vertex);
            }
            canvas_renderer.set_color(color);
            canvas_renderer.fill_poly(feature_vertex);
        } 
        else if(point_amount == 1){ //only draw a point
            double feature_radius =0.0005 ;
            draw_single_point(canvas_renderer, getFeaturePoint(0,featureIdx),color,
                feature_radius,feature_radius);
        } else{//draw several segments
            for(int i = 0; i<point_amount-1;i++){
                int feature_width = 3;
                draw_line(canvas_renderer, getFeaturePoint(i,featureIdx),
                    getFeaturePoint(i+1,featureIdx),color,feature_width);
            }
        }
    }
}

void draw_map(){

    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    ezgl::application application(settings);
    //we want to separate code from M1, so load graph related information here
    load_intersection();
    load_OSMWay();
    load_street_graph_info_list();
    load_OSM_POI_list();
    load_parking();
    load_feature();
    load_feature_area();
    ezgl::rectangle initial_world({m2_map.main_size.x_min,m2_map.main_size.y_min},{m2_map.main_size.x_max,m2_map.main_size.y_max});
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world);
    application.run(initial_setup, act_on_mouse_press ,act_on_mouse_move,nullptr);
}

//////////////////
//UI Part/////////
//////////////////

void initial_setup(ezgl::application *application)
{
    // Update the status bar message
    application->update_message("EZGL Application");
  
    application->create_button("SearchButton",0,act_on_search_button);
  
    application->create_button("Click Find Path", 6,act_on_show_path_find_button);
  
    application->create_button("Type Find Path", 7,act_on_type_to_show_path_button);
  
    application->create_button("Detailed Path",8,act_on_show_route_info); 
  
    application->create_button("Natural Feature", 9,act_on_show_feature_button);
  
    application->create_button("Show Street Name", 10,act_on_show_name_button);
  
    application->create_button("Show Fuel", 11,act_on_show_fuel_button);
  
    application->create_button("Show Food", 12,act_on_show_food_button);

    application->create_button("Switch Mode",-1,act_on_dual_search);
  
    application->create_button("Find parking",13,act_on_find_parking);
  
    application->create_button("Reload",14,act_on_reload);
  
    application->create_button("Help",15,act_on_pop_help);
  
  

    GtkButton* allocation = (GtkButton*) application->get_object("Find_location");
    g_signal_connect(allocation, "clicked", G_CALLBACK(act_on_allocation), application);
 
}



void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y){
    UNUSED(event);
    UNUSED(x);
    UNUSED(y);
    GtkAdjustment* suggestion_adjustment= (GtkAdjustment*) application->get_object("adjustment1");
    GtkLabel* suggestion_label = (GtkLabel*) application->get_object("name_suggestion");
    m2_map.scroll_index = gtk_adjustment_get_value(suggestion_adjustment);
    int name_size = m2_map.matching_name.size();
    std::string name_list;
    //print'\n' if empty
    if(m2_map.matching_name.empty()){
        for(int i=0;i<m2_map.main_status.max_label_show_name;i++){
            name_list.push_back('\n');
        }
    } else{
        int i;
        for(i=m2_map.scroll_index;i<std::min(name_size-i, m2_map.main_status.max_label_show_name);i++){
            name_list = name_list + m2_map.matching_name[i];
            name_list.push_back('\n');
        }
        while(i-m2_map.scroll_index<m2_map.main_status.max_label_show_name){
            name_list.push_back('\n');
            i++;
        }
    }
    gtk_label_set_text(suggestion_label,name_list.c_str());
}

void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y){      
    ezgl::point2d cursor_location(x,y);
    application->update_message("Mouse Clicked at");
    std::cout<<"Mouse Clicked at"<<x<<","<<y<<"\n\n";
    unsigned closest_POI_Idx = find_closest_point_of_interest(xy_to_latlon(cursor_location));
    unsigned closest_intersection_Idx=find_closest_intersection(xy_to_latlon(cursor_location));
    
    //mouse left clicked
    //to show POI name

    if(event->button == 1){
        if(m2_map.main_status.draw_fuel_station_flag){
            std::pair<double,POIIndex> closest_fuel_pair = find_closest_fuel_station(cursor_location);
            if(closest_fuel_pair.first <=0.0001){
                POIIndex POI_Idx = closest_fuel_pair.second;
                std::string info = m2_map.fuel_info[POI_Idx].second;
                GtkWidget *fuel_info_label;
                GObject *window;           
                GtkWidget *content_area;
                GtkWidget *fuel_info_dialog;

                fuel_info_label = gtk_label_new(info.c_str());
                window = application->get_object(application->get_main_window_id().c_str());
                fuel_info_dialog = gtk_dialog_new_with_buttons("Detailed Information",(GtkWindow*) window,GTK_DIALOG_MODAL,
                        ("OK"),GTK_RESPONSE_ACCEPT,("CANCEL"),GTK_RESPONSE_REJECT,NULL);
                g_signal_connect(GTK_DIALOG(fuel_info_dialog),"response",G_CALLBACK(response_dialog_destroy_dialog),NULL);
                content_area = gtk_dialog_get_content_area(GTK_DIALOG(fuel_info_dialog));
                gtk_container_add(GTK_CONTAINER(content_area), fuel_info_label);
                gtk_widget_show_all(fuel_info_dialog);

            }
        }else{
            if(distance_between_latlon(xy_to_latlon(cursor_location), 
              getPointOfInterestPosition(closest_POI_Idx)) <= 0.0001 ){
                m2_map.main_status.draw_closeset_POI_name = true;
                m2_map.main_status.draw_POI_Idx = closest_POI_Idx;
            } else{
                m2_map.main_status.draw_closeset_POI_name = false;
                m2_map.main_status.draw_closest_parking = false;
            }

        }
        //user want to find a path
        if (m2_map.main_status.start_find_path){
            if((distance_between_latlon(xy_to_latlon(cursor_location), 
                    getIntersectionPosition(closest_intersection_Idx)) <= 0.0002 )
                    &&(m2_map.main_status.intersections_choosed==0)){
                m2_map.main_status.path_from=closest_intersection_Idx;
                m2_map.main_status.intersections_choosed+=1;
                application->update_message("First intersection picked,continue choose new one");
            }
            else if((distance_between_latlon(xy_to_latlon(cursor_location), 
              getIntersectionPosition(closest_intersection_Idx)) <= 0.0002 )
                    &&(m2_map.main_status.intersections_choosed==1)){
                m2_map.main_status.path_to=closest_intersection_Idx;
                m2_map.main_status.intersections_choosed=0;
                //we have found two intersections
                m2_map.main_status.start_find_path=false;
                application->update_message("Both intersection picked, let's draw the path");
                m2_map.main_status.draw_path=true;
                //get penalty from user
            GtkSpinButton* left_p=(GtkSpinButton*)(application->get_object("left_penalty"));
            m2_map.left_penalty=gtk_spin_button_get_value (left_p);
            GtkSpinButton* right_p=(GtkSpinButton*)(application->get_object("right_penalty"));
            m2_map.right_penalty=gtk_spin_button_get_value (right_p);
            //call function to find path here
            m2_map.path_founded=find_path_between_intersections(m2_map.main_status.path_from,m2_map.main_status.path_to,m2_map.left_penalty,m2_map.right_penalty);
                std::cout<<"from:"<<m2_map.main_status.path_from<<"to::"<<
                            m2_map.main_status.path_to<<std::endl;
                }
            }
    }
    //show intersection
    if (event->button ==2){
        if (distance_between_latlon(xy_to_latlon(cursor_location),
                getIntersectionPosition(closest_intersection_Idx))<=0.0001){
            m2_map.main_status.clicked_intersection=closest_intersection_Idx;
            m2_map.main_status.draw_intersection_flag=true;
            std::cout<<"the intersection is "<<closest_intersection_Idx<<std::endl;
        }
        else {
            m2_map.main_status.clicked_intersection=-1;
            m2_map.main_status.draw_intersection_flag=false;
        }
    }
    //click mouse right
    else if(event->button == 3){
        unsigned segment_Idx = find_closest_street_segment(cursor_location);
        InfoStreetSegment segment_info = getInfoStreetSegment(segment_Idx);
        std::string street_name = getStreetName(segment_info.streetID);
        int limit = (int)segment_info.speedLimit;
        std::string prompt = "The speed limit of "+street_name+" is "+std::to_string(limit)+"km/h" +std::to_string(segment_Idx);
        m2_map.main_status.draw_segment_speed_limit = true;
        m2_map.main_status.speed_limit_segment_Idx = segment_Idx;
        application->refresh_drawing();
        
        //prompt dialog
        GtkWidget *speed_limit_label;
        GObject *window;           
        GtkWidget *content_area;
        GtkWidget *speed_limit_dialog;
        
        speed_limit_label = gtk_label_new(prompt.c_str());
        window = application->get_object(application->get_main_window_id().c_str());
        speed_limit_dialog = gtk_dialog_new_with_buttons("Speed Limit",(GtkWindow*) window,GTK_DIALOG_MODAL,
                ("OK"),GTK_RESPONSE_ACCEPT,("CANCEL"),GTK_RESPONSE_REJECT,NULL);
        g_signal_connect(GTK_DIALOG(speed_limit_dialog),"response",G_CALLBACK(response_dialog_destroy_dialog),NULL);
        content_area = gtk_dialog_get_content_area(GTK_DIALOG(speed_limit_dialog));
        gtk_container_add(GTK_CONTAINER(content_area), speed_limit_label);
        gtk_widget_show_all(speed_limit_dialog);
    }
    std::cout<<m2_map.main_status.intersections_choosed<<std::endl;
    if (m2_map.main_status.start_find_path)
        std::cout<<"YES"<<std::endl;
    if (!m2_map.main_status.start_find_path)
        std::cout<<"NO"<<std::endl;
    if (m2_map.main_status.draw_path)
        std::cout<<"RUA DRAW"<<std::endl;
    application->refresh_drawing();
}



void act_on_show_feature_button(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    application->update_message("Natural eature shown");
    m2_map.main_status.draw_natural_feature_flag=!m2_map.main_status.draw_natural_feature_flag;
    application->refresh_drawing();
}

void act_on_show_name_button(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    application->update_message("Street name shown");
    m2_map.main_status.draw_name_flag=!m2_map.main_status.draw_name_flag;
    application->refresh_drawing();
}

void act_on_show_fuel_button(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    application->update_message("Fuel position shown");
    m2_map.main_status.draw_fuel_station_flag=!m2_map.main_status.draw_fuel_station_flag;
    application->refresh_drawing();
}

void act_on_show_food_button(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    application->update_message("Food position shown");
    m2_map.main_status.draw_food_flag=!m2_map.main_status.draw_food_flag;
    application->refresh_drawing();
   
}

void act_on_dual_search(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    application->update_message("Dual Search");
    m2_map.main_status.two_input_flag = !m2_map.main_status.two_input_flag;
    m2_map.main_status.intersection_Idx_between_streets.clear();
    GtkEntry* text_entry_2 = (GtkEntry*) application->get_object("SearchInput_up");   
    if(!m2_map.main_status.two_input_flag) {
        gtk_widget_hide(GTK_WIDGET (text_entry_2));
        //reset the input in up entry
        gtk_entry_set_text (text_entry_2,"");
    }
    else gtk_widget_show(GTK_WIDGET(text_entry_2));  
}

//to use this, have to click a POI first
void act_on_find_parking(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    //error report
    if(!m2_map.main_status.draw_closeset_POI_name){
        GObject *window;           
        GtkWidget *content_area;
        GtkWidget *error_report_dialog; 
        GtkWidget *dialog_label;
        dialog_label = gtk_label_new("You have to click a Point of Interest first");
        window = application->get_object(application->get_main_window_id().c_str());
        error_report_dialog = gtk_dialog_new_with_buttons("Error Report",(GtkWindow*) window,GTK_DIALOG_MODAL,
                ("OK"),GTK_RESPONSE_ACCEPT,("CANCEL"),GTK_RESPONSE_REJECT,NULL);
        g_signal_connect(GTK_DIALOG(error_report_dialog),"response",G_CALLBACK(response_dialog_destroy_dialog),NULL);
        content_area = gtk_dialog_get_content_area(GTK_DIALOG(error_report_dialog));
        gtk_container_add(GTK_CONTAINER(content_area), dialog_label);
        gtk_widget_show_all(error_report_dialog);
    }else{
        m2_map.main_status.parking_idx = find_closest_parking_of_POI(m2_map.main_status.draw_POI_Idx);
        m2_map.main_status.draw_closest_parking = true;
        application->update_message("PARKING");
        application->refresh_drawing();
    }
}

void act_on_reload(GtkWidget *widgets,ezgl::application *application){
    GtkComboBoxText* text_list1=(GtkComboBoxText*)application->get_object("map_list");
    UNUSED(widgets);
    //GtkWindow* main_window=(GtkWindow*)application->get_object("MainWindow");
    std::string current_map;
    if (gtk_combo_box_text_get_active_text (text_list1)!=NULL){
        current_map=gtk_combo_box_text_get_active_text (text_list1);
    }
    if (current_map=="Toronto"){
        current_map_data="/cad2/ece297s/public/maps/toronto_canada.streets.bin";
    }
        else if(current_map== "NewYork"){
          current_map_data="/cad2/ece297s/public/maps/new-york_usa.streets.bin";
          }
        else if(current_map== "Beijing"){
          current_map_data="/cad2/ece297s/public/maps/beijing_china.streets.bin";
          }
        else if(current_map== "Cairo"){
            current_map_data="/cad2/ece297s/public/maps/cairo_egypt.streets.bin";
            }
        else if(current_map== "Cape Town"){
            current_map_data="/cad2/ece297s/public/maps/cape-town_south-africa.streets.bin";
            }
        else if(current_map== "Golden Horseshoe"){
            current_map_data="/cad2/ece297s/public/maps/golden-horseshoe_canada.streets.bin";
            }
        else if(current_map== "Hamilton"){
            current_map_data="/cad2/ece297s/public/maps/hamilton_canada.streets.bin";
            }
        else if(current_map== "Hong Kong"){
            current_map_data="/cad2/ece297s/public/maps/hong-kong_china.streets.bin";
            }
        else if(current_map== "Iceland"){
            current_map_data="/cad2/ece297s/public/maps/iceland.streets.bin";
            }
        else if(current_map== "Interlaken"){
            current_map_data="/cad2/ece297s/public/maps/interlaken_switzerland.streets.bin";
            }
        else if(current_map== "London"){
            current_map_data="/cad2/ece297s/public/maps/london_england.streets.bin";
            }
        else if(current_map== "Moscow"){
            current_map_data="/cad2/ece297s/public/maps/moscow_russia.streets.bin";
            }
        else if(current_map=="New Delhi"){
            current_map_data="/cad2/ece297s/public/maps/new-delhi_india.streets.bin";
            }
        else if(current_map== "Rio De Janeiro"){
            current_map_data="/cad2/ece297s/public/maps/rio-de-janeiro_brazil.streets.bin";
            }
        else if(current_map== "Saint Helena"){
            current_map_data="/cad2/ece297s/public/maps/saint-helena.streets.bin";
            }
        else if(current_map== "Singapore"){
            current_map_data="/cad2/ece297s/public/maps/singapore.streets.bin";
            }
        else if(current_map== "Sydney"){
            current_map_data="/cad2/ece297s/public/maps/sydney_australia.streets.bin";
            }
        else if(current_map== "Tokyo"){
            current_map_data="/cad2/ece297s/public/maps/tokyo_japan.streets.bin";
            } 
    if (gtk_combo_box_text_get_active_text (text_list1)==NULL){
        application->update_message("Please first select a map and then to reload!");
    }
    else {
        m2_map.clear();
        close_map();
        load_map(current_map_data);
        load_intersection();
        load_OSMWay();
        load_street_graph_info_list();
        load_OSM_POI_list();
        load_parking();
        load_feature();
        load_feature_area();

        ezgl::rectangle new_world({m2_map.main_size.x_min,m2_map.main_size.y_min},{m2_map.main_size.x_max,m2_map.main_size.y_max});
        
        application->get_canvas(application->get_main_canvas_id())->get_camera().reset_initial_world(new_world);
        
        application->get_canvas(application->get_main_canvas_id())->get_camera().set_world(new_world);
        
        ezgl::point2d new_min=application->get_canvas(application->get_main_canvas_id())->get_camera().world_to_screen(new_world.m_first);
        ezgl::point2d new_max=application->get_canvas(application->get_main_canvas_id())->get_camera().world_to_screen(new_world.m_second);

        ezgl::rectangle new_screen(new_min,new_max);
        application->get_canvas(application->get_main_canvas_id())->get_camera().update_m_screeen(new_screen);
       
        application->get_canvas(application->get_main_canvas_id())->redraw();    
        application->refresh_drawing();
              
    }
}



gboolean act_on_allocation(GtkWidget *, gpointer data){
    auto application = static_cast<ezgl::application *>(data);
    std::string main_canvas_id = application->get_main_canvas_id();
    auto canvas = application->get_canvas(main_canvas_id);
    if(!m2_map.main_status.intersection_Idx_between_streets.empty() && m2_map.main_status.two_input_flag){
        //choose zoom_level_six as outcome
        ezgl::point2d centre_point = latlon_to_xy(getIntersectionPosition(
                m2_map.main_status.intersection_Idx_between_streets[0]) );
        canvas->get_camera().set_world(create_new_world_intersection(centre_point));
        canvas->redraw();
        application->refresh_drawing();
    }else if(!m2_map.main_status.partial_name_street_Idx.empty()&& !m2_map.main_status.two_input_flag){
        canvas->get_camera().set_world(creat_new_world_street(m2_map.main_status.partial_name_street_Idx));
        canvas->redraw();
        application->refresh_drawing();
    } 
    if(m2_map.main_status.draw_closest_parking){
        ezgl::point2d centre_point = latlon_to_xy(getPointOfInterestPosition(
                m2_map.main_status.parking_idx));
        canvas->get_camera().set_world(create_new_world_intersection(centre_point));
        canvas->redraw();
        application->refresh_drawing();
    }
    if (m2_map.main_status.draw_path){
        std::vector<unsigned> path = {113977, 113978, 62281, 62282, 62283, 62304, 62305, 14220, 14221, 14222, 14223, 14192, 14193, 61811, 61812, 111804, 111803, 111798, 111785, 111786, 111787, 111788, 103384, 16194, 16193, 8272, 124052, 124053, 124054, 645, 123655, 9538, 9539, 39239, 39240, 39241, 39242, 39243, 143259, 39223, 39224, 39225, 140559, 129208, 82202, 82203, 82196, 82195, 82194, 101365, 82180, 140550, 140553, 140555, 140552, 9217, 28831, 111927, 28829, 28830, 111932, 102553, 124060, 127723, 11306, 11307, 111937, 111938, 111940, 23453, 23454, 127735, 127736, 127734, 644, 101355, 23484, 23485, 101358, 101359, 101360, 28468, 28469, 28478, 28472, 111962, 84433, 84424, 111963, 111968, 111969, 12330, 84442, 84443, 111984, 111985, 5737, 13590, 13597, 13600, 13598, 13599, 123962, 5738, 636, 637, 123951, 639, 26049, 112793, 115442, 115441, 86128, 524, 97347, 97348, 30377, 30376, 523, 30033, 113538, 113539, 134082, 116899, 116900, 116901, 129020, 126443, 442, 443, 444, 90685, 90686, 153482, 153483, 26728, 126444, 607, 137005, 137004, 97902, 98000, 97883, 97963, 97985, 98069, 97860, 97867, 98057, 97888, 98003, 97950, 97915, 97988, 75033, 74968, 74967, 117966};
        
        canvas->get_camera().set_world(create_new_world_from_and_to(path));
        canvas->redraw();
        application->refresh_drawing();
    }
    
//    ezgl::point2d centre_point = latlon_to_xy(getIntersectionPosition(rua));
//    std::string main_canvas_id = application->get_main_canvas_id();
//     auto canvas = application->get_canvas(main_canvas_id);
//        canvas->get_camera().set_world(create_new_world_intersection(centre_point));
//        canvas->redraw();
//        application->refresh_drawing();
    
    return 0;
}
void act_on_pop_help(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    GObject *window; // the parent window over which to add the dialog
    window = application->get_object(application->get_main_window_id().c_str());
    GtkWidget *help_dialogue;
    help_dialogue=gtk_dialog_new_with_buttons(
            "Help window",
            (GtkWindow*) window,
            GTK_DIALOG_MODAL,
            ("OK"),
            GTK_RESPONSE_ACCEPT,
            NULL,
            GTK_RESPONSE_REJECT,
            NULL);
    GtkWidget *content_area;
    GtkWidget *label; // the label we will create to display a message in the content area
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(help_dialogue));
    label = gtk_label_new("This map has two following ways to find your path.\n\n"
                          "1. You can click the button 'Click Find Path'\n"
                          "    to choose the starting intersection and your\n "
                          "   destination.\n\n"
                          "2. You can type the roads that will form a intersection\n"
                          "    as the starting point, click 'Type Find Path',\n"
                          "    then do it again to be your destination.\n"
                          "    Red point will be your start point and green\n"
                          "    point will be your destination!\n\n"
                          "3. You can also use spin button to change turn penalty\n"
                          "    time which is in seconds\n\n"
                          "4. If you want to find a single road, then click\n"
                          "    'Dual Search' button so only one input entry will\n"
                          "    show up, then click 'Search'.\n\n "
                          "5. After your search is finished, you can always click\n"
                          "    the locate button to center the map to be your destination.\n\n"
                          "    Click OK to continue."  );
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(help_dialogue);
    int result=gtk_dialog_run(GTK_DIALOG(help_dialogue));
     switch (result)
    {
    case GTK_RESPONSE_ACCEPT:
       // do_application_specific_something ();
       break;
    default:
       // do_nothing_since_dialog_was_cancelled ();
       break;
    }
    gtk_widget_destroy (help_dialogue);
}

void act_on_search_button(GtkWidget *widgets,ezgl::application *application){
    UNUSED(widgets);
    GtkEntry* text_entry_1 = (GtkEntry*) application->get_object("SearchInput_down");
    m2_map.main_input.down_entry_input = gtk_entry_get_text(text_entry_1);
  
    GtkEntry* text_entry_2 = (GtkEntry*) application->get_object("SearchInput_up");
    m2_map.main_input.up_entry_input = gtk_entry_get_text(text_entry_2);
    application->update_message(m2_map.main_input.down_entry_input);
    
    //widgets about scroll
    GtkLabel* suggestion_label = (GtkLabel*) application->get_object("name_suggestion");
    //erase the previous output 
    gtk_label_set_text(suggestion_label,"");    
    GtkAdjustment* suggestion_adjustment= (GtkAdjustment*) application->get_object("adjustment1");
    //to record the location of scroll

    //reset the result
    m2_map.main_status.partial_name_street_Idx.clear();
    m2_map.main_status.intersection_Idx_between_streets.clear();
    
    if (!m2_map.main_input.down_entry_input.empty()&&m2_map.main_input.up_entry_input.empty()){
        m2_map.main_status.partial_name_street_Idx = find_street_ids_from_partial_street_name(m2_map.main_input.down_entry_input);         
            //text shows in suggestion label
            m2_map.matching_name =remove_duplicate_name_from_street_Idx(
            find_street_ids_from_partial_street_name(m2_map.main_input.down_entry_input));
        //no corresponding result
            if(m2_map.matching_name.empty()) gtk_label_set_text(suggestion_label,"No possible solution");
            else{
                std::string name_list;
                int size = m2_map.matching_name.size();
                    //choose less amount
                    for(int i=0;i<std::min(size, m2_map.main_status.max_label_show_name);i++){
                        name_list = name_list + m2_map.matching_name[i];
                        name_list.push_back('\n');
                    }
                //reset the scroll
                m2_map.scroll_index = 0;
                gtk_adjustment_set_value (suggestion_adjustment,m2_map.scroll_index);
                
                gtk_label_set_text(suggestion_label,name_list.c_str());
            }
    }//two streets to find the intersection
    else if (!m2_map.main_input.down_entry_input.empty()&& !m2_map.main_input.up_entry_input.empty()){
      std::vector<std::pair<unsigned,unsigned>> street_Idx_pair = possible_intersection_of_two_streets_Idx(
        find_street_ids_from_partial_street_name(m2_map.main_input.down_entry_input),
              find_street_ids_from_partial_street_name(m2_map.main_input.up_entry_input));
      //only print when there is only one intersection
      if((street_Idx_pair.size() >= 1)&&((street_Idx_pair.size() <= 3))){
          m2_map.main_status.intersection_Idx_between_streets = find_intersection_ids_from_street_ids(
                 street_Idx_pair[0].first,street_Idx_pair[0].second);
      }
      
    }
    
    //situation for error report
    //show kind of error
    GtkWidget *dialog_label;
    
    bool error = false;
    if(!m2_map.main_status.two_input_flag){
    //no matching for partial name search
    if( m2_map.main_status.partial_name_street_Idx.empty()){
        if(m2_map.main_input.down_entry_input.empty()) dialog_label = gtk_label_new("Input is empty");
        else {
            dialog_label = gtk_label_new("No matching street name");
        }
        error = true;
        }
    }
    else if(m2_map.main_status.two_input_flag && m2_map.main_input.up_entry_input.empty()){
        dialog_label = gtk_label_new("need another street name");
        error = true;       
    } else if(m2_map.main_status.two_input_flag && m2_map.main_status.intersection_Idx_between_streets.empty()){
            std::vector<std::pair<std::string,std::string>> matching_name_pair = possible_intersection_of_two_streets_name(
        find_street_ids_from_partial_street_name(m2_map.main_input.down_entry_input),
        find_street_ids_from_partial_street_name(m2_map.main_input.up_entry_input));
        
        
        std::string pair_list;
        for(unsigned i=0;i<matching_name_pair.size();i++){
            pair_list =pair_list+ matching_name_pair[i].first;
            pair_list.push_back('\t');
            pair_list.push_back('&');
            pair_list.push_back(' ');
            pair_list =pair_list+ matching_name_pair[i].second;
            pair_list.push_back('\n');
        }
        if(pair_list.empty()) dialog_label = gtk_label_new("No such intersection");
        else dialog_label = gtk_label_new(pair_list.c_str());
        error = true;
    }           

    application->refresh_drawing();
    
    if(error){
        //reset the matching name
        m2_map.matching_name.clear();
        GObject *window;           
        GtkWidget *content_area;
        GtkWidget *error_report_dialog;  
            window = application->get_object(application->get_main_window_id().c_str());
            error_report_dialog = gtk_dialog_new_with_buttons("Possible Intersections",(GtkWindow*) window,GTK_DIALOG_MODAL,
                ("OK"),GTK_RESPONSE_ACCEPT,("CANCEL"),GTK_RESPONSE_REJECT,NULL);
            g_signal_connect(GTK_DIALOG(error_report_dialog),"response",G_CALLBACK(response_dialog_destroy_dialog),NULL);
            content_area = gtk_dialog_get_content_area(GTK_DIALOG(error_report_dialog));
            gtk_container_add(GTK_CONTAINER(content_area), dialog_label);

     gtk_widget_show_all(error_report_dialog);
    }            
}

//normally,just exit the dialog as long as there is a response
void response_dialog_destroy_dialog(GtkDialog *dialog, gint response_id, gpointer user_data){
    m2_map.main_status.draw_segment_speed_limit = false;
    UNUSED(user_data);
    UNUSED(response_id);
    gtk_widget_destroy(GTK_WIDGET (dialog));
}

//////////////////////////////////////
//functionality and calculation part//
//////////////////////////////////////

//return different color according to different types of feature
ezgl::color feature_color(FeatureType feature_type){
    switch(feature_type)
    {
        case Unknown://grey
            return ezgl::color(225, 225, 234,255);
            break;
        case Park://birght green
            return ezgl::color(159, 229, 70,255);
            break;
        case Beach://sand color
            return ezgl::color(244, 223, 137,255);
            break;
        case Lake: //dark blue
            return ezgl::color(65, 143, 217,255);
            break;
        case River: //dark blue
            return ezgl::color(65, 143, 217,255);
            break;
        case Island://bright brown
            return ezgl::color(245, 221, 152,255);
            break;          
        case Building://carnation color 
            return ezgl::color(249, 229, 202,255);
            break;
        case Greenspace: //mint green
            return ezgl::color(152, 245, 182,255);
            break;
        case Golfcourse://mixed green and yellow
            return ezgl::color(153, 255, 51,255);
            break;
        case Stream: //azure
            return ezgl::color(152, 219, 245,255 );
            break;
        default: return ezgl::WHITE;
        break;
    }
}

//convert LatLon to point2d
ezgl::point2d latlon_to_xy(LatLon point_latlon){
    ezgl::point2d result(point_latlon.lon()*cos(DEG_TO_RAD*m2_map.main_size.lat_avg),point_latlon.lat());
    return result;
}

//convert point2d to LatLon
LatLon xy_to_latlon(ezgl::point2d point_xy){
    float lat = point_xy.y;
    float lon = point_xy.x /cos(m2_map.main_size.lat_avg*DEG_TO_RAD);
    LatLon result(lat,lon);
    return result;
}

//check same position
bool latlon_is_same(LatLon lhs,LatLon rhs){
    if((lhs.lat() == rhs.lat()) && (lhs.lon() == rhs.lon())) 
        return true;
    else
        return false;
}

//return geographical distance
double distance_between_latlon(LatLon lhs, LatLon rhs){
    double result = sqrt( pow( latlon_to_xy(lhs).x-latlon_to_xy(rhs).x, 2 )+
                        pow( latlon_to_xy(lhs).y-latlon_to_xy(rhs).y, 2 ) );
    return result;
}

//return the middle point in XY coordinates
ezgl::point2d middle_point_xy(ezgl::point2d p1,ezgl::point2d p2){
    ezgl::point2d result((p1.x+p2.x)/2,(p1.y+p2.y)/2);
    return result;
}

//returns angle used by draw names of streets
double tangent_two_points_degree(ezgl::point2d p1,ezgl::point2d p2){
    double max_angle = 75;
    double min_angle = -105;
    //atan returns [-pi/2,+pi/2] radians
    double angle = RAD_TO_DEG*atan2((p2.y-p1.y),(p2.x-p1.x));
    //as the return interval of atan, the it can only be larger than max_anlge
    if(angle >= max_angle){
        angle = angle-180;
    }
    if(angle <= min_angle){
        angle = angle+180;
    }
    return angle;
}

//if the feature is a line, it returns its length
//reference for calculating polygon:
//https://www.wikihow.com/Calculate-the-Area-of-a-Polygon
double feature_area(FeatureIndex feature_Idx){
    int total_points = getFeaturePointCount(feature_Idx);
    double result = 0;
    //situation that feature is a line
    if(latlon_to_xy(getFeaturePoint(0,feature_Idx)) != latlon_to_xy(getFeaturePoint(total_points-1,feature_Idx))){
        for(int i=0;i<total_points-1;i++){
            result = result+distance_between_latlon(getFeaturePoint(i,feature_Idx),
                        getFeaturePoint(i+1,feature_Idx));
        }
        return result;
    }else{
        std::vector<ezgl::point2d> vertices_list;
        //discard the last point, cause it's same as first point in this situation
        for(int i=0;i<total_points-1;i++){
            vertices_list.push_back(latlon_to_xy(getFeaturePoint(i,feature_Idx)));
        }
    int j = vertices_list.size()-1;  // The last vertex is the 'previous' one to the first

    for (unsigned i=0; i<vertices_list.size(); i++)
    { 
        result = result + (vertices_list[i].x+vertices_list[j].x) * (vertices_list[j].y-vertices_list[i].y);
        j = i;  //j is previous vertex to i
    }
    return abs(result/2);
        
    }
}

//calculate the angle of a segment, return in 360 degrees
//used for draw direction of a single way street
double angle_of_segment(ezgl::point2d point_from,ezgl::point2d point_to){
    //cases that 2 points have the same x or y coordinates
    if (point_from.x==point_to.x){
        if (point_from.y>point_to.y)
            return 270;
        else if (point_from.y<point_to.y)
            return 90;
    }
    if (point_from.y==point_to.y){
        if (point_from.x>point_to.x)
            return 180;
        else if (point_from.x<point_to.x)
            return 0;
    }
    //cases that point_to is on right up corner
    if ((point_from.y<point_to.y)&&(point_from.x<point_to.x)){
        return atan(abs(point_to.y-point_from.y)/abs(point_to.x-point_from.x))*180/PI+180;
    }
    //point_to is on left up corner
    if ((point_from.y<point_to.y)&&(point_from.x>point_to.x)){
        return atan(abs(point_from.x-point_to.x)/abs(point_to.y-point_from.y))*180/PI+270;
    }
    //point_to is on right down corner
    if ((point_from.y>point_to.y)&&(point_from.x<point_to.x)){
        return atan(abs(point_to.x-point_from.x)/abs(point_from.y-point_to.y))*180/PI+90;
    }
    //point_to is on left down corner
    if ((point_from.y>point_to.y)&&(point_from.x>point_to.x)){
        return atan(abs(point_from.y-point_to.y)/abs(point_from.x-point_to.x))*180/PI;
    }
    else
        return 0;
}

//calculate the opposite line size
double rectangle_length(ezgl::rectangle rectangle){
    double length=sqrt(pow((rectangle.m_first.x-rectangle.m_second.x),2)+pow((rectangle.m_first.y-rectangle.m_second.y),2));
    return length;
}

//returns true if the position given is inside the screen size
bool position_in_screen_flag(ezgl::rectangle current_screen,LatLon geo_position){
    ezgl::point2d on_screen_position=latlon_to_xy(geo_position);
    if ((on_screen_position.x>current_screen.m_first.x)
        &&(on_screen_position.x<current_screen.m_second.x)
        &&(on_screen_position.y>current_screen.m_first.y)
        &&(on_screen_position.y<current_screen.m_second.y))
        return true;
    else return false;
}

//Return true if the street is straight or curvature degree is 
//relatively low. 
//First connect a line between two terminal points, and
//use the distance between curve point and line to judge wether the street is 
//cureved enough
bool segment_nearly_straight(StreetSegmentIndex segment_Idx){
    InfoStreetSegment segment_info = getInfoStreetSegment(segment_Idx);
    if(segment_info.curvePointCount == 0) return true;
    ezgl::point2d p1(latlon_to_xy(getIntersectionPosition(segment_info.from) ) );
    ezgl::point2d p2(latlon_to_xy(getIntersectionPosition(segment_info.to) ) );
    //line expression:  y = slope*x + const
    double line_slope = (p1.y-p2.y)/(p1.x-p2.x);
    double line_const = p1.y-p1.x*line_slope;
    for(int i=0; i<segment_info.curvePointCount;i++){
        ezgl::point2d curve_point(latlon_to_xy(getStreetSegmentCurvePoint(i,segment_Idx)));
        //line go through curve_point and perpendicular to origin line
        double slope_prime = -1/line_slope;
        double const_prime = curve_point.y - curve_point.x*slope_prime;
        
        //find intersect point
        double intersect_x = (const_prime-line_const)/(line_slope-slope_prime);
        double intersect_y = intersect_x*line_slope+line_const;
        //get distance
        double distance =sqrt(pow(curve_point.x-intersect_x, 2) + pow(curve_point.y-intersect_y,2));
        if(distance > curve_tolerance) return false;
    }
    return true;
}

//convert current zoom_level into an integer,which is the index of main_zoom_level
int find_zoom_level(double zoom_factor){
    if(zoom_factor>= zoom_level_one) return 0;
    else if(zoom_factor<zoom_level_one && zoom_factor>= zoom_level_two) return 2;
    else if(zoom_factor<zoom_level_two && zoom_factor>= zoom_level_three) return 3;
    else if(zoom_factor<zoom_level_three && zoom_factor>= zoom_level_four) return 4;
    else if(zoom_factor<zoom_level_four && zoom_factor>= zoom_level_five) return 5;
    else if(zoom_factor<zoom_level_five && zoom_factor>= zoom_level_six) return 6;
    else if(zoom_factor<zoom_level_six && zoom_factor>= zoom_level_seven) return 7;
    else if(zoom_factor<zoom_level_seven && zoom_factor>= zoom_level_eight) return 8;
    else if(zoom_factor<zoom_level_eight && zoom_factor>= zoom_level_nine) return 9;
    else if(zoom_factor<zoom_level_nine && zoom_factor>= zoom_level_ten) return 10;
    else if(zoom_factor<zoom_level_ten && zoom_factor>= zoom_level_eleven) return 11;
    else if(zoom_factor<zoom_level_eleven && zoom_factor>= zoom_level_twelve) return 12;
    else if(zoom_factor<zoom_level_twelve && zoom_factor>= zoom_level_thirteen) return 13;
    else return 0;
}

//pass in a set of street id, and returns street name without duplication
std::vector<std::string> remove_duplicate_name_from_street_Idx(std::vector<unsigned> street_Idx_list){
    std::vector <std::string>  result;
    for(unsigned i=0;i<street_Idx_list.size();i++){
        std::string name_str = getStreetName(street_Idx_list[i]);

        //check whether duplicate
        if(std::find(result.begin(),result.end(),name_str) == result.end())
            result.push_back(name_str);
    }
    return result;
}

std::vector<std::pair<unsigned,unsigned> >possible_intersection_of_two_streets_Idx(
        std::vector<unsigned> street_Idx_list_1, std::vector<unsigned> street_Idx_list_2){
        std::vector<std::pair<unsigned,unsigned>> result;
        std::vector<std::pair<std::string,std::string>> name_in_result;
    for(unsigned i=0;i<street_Idx_list_1.size();i++){
        for(unsigned j=0;j<street_Idx_list_2.size();j++){
            //if they really have an intersection
            if(!find_intersection_ids_from_street_ids(street_Idx_list_1[i], street_Idx_list_2[j]).empty()){
                std::pair<unsigned,unsigned> street_Idx_pair;
                street_Idx_pair.first = street_Idx_list_1[i];
                street_Idx_pair.second =street_Idx_list_2[j];
                //check duplicate
                std::pair<std::string,std::string> name_pair;
                name_pair.first = getStreetName(street_Idx_list_1[i]);
                name_pair.second = getStreetName(street_Idx_list_2[j]);
                if(std::find(name_in_result.begin(),name_in_result.end(),name_pair) == name_in_result.end() ){
                    result.push_back(street_Idx_pair);
                    name_in_result.push_back(name_pair);
                }
            }
        }
    }
    return result;

}

//return pairs of street which have intersections, without duplication
std::vector<std::pair<std::string,std::string>> possible_intersection_of_two_streets_name(
        std::vector<unsigned> street_Idx_list_1, std::vector<unsigned> street_Idx_list_2){
    std::vector<std::pair<std::string,std::string>> result;
    for(unsigned i=0;i<street_Idx_list_1.size();i++){
        for(unsigned j=0;j<street_Idx_list_2.size();j++){
            //if they really have an intersection
            if(!find_intersection_ids_from_street_ids(street_Idx_list_1[i], street_Idx_list_2[j]).empty()){
                std::pair<std::string,std::string> street_name_pair;
                street_name_pair.first  = getStreetName(street_Idx_list_1[i]);
                street_name_pair.second = getStreetName(street_Idx_list_2[j]);
                
                //check duplication
                if(std::find(result.begin(),result.end(),street_name_pair) == result.end() ){
                    result.push_back(street_name_pair);
                }
            }
        }
    }
    return result;
}
//a new camera for found intersection
ezgl::rectangle create_new_world_intersection(ezgl::point2d centre_point){
    double half_length = (m2_map.main_size.x_max - m2_map.main_size.x_min)/2;
    double half_width = (m2_map.main_size.y_max - m2_map.main_size.y_min)/2;
    double zoom_level = zoom_level_ten;
    double left_x = centre_point.x-half_length*zoom_level;
    double left_y = centre_point.y-half_width*zoom_level;
    double right_x = centre_point.x+half_length*zoom_level;
    double right_y = centre_point.y+half_width*zoom_level;
    ezgl::point2d left(left_x,left_y);
    ezgl::point2d right(right_x,right_y);
    ezgl::rectangle result(left,right);
    return result;
}

ezgl::rectangle create_new_world_from_and_to(std::vector<unsigned> segment_list){
    double length = (m2_map.main_size.x_max - m2_map.main_size.x_min);
    double width = (m2_map.main_size.y_max - m2_map.main_size.y_min);
    //set initial value
    double min_x = m2_map.main_size.x_max;
    double max_x = m2_map.main_size.x_min;
    double min_y = m2_map.main_size.y_max;
    double max_y = m2_map.main_size.y_min;

        
        for(unsigned i=0;i<segment_list.size();i++){
            ezgl::point2d from_point(latlon_to_xy(getIntersectionPosition(getInfoStreetSegment(i).from)));
            if(from_point.x< min_x) min_x = from_point.x;
            if(from_point.x> max_x) max_x = from_point.x;
            if(from_point.y< min_y) min_y = from_point.y;
            if(from_point.y> max_y) max_y = from_point.y;  
            ezgl::point2d to_point(latlon_to_xy(getIntersectionPosition(getInfoStreetSegment(i).to)));
            if(to_point.x< min_x) min_x = to_point.x;
            if(to_point.x> max_x) max_x = to_point.x;
            if(to_point.y< min_y) min_y = to_point.y;
            if(to_point.y> max_y) max_y = to_point.y; 
        }
    
    //get vertices
    //make sure all streets are included in the screen
    int zoom_index = 12;
    double left_x,left_y,right_x,right_y;
    do
    {   
        left_x = (max_x+min_x)/2 - length/2*main_zoom_level[zoom_index];
        right_x = (max_x+min_x)/2 + length/2*main_zoom_level[zoom_index];
        left_y = (max_y+min_y)/2 - width/2*main_zoom_level[zoom_index];
        right_y = (max_y+min_y)/2 + width/2*main_zoom_level[zoom_index];
        zoom_index--;
    }while( (left_x>min_x ||left_y>min_y || right_x<max_x||right_y<max_y) && zoom_index>=0);
    
    ezgl::point2d left(left_x,left_y);
    ezgl::point2d right(right_x,right_y);
    ezgl::rectangle result(left,right);
    return result;
}

ezgl::rectangle creat_new_world_street(std::vector<unsigned> street_list){
    double length = (m2_map.main_size.x_max - m2_map.main_size.x_min);
    double width = (m2_map.main_size.y_max - m2_map.main_size.y_min);
    //set initial value
    double min_x = m2_map.main_size.x_max;
    double max_x = m2_map.main_size.x_min;
    double min_y = m2_map.main_size.y_max;
    double max_y = m2_map.main_size.y_min;
    for(unsigned i=0;i<street_list.size();i++){
        std::vector<unsigned> intersections_in_street = find_all_street_intersections(street_list[i]);
        for(unsigned j=0;j<intersections_in_street.size();j++){
            ezgl::point2d inter_point(latlon_to_xy(getIntersectionPosition(intersections_in_street[j])));
            if(inter_point.x< min_x) min_x = inter_point.x;
            if(inter_point.x> max_x) max_x = inter_point.x;
            if(inter_point.y< min_y) min_y = inter_point.y;
            if(inter_point.y> max_y) max_y = inter_point.y;   
        }
    }

 
    //get vertices
    //make sure all streets are included in the screen
    int zoom_index = 12;
    double left_x,left_y,right_x,right_y;
    do
    {   
        left_x = (max_x+min_x)/2 - length/2*main_zoom_level[zoom_index];
        right_x = (max_x+min_x)/2 + length/2*main_zoom_level[zoom_index];
        left_y = (max_y+min_y)/2 - width/2*main_zoom_level[zoom_index];
        right_y = (max_y+min_y)/2 + width/2*main_zoom_level[zoom_index];
        zoom_index--;
    }while( (left_x>min_x ||left_y>min_y || right_x<max_x||right_y<max_y) && zoom_index>=0);
    
    ezgl::point2d left(left_x,left_y);
    ezgl::point2d right(right_x,right_y);
    ezgl::rectangle result(left,right);
    return result;
}


unsigned find_closest_parking_of_POI(POIIndex POI_Idx){
	std::map<double,unsigned> Parking_distance_chart;//the second int is the POI idx and the first is the distance between POI and MY_POS
	for (unsigned i=0;i<m2_map.parking_POI_Idx.size();i++){
    	Parking_distance_chart.insert(std::make_pair(find_distance_between_two_points
                (getPointOfInterestPosition(POI_Idx),
                getPointOfInterestPosition(m2_map.parking_POI_Idx[i])),m2_map.parking_POI_Idx[i]));//push the data into the map
	}
	return Parking_distance_chart.begin()->second;
}

unsigned find_closest_street_segment(ezgl::point2d my_position){
    std::vector<unsigned> segment_list = find_intersection_street_segments(
            find_closest_intersection(xy_to_latlon(my_position)) );
    unsigned closest_segment_id=0;
    double distance;
    for(unsigned i=0;i<segment_list.size();i++){
        InfoStreetSegment segment_info =  getInfoStreetSegment(segment_list[i]);
        double distance_new = distance_point_to_line(latlon_to_xy(getIntersectionPosition(segment_info.from)),
                latlon_to_xy(getIntersectionPosition(segment_info.to)),my_position);
        if(i == 0){
            distance = distance_new;
            closest_segment_id = segment_list[i];
        }
        else if(i!=0 && distance>distance_new){
            distance = distance_new;
            closest_segment_id = segment_list[i];
        }
            
    }
    return closest_segment_id;
    
}

std::pair<double,POIIndex> find_closest_fuel_station(ezgl::point2d my_position){
    std::map<double,POIIndex> fuel_map;
    for(unsigned i=0;i<m2_map.fuel_info.size();i++){
        if(!m2_map.fuel_info[i].second.empty()){
            POIIndex POI_Idx = m2_map.fuel_info[i].first;
            double distance = distance_between_latlon(xy_to_latlon(my_position),
                    getPointOfInterestPosition(POI_Idx));
            fuel_map.insert(std::make_pair(distance,POI_Idx));
        }
    }
    return *fuel_map.begin();
}


double distance_point_to_line(ezgl::point2d line_from,ezgl::point2d line_to,
                    ezgl::point2d point){
    double line_slope = (line_from.y-line_to.y)/(line_from.x-line_to.x);
    double line_const = line_from.y-line_from.x*line_slope;
    
    //line go through curve_point and perpendicular to origin line
    double slope_prime = -1/line_slope;
    double const_prime = point.y - point.x*slope_prime;
        
        //find intersect point
    double intersect_x = (const_prime-line_const)/(line_slope-slope_prime);
    double intersect_y = intersect_x*line_slope+line_const;
        //get distance
    double distance =sqrt(pow(point.x-intersect_x, 2) + pow(point.y-intersect_y,2));
    return distance;
 
}
