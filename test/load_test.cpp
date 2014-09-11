

#define BOOST_TEST_MODULE LOADING_TEST

#include <boost/test/unit_test.hpp>
#include "../NI.hpp"


BOOST_AUTO_TEST_CASE( LOAD_BUILDING_TEST )
{
    NI ni;
    ni.load_building_data( "PointPerUG.shp" );
}

BOOST_AUTO_TEST_CASE( LOAD_AREA_TEST )
{
    NI ni;
    ni.load_area_data( "area.shp") ;
}

BOOST_AUTO_TEST_CASE( LOAD_ROAD_TEST )
{
    NI ni;
    ni.load_target_data( "road_test.shp") ;
}
