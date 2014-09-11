
#pragma once

#include "Potential.hpp"
#include "SHPData.hpp"

using namespace std;

class Potentials : public SHPData<Potential> {
public:
    Potentials ( std::string infile ) 
    {
	load_data( infile);
    }
    virtual ~Potentials () {}

};

