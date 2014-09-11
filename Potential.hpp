

#pragma once
#include "GeographicalShape.hpp"

class Potential : public GeographicalShape {
public:
    Potential () {

    }
    Potential ( SHPObject& object ) : GeographicalShape( object )
    {
	potential = 1;
    }
    virtual ~Potential () {}

    double potential;
};

namespace boost {
    namespace serialization {
	 
	template<class Archive>
	void serialize(Archive & ar, Potential& p, const unsigned int version)
	{
	    ar & p.potential;
	    ar & p.center;
	}
	 
    } // namespace serialization
} // namespace boost
