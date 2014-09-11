

#pragma once

#include "GeographicalShape.hpp"

class Target : public GeographicalShape {
public:
    Target ( SHPObject& object ) :GeographicalShape( object )
    {
    }
    virtual ~Target () {}

protected:
    
};
