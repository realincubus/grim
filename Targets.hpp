

#pragma once
#include "Target.hpp"
#include "SHPData.hpp"

class Targets : public SHPData<Target>{
public:
    Targets (){

    }

    void load( string infile ) 
    {
	load_data( infile );
    }
    virtual ~Targets () {}

private:
    /* data */
};


namespace boost {
    namespace serialization {
	 
	template<class Archive>
	void serialize(Archive & ar, Targets& p, const unsigned int version)
	{
	    ar & p.shape_type;
	    ar & p.objects;
	    ar & p.csv_data;
	}
	 
    } // namespace serialization
} // namespace boost
