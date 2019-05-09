//#include <random>
//#include <iostream>
//#include <unittest++/UnitTest++.h>
//
//#include "StreetsDatabaseAPI.h"
//#include "m1.h"
//#include "m3.h"
//#include "m4.h"
//
//#include "unit_test_util.h"
//#include "courier_verify.h"
//
//using ece297test::relative_error;
//using ece297test::courier_path_is_legal_with_capacity;
//
//
//SUITE(simple_legality_toronto_canada_public) {
//    TEST(simple_legality_toronto_canada) {
//        std::vector<DeliveryInfo> deliveries;
//        std::vector<unsigned> depots;
//        std::vector<CourierSubpath> result_path;
//        float right_turn_penalty;
//        float left_turn_penalty;
//        float truck_capacity;
//
//        deliveries = {DeliveryInfo(18513, 22066, 156.64452)};
//        depots = {4};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 5000.000000000;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(15563, 25039, 186.19882), DeliveryInfo(22766, 37042, 2.90214), DeliveryInfo(43108, 53130, 89.01582)};
//        depots = {10880, 51183, 105495};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 6185.435058594;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(24830, 19095, 60.36134), DeliveryInfo(55521, 65085, 37.21818)};
//        depots = {26616, 37589};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 11899.549804688;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(13820, 55104, 146.33839), DeliveryInfo(41932, 69463, 100.59782), DeliveryInfo(71507, 7087, 157.35182)};
//        depots = {35855, 9003, 37807};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 9747.506835938;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(17965, 51983, 174.61108), DeliveryInfo(55540, 108475, 52.14658), DeliveryInfo(49563, 41007, 167.38791), DeliveryInfo(55102, 50132, 182.28844), DeliveryInfo(71655, 88693, 45.65729)};
//        depots = {27670, 97046, 51451};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 6967.588867188;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(20118, 20385, 115.91381), DeliveryInfo(23837, 94111, 75.65656)};
//        depots = {67157};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 1328.184082031;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(41299, 3260, 159.20335)};
//        depots = {101883};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 5000.000000000;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(49250, 101590, 4.68502), DeliveryInfo(64685, 92759, 150.65356)};
//        depots = {17903};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 14852.536132812;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(52102, 68392, 175.53738)};
//        depots = {25407, 17230};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 5000.000000000;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(88679, 13039, 58.09481), DeliveryInfo(38882, 49448, 84.76921), DeliveryInfo(85712, 49448, 128.60576), DeliveryInfo(88679, 53483, 94.46448), DeliveryInfo(85712, 53483, 128.41371), DeliveryInfo(63190, 49448, 94.63832), DeliveryInfo(88679, 17020, 94.87312), DeliveryInfo(88679, 53483, 94.46448), DeliveryInfo(36429, 55043, 20.23751)};
//        depots = {36641, 61873, 10465};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 6465.099121094;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(61041, 18281, 72.53727)};
//        depots = {41771, 48683};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 5000.000000000;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(53017, 19496, 24.15597)};
//        depots = {57239};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 5000.000000000;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(15105, 22791, 188.57120), DeliveryInfo(65047, 72971, 120.73338), DeliveryInfo(41755, 44970, 120.28966), DeliveryInfo(43274, 72971, 120.91861), DeliveryInfo(75625, 104239, 99.21876), DeliveryInfo(89518, 72971, 62.35834), DeliveryInfo(107548, 30762, 163.40495), DeliveryInfo(56298, 30762, 67.73763)};
//        depots = {69462, 42782, 94619};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 7671.847167969;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(27234, 32935, 158.54733), DeliveryInfo(19344, 97726, 38.37209), DeliveryInfo(8847, 9275, 59.23335)};
//        depots = {59158, 43097, 58889};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 3976.689208984;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(12706, 39149, 46.84510), DeliveryInfo(80591, 29100, 59.81778), DeliveryInfo(99352, 104558, 64.17639), DeliveryInfo(40417, 93901, 58.65097), DeliveryInfo(24156, 55300, 141.06827)};
//        depots = {74988, 77680, 4508};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 5195.080566406;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(59353, 60241, 32.81808), DeliveryInfo(23718, 32861, 161.50987), DeliveryInfo(41874, 15534, 43.20779), DeliveryInfo(39921, 30991, 82.98276), DeliveryInfo(53809, 27405, 60.49590)};
//        depots = {54494, 6299, 44483};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 10622.385742188;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(67496, 63896, 61.16725), DeliveryInfo(63711, 13045, 113.45358), DeliveryInfo(98503, 908, 28.82484), DeliveryInfo(105067, 13855, 4.02114), DeliveryInfo(101121, 48380, 104.51654), DeliveryInfo(98503, 75575, 117.75246), DeliveryInfo(63711, 22087, 165.01239), DeliveryInfo(98503, 89579, 3.76336)};
//        depots = {61050, 32125, 18579};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 8867.809570312;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(74374, 44227, 13.91874), DeliveryInfo(98761, 7446, 8.56278), DeliveryInfo(74374, 45128, 41.87980), DeliveryInfo(51183, 78818, 71.50895), DeliveryInfo(74374, 104039, 179.63609), DeliveryInfo(11117, 69156, 20.90369), DeliveryInfo(11117, 95819, 95.92138), DeliveryInfo(10770, 82657, 134.08324)};
//        depots = {95001, 105335, 24777};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 9899.384765625;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(78810, 69624, 166.39528), DeliveryInfo(85905, 54767, 6.80600), DeliveryInfo(78810, 52842, 101.82253), DeliveryInfo(71451, 35992, 14.59690), DeliveryInfo(88474, 51194, 47.61852), DeliveryInfo(88474, 87881, 193.32730), DeliveryInfo(14539, 37974, 66.61172), DeliveryInfo(78810, 38317, 75.14173)};
//        depots = {66903, 49166, 68918};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 9917.281250000;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(82276, 77936, 179.91382), DeliveryInfo(53233, 72181, 189.80901), DeliveryInfo(7549, 62806, 160.49899), DeliveryInfo(10307, 77936, 51.18974), DeliveryInfo(3962, 94120, 163.61295), DeliveryInfo(38326, 100046, 20.06098), DeliveryInfo(5042, 77936, 70.15913), DeliveryInfo(45314, 72181, 121.61732)};
//        depots = {91512, 87928, 25354};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 3549.154541016;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(84863, 41554, 66.15946), DeliveryInfo(99834, 106858, 45.42579)};
//        depots = {14522, 72436};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 11119.174804688;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(107201, 59404, 28.66476)};
//        depots = {21673, 75097};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 5000.000000000;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(46951, 100454, 176.74138), DeliveryInfo(102787, 34643, 93.27872), DeliveryInfo(99047, 74787, 72.90471), DeliveryInfo(65117, 100454, 79.35565), DeliveryInfo(46558, 74787, 82.74268), DeliveryInfo(67, 101034, 157.75711), DeliveryInfo(78753, 18300, 182.96613), DeliveryInfo(103007, 74787, 71.61387)};
//        depots = {65284, 54262, 30047};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 4273.869628906;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(88578, 64015, 184.61324), DeliveryInfo(94979, 90276, 65.88518)};
//        depots = {86978};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 10739.620117188;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(95296, 59082, 122.03896), DeliveryInfo(76291, 47218, 142.83829), DeliveryInfo(86885, 3352, 146.78773), DeliveryInfo(68385, 86668, 190.66869), DeliveryInfo(60466, 20292, 168.13336)};
//        depots = {92748, 83507, 2935};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 14417.934570312;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(97874, 88337, 193.41937), DeliveryInfo(89626, 43755, 146.50169), DeliveryInfo(9004, 77546, 183.47961), DeliveryInfo(71702, 51087, 144.07825), DeliveryInfo(60297, 27262, 0.80647)};
//        depots = {61212, 6429, 18409};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 11383.201171875;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(101372, 39409, 137.59500), DeliveryInfo(98891, 101567, 59.62290), DeliveryInfo(19500, 39409, 15.90577), DeliveryInfo(101372, 90599, 187.31200), DeliveryInfo(19500, 101567, 46.18541), DeliveryInfo(101372, 39409, 137.59500), DeliveryInfo(47936, 101567, 63.51763), DeliveryInfo(101372, 79470, 48.40664), DeliveryInfo(91206, 21071, 36.90132)};
//        depots = {81027, 27440, 67536};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 4274.875000000;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(103597, 95750, 147.03349), DeliveryInfo(103597, 37400, 83.66404), DeliveryInfo(1542, 61568, 24.86163), DeliveryInfo(49787, 61568, 55.82699), DeliveryInfo(103597, 39266, 53.61071), DeliveryInfo(70253, 95750, 156.97383), DeliveryInfo(70253, 61568, 9.75238), DeliveryInfo(103597, 95750, 147.03349), DeliveryInfo(84309, 86958, 42.47386)};
//        depots = {19580, 31525, 20286};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 7168.385742188;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(104829, 73821, 66.27128), DeliveryInfo(25859, 72189, 180.74861), DeliveryInfo(23664, 40037, 115.90621), DeliveryInfo(48816, 34210, 108.68627), DeliveryInfo(67440, 48376, 194.95090)};
//        depots = {23014, 18617, 41110};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 12754.851562500;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//        deliveries = {DeliveryInfo(107184, 16777, 65.56896), DeliveryInfo(43222, 72903, 182.31299)};
//        depots = {89697, 89023};
//        right_turn_penalty = 15.000000000;
//        left_turn_penalty = 15.000000000;
//        truck_capacity = 2286.546142578;
//        result_path = traveling_courier(deliveries, depots, right_turn_penalty, left_turn_penalty, truck_capacity);
//        CHECK(courier_path_is_legal_with_capacity(deliveries, depots, result_path, truck_capacity));
//
//    } //simple_legality_toronto_canada
//
//} //simple_legality_toronto_canada_public
