
////this one is use the Dijkstra to find one source 
////to multi destinations
////includes the situation for same point
//std::vector<PathInfo> find_path_multi_dest(unsigned start_point,
//            std::vector<unsigned> dest_list,
//            const double right_turn_penalty, 
//            const double left_turn_penalty){
//    std::vector<PathInfo> result;
//    if(dest_list.empty()) return result;
//    for(int index =0;index<dest_list.size();index++){
//        //time used for this path
//        double time = 0;
//        PathInfo single_dest(start_point);
//        single_dest.intersection_end = dest_list[index];
//        if(start_point == dest_list[index]){
//            single_dest.cost_time = Initial_status;
//            ///////////////////////////////////////////
//            std::vector<unsigned> temp;
//            single_dest.route = temp;
//            //////////////////////////////////////////
//        } else{
//            //this one uses a global variable
//        single_dest.route = find_path_shortest_time(
//            start_point,dest_list[index],right_turn_penalty,
//               left_turn_penalty,time);
//        single_dest.cost_time = time;
//        }
//        result.push_back(single_dest);
//    }
//    //this make it possible for us to 
//    //std::make_heap(result.begin(),result.back());
//    return result;
//}

//
//
//std::vector<PointInfo> find_deliver_sequence
//        (std::vector<std::vector<PathInfo>>,
//        const std::vector<DeliveryInfo>& deliveries,
//        float current_capacity){   
//    float truck_capacity=current_capacity;
//    //this vector contains the cargo status
//    //1 to be unpicked
//    //0 to be picked but undropped
//    //-1 to be dropped
//    std::vector<unsigned> deliver_status;
//    for (int i=0;i<deliveries.size();i++){
//        deliver_status.push_back(1);
//    }
//    
//    //this vector contains the work to do
//    std::vector<PointInfo> work_to_do;
//    for (int i=0;i<deliveries.size();i++){
//        work_to_do.push_back(PointInfo(deliveries[i].pickUp,i,true));//first we add all pickups to the list
//    } 
//    
//    //this vector contains the deliver sequence with out the depo
//    std::vector<PointInfo> deliver_sequence;
//    
//    unsigned current_index;//current work done
//    while (!work_to_do.empty()){//while we have work to do
//        if (deliver_sequence.empty()){//if we have nothing done, we pick up the first cargo
//            deliver_sequence.push_back(PointInfo(deliveries[0].pickUp,0,true));
//            current_index=0;
//            truck_capacity-=deliveries[0].itemWeight;
//            deliver_status[0]=0;
//            work_to_do.push_back(PointInfo(deliveries[0].pickUp,0,false));//add the drop work to the list
//            work_to_do.
//        }
//    }
//    
//    
//    return deliver_sequence;
//}






//std::vector<CourierSubpath> get_actual_path(std::vector<PointInfo> point_list,
//                                const std::vector<unsigned>& depots,
//                                const double right_turn_penalty, 
//                                const double left_turn_penalty){
//   // auto start = std::chrono::high_resolution_clock::now();
//    std::vector<CourierSubpath> actual_path;
//    std::vector<unsigned> current_items_on_car;
//    //first we create the subpath from depo to first pickup point
//    CourierSubpath sub_path;
//    sub_path.end_intersection=point_list[0].intersection_Idx;
//    sub_path.start_intersection=find_closest_depot(point_list[0].intersection_Idx,
//                                                            depots);
//    sub_path.pickUp_indices=current_items_on_car;
//    sub_path.subpath=find_path_between_intersections(sub_path.start_intersection,
//                                                        sub_path.end_intersection,
//                                                        right_turn_penalty,
//                                                        left_turn_penalty);
//    actual_path.push_back(sub_path);
//    //traversing the point_list to create subpath for each 
//    for (unsigned i=0;i<point_list.size();i++){
//        if (point_list[i].is_pick_up){//we are picking up a new cargo 
//                                      //in this case we don't need to consider whether it is 
//                                      //the last point in the list as we cant 
//                                      //pick it without droping it
//            sub_path.start_intersection=point_list[i].intersection_Idx;
//            sub_path.end_intersection=point_list[i+1].intersection_Idx;
//            current_items_on_car.push_back(point_list[i].deliver_indices);//now we have a cargo
//            sub_path.pickUp_indices=current_items_on_car;
//            sub_path.subpath=find_path_between_intersections(sub_path.start_intersection,
//                                                        sub_path.end_intersection,
//                                                        right_turn_penalty,
//                                                        left_turn_penalty);
//            actual_path.push_back(sub_path);
//            sub_path.subpath.clear();
//            sub_path.pickUp_indices.clear();
//        }
//        else if (!point_list[i].is_pick_up){//we need to drop a cargo
//            if (i==point_list.size()-1){//we have reached the end of list, breakout
//                break;
//            }
//            else {
//                //first traverse all items on truck to pick out the item we want to drop 
//                for (unsigned k=0;k<current_items_on_car.size();k++){
//                    if (current_items_on_car[k]==point_list[i].deliver_indices){//we found it
//                        current_items_on_car.erase(current_items_on_car.begin()+k);//delete it
//                    }
//                }
//                //now the item has been dropped off
//                sub_path.start_intersection=point_list[i].intersection_Idx;
//                sub_path.end_intersection=point_list[i+1].intersection_Idx;
//                sub_path.pickUp_indices=current_items_on_car;
//                sub_path.subpath=find_path_between_intersections(sub_path.start_intersection,
//                                                        sub_path.end_intersection,
//                                                        right_turn_penalty,
//                                                        left_turn_penalty);
//                actual_path.push_back(sub_path);
//                sub_path.subpath.clear();
//                sub_path.pickUp_indices.clear();
//            }
//        }       
//    }
//    //now we have reached the end of list,create the sub path from it to the nearest depo
//    sub_path.start_intersection=point_list[point_list.size()-1].intersection_Idx;
//    sub_path.end_intersection=find_closest_depot(sub_path.start_intersection,
//                                                            depots);
//    current_items_on_car.clear();//no items any more
//    sub_path.pickUp_indices=current_items_on_car;
//    sub_path.subpath=find_path_between_intersections(sub_path.start_intersection,
//                                                        sub_path.end_intersection,
//                                                        right_turn_penalty,
//                                                        left_turn_penalty);
//    actual_path.push_back(sub_path);
//
//    sub_path.subpath.clear();
//    sub_path.pickUp_indices.clear();
//    
////    auto finish = std::chrono::high_resolution_clock::now();
////    std::chrono::duration<double> elapsed = finish - start;
////    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
//    return actual_path;
//}