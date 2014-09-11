#define BOOST_TEST_MODULE ASSOZIATION_TEST

#include <boost/test/unit_test.hpp>
#include "../NI.hpp"
#include <string>

using namespace std;

BOOST_AUTO_TEST_CASE( LOAD_BUILDING_TEST )
{
    NI ni;
    ni.load_building_data( "PointPerUG.shp" );
    ni.load_area_data( "area.shp" );
    
    ni.assoziate_buildings_to_area();

    auto& building_shp_data = ni.building_data.shp_data;
    auto& building_csv_data = ni.building_data.csv_data;

    auto& area_shp_data = ni.area_data.shp_data;
    auto& area_csv_data = ni.area_data.csv_data;

    int building_ctr = 0;
    for( auto& building : building_shp_data ){
	auto assoziated_area = ni.building_to_area[building_ctr];
	auto kn_building = building_csv_data[building_ctr+1][0];
	auto kn_area = area_csv_data[assoziated_area+1][19];
	cout << building_ctr << " kn_building " << kn_building << " kn_area " << kn_area << endl;

	// convert to int 
	auto kn_building_int = std::stoi(kn_building);
	auto kn_area_int = std::stoi(kn_area);
    
	BOOST_CHECK( kn_area_int ==  kn_building_int );
	
	building_ctr++;
    }

}

BOOST_AUTO_TEST_CASE( QUAD_BUILDING_TEST )
{
    NI ni;
    ni.load_building_data( "QuadPoint.shp" );
    ni.load_area_data( "Quad.shp" );
    
    ni.assoziate_buildings_to_area();

    auto& building_shp_data = ni.building_data.shp_data;
    auto& building_csv_data = ni.building_data.csv_data;

    auto& area_shp_data = ni.area_data.shp_data;
    auto& area_csv_data = ni.area_data.csv_data;

    int building_ctr = 0;
    for( auto& building : building_shp_data ){

	auto kn_building = building_csv_data[building_ctr+1][0];
	auto it = ni.building_to_area.find(building_ctr);
	string kn_area;
	if ( it != ni.building_to_area.end() ) {
	    auto assoziated_area = ni.building_to_area[building_ctr];
	    kn_area = area_csv_data[assoziated_area+1][18];
	}else{
	    kn_area = "-1";
	}
	cout << building_ctr << " kn_building " << kn_building << " kn_area " << kn_area << endl;

	// convert to int 
	auto kn_building_int = std::stoi(kn_building);
	auto kn_area_int = std::stoi(kn_area);
	BOOST_CHECK( kn_area_int ==  kn_building_int );
	
	building_ctr++;
    }

}
