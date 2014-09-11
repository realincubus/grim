

#pragma once
#include <libshp/shapefil.h>
#include "GeographicalPosition.hpp"

class GeographicalShape : public GeographicalPosition{
public:

    GeographicalShape ( ) {
	shape = nullptr;
    }

    GeographicalShape ( SHPObject& object ) : shape( &object )
    {
	center = calculate_center( object );
    }
    virtual ~GeographicalShape () {}

    SHPObject* shape;
protected:

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

};
