//fichier robot.cc, THEVENOZ Julien et MANEFF Lucas
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "robot.h"
#include "constantes.h"
#include "gisement.h"

using namespace std;

void Robot::set_visited(bool v){
	
	visited = v;
}


void Robot::set_connected(bool c){
	
	connected = c;
}


void Robot::setDp(double new_dp){

	dp = new_dp;
}


void Robot::setGoal(Point new_goal){
	
	goal.x = new_goal.x;
	goal.y = new_goal.y;
}


void Robot::setReached(bool r){
	
	reached = r;
}


void Robot::affiche_link_voisins() const{
	
	for(auto voiz : voisins)
		displayLine(pos, voiz->getPos());	
}


void Robot::clear_voisins(){
	
	voisins.clear();	
}


void Robot::addVoisin(Robot* ptr_rob){
	
	voisins.push_back(ptr_rob);
}


void Robot::move_to(Point goal){
	
	pos.x = goal.x;
	pos.y = goal.y;
}


bool Robot::on_goal() {
	
	if(pointsEqual(pos, goal)){
		reached = true;
		return true;
	}else{
		return false;
	}
}


bool Robot::dead() const{
							//ce corps sert uniquement à éviter 
	return false;			//un warning pour la méthode virtuelle	
}


bool Robot::getFound() const{
							//ce corps sert uniquement à éviter 
	return false;			//un warning pour la méthode virtuelle	
}


double Robot::getCap() const{
							//ce corps sert uniquement à éviter 
	return false;			//un warning pour la méthode virtuelle	
}


bool Robot::getRetour() const{
							//ce corps sert uniquement à éviter 
	return false;			//un warning pour la méthode virtuelle	
}


Gis_info Robot::getGis_info() const{
							//ce corps sert uniquement à éviter 	
	return {{0,0},0,0};		//un warning pour la méthode virtuelle	
}


void Prospector::setFound(bool f){
	
	found = f;
}


bool Prospector::dead() const{
	
	if(dp >= maxD_prosp)
		return true;
	
	return false;
}


void Prospector::save(std::ofstream& fichier) const{
	
	fichier<<uid<<" "<<dp<<" "<<pos.x<<" "<<pos.y<<" "<<goal.x<<" "<<goal.y<<" "
		   <<reached<<" "<<retour<<" "<<found<<" "<<gisement.x<<" "<<gisement.y
		   <<" "<<rad_gis<<" "<<cap_gis<<"    #Prospector"<<endl;
}


void Prospector::set_gisement(Point center, double rad, double cap){
	
	gisement = center;
	rad_gis = rad;
	cap_gis = cap;
}


void Prospector::autonomous(Point center){	
	
	if(found and reached and pointsEqual(pos, center)){
		found = false;
		reached = false;
		
		double n(rand() % 360);
		double alpha((n/360)*2*M_PI);
		
		goal.x = pos.x + cos(alpha)*20;
		goal.y = pos.y + sin(alpha)*20;		
	}	
	vector<Gisement*> gis_tab_ptr = getGisement_tab_ptr();
	
	for(auto gis_ptr : gis_tab_ptr){
		if(inCircle(pos, {gis_ptr->getX(), gis_ptr->getY(), gis_ptr->getRad()})){
			found = true;
			gisement = {gis_ptr->getX(), gis_ptr->getY()};
			rad_gis = gis_ptr->getRad();
			cap_gis = gis_ptr->getCap();
		}
	}	
	Vect path_vect;
	double d(makeVector(pos, goal, path_vect));
	double alpha(atan2(path_vect.y, path_vect.x));
	
	if(found){
		goal.x = center.x;
		goal.y = center.y;
		reached = false;
	}else{
		goal.x = pos.x + cos(alpha)*20;
		goal.y = pos.y + sin(alpha)*20;
		reached = false;
	}
	
	if(d < deltaD)
		finalStep(d);
	else
		intermediateStep(d, alpha);	
}


void Prospector::finalStep(double d){
	
	dp += d;
	move_to(normalize(goal));
	reached = true;
}


void Prospector::intermediateStep(double d, double alpha){
	
	dp += deltaD;
	Point intermediateGoal({pos.x + cos(alpha)*deltaD, pos.y + sin(alpha)*deltaD});
	move_to(normalize(intermediateGoal));
}


void Prospector::afficher_robot(bool link, bool range, int couleur){
	
	displayTriangle(pos,couleur,found);
	
	if(range)	
		displayRayonCom({pos.x, pos.y, rayon_comm});
	
	if(link)
		affiche_link_voisins();	
}


double Forage::getCap() const{
	
	double capacity (verif_capacity(pos));
	
	return capacity;
}


bool Forage::dead() const{
	
	if(dp >= maxD_forage){
		return true;
	}
	
	return false;
}


void Forage::save(std::ofstream& fichier) const{
	
	fichier<<uid<<" "<<dp<<" "<<pos.x<<" "<<pos.y<<" "<<goal.x<<" "<<goal.y<<" "
		   <<reached<<"    #Forage"<<endl;	
}


void Forage::autonomous(Point center){
	
	Vect path_vect;
	
	double d(makeVector(pos, goal, path_vect));
	double alpha(atan2(path_vect.y, path_vect.x));
	
	if(d < deltaD){
		dp += d;
		move_to(normalize(goal));
		reached = true;
	}else{
		dp += deltaD;
		Point intermediateGoal({pos.x + cos(alpha)*deltaD, pos.y + sin(alpha)*deltaD});
		move_to(normalize(intermediateGoal));
	}	
}


void Forage::afficher_robot(bool link, bool range, int couleur){
	
	displayCroix(pos, couleur);
	
	if(range)	
		displayRayonCom({pos.x, pos.y, rayon_comm});
	
	if(link)
		affiche_link_voisins();	
}


void Transport::setRetour(bool r){
	
	retour = r;
}


bool Transport::dead() const{
	
	if(dp >= maxD_transp){
		return true;
	}
	
	return false;
}


void Transport::save(std::ofstream& fichier) const{
	
	fichier<<uid<<" "<<dp<<" "<<pos.x<<" "<<pos.y<<" "<<goal.x<<" "<<goal.y<<" "
		   <<reached<<" "<<retour<<"    #Transport"<<endl;
}


void Transport::autonomous(Point center){
	
	if(retour){
		goal.x = center.x;
		goal.y = center.y;
		reached = false;
	}
	
	Vect path_vect;
	
	double d(makeVector(pos, goal, path_vect));
	double alpha(atan2(path_vect.y, path_vect.x));
	
	if(d < deltaD){
		dp += d;
		move_to(normalize(goal));
		reached = true;
	}else{
		dp += deltaD;
		Point intermediateGoal({pos.x + cos(alpha)*deltaD, pos.y + sin(alpha)*deltaD});
		move_to(normalize(intermediateGoal));
	}
	
}


void Transport::afficher_robot(bool link, bool range, int couleur){
	
	displayCarre(pos, couleur, retour);
	
	if(range)	
		displayRayonCom({pos.x, pos.y, rayon_comm});
	
	if(link)
		affiche_link_voisins();	
}


bool Communication::dead() const{
	
	if(dp >= maxD_com){
		return true;
	}
	
	return false;
}


void Communication::save(std::ofstream& fichier) const{
	fichier<<uid<<" "<<dp<<" "<<pos.x<<" "<<pos.y<<" "<<goal.x<<" "<<goal.y<<" "
		   <<reached<<"    #Communication"<<endl;
	
}


void Communication::autonomous(Point center){
	
	Vect path_vect;
	
	double d(makeVector(pos, goal, path_vect));
	double alpha(atan2(path_vect.y, path_vect.x));
	
	if(d < deltaD){
		dp += d;
		move_to(normalize(goal));
		reached = true;
	}else{
		dp += deltaD;
		Point intermediateGoal({pos.x + cos(alpha)*deltaD, pos.y + sin(alpha)*deltaD});
		move_to(normalize(intermediateGoal));
	}
}


void Communication::afficher_robot(bool link, bool range, int couleur){
	
	displayPoint(pos, couleur);
	
	if(range)	
		displayRayonCom({pos.x, pos.y, rayon_comm});
	
	if(link)
		affiche_link_voisins();	
}
