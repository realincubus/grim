

#pragma once

#include "GeographicalShape.hpp"

class Target : public GeographicalShape {
public:
    Target ( ) {

    }
    Target ( SHPObject& object ) : GeographicalShape( object )
    {
    }
    virtual ~Target () {}

protected:
    
};

namespace boost {
    namespace serialization {
	 
	template<class Archive>
	void serialize(Archive & ar, Target& p, const unsigned int version)
	{
	    ar & p.center;    
	}
	 
    } // namespace serialization
} // namespace boost
