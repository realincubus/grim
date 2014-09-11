

#pragma once
#include "Target.hpp"
#include "SHPData.hpp"

class Targets : public SHPData<Target>{
public:
    Targets ( string infile ) 
    {
	load_data( infile );
    }
    virtual ~Targets () {}

private:
    /* data */
};
