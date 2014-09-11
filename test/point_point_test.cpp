
#define BOOST_TEST_MODULE POINT_POINT_TEST

#include <boost/test/unit_test.hpp>
#include "../NI.hpp"


BOOST_AUTO_TEST_CASE( LOAD_POINT_POINT_TEST )
{
    NI ni;
    ni.load_target_data( "PointPerUG.shp");
    ni.load_building_data( "building_test.shp" );
}

BOOST_AUTO_TEST_CASE( CALCULATE_TEST )
{
    NI ni;
    ni.load_target_data( "PointPerUG.shp");
    ni.load_building_data( "building_test.shp" );

    ni.calculate_distances();
}
