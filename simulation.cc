//fichier simulation.cc THEVENOZ Julien et MANEFF Lucas

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "simulation.h"
#include "constantes.h"
#include "message.h"
#include "geomod.h"

using namespace std;
using namespace message;

namespace{
	
	Simulation simulation;
	enum Etat_lecture {NBG, GISEMENT, NBB, BASE, ROBOT};
	int etat(NBG);
}


Simulation* get_sim_ptr(){
	
	return &simulation;
}


void lecture_directe(std::string  nom_fichier){
	
	simulation.lecture(nom_fichier);
}


void Simulation::lecture(string nom_fichier){
	
	if(existe)
		clear_simulation();
	
	string line;
	ifstream fichier(nom_fichier);
	setSize(dim_max);
		
	if(!fichier.fail()){	
		
		while (getline(fichier>>ws, line)){
			
			clean_hash(line);
			if(line.size()==0)
				continue;
				
			if(!(decodage(line)))
				return;
		}			
	}else{
		cout << "Erreur"<<endl;
		clear_simulation();
		return;
	}
	
	cout << success();
	existe = true;
}


void Simulation::update(bool link, bool range){
	
	static unsigned count(0);
	cout << "simulation, étape : " << count++ << endl;
		
	for(size_t i(0); i<base_tab.size(); i++)
			base_tab[i].clear_all_voisins();
	
	for(size_t i(0); i<base_tab.size(); i++)
		for(size_t j(0); j<base_tab.size(); j++)
			update_voisins(base_tab[i], base_tab[j]);
							
	for(size_t i(0); i < base_tab.size(); i++)
		if(not(base_tab[i].terminados())){
			base_tab[i].connexion();
			base_tab[i].maintenance();
			base_tab[i].creation();
			base_tab[i].update_robot();
			base_tab[i].robs_extract();
		}
		
	detruire_bases_pauvres();
}


void Simulation::faire_dessin(bool link, bool range) const{
	
	afficher_tout(link, range);
}


bool Simulation::clear_simulation(){
	
	gisement_tab.clear();

	//reset_colors() est appelé sur la première base mais pourrait être appelé
	//sur n'importe quelle base en réalité
	base_tab[0].reset_colors();		
		
	for(size_t i(0); i<base_tab.size(); i++)
		base_tab[i].clear_base();
	base_tab.clear();
	
	existe = false;
	etat = NBG;
						//clear simulation retourne toujours faux : c'est utile pour
	return false;		//réduire la taille de la fonction décodage
}


void Simulation::sauvegarde (std::string nom_fichier) const{
	cout<<"La fonction de sauvegarde a été appelée sur le fichier "<<nom_fichier<<endl;
	
	ofstream fichier(nom_fichier);	
	
	if(!fichier.fail()){
		save_gisements(fichier);
		fichier<<endl;
		save_bases_et_robs(fichier);
	}
	
	fichier.close();
}


vector<SimData> Simulation::report_bases(){
	
	vector<SimData> report(base_tab.size());
	
	for(size_t i(0); i<base_tab.size(); i++){
		report[i]=base_tab[i].give_info();
		report[i].ressource_p = (report[i].ressource/finR)*100;
	}
		
	return report;
}


void Simulation::afficher_tout(bool link, bool range) const{
	
	for(size_t i(0); i<gisement_tab.size(); i++){	
		gisement_tab[i].afficher();	
	}
	
	for(size_t i(0); i<base_tab.size(); i++)
		base_tab[i].afficher_base_et_rob(link,range);
}


void Simulation::save_gisements (std::ofstream& fichier) const{
	
	fichier<<gisement_tab.size()<<endl;
	for(size_t i(0); i<gisement_tab.size(); i++)
		gisement_tab[i].save(fichier);
	
}


void Simulation::save_bases_et_robs(ofstream& fichier) const{
	
	fichier<<base_tab.size()<<endl;
	for(size_t i(0); i<base_tab.size(); i++)
		base_tab[i].save(fichier);
}


void Simulation::clean_space(string& line) const{
	
	for(size_t i (line.size()-1); i >=0 ; i--){	
			
		if(line[i] == ' '){
			line.replace(i, 1, "");
			
		}else{
			
			break;
		}
	}
}


void Simulation::clean_hash(string& line) const{
	
	if(line.find('#') == string::npos){
		clean_space(line);
		
	}else{		
				
		line.replace(line.find('#'), line.size() - line.find('#'), "");
		
		if(line.size()!=0)
			clean_space(line);
	}	
}


bool Simulation::decodage(string line){	
		
	istringstream data(line);	
	static int i(0), j(0), total_gisement(0), total_base(0), total_robot(0);
	string name;
	
	switch(etat) 
	{
		case NBG: 
		{
			if(!(data >> total_gisement))
				return clear_simulation();
			else
				i = 0;
			if(total_gisement == 0)
				etat=NBB; 
			else       
				etat = GISEMENT; 
		}break;

		case GISEMENT: 
		{	
			double x(0), y(0), radius(0), cap(0);
			if(!(data >> x >> y >> radius >> cap) or collisionGisement(x, y, radius))
				return clear_simulation();	
			else{
				Gisement g({x,y},radius,cap);
				gisement_tab.push_back(g);
				++i;
			}
			if(i == total_gisement){
				initialize_gis_pointers(gisement_tab);
				etat = NBB ;
			}
		}break;

		case NBB: 
		{
			if(!(data >> total_base))
				return clear_simulation();		
			etat = BASE;  
		}break;

		case BASE: 	
		{				
			double x(0), y(0), ressource(0);										
			int nbP(0), nbF(0), nbT(0), nbC(0);
		
			if(!(data>>x>>y>>ressource>>nbP>>nbF>>nbT>>nbC) or collisionBase(x,y)) 
				return clear_simulation();
			else{
				if(nbC == 0){
					cout << missing_robot_communication(x, y);
					return clear_simulation();
				}
				Base b({x,y}, ressource, nbP, nbF, nbT, nbC);
				base_tab.push_back(b);	
				j = 0;		
				total_robot = nbP + nbF + nbT + nbC;
				etat = ROBOT;
			}		
		}break;

		case ROBOT: 
		{
			if(not(base_tab[base_tab.size()-1].initialiserRobot(line, i)))
				return clear_simulation();												
						
			j++;			
			if(j == total_robot){
				if(!(base_tab[base_tab.size()-1].robot_com_center()))
					return clear_simulation();
				
				etat = BASE;
			}	
		}break;
	}	
	return true;
}


void Simulation::update_voisins(Base& base_1, Base& base_2){	
		 	
	for(size_t i(0); i<base_1.get_taille_flotte(); i++){
		Robot* rob_1(base_1.get_ptr_rob(i));
		for(size_t j(0); j<base_2.get_taille_flotte(); j++){
			Robot* rob_2(base_2.get_ptr_rob(j));
			if(rob_1 != rob_2){				
				Circle c1 = {rob_1->getPos().x, rob_1->getPos().y, rayon_comm};
				if(inCircle(rob_2->getPos(),c1))
					rob_1->addVoisin(rob_2);
			}
		}
	}
}


bool Simulation::collisionGisement(double x, double y, double rad) const {
	
	for(auto gis : gisement_tab){
		
		if(crossingCircles({x, y, rad}, {gis.getX(), gis.getY(), gis.getRad()})){
			cout << field_superposition(x, y, gis.getX(), gis.getY());
			return true;
		}
	}
	
	return false;
}


bool Simulation::collisionBase(double x, double y) const {
	
	for(auto base : base_tab){
		
		if(crossingCircles({x,y,rayon_base}, {base.getX(),base.getY(),rayon_base})){
			cout << base_superposition(x, y, base.getX(), base.getY());
			return true;
		}
	}
	
	for(auto gis : gisement_tab){
		
		if(crossingCircles({x,y,rayon_base}, {gis.getX(),gis.getY(),gis.getRad()})){
			cout << base_field_superposition(x, y, gis.getX(), gis.getY());
			return true;
		}
	}
	
	return false;
}


void Simulation::detruire_bases_pauvres(){
	
	for(size_t i(0); i<base_tab.size();i++)
		if(base_tab[i].getRessources() <= 0){
			base_tab[i].clear_base();	
			base_tab.erase(base_tab.begin()+i, base_tab.begin()+(i+1));
			cout << "RIP base "<<(base_tab.begin()+i)->getX()<<" "
				 <<(base_tab.begin()+i)->getY()<<endl;
		}	
}
