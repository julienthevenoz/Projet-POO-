#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <gtkmm.h>
#include "geomod.h"

struct Couleur;

void setCr(const Cairo::RefPtr<Cairo::Context>& cr);
void paint();
void drawLine(Point start, Point end);
void drawDisque(double x, double y, double rad);
void drawBase(double x, double y, int nbr_c);
void drawPoint(Point p, int nbr_c);
void drawCarre(Point p, int nbr_c, bool rempli);
void drawCroix(Point p, int nbr_c);
void drawTriangle(Point p, int nbr_c,bool rempli);
void drawCercleCom(double x, double y);
void drawCercle(double x, double y, double rad);

struct Couleur{
	
	double r;
	double g;
	double b;	
};

#endif
