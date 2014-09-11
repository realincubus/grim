

#define BOOST_TEST_MODULE CSV_TEST

#include <boost/test/unit_test.hpp>
#include "../CommaSeparatedValue.hpp"


BOOST_AUTO_TEST_CASE( LOAD_CSV_TEST )
{
    CommaSeparatedValue csv;
    csv.load( "road_test.csv" );

    string& csv_data_cell = csv.getDataAt( 1 , "EDBS_" );

    BOOST_CHECK(csv_data_cell.compare("338710.00000000000") == 0 );

    
}

