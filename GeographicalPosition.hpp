

#pragma once

#include <utility>
#include <cmath>

typedef std::pair<double, double> Point2D;

class GeographicalPosition {
public:
    GeographicalPosition () 
    {
    }
    virtual ~GeographicalPosition () {}

    double distance ( GeographicalPosition& other ){
	double x = center.first - other.center.first;
	double y = center.second - other.center.second;
	return std::hypot ( x,y ); 
    }	

    Point2D center;
};

namespace boost {
    namespace serialization {
	 
	template<class Archive>
	void serialize(Archive & ar, GeographicalPosition& p, const unsigned int version)
	{
	    ar & p.center;
	}
	 
    } // namespace serialization
} // namespace boost
