

#include "Potentials.hpp"
#include "Targets.hpp"

// TODO merge into lambda
double linear_decay( double d, double n, double decay ) noexcept {
    return std::max(-decay * d + n, 0.0);
}

void calculate_distances( Potentials& p, Targets& t) {
    vector<double> target_ctr;

    // TODO dont use local references to these types
    auto& potential_csv = p.csv_data;

    target_ctr.reserve(t.objects.size());

    // declare some functions for decay calculation
    auto linear_decay_bind = [](double arg1){ return linear_decay(arg1,1,0.001); };
    //auto constant_distance_bind = [&](double arg1){ return constant_distance(arg1, maxium_mobility_distance ); };

    std::cout << "calculating" << std::endl;
    // cycle over all t.objects
    #pragma omp parallel for
    for (int i = 0; i < t.objects.size(); ++i) {
	auto& target_object = t.objects[i];
	target_ctr[i] = 0.0;
	// cycle over all p.objects
	int ctr = 1;
	for (auto & potential : p.objects) {
	    auto& potential_object = potential;
	    // thats the distance function 1 / x 
	    //target_ctr[i] += reciprocal_decay( calculate_distance(target_object, potential_object));
	    target_ctr[i] += linear_decay_bind( target_object.distance( potential_object ) ) * potential_object.potential;
	    //target_ctr[i] += constant_distance_bind( calculate_distance(target_object, potential_object) );
	    ctr++;
	}
    }

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

int main(int argc, char** argv){
    

    Potentials p("potential_data.shp");
    Targets t("target_data.shp");
    
    calculate_distances(p,t);

    return 0;
}
