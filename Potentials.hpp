
#pragma once

#include "Potential.hpp"
#include "SHPData.hpp"

using namespace std;

class Potentials : public SHPData<Potential> {
public:

    Potentials ( ) {

    }

    void load ( std::string infile ) 
    {
	load_data( infile);
    }
    virtual ~Potentials () {}

};


namespace boost {
    namespace serialization {
	 
	template<class Archive>
	void serialize(Archive & ar, Potentials& p, const unsigned int version)
	{
	    ar & p.shape_type;
	    ar & p.objects;
	    ar & p.csv_data;
	}
	 
    } // namespace serialization
} // namespace boost
