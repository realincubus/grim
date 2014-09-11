

#pragma once
#include "GeographicalShape.hpp"

class Potential : public GeographicalShape {
public:
    Potential ( SHPObject& object ) : GeographicalShape( object )
    {
	potential = 1;
    }
    virtual ~Potential () {}

    double potential;
};
