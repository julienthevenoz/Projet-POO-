//fichier gisement.h, THEVENOZ Julien et MANEFF Lucas

#ifndef GISEMENT_H
#define GISEMENT_H

class Gisement;

void initialize_gis_pointers(std::vector<Gisement>& gisement_tab);
std::vector<Gisement*> getGisement_tab_ptr();
double verif_capacity(Point pos);
void extraction(Point pos);


class Gisement{
	
public:
	Gisement(Point center, double rad, double capacity)
	:center(center), rad(rad), capacity(capacity) {};
	
	double getX() const		{return center.x;};
	double getY() const		{return center.y;};
	double getRad() const	{return rad;};
	double getCap() const 	{return capacity;};
	void afficher() const;
	void save(std::ostream& fichier) const;
	void extract_deltaR();
	
private:	
	Point center;
	double rad;
	double capacity;
};

#endif
