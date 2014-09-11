#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>
#include <map>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>

using namespace std;

struct CommaSeparatedValue{
    /// @ brief 
    template <typename T>
    class Table : public std::vector<T>{
    public:
        Table (){
    
        }
        virtual ~Table (){
    
        }
	void assoziate( pair<string,int> pa ) {
	    map.insert( pa );
	}

	string& getDataAt( int i, int j ) {
	    assert ( i >= 0 && i < this->size() );
	    cout << "j " << j << endl;
	    assert ( j >= 0 && j < (*this)[i].size() );
	    return (*this)[i][j];
	}

	string& getDataAt( int i, string column_name ) {
	    auto it = map.find(column_name);
	    assert( it != map.end() && "could not find name in the list" );
	    int idx = (*it).second;
	    return getDataAt( i , idx );
	}

	std::map<string, int> map;
    };

    Table< vector<string> > table;

    string& getDataAt( int i , string column_name ) {
	return table.getDataAt( i , column_name );
    }

    void load( string file ){
	cout << "loading cvs file " << file << endl;
	ifstream input;
	input.open( file.c_str() );
	if ( !input.is_open() ) {
	    cout << "could not open file csv file " << file << endl;
	    return;
	    //exit(-1);
	}

	bool isFirstLine = true;

	while( input.good() ){
	    string line;
	    if ( !getline( input , line ) ) break;
	    istringstream sstr(line);
	    vector<string> record;

	    if ( isFirstLine ) {
		readHeader( sstr, record );
		isFirstLine = false;
	    }else{
		string value;
		while( sstr ) {
		    if ( !getline( sstr, value, ',' ) ) break;
		    record.push_back( value );
		}
	    }
	    table.push_back( record );
	}
	input.close();
	printStats();
	cout << "done loading cvs file " << file << endl;
    }	

    void readHeader( istringstream& sstr, vector<string>& record ) noexcept {
	int ctr = 0;
	while( sstr ){
	    string value;
	    if ( !getline( sstr, value, '"' ) ) break;
	    if ( ctr % 2 ) {
		static int column_ctr = 0;
		istringstream comma_string_stream( value );
		string first_element;
		getline( comma_string_stream, first_element, ',' );
		cout << "header first_element " << ctr / 2 << " " << first_element << endl;
		table.assoziate( make_pair( first_element, column_ctr++ ) );
		record.push_back( first_element );
	    }
	    ctr++;
	}

	for( auto& element1 : record ){
	    for( auto& element2 : record ){
	        if ( &element1 == &element2 ) {
		    continue;
		}
		if ( element1.compare(element2) == 0 ) {
		    cout << "element 1 : " << element1 << " element 2 " << element2 << endl;
		    assert( 0 && "two elements in the record list are the same" );
		}
	    }
	}	

    }

    void save( string file ){
	ofstream output;
	output.open( file.c_str() );
	
	for( auto line  : table  ){
	    for( auto i = line.begin() ; i != line.end() ; i++ ){
	        output << *i;
		if ( (i+1) != line.end() ) {
		    output << ",";
		}
	    }
	    output << endl;
	}
	output.close();
    }

    void printStats( ){
	cout << "table size " << table.size();
	std::flush(cout) ;
	cout << " x " << table[1].size() << endl;
	for (int j = 0; j < 3; ++j){
	    if (table.size() <= j ) continue;
	    for (int i = 0; i < table[j].size(); ++i){
		cout << table[j][i] << " " ;
	    }
	    cout << endl;
	}
    }

    int size() {
	return table.size();
    }

    void addLine( vector< string >& line ) {
	table.push_back( line );
    }

    vector<string>& operator[]( int index ){
	assert( index >= 0 && index < table.size() ) ;
        return table[index];
    }
};


namespace boost {
    namespace serialization {
	 
	template<class Archive>
	void serialize(Archive & ar, CommaSeparatedValue& p, const unsigned int version)
	{
	    ar & p.table;
	}

	template<class Archive>
	void serialize(Archive & ar, CommaSeparatedValue::Table<vector<string>>& p, const unsigned int version)
	{
	    ar & p.map;
	}
	 
    } // namespace serialization
} // namespace boost
