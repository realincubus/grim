
#include <iosfwd>
#include <libshp/shapefil.h>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <vector>
#include <assert.h>
#include <memory>
#include <map>
#include <iomanip>
#include "CommaSeparatedValue.hpp"
#include "NI.hpp"

using namespace std;


int main(int argc, char** argv) {

    bool norm_targets = false;
    bool norm_buildings = false;
    bool target_arg_set = false;
    bool building_arg_set = false;
    bool area_arg_set = false;
    string target_file_arg = "";
    string target_file_csv_arg = "";
    string building_file_arg = "";
    string building_file_csv_arg = "";
    string area_file_arg = "";
    string area_file_csv_arg = "";
    SHPHandle target_shape_file;
    SHPHandle building_shape_file;
    SHPHandle area_shape_file;

    NI ni;
    cout << "parsing arguments" << endl;
    // parse command line arguments
    if (argc > 1) {
	auto arg = string(argv[1]);
	if (arg.compare("-nr") == 0 || arg.compare("--normalize-targets") == 0){
	    norm_targets = true;
	}
	if (arg.compare("-nb") == 0 || arg.compare("--normalize-buildings") == 0){
	    norm_buildings = true;
	}
	if (norm_targets) {
	    target_file_arg = string(argv[2]);
	    target_arg_set = 1;
	} else if (norm_buildings) {
	    assert ( argc >= 3 );
	    building_file_arg = string(argv[2]);
	    building_arg_set = 1;
	    area_file_arg = string(argv[3]);
	    area_arg_set = 1;
	    ni.offset = stoi( argv[4] );
	    cout << "offset is " << ni.offset << endl;
	} else {
	    target_file_arg = string(argv[1]);
	    target_arg_set = 1;
	    building_file_arg = string(argv[2]);
	    building_arg_set = 1;
	}
    }


    // load all data from the files
    cout << "loading file data" << endl;

    if (target_arg_set) {
	ni.load_target_data( target_file_arg );
    }
    if (building_arg_set) {
	ni.load_building_data( building_file_arg );
    }
    if (area_arg_set) {
	ni.load_area_data( area_file_arg );
    }

    cout << "starting shape file analysis" << endl;

    // normalize data
    if (norm_targets) {
	ni.normalize_target_data();
	exit(0);
    }
    if (norm_buildings) {
	cout << "normalizing buildings" << endl;
	ni.calculate_entities_per_building();
	exit(0);
    }

    ni.calculate_distances();

    return 0;
}
