//fichier base.cc, THEVENOZ Julien et MANEFF Lucas

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <thread>
#include "base.h"
#include "geomod.h"
#include "gisement.h"
#include "message.h"
#include "constantes.h"

using namespace std;
using namespace message;

namespace {
	
	int max_prosp = 10;
	int c(0);
}

Base::Base(Point center, double ressource, int nbP, int nbF, int nbT, int nbC)
: center(center), ressource(ressource), 
  nbP(nbP), nbF(nbF), nbT(nbT), nbC(nbC), init_nbP(nbP), init_nbC(nbC)
	{
		couleur = c;
		
		if(c >= 5){
			c = 0;		
		}else{
			c++;
		}
		net_com_pos = set_com_net();
	}
	
	
void Base::clear_all_voisins(){
	
	for(auto ptr_rob : flotte)
		ptr_rob->clear_voisins();	
}


void Base::connex_rec(vector<Robot*>& result, Robot* next){
	
	next->set_visited(true);
	result.push_back(next);
	
	const vector<Robot*> voisins_de_next = next-> get_voisins();
	
	for(unsigned i(0); i < voisins_de_next.size(); ++i)
		if(not(voisins_de_next[i]->get_visited()))
			connex_rec(result, voisins_de_next[i]);
}


void Base::connexion(){
	
	connected_robs.clear();
	
	for(auto rob_ptr : flotte)
		rob_ptr-> set_connected(false);	
		
	connected_robs = parcours_connexion(comm_robot_in_base);
	
	for(auto rob_ptr : connected_robs)
		rob_ptr -> set_connected(true);	
}


void Base::createRobot(double xb, double yb, robot_type type){
	
	int uid(newUid());
	
	switch(type){
		
		case PROSP:
		{
			Prospector* rob = new Prospector(uid, 0, {center.x, center.y}, {xb, yb},
											 false, false,false, {0, 0}, 0, 0);
			prospectors.push_back(rob);
			flotte.push_back(rob);
			nbP++;
			ressource -= cost_prosp;
											 
		}break;
		
		case FOR:
		{
			Forage* rob = new Forage(uid, 0, {center.x, center.y}, {xb, yb}, false);
			forages.push_back(rob);
			flotte.push_back(rob);
			nbF++;
			ressource -= cost_forage;
			
		}break;
		
		case TRANSP:
		{
			Transport* rob = new Transport(uid, 0, {center.x, center.y},
										   {xb, yb}, false, false);
			transporters.push_back(rob);
			flotte.push_back(rob);
			nbF++;
			ressource -= cost_transp;
			
		}break;
		
		case COM:
		{
			Communication* rob = new Communication(uid, 0, {center.x, center.y},
												   {xb, yb}, false);
			communications.push_back(rob);
			flotte.push_back(rob);
			nbC++;
			ressource -= cost_com;
			
		}break;
	}	
}


void Base::fix_rob(Robot* rob_ptr, double d){
	
	ressource -= (d * cost_repair);
	rob_ptr->setDp(rob_ptr->getDp() - d);
}

void Base::fix_completely(Robot* rob_ptr){
	
	ressource -= (rob_ptr->getDp() * cost_repair);
	rob_ptr->setDp(0);
	
}


void Base::save(std::ofstream& fichier) const {
	
	fichier<<center.x<<" "<<center.y<<" "<<ressource<<" "<<nbP<<" "<<nbF<<" "
		   <<nbT<<" "<<nbC<<endl;
			
	for(auto p : prospectors)
		p->save(fichier);
	
	for(auto f : forages)
		f->save(fichier);
			
	for(auto t : transporters)
		t->save(fichier);
			
	for(auto c : communications)
		c->save(fichier);
				
	fichier<<endl;
}


void Base::clear_base(){
	
	for(auto rob_ptr : flotte)
		delete rob_ptr;
		
	flotte.clear();
	prospectors.clear();
	forages.clear();
	transporters.clear();
	communications.clear();
}


void Base::afficher_base_et_rob(bool link, bool range) const{
		
	displayBase({center.x,center.y, rayon_base}, couleur);
	
	for(auto rob : flotte)
		rob->afficher_robot(link,range, couleur);
}


void Base::update_robot(){
	
	for(size_t i(0); i < flotte.size(); i++){
		
		if(typeid(*flotte[i]) == typeid(Transport))
			update_transp(flotte[i]);
		
		if(not flotte[i]->dead())
			flotte[i]->autonomous(center);
	}	
}


void Base::discover_gisements(std::vector<Gis_info> tab){
	
	for(auto rob_ptr : flotte)
	
		if(typeid(*rob_ptr) == typeid(Prospector))
			if(rob_ptr->on_goal())
			
				for(auto giz : tab)
				
					if(inCircle(rob_ptr->getPos(), {giz.center.x, 
					   giz.center.y,giz.rad})){
						   		
						rob_ptr->setFound(true);
						rob_ptr->set_gisement(giz.center, giz.rad, giz.cap);
					}
}


void Base::robs_extract(){

	for(auto f : forages){
		
		if(f->getReached() and (f->getCap() > deltaR)){
			
			bool forer_une_seule_fois_par_robot(true);
			for(auto t : transporters){
				
				if(forer_une_seule_fois_par_robot){
					if(pointsEqual(f->getPos(), t->getPos())){
						
						forer_une_seule_fois_par_robot = false;
						t->setRetour(true);			
						extraction(f->getPos());		
					}
				}
			}
		}
	}
}


void Base::createProspector(){
	
	double pas(M_PI/max_prosp);
	int n(prospectors.size());
	
	double angle(n*pas);
	double xb(center.x + cos(angle) * 10);
	double yb(center.y + sin(angle) * 10);
	
	createRobot(xb, yb, PROSP);
}


void Base::creation(){
	
	int robs_left(max_robots);
	
	for(unsigned i(0); i < max_robots; i++)
		if(robs_left > 0)
			creation_sub_method(robs_left);			
}


void Base::reset_colors(){	
	
	c = 0;
}


void Base::maintenance(){
	
	for(size_t i(0); i < transporters.size(); i++){
		Robot* t_ptr(transporters[i]);
		
		if(t_ptr->getConnected()){
			if(inCircle(t_ptr->getPos(), {center.x, center.y, rayon_base})){
					
					fix_completely(t_ptr);
			}
		}
	}
	
	for(size_t i(0); i < prospectors.size(); i++){
		Robot* p_ptr(prospectors[i]);
		
		if(p_ptr->getConnected()){
			if(inCircle(p_ptr->getPos(), {center.x, center.y, rayon_base})){
				
				if(maxD_prosp - p_ptr->getDp() < 2*sqrt(2)*dim_max){
					fix_rob(p_ptr, 2*sqrt(2)*dim_max);
				}
			}
		}
	}
}


void Base::update_transp(Robot* t_ptr){
	
	if(t_ptr->getConnected()){
		if(t_ptr->getRetour() and t_ptr->getReached() 
		   and pointsEqual(t_ptr->getPos(), center)){
			
			ressource += deltaR;
			
			if(ressource > finR)
				ressource = finR;
		
			double r_max(0);
			int i_max;
			
			for(size_t i(0); i < forages.size(); i++){
				Robot* f_ptr(forages[i]);
				
				if(f_ptr->getConnected()){
					
					if(f_ptr->getCap() > r_max){
						r_max = f_ptr->getCap();
						i_max = i;
					}
				}
			}
			t_ptr->setRetour(false);
			t_ptr->setReached(false);
			t_ptr->setGoal({forages[i_max]->getPos().x, forages[i_max]->getPos().y});
		}
	}
}


bool Base::terminados() const{
	
	return (ressource >= finR);
}


bool Base::initialiserRobot(std::string line, int nbr){
	
	bool created(false);
	bool everything_okay(true);
	
	istringstream data(line);
	everything_okay = verif_uid(line);
	
	if(not(everything_okay))
		return false;
	
	if(not(created))
		created = verif_and_create_Prosp(data);
		
	if(not(created))
		created = verif_and_create_Forage(data);
		
	if(not(created))
		created = verif_and_create_Transport(data);
		
	if(not(created))
		created = verif_and_create_Comm(data);	
		
	return true;
}


bool Base::robot_com_center() {
	
	bool in_center(false);
	
	if(flotte.size() == 1){
		if(pointsEqual(center, flotte[0]->getPos())){
			in_center = true;
			comm_robot_in_base = flotte[0];
		}
	}else{	
		for(size_t i(flotte.size()-1); i > flotte.size()-1-nbC; i--){
			
			if(pointsEqual(center, flotte[i]->getPos())){
				in_center= true;
				comm_robot_in_base = flotte[i];
			}
		}
	}
	
	if(not(in_center)){
		cout << missing_robot_communication(center.x, center.y);{
				cout << "ERREUR6"<<endl;
				return false;
			}
	}	
	return true;
}


double Base::d_from_farthest_for(){
	
	double d(0);
	double d_max(0);
	Vect v;
	
	for(size_t i(0); i < forages.size(); i++){
		Robot* f_ptr(forages[i]);
		if(f_ptr->getConnected()){
			d = makeVector(center, f_ptr->getGoal(), v);
			if(d > d_max){
				d_max = d;
			}
		}
	}	
	
	return d_max;
}


vector<Point> Base::set_com_net(){
	
	vector<Point> com_pos_tab;
	int d(2*dim_max/7);
	
	for(int i(-3); i <= 3; i++){
		for(int j(-3); j <= 3; j++){
			
			if(!(i == 0 and j == 0)){
				
				Point goal;
				goal.x = center.x + i*d;
				goal.y = center.y + j*d;
			
				com_pos_tab.push_back(normalize(goal));
			}
		}
	}
	
	return com_pos_tab;
}


vector<Robot*> Base::parcours_connexion(Robot* depart){
	
	vector<Robot*> result;
	
	for(auto rob_ptr : flotte)
		rob_ptr->set_visited(false);
		
	connex_rec(result, depart);
	
	return result;
}


SimData Base::give_info(){
	
	return {nbP,nbF,nbT,nbC,ressource, 0};
}


void Base::deploy_com_net(int nb_rob){
	
	for(int i(0); i < nb_rob; i++){
		int n(communications.size()-init_nbC);
		createRobot(net_com_pos[n-1].x, net_com_pos[n-1].y, COM);
	}
}


void Base::creation_sub_method(int& robs_left){
	
	unsigned int max_comm(init_nbC+49), max_prosp(init_nbP+10);
	double reserve(201), minimum(1);
	
	if(nbC < max_comm){
		if(verif_res_dispo(reserve+cost_com)){
			deploy_com_net();
			robs_left -= 1;
		}
	}else{
		Point but({0,0});
		if(gisement_dispo(but))
			if(verif_res_dispo(minimum+cost_forage+cost_transp) and (robs_left>=2)){
				createRobot(but.x, but.y, FOR);
				createRobot(but.x, but.y, TRANSP);
				robs_left -= 2;
			}
			
		if((nbP < max_prosp)and(robs_left>0) and verif_res_dispo(reserve+cost_prosp)){
			createProspector();
			robs_left -= 1;			
		}
	}
}


void Base::closest_point_for_extraction(Point& but) const{
	
	Gis_info giz;
	
	for(auto r : connected_robs)	
		if(typeid(*r) == typeid(Prospector))
			if(pointsEqual(but, r->getGis_info().center)){
				giz = r->getGis_info();
				break;
			}
	
	double x(0),y(0),R(0),alpha(0);
	x = center.x - giz.center.x;
	y = center.x - giz.center.y;
	R = giz.rad;
	alpha = atan2(y, x);
	
	but.x = giz.center.x + R*cos(alpha);
	but.y = giz.center.y + R*sin(alpha);
}


bool Base::verif_and_create_Prosp(std::istringstream& data){
	
	if((nbP == 0) or (nbP <= flotte.size()))
		return false;
			
	int uid(0);
	double dp(0), rad_gis(0), cap_gis(0), x(0), y(0);
	double x_go(0), y_go(0), x_gis(0), y_gis(0);
	bool reached(0), retour(0), found(0);
	
	data >> boolalpha >> uid >> dp >> x >> y >> x_go >> y_go>> reached >> 
	retour >> found >> x_gis >> y_gis >> rad_gis >> cap_gis;
	
	Prospector* p = new Prospector(uid, dp, {x,y}, {x_go, y_go}, reached, retour,
								   found, {x_gis, y_gis}, rad_gis, cap_gis);
	flotte.push_back(p);
	prospectors.push_back(p);
		
	return true;
}


bool Base::verif_and_create_Forage(std::istringstream& data){
	
	if((nbF == 0) or (nbF<=flotte.size()-nbP))
		return false;
		
	int uid(0);
	double dp(0), x(0), y(0), x_go(0), y_go(0);
	bool reached(0);
	
	data >> boolalpha >> uid >> dp >> x >> y >> x_go >> y_go >> reached;
	
	Forage* f = new Forage(uid, dp, {x,y}, {x_go, y_go}, reached);
	flotte.push_back(f);
	forages.push_back(f);
		
	return true ;
}


bool Base::verif_and_create_Transport(std::istringstream& data){
	
	if((nbT == 0) or (nbT <= flotte.size()-nbP-nbF))
		return false;
		
	int uid(0);
	double dp(0), x(0), y(0), x_go(0), y_go(0);
	bool reached(0), retour(0);
	
	data >> boolalpha >> uid >> dp >> x >> y >> x_go >> y_go >> reached >> retour;
	
	Transport* t = new Transport(uid, dp, {x,y}, {x_go, y_go}, reached, retour);
	flotte.push_back(t);
	transporters.push_back(t);
	
	return true;
}


bool Base::verif_and_create_Comm(std::istringstream& data){
	
	if((nbC == 0) or (nbC <= flotte.size()-nbP-nbF-nbT))
		return false;
		
	int uid(0);
	double dp(0), x(0), y(0), x_go(0), y_go(0);
	bool reached(0);
	
	data >> boolalpha >> uid >> dp >> x >> y >> x_go >> y_go >> reached;
	
	Communication* c = new Communication(uid, dp, {x,y}, {x_go, y_go}, reached);
	flotte.push_back(c);
	communications.push_back(c);

	return true ;
}


bool Base::verif_uid(string line) const{
	
	istringstream data(line);
	unsigned int uid(0);
	
	data >> uid;
		
	for(auto rob : flotte){
		
		if(uid == rob->getUid()){
			cout<<identical_robot_uid(uid);
			return false;	
		}
	}
	return true;
}


bool Base::verif_res_dispo(double cost){
	
	if(cost<=ressource)
		return true;
	else
		return false;
}


bool Base::gisement_dispo(Point& but){

	//on créé un tableau de tous les gisements qu'on connait
	vector<Gis_info> tab;
	
	for(auto r : connected_robs)	
		if(typeid(*r) == typeid(Prospector))
			if(r->getFound()){
				tab.push_back(r->getGis_info());
			}
			
	//on regarde si un forage l'exploite déjà	
	for(auto g : tab){
		bool exploited(false);
		for(auto f : forages){
			if(f->getConnected()){
				if(inCircle(f->getGoal(), {g.center.x, g.center.y, g.rad})){
					exploited = true;
					break;
				}
			}
		}
		
		if(not(exploited)){
			if(g.cap < deltaR)
				return false;

			but = g.center;		
			closest_point_for_extraction(but);
			return true;
		}
	}			
	return false;
}


int Base::newUid(){
	
	bool found(false);
	unsigned int newUid(flotte.size());
	
	while(not found){
		
		newUid++;
		found = true;
		
		for(auto rob_ptr : flotte){
			
			if(newUid == rob_ptr->getUid()){
				found = false;
			}	
		}
	}
	return newUid;
}
