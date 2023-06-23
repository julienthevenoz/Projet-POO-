//fichier gisement.cc, THEVENOZ Julien et MANEFF Lucas
#include <iostream>
#include <fstream>
#include <vector>
#include "geomod.h"
#include "gisement.h"
#include "constantes.h"

using namespace std;

namespace{
	
	vector<Gisement*> gis_ptr_tab;
}


void initialize_gis_pointers(vector<Gisement>& gisement_tab){
	
	gis_ptr_tab = vector<Gisement*> (gisement_tab.size());
	
	if(gis_ptr_tab.size() == gisement_tab.size())
		cout << "YES"<<endl;
	
	for(size_t i(0); i<gisement_tab.size(); i++){
		gis_ptr_tab[i] = &(gisement_tab[i]);
		cout << gis_ptr_tab[i]->getX()<<" "<< gis_ptr_tab[i]->getY()<<" "
			 <<gis_ptr_tab[i]->getRad()<<" "<<gis_ptr_tab[i]->getCap()<<endl;		
	}
}


vector<Gisement*> getGisement_tab_ptr(){
	
	return gis_ptr_tab;
}


double verif_capacity(Point pos){
	
	double capacity(0);
	
	for(auto giz : gis_ptr_tab)
		if(inCircle(pos, {giz->getX(), giz->getY(),giz->getRad()}))
			capacity = giz->getCap();
			
	return capacity;	
}


void extraction(Point pos){
	
	for(auto giz : gis_ptr_tab)
		if(inCircle(pos, {giz->getX(), giz->getY(),giz->getRad()}))
			giz->extract_deltaR();
}


void Gisement::afficher() const{
	
	displayCircle({center.x, center.y, rad});	
	double init_cap(1000*(rad/rayon_min)*(rad/rayon_min));
	double ratio_actuel(capacity/init_cap);
	displayDisque({center.x, center.y, ratio_actuel*rad});
}


void Gisement::save(std::ostream& fichier) const{
	
	fichier<< center.x <<" "<< center.y <<" "<< rad <<" "<< capacity<<endl;
}


void Gisement::extract_deltaR(){
	
	capacity = capacity - deltaR;
}
