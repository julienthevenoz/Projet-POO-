//fichier base.h, THEVENOZ Julien et MANEFF Lucas

#ifndef BASE_H
#define BASE_H

#include "robot.h"

enum robot_type {PROSP, FOR, TRANSP, COM};

struct SimData;

class Base{

public:

	Base(Point center, double ressource, int nbP, int nbF, int nbT, int nbC);
	
	double getX() const	{return center.x;}
	double getY() const {return center.y;}
	double getRessources() {return ressource;};
	unsigned int get_taille_flotte() const {return flotte.size();};
	Robot* get_ptr_rob(unsigned int indice) const {return flotte[indice];};
	void clear_all_voisins();
	void connex_rec(std::vector<Robot*>& result, Robot* next);
	void connexion();
	void createRobot(double xb, double yb, robot_type type);
	void fix_rob(Robot* rob_ptr, double d);
	void fix_completely(Robot* rob_ptr);
	void save(std::ofstream& fichier) const;
	void clear_base();
	void afficher_base_et_rob(bool link, bool range) const;
	void update_robot();
	void discover_gisements(std::vector<Gis_info> tab);
	void robs_extract();
	void createProspector();
	void creation();
	void reset_colors();
	void maintenance();
	void update_transp(Robot* t_ptr);
	bool terminados() const;
	bool initialiserRobot(std::string line, int nbr);	
	bool robot_com_center();
	double d_from_farthest_for();
	std::vector<Point> set_com_net();
	std::vector<Robot*> parcours_connexion(Robot* depart);
	SimData give_info();
	
private:
	
	Point center;
	double ressource;
	int couleur;
	unsigned int nbP, nbF, nbT, nbC;
	unsigned int init_nbP, init_nbC;
	std::vector<Robot*> flotte;
	std::vector<Robot*> connected_robs;
	std::vector<Robot*> prospectors;
	std::vector<Robot*> forages;
	std::vector<Robot*> transporters;
	std::vector<Robot*> communications;
	std::vector<Point> net_com_pos;
	Robot* comm_robot_in_base;
	
	void deploy_com_net(int nb_rob=1);
	void creation_sub_method(int& robs_left);
	void closest_point_for_extraction(Point& but) const;
	bool verif_and_create_Prosp(std::istringstream& data);
	bool verif_and_create_Forage(std::istringstream& data);
	bool verif_and_create_Transport(std::istringstream& data);
	bool verif_and_create_Comm(std::istringstream& data);
	bool verif_uid(std::string line) const;
	bool verif_res_dispo(double cost);
	bool gisement_dispo(Point& gisement);
	int newUid();

	
};

struct SimData{
	
	unsigned nbP;
	unsigned nbF;
	unsigned nbT;
	unsigned nbC;
	double ressource;
	double ressource_p;
};

#endif
