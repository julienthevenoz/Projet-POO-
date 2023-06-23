//fichier geomod.cc, THEVENOZ Julien et MANEFF Lucas

#include <iostream>
#include <vector>
#include <cstdlib>
#include "geomod.h"
#include "math.h"
#include "graphic.h"
#include "constantes.h"

using namespace std;


namespace{
	
	double epsilon_zero(0);
	double size(0);	
	
}


double getSize(){ 
	return size; 
}

void setSize(double new_size){
	
	size = new_size;
	epsilon_zero = pow(10, -10) * size;
}


double getEpsilonZero(){ 
	return epsilon_zero;
}


bool equalZero(double p){
	
	if(p < epsilon_zero){
		return true;
	}
	
	return false;
}


vector<Point> findEquiPoint(Point pt){

	vector<Point> point_list;
	
	for(int i(-1); i <= 1; ++i){
		for(int j(-1); j <= 1; ++j){
			
			Point new_point({pt.x + 2*i*size, pt.y + 2*j*size});
			point_list.push_back(new_point);
		}
	}
	
	return point_list;	
}


double normalize(double scalar){
	
	while(scalar < -size or scalar > size){
		if(scalar < 0){
			scalar += 2*size;
		}else{
			scalar -= 2*size;
		}
	}
	
	return scalar;
}


Point normalize(Point pt){
	
	Point normalized_point;
	
	normalized_point.x = normalize(pt.x);
	normalized_point.y = normalize(pt.y);
	
	return normalized_point;
}


double makeVector(Point start_point, Point end_point, Vect& smaller_vect){
	
	vector<Point> end_equi_points(findEquiPoint(end_point));
	
	smaller_vect.x = end_equi_points[0].x - start_point.x;
	smaller_vect.y = end_equi_points[0].y - start_point.y;
	
	double min_norm(calculateNorm(smaller_vect));
	
	for(size_t i(1); i < end_equi_points.size(); ++i){
		Vect current_vect;
		
		current_vect.x = end_equi_points[i].x - start_point.x;
		current_vect.y = end_equi_points[i].y - start_point.y;
		
		double current_norm(calculateNorm(current_vect));
		
		if(current_norm < min_norm){
			min_norm = current_norm;
			smaller_vect = current_vect;
		}
	}
	
	return min_norm;
}


double calculateNorm(Vect v){
	
	return sqrt(pow(v.x, 2) + pow(v.y, 2));
}


bool pointsEqual(Point pt1, Point pt2){
	
	Vect v;
	double distance(makeVector(pt1, pt2, v));
	
	return equalZero(distance);
}


bool inCircle(Point pt, Circle c){
	
	Vect v;
	double distance(makeVector({c.center_x, c.center_y}, pt, v));
	
	if(distance < (c.radius + getEpsilonZero())){ 
		return true; 
	}
	
	return false;
}


bool crossingCircles(Circle c1, Circle c2){
	
	Vect v;
	double distance(makeVector({c1.center_x, c1.center_y}, {c2.center_x, c2.center_y}, v));
	
	if(distance < (c1.radius + c2.radius - getEpsilonZero())){ 
		return true ;
	}
	
	return false;
}


void displayCircle(Circle c){
	
	if(size - c.center_x < c.radius or c.center_x + size < c.radius or 
	   size - c.center_y < c.radius or c.center_y + size < c.radius){
		
		vector<Point> equi_points(findEquiPoint({c.center_x, c.center_y}));
		
		for(auto point : equi_points){
			drawCercle(point.x, point.y, c.radius);
		}
			
	}else{
		drawCercle(c.center_x, c.center_y, c.radius);
	}
}


void displayDisque(Circle c){
	
	if(size - c.center_x < c.radius or c.center_x + size < c.radius or 
	   size - c.center_y < c.radius or c.center_y + size < c.radius){
		
		vector<Point> equi_points(findEquiPoint({c.center_x, c.center_y}));
		
		for(auto point : equi_points){
			drawDisque(point.x, point.y, c.radius);
		}
			
	}else{
		drawDisque(c.center_x, c.center_y, c.radius);
	}
}


void displayBase(Circle c, int couleur){
	
	if(size - c.center_x < c.radius or c.center_x + size < c.radius or 
	   size - c.center_y < c.radius or c.center_y + size < c.radius){
		
		vector<Point> equi_points(findEquiPoint({c.center_x, c.center_y}));
		
		for(auto point : equi_points){
			drawBase(point.x, point.y, couleur);
		}
			
	}else{
		drawBase(c.center_x, c.center_y, couleur);
	}
}


void displayRayonCom(Circle c){
		
	if(size - c.center_x < c.radius or c.center_x + size < c.radius or 
	   size - c.center_y < c.radius or c.center_y + size < c.radius){
		
		vector<Point> equi_points(findEquiPoint({c.center_x, c.center_y}));
		
		for(auto point : equi_points){
			drawCercleCom(point.x, point.y);
		}
			
	}else{
		drawCercleCom(c.center_x, c.center_y);
	}
}


void displayLine(Point start, Point end){
	
	Vect shorter_path;
	makeVector(start, end, shorter_path);
	
	if(abs(start.x + shorter_path.x) > dim_max or abs(start.y + shorter_path.y) > dim_max){
		Point newEnd({start.x + shorter_path.x, start.y + shorter_path.y});
		drawLine(start, newEnd);
		Point newStart = normalize(newEnd);
		drawLine(newStart, {newStart.x - shorter_path.x, newStart.y - shorter_path.y});
		
	}else{
		drawLine(start, end);
	}
}


void displayPaint(){
	
	paint();
}


void displayPoint(Point pos, int couleur){
	
	drawPoint(pos, couleur);
	
}


void displayCarre(Point pos, int couleur,bool rempli){

	drawCarre(pos, couleur, rempli);
}

void displayCroix(Point pos, int couleur){
	
	drawCroix(pos, couleur);
}

void displayTriangle(Point pos, int couleur,bool rempli){
	
	drawTriangle(pos,couleur,rempli);
}
