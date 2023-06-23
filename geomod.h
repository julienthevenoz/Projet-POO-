//fichier geomod.h, THEVENOZ Julien et MANEFF Lucas

#ifndef GEOMOD_H
#define GEOMOD_H

#include <vector>

struct Point{
	
	double x;
	double y;
};

struct Vect{
		
	double x;
	double y;
};

struct Circle{
	
	double center_x;
	double center_y;
	double radius;
};


double getSize();
void setSize(double new_size);
double getEpsilonZero();
bool equalZero(double p);
std::vector<Point> findEquiPoint(Point pt);
double normalize(double scalar);
Point normalize(Point pt);
double makeVector(Point start_point, Point end_point, Vect& smaller_vect);
double calculateNorm(Vect v);
bool pointsEqual(Point pt1, Point pt2);
bool inCircle(Point pt, Circle c);
bool crossingCircles(Circle c1, Circle c2);
void displayCircle(Circle c);
void displayDisque(Circle c);
void displayBase(Circle c, int couleur);
void displayRayonCom(Circle c);
void displayLine(Point start, Point end);
void displayPaint();
void displayPoint(Point pos, int couleur);
void displayCarre(Point pos, int couleur, bool rempli);
void displayCroix(Point pos, int couleur);
void displayTriangle(Point pos, int couleur,bool rempli);

#endif
