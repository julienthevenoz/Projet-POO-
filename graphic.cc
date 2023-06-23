#include "graphic.h"
#include "geomod.h"
#include "constantes.h"
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;


namespace{
	
	static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr); 
	
	vector<Couleur> palette = {{1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {1,0,1},
							   {0,1,1}, {0.5,0.5,0.5}, {0,0,0}, {0.93,0.51,0.93}}; 
}


void setCr(const Cairo::RefPtr<Cairo::Context>& cr){
	
	ptcr = &cr;
}


void paint(){
	
	Couleur c(palette[3]);
	
	(*ptcr)->save();
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	(*ptcr)->paint();
	(*ptcr)->restore();
	(*ptcr)->stroke();
	
}


void drawLine(Point start, Point end){
	
	Couleur c(palette[8]);
	
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	(*ptcr)->move_to(start.x, start.y);
	(*ptcr)->line_to(end.x, end.y);
	(*ptcr)->stroke();
}


void drawDisque(double x, double y, double rad){
	
	Couleur c(palette[7]);
	
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	(*ptcr)->arc(x, y, rad, 0, 2*M_PI);
	(*ptcr)->fill_preserve();
	(*ptcr)->stroke();
	
}

void drawBase(double x, double y, int nbr_c){
	
	Couleur c(palette[nbr_c]);
	
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	(*ptcr)->arc(x, y, 20*rayon_base, 0, 2*M_PI);
	(*ptcr)->stroke();
}


void drawPoint(Point p, int nbr_c){
	
	Couleur c(palette[nbr_c]);
	
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	(*ptcr)->arc(p.x, p.y, 2.5, 0, 2*M_PI);
	(*ptcr)->fill_preserve();
	(*ptcr)->stroke();
}


void drawCarre(Point p, int nbr_c, bool rempli){
	
	Couleur c(palette[nbr_c]);
	
	int arrete(5);
	
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	(*ptcr)->move_to(p.x-arrete, p.y+arrete);
	(*ptcr)->line_to(p.x+arrete, p.y+arrete);
	(*ptcr)->line_to(p.x+arrete, p.y-arrete);
	(*ptcr)->line_to(p.x-arrete, p.y-arrete);
	(*ptcr)->line_to(p.x-arrete, p.y+arrete);
	if(rempli)
		(*ptcr)->fill_preserve();
	(*ptcr)->stroke();
}


void drawCroix(Point p, int nbr_c){
	
	Couleur c(palette[nbr_c]);
	
	int arrete(8);
	
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	(*ptcr)->move_to(p.x-arrete, p.y+arrete);
	(*ptcr)->line_to(p.x+arrete, p.y-arrete);
	
	(*ptcr)->move_to(p.x+arrete, p.y+arrete);
	(*ptcr)->line_to(p.x-arrete, p.y-arrete);

	(*ptcr)->stroke();	
}


void drawTriangle(Point p, int nbr_c, bool rempli){
	
	Couleur c(palette[nbr_c]);
	
	double base(15);
	double hauteur(sqrt(pow(base, 2) + pow(base/2,2))); //h = sqrt(b²+(b/2)²)
	
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	(*ptcr)->move_to(p.x, p.y+hauteur/2);
	(*ptcr)->line_to(p.x+base/2, p.y-hauteur/2);
	(*ptcr)->line_to(p.x-base/2, p.y-hauteur/2);
	(*ptcr)->line_to(p.x, p.y+hauteur/2);
	if(rempli)
		(*ptcr)->fill_preserve();
	(*ptcr)->stroke();
}
	

void drawCercleCom(double x, double y){
	
	Couleur c(palette[6]);
	int n(100); 				
	double pas(2*M_PI/n);
	
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	
	for(int i(0); i < n; i+=2){
		
		(*ptcr)->arc(x, y, rayon_comm, i*pas, (i+1)*pas);
		(*ptcr)->stroke();
	}
	
}


void drawCercle(double x, double y, double rad){
	
	Couleur c(palette[7]);
	
	(*ptcr)->set_source_rgb(c.r, c.g, c.b);
	(*ptcr)->arc(x, y, rad, 0, 2*M_PI);
	(*ptcr)->stroke();
	
}
