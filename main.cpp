

#include "Potentials.hpp"
#include "Targets.hpp"
#include <boost/mpi.hpp>
#include <functional>

namespace mpi = boost::mpi;

// TODO merge into lambda
double linear_decay( double d, double n, double decay ) noexcept {
    return std::max(-decay * d + n, 0.0);
}

void calculate_distances( Potentials& p, Targets& t, mpi::communicator& world) {

    vector<double> target_ctr(t.objects.size());
    std::fill( begin(target_ctr), end(target_ctr), 0.0 );

    // declare some functions for decay calculation
    auto linear_decay_bind = [](double arg1){ return linear_decay(arg1,1,0.001); };
    //auto constant_distance_bind = [&](double arg1){ return constant_distance(arg1, maxium_mobility_distance ); };

    std::cout << "calculating on rank " << world.rank() << std::endl;
    // cycle over all t.objects

    int total = t.objects.size();
    int chunk_size = total / world.size();
    int rank_begin = chunk_size * world.rank();
    int rank_end = std::min ( rank_begin + chunk_size , total );

    std::cout << "calculating from " << rank_begin << " to " << rank_end << std::endl;
    #pragma omp parallel for
    for (int i = rank_begin; i < rank_end; ++i) {
	auto& target_object = t.objects[i];
	// cycle over all p.objects
	int ctr = 1;
	for (auto & potential : p.objects) {
	    auto& potential_object = potential;
	    target_ctr[i] += linear_decay_bind( target_object.distance( potential_object ) ) * potential_object.potential;

	    //target_ctr[i] += constant_distance_bind( calculate_distance(target_object, potential_object) );
	    // thats the distance function 1 / x 
	    //target_ctr[i] += reciprocal_decay( calculate_distance(target_object, potential_object));
	    ctr++;
	}
    }

    // reduce to root
    if ( world.rank() == 0 ) {
	vector<double> temp(target_ctr.size());
	std::cout << "reducing" << std::endl;
	reduce( world, &target_ctr.front(), target_ctr.size(), &temp.front(), std::plus<double>(), 0);
	target_ctr = temp;
    }else{
	reduce( world, &target_ctr.front(), target_ctr.size(), std::plus<double>(), 0);
    }

    if ( world.rank() == 0 ) {
	ofstream gnuplot_file("raster.gp");
	for (int i = 0; i < t.objects.size(); ++i){
	    auto& target_object = t.objects[i];
	    auto pt = target_object.center;
	    gnuplot_file << pt.first << " " << pt.second << " " << target_ctr[i] << std::endl;
	}
	gnuplot_file.close();

	std::cout << "done calculating" << std::endl;

	ofstream csv_file;
	// put the results to test.csv 
	// this can be loaded as a replacement for the old target.csv file
	csv_file.open("test.csv");

	for (int i = 0; i < t.objects.size(); ++i) {
	    csv_file << i << "," << target_ctr[i] << endl;
	}
	csv_file.close();
    }
}

int main(int argc, char** argv){
    
    mpi::environment env;
    mpi::communicator world;

    Potentials p;  
    Targets t;
    if ( world.rank() == 0 ) {
	p.load("potential_data.shp");
	t.load("target_data.shp");
    }

    broadcast(world, p, 0);
    broadcast(world, t, 0);

    calculate_distances(p,t, world);

    return 0;
}
