//fichier simulation.h, THEVENOZ Julien et MANEFF Lucas

#ifndef SIMULATION_H
#define SIMULATION_H

#include "base.h"
#include "gisement.h"

class Simulation;

Simulation* get_sim_ptr();
void lecture_directe(std::string nom_fichier);


class Simulation{

public:
	Simulation()
	:existe(false) {}
	
	bool get_existe() const {return existe;};
	void lecture(std::string nom_fichier);
	void update(bool link, bool range);
	void faire_dessin(bool link, bool range) const;
	bool clear_simulation();
	void sauvegarde(std::string nom_fichier) const;
	std::vector<SimData> report_bases();
	
private:	
	std::vector<Gisement> gisement_tab;
	std::vector<Base> base_tab;
	bool existe;

	void afficher_tout(bool link, bool range) const;	
	void save_bases_et_robs(std::ofstream& fichier) const;
	void save_gisements(std::ofstream& fichier) const;	
	void clean_space(std::string& line) const;
	void clean_hash(std::string& line) const;
	bool decodage(std::string line);	
	void update_voisins(Base& base_1, Base& base_2);
	bool collisionGisement(double x, double y, double rad) const;
	bool collisionBase(double x, double y) const;
	void detruire_bases_pauvres();
};

#endif
