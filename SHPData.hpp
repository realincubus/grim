

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <libshp/shapefil.h>
#include "Potential.hpp"
#include "CommaSeparatedValue.hpp"

using namespace std;

template < typename T >
class SHPData {
    public:
    auto replace_extension( string file_arg , string new_extension ) {
	string building_file_csv_arg = file_arg;
	auto found = building_file_csv_arg.rfind(".");
	if (found != std::string::npos) {
	    building_file_csv_arg.replace(found, 4, new_extension);
	    cout << "csv file name " << building_file_csv_arg << endl;
	}
	return building_file_csv_arg;
    }

    void prefetch_data(SHPHandle& shape_file, int number_of_entities) {
	for (int i = 0; i < number_of_entities; ++i) {
	    // read an object from the file
	    auto object = SHPReadObject(shape_file, i);
	    // store a copy of the object in the corresponding vector
	    objects.emplace_back( *object  );
	}
    }

    void load_data( string file_name ){
	// read the shape file
	SHPHandle shape_file;
	shape_file = SHPOpen( file_name.c_str(), "rb");
	int number_of_entities = -1;
	SHPGetInfo(shape_file, &number_of_entities, &shape_type, nullptr, nullptr);
	prefetch_data(shape_file, number_of_entities);
	std::cout <<	"successfuly loaded " << 
			number_of_entities << " = " << 
			objects.size() << " from " << 
			file_name << std::endl;

	// read the corresponding csv file
	auto file_name_csv = replace_extension( file_name, ".csv" );
	csv_data.load(file_name_csv);
    }

    int shape_type;
    vector<T> objects;
    CommaSeparatedValue csv_data;

};
