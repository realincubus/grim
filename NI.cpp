

#include "NI.hpp"
#include "CommaSeparatedValue.hpp"
#include "PointInPolygon.hpp"

#include <math.h>
#include <iomanip>
#include <map>

using namespace std;

#define HOUSEHOLD_THRESHOLD (180)
#define TWO_HOUSEHOLD_THRESHOLD (HOUSEHOLD_THRESHOLD * 1.8)
#define BUILDING_PART_SIZE (125.0)
#define APARTMENT_BUILDING_LEVELS 4.0

auto estimate_household_count_of_apartment_building( vector<string>& database_line) {
    auto area = stoi(database_line[0]);
    auto number_of_building_parts = (area / BUILDING_PART_SIZE) * APARTMENT_BUILDING_LEVELS;
    // TODO test other rounding operations
    return int(number_of_building_parts);
}

// depends on area and perimeter
auto estimate_household_count( vector<string>& database_line ) noexcept {
     assert( database_line.size() >= 2 );
    auto area = stoi(database_line[0]);
    auto perimeter = stoi(database_line[1]);

    int building_type = 0;

    if (area < HOUSEHOLD_THRESHOLD) {
	building_type = 1;
    } else if (area < TWO_HOUSEHOLD_THRESHOLD) {
	building_type = 2;
    } else {
	building_type =
	    estimate_household_count_of_apartment_building(database_line);
	assert(building_type >= 2 && "strange data");
    }
    return building_type;
}

auto estimate_entity_count(SHPObject& building, vector<string>& database_line, double weighting_factor) {
    auto household_size = estimate_household_count( database_line );
    return household_size * weighting_factor;
}

auto estimate_car_count( SHPObject& building, vector<string>& database_line ) noexcept {
    auto building_type = estimate_household_count( database_line );
    auto ret = building_type * 600.0/1000.0;
    return ret;
}

auto normalize_building_data(vector<SHPObject>& buildings,
			     CommaSeparatedValue& building_csv_data) {
    cout << "entering normalize_building_data" << endl;
    vector<SHPObject> filtered_buildings;
    int i = 0;
    int stat_filter_dmg = 0;
    int stat_filter_type = 0;
    for (auto building : buildings) {
	i++;
	if (building_csv_data.getDataAt(i,"ZUS").compare("1200") == 0 ||
	    building_csv_data.getDataAt(i,"ZUS").compare("1700") == 0 ||
	    building_csv_data.getDataAt(i,"ZUS").compare("1800") == 0) {
	    stat_filter_dmg++;
	    continue;
	}
	if (building_csv_data.getDataAt(i,"GFK").compare("1300") == 0 ||
	    building_csv_data.getDataAt(i,"GFK").compare("1301") == 0 ||
	    building_csv_data.getDataAt(i,"GFK").compare("1381") == 0 ||
	    building_csv_data.getDataAt(i,"GFK").compare("1840") == 0) {
	    filtered_buildings.push_back(building);
	} else {
	    stat_filter_type++;
	}
    }
    cout << "filtered by dmg " << stat_filter_dmg << endl;
    cout << "filtered by type " << stat_filter_type << endl;
    cout << "leaving normalize_building_data" << endl;
    return filtered_buildings;
}


auto replace_extension( string file_arg , string new_extension ) {
    string building_file_csv_arg = file_arg;
    auto found = building_file_csv_arg.rfind(".");
    if (found != std::string::npos) {
	building_file_csv_arg.replace(found, 4, new_extension);
	cout << "csv file name " << building_file_csv_arg << endl;
    }
    return building_file_csv_arg;
}

typedef std::pair<double, double> Point2D;

Point2D calculate_center(SHPObject& object) {
    Point2D center;

    for (int j = 0; j < object.nVertices; ++j) {
	center.first += object.padfX[j];
	center.second += object.padfY[j];
    }

    center.first /= object.nVertices;
    center.second /= object.nVertices;
    return center;
}

double calculate_distance(SHPObject& target_object, SHPObject& building_object) {

    // calculate the central last_point of the target segment
    auto rc = calculate_center(target_object);

    // calculate the central last_point of the building
    auto bc = calculate_center(building_object);

    return sqrt(pow(rc.first - bc.first, 2) + pow(rc.second - bc.second, 2));
}

Point2D normalize(const Point2D& vect, double length) {
    Point2D n_vect;
    n_vect = vect;
    n_vect.first /= length;
    n_vect.second /= length;
    return n_vect;
}

double length(const Point2D& vect) {
    return sqrt(pow(vect.first, 2) + pow(vect.second, 2));
}

vector<SHPObject> interpolate(const SHPObject& target, double distance,
			      double len) {
    decltype(interpolate(target, distance, len)) ret;

    Point2D last_point;
    bool once = true;

    for (double i = 0.0; i < len; i += distance) {
	double lb = 0.0;
	double ub = 0.0;
	int j = 0;
	for (; j < target.nVertices - 1; ++j) {
	    Point2D vect{target.padfX[j] - target.padfX[j + 1],
			 target.padfY[j] - target.padfY[j + 1]};
	    ub = lb + length(vect);
	    if (i >= lb && i < ub) break;
	    lb = ub;
	}
	// generate the vertex for the vector we are in
	Point2D vect{target.padfX[j + 1] - target.padfX[j],
		     target.padfY[j + 1] - target.padfY[j]};
	auto len_vect = length(vect);
	Point2D new_point;
#if 0
	cout << "ratio " << distance/len_vect << endl;
	cout << "len_vect " << len_vect << endl;
	cout << "distance " << distance << endl;
	cout << "difference in x " << vect.first*(i/len_vect) << endl;
	cout << "difference in y " << vect.second*(i/len_vect) << endl;
	cout << "vect in x " << vect.first << endl;
	cout << "vect in y " << vect.second << endl;
#endif
	new_point.first = target.padfX[j] + vect.first * ((i - lb) / len_vect);
	new_point.second = target.padfY[j] + vect.second * ((i - lb) / len_vect);

	if (once) {
	    once = false;
	    last_point = new_point;
	    continue;
	} else {
	    // allocate storage
	    auto  padfX = new double[2];
	    auto  padfY = new double[2];
	    auto  padfZ = new double[2];

	    // fill with data
	    padfX[0] = last_point.first;
	    padfX[1] = new_point.first;
	    padfY[0] = last_point.second;
	    padfY[1] = new_point.second;
	    padfZ[0] = 0.0;
	    padfZ[1] = 0.0;

	    // create new street segment
	    auto new_object = SHPCreateSimpleObject(3, 2, padfX, padfY, padfZ);
	    SHPComputeExtents(new_object);
	    // cout << " new object " << new_object->padfX[0] << " " <<
	    // new_object->padfY[0] << " : " << new_object->padfX[1] << " " <<
	    // new_object->padfY[1] << endl;
	    ret.push_back(*new_object);

	    last_point = new_point;
	}
    }
    cout << "interpolation created " << ret.size() << " new target segments"
	 << endl;

    return ret;
}

double calculate_target_length(const SHPObject& target) {
    double ret = 0.0;
    for (int i = 0; i < target.nVertices - 1; ++i) {
	Point2D vect{target.padfX[i] - target.padfX[i + 1],
		     target.padfY[i] - target.padfY[i + 1]};
	ret += length(vect);
    }
    return ret;
}

vector<SHPObject> normalize_vertices(vector<SHPObject>& targets,
				     double normalization_distance) {

    decltype(normalize_vertices(targets, normalization_distance)) ret;

    // for every target create a new normalized target segment
    for (auto target : targets) {
	auto length = calculate_target_length(target);
	auto target_segments = interpolate(target, normalization_distance, length);
	for (auto element : target_segments) {
	    ret.push_back(element);
	}
    }
    cout << "normalization created " << ret.size() << " new target segments"
	 << endl;
    return ret;
}

void NI::prefetch_data(SHPHandle& shape_file, int number_of_entities,
		   vector<SHPObject>& storage) {
    for (int i = 0; i < number_of_entities; ++i) {
	auto target_object = SHPReadObject(shape_file, i);
	storage.push_back(*target_object);
    }
}

/// @brief generalized loading of shp data with the assoziated csv file
void NI::load_data( string file_name, PrefetchData& data ){
    SHPHandle shape_file;
    shape_file = SHPOpen( file_name.c_str(), "rb");
    int number_of_entities = -1;
    SHPGetInfo(shape_file, &number_of_entities, &data.shape_type,nullptr,nullptr);
    data.shp_data.reserve(number_of_entities);
    prefetch_data(shape_file, number_of_entities, data.shp_data);
    std::cout << "successfuly loaded " << number_of_entities << " = " << data.shp_data.size() << " from " << file_name << std::endl;

    auto file_name_csv = replace_extension( file_name, ".csv" );
    data.csv_data.load(file_name_csv);
    std::cout << "loaded csv file with " << data.csv_data.size() << std::endl;
}

void NI::load_target_data( string target_file_arg ) {
    load_data( target_file_arg, target_data );
}

void NI::load_building_data( string building_file_arg ) {
    load_data( building_file_arg, building_data);
}

void NI::load_area_data( string area_file_arg ) {
    load_data( area_file_arg, area_data);
}

void NI::normalize_target_data() {
    vector<SHPObject> filtered_targets;
    int i = 0;
    int stat_filter_type = 0;
    auto& targets = target_data.shp_data;
    for (auto target : targets) {
	i++;
	if (target_data.csv_data.getDataAt(i,"OBJART").compare("3101") == 0 ||
	    target_data.csv_data.getDataAt(i,"OBJART").compare("3106") == 0 ||
	    target_data.csv_data.getDataAt(i,"OBJART").compare("3102") == 0) {
	    filtered_targets.push_back(target);
	} else {
	    stat_filter_type++;
	}
    }
    cout << "filtered by type " << stat_filter_type << endl;
    targets = filtered_targets;

    auto normalized_targets = normalize_vertices(targets, 50);
    auto norm_target_shape_file = SHPCreate("norm_target_test.shp", target_data.shape_type);

    for (auto element : normalized_targets) {
	auto entity_number = SHPWriteObject(norm_target_shape_file, -1, &element);
    }

    SHPClose(norm_target_shape_file);
}

void NI::estimate() {
    auto& areas = area_data.shp_data;

    sum_entity_count_per_area.clear();
    sum_entity_count_per_area.reserve(areas.size());
    for (auto& area : areas) {
	sum_entity_count_per_area.push_back(0.0);
    }
    entities_per_building.clear();

    auto& building_csv_data = building_data.csv_data;

    auto estimate_function = []( SHPObject& building, vector<string>& database_line ){ return estimate_entity_count(building, database_line, 0.7); };
    //auto estimate_function = []( SHPObject& building, vector<string>& database_line ){ return estimate_entity_count(building, database_line, 1.016); };
    //auto estimate_function = []( SHPObject& building, vector<string>& database_line ){ return estimate_car_count(building, database_line); };

    auto& buildings = building_data.shp_data;
    int building_ctr = 0;
    std::cout << "number of buildings " << buildings.size() << std::endl;
    for( auto& building : buildings ){
	auto estimated_entity_count = estimate_function(
	    building, building_csv_data[building_ctr + 1]);
        
	auto it = building_to_area.find( building_ctr );
	if ( it != building_to_area.end() ) {
	    int area_ctr = building_to_area[building_ctr];
	    sum_entity_count_per_area[area_ctr] += estimated_entity_count;
	    entities_per_building.push_back(estimated_entity_count);
	}else{
	    entities_per_building.push_back(-1);
	    std::cout << "could not find an area for building " << building_ctr << std::endl;
	}
	building_ctr++;
    }
    
}

// TODO separate the estimation from the assoziation
void NI::assoziate_buildings_to_area() {
    int building_ctr = 0;
    // std::map<int , vector<string>* > area_to_database_line;

    auto& areas = area_data.shp_data;


    ofstream area_ofstream;
    area_ofstream.open( "a.gpl" );

    int area_ctr = 0 ;
    for( auto area : areas ){
	area_ofstream << "area no " << area_ctr << " has nParts " << area.nParts << endl;
	for (int i = 0; i < area.nParts; ++i) {
	    area_ofstream << "part no " << i << endl;
	    int startPoly = area.panPartStart[i];
	    int endPoly = -1;
	    if (!(i + 1 < area.nParts)) {
		endPoly = area.nVertices;
	    } else {
		endPoly = area.panPartStart[i + 1];
	    }

#if 1
	    auto distance = [](double x1, double y1, double x2, double y2){ 
		return sqrt(pow(x1-x2,2) + pow(y1-y2,2)); 
	    };
	    double ret = -1;

	    if ( (ret = distance( area.padfX[startPoly], area.padfY[startPoly], area.padfX[endPoly-1], area.padfY[endPoly-1] )) > 0.01 ) {
		cout << "error area no " << area_ctr << " part " << i << " has DIFFERENT end and start points " << ret << endl;
	    }
#endif

	    for (int j = startPoly; j < endPoly; ++j) {
		if (j + 1 != endPoly) {
		    if ( !((area.padfX[j] - area.padfX[j + 1] != 0) && (area.padfY[j] - area.padfY[j + 1] != 0 )) ) {
			cout << "error area no " << area_ctr << " has two points with identical coordinates" << endl;
		    }
		    double d = -1;
		    if ( (d =distance(area.padfX[j], area.padfY[j], area.padfX[j+1], area.padfY[j+1])) < 0.001 ){
			cout << "error area no " << area_ctr << " has two points with distance lower than 0.001 d is " << d << endl;
		    }
		}
		area_ofstream << j << " x " << std::setprecision(16)
		     << area.padfX[j] << " y " << std::setprecision(16)
		     << area.padfY[j] << endl;
	    }
	}
	area_ctr++;
    }

    area_ofstream.close();
    cout << "done area analysis" << endl; 

    ofstream building_ofstream;
    building_ofstream.open( "b.gpl" );

    auto& buildings = building_data.shp_data;
    // calculate entities per area and map building to area
    for (auto building : buildings) {
	auto building_center = calculate_center(building);
	int area_ctr = 0;
	bool isInPolygon = false;
	int found_n_times = 0;
	for (auto area : areas) {
	    for (int i = 0; i < area.nParts; ++i) {
		int startPoly = area.panPartStart[i];
		int endPoly = -1;
		if (!(i + 1 < area.nParts)) {
		    endPoly = area.nVertices;
		} else {
		    endPoly = area.panPartStart[i + 1];
		}
		int points = endPoly - startPoly;

		isInPolygon ^=
		    pointInPolygon(points, &area.padfX[startPoly],
				   &area.padfY[startPoly], building_center);
		
	    }

	    if (isInPolygon) {
		//assert(area_ctr != 0);
		building_to_area.insert(make_pair(building_ctr, area_ctr));
		found_n_times++;
		//break;
	    }
	    area_ctr++;
	}

	if (isInPolygon == false) {
	    building_ofstream << std::setprecision(16) << building_center.first << " "
		     << std::setprecision(16) << building_center.second << endl;
	}
	building_ctr++;
    }

    building_ofstream.close();
    cout << "done building assiziation" << endl;


}

void NI::calculate_entities_per_building() {
    cout << "entering calculate_enities_per_building" << endl;

    assoziate_buildings_to_area();
    estimate();
    
#if 0
    cout << "sum_entity_count_per_area" << endl;
    int i = 0;
    for (auto sum : sum_entity_count_per_area) {
	cout << i << " " << sum << endl;
	i++;
    }
    cout << "entities_per_building" << endl;
    i = 0;
    for (auto sum : entities_per_building) {
	cout << i << " " << sum << endl;
	i++;
    }
#endif

    //ofstream normalization_file;
    //normalization_file.open( "normalization_file.txt" );
    // normalize
    auto& area_csv_data = area_data.csv_data;
    int ppb = 0;
    for (auto& entities : entities_per_building) {
	//cout << "ppb " << ppb << endl;
	auto it = building_to_area.find(ppb);

	if ( it != building_to_area.end() ) {
	    auto assoziated_area = building_to_area[ppb];
	    auto entities_per_building_from_csv =
		stoi(area_csv_data[assoziated_area + 1][24+offset]);
		//stoi(area_csv_data.getDataAt( assoziated_area + 1 , 19 ));
	    //auto area_kn = stoi(area_csv_data.getDataAt( assoziated_area + 1 , "KN" ));
	    auto area_kn = stoi(area_csv_data[assoziated_area + 1][19]);
	    auto entities_of_assoziated_area =
		sum_entity_count_per_area[assoziated_area];
#if 0
	    normalization_file << "normalization factor "
		 <<  entities_per_building_from_csv / entities_of_assoziated_area << " "
		 << "entities_per_building_from_csv " <<  entities_per_building_from_csv << " "  
		 << "entities_of_assoziated_area " << entities_of_assoziated_area << 
		 " area " << assoziated_area << " area_kn " << area_kn 
		 << endl;
#endif
	    entities = entities * (entities_per_building_from_csv / entities_of_assoziated_area);
	}
	ppb++;
    }
    //normalization_file.close();
    ofstream norm_result("norm_result.txt");
    cout << "entities_per_building after norm" << endl;
    int i = 0;
    norm_result << "id, estimated_entity_count" << endl;
    for (auto ppb : entities_per_building) {
	norm_result << i << "," << ppb << endl;
	i++;
    }
    norm_result.close();
    cout << "leaving calculate_entities_per_building" << endl;
}

double reciprocal_decay( double d ) noexcept {
    return 1.0 / (1.0 + d );
}

double linear_decay( double d, double n, double decay ) noexcept {
    return std::max(-decay * d + n, 0.0);
}

double constant_distance( double d, double constant_d ) noexcept {
    if ( d < constant_d ) return 1.0;
    return 0.0;
}

void NI::calculate_distances( ) {
    vector<double> target_ctr;
    auto& buildings = building_data.shp_data;
    auto& building_csv = building_data.csv_data;
    auto& targets = target_data.shp_data;
    target_ctr.reserve(targets.size());

    auto linear_decay_bind = [](double arg1){ return linear_decay(arg1,1,0.001); };
    auto constant_distance_bind = [&](double arg1){ return constant_distance(arg1, maxium_mobility_distance ); };
    std::cout << "calculating" << std::endl;
    // cycle over all targets
    #pragma omp parallel for
    for (int i = 0; i < targets.size(); ++i) {
	auto& target_object = targets[i];
	target_ctr[i] = 0.0;
	// cycle over all buildings
	int ctr = 1;
	for (auto & building : buildings) {
	    auto& building_object = building;
	    // thats the distance function 1 / x 
	    //target_ctr[i] += reciprocal_decay( calculate_distance(target_object, building_object));
	    target_ctr[i] += linear_decay_bind( calculate_distance(target_object, building_object) ) * stof(building_csv[ctr][1]);
	    //target_ctr[i] += constant_distance_bind( calculate_distance(target_object, building_object) );
	    ctr++;
	}
    }

    ofstream gnuplot_file("raster.gp");
    for (int i = 0; i < targets.size(); ++i){
	auto& target_object = targets[i];
	auto pt = calculate_center(target_object);
	gnuplot_file << pt.first << " " << pt.second << " " << target_ctr[i] << std::endl;
    }
    gnuplot_file.close();

    std::cout << "done calculating" << std::endl;

    ofstream csv_file;
    // put the results to test.csv 
    // this can be loaded as a replacement for the old target.csv file
    csv_file.open("test.csv");

    for (int i = 0; i < targets.size(); ++i) {
	csv_file << i << "," << target_ctr[i] << endl;
    }
    csv_file.close();

}

