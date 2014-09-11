

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
