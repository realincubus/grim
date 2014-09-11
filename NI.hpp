

#pragma once

#include <string>
#include <iosfwd>
#include <vector>
#include <libshp/shapefil.h>
#include <map>

#include "CommaSeparatedValue.hpp"

using namespace std;

class NI{
    struct PrefetchData {
	vector<SHPObject> shp_data;
	CommaSeparatedValue csv_data;
	int shape_type = -1;
    };
    public:
	NI() :
	    maxium_mobility_distance(1000)
	{

	}
	void load_target_data( string );
	void load_building_data( string ); 
	void load_area_data( string );
	void normalize_target_data();
	void calculate_entities_per_building();
	void calculate_distances();
	void assoziate_buildings_to_area();
	void estimate();

    //private:

	int offset = 0;
	void prefetch_data(SHPHandle& shape_file, int number_of_entities, vector<SHPObject>& storage); 
	void load_data( string file_name, PrefetchData& data );

	vector<double> sum_entity_count_per_area;
	vector<double> entities_per_building;
	std::map<int, int> building_to_area;
	int maxium_mobility_distance;

	PrefetchData target_data;
	PrefetchData building_data;
	PrefetchData area_data;

};

