#pragma once
#include <utility>
#include <iostream>

using namespace std;

//  from http://alienryderflex.com/polygon/
//  Globals which should be set before calling this function:
//
//  int    polySides  =  how many corners the polygon has
//  float  polyX[]    =  horizontal coordinates of corners
//  float  polyY[]    =  vertical coordinates of corners
//  float  x, y       =  point to be tested
//
//  (Globals are used in this example for purposes of speed.  Change as
//  desired.)
//
//  The function will return YES if the point x,y is inside the polygon, or
//  NO if it is not.  If the point is exactly on the edge of the polygon,
//  then the function may return YES or NO.
//
//  Note that division by zero is avoided because the division is protected
//  by the "if" clause which surrounds it.

bool pointInPolygon( int polySides, double* polyX, double* polyY, pair<double,double>& point ) {

    int i;
    int j=polySides-1;
    bool oddNodes = false;
    double x = point.first;
    double y = point.second;

    for ( i=0; i < polySides; i++) {
	if (	(polyY[i] < y && polyY[j] >= y) || 
		( (polyY[j]< y && polyY[i]>= y) 
		&&  (polyX[i]<=x || polyX[j]<=x) )
	    ) {
	    oddNodes^=(polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i])<x); 
	}
	j=i; 
    }

    return oddNodes; 
}
