//fichier robot.h, THEVENOZ Julien et MANEFF Lucas

#ifndef ROBOT_H
#define ROBOT_H

#include "geomod.h"

struct Gis_info{
	Point center;
	double rad;
	double cap;
};


class Robot{
	
public:
	Robot(int uid, double dp, Point pos, Point goal, bool reached)
	:uid(uid), dp(dp), pos(pos), goal(goal), reached(reached) {};

	virtual ~Robot() {};
	
	Point getPos() const {return pos;}
	unsigned int getUid() const {return uid;};	
	bool get_visited() const {return visited;};
	double getDp() const {return dp;};
	bool getConnected() {return connected;};
	bool getReached() const {return reached;};
	Point getGoal() const {return goal;};
	std::vector<Robot*> get_voisins() const {return voisins;};
	void set_visited(bool v);
	void set_connected(bool c);
	void setDp(double new_dp);
	void setGoal(Point new_goal);
	void setReached(bool r);	
	void affiche_link_voisins() const;	
	void clear_voisins();
	void addVoisin(Robot* ptr_rob);
	void move_to(Point goal);
	bool on_goal();
	
	virtual void afficher_robot(bool link, bool range, int couleur) =0;
	virtual void save(std::ofstream& fichier) const =0;	
	virtual void setFound(bool f) {};
	virtual void set_gisement(Point center, double rad, double cap) {};	
	virtual void setRetour(bool r) {};	
	virtual void autonomous(Point center) {};		
	virtual bool dead() const;
	virtual bool getFound() const;
	virtual double getCap() const;
	virtual bool getRetour() const;
	virtual Gis_info getGis_info() const;
	
	
protected:
	int uid;
	double dp;
	Point pos;
	Point goal;
	bool reached;
	bool visited;
	bool connected;
	std::vector<Robot*> voisins;
};

class Prospector : public Robot{

public:
	Prospector(int uid, double dp, Point pos, Point goal, bool reached, 
			   bool retour, bool found, Point gisement={0,0}, 
			   double rad_gis=0, double cap_gis=0)
	:Robot(uid, dp, pos, goal, reached), retour(retour), found(found),
	 gisement(gisement), rad_gis(rad_gis), cap_gis(cap_gis) {}

	~Prospector() {};
	
	bool getFound() const override {return found;};
	Gis_info getGis_info() const override {return {gisement, rad_gis, cap_gis};};

	void setFound(bool f) override;
	bool dead() const override;
	void save(std::ofstream& fichier) const override;
	void set_gisement(Point center, double rad, double cap) override;
	void autonomous(Point center) override;	
	void finalStep(double d);
	void intermediateStep(double d, double alpha);
	void afficher_robot(bool link, bool range, int couleur) override;
	
private:
	bool retour;
	bool found;
	Point gisement;
	double rad_gis;
	double cap_gis;
};

class Forage : public Robot{

public:
	Forage(int uid, double dp, Point pos, Point goal, bool reached)
	:Robot(uid, dp, pos, goal, reached) {};
	
	~Forage() {};
	
	double getCap() const override;	
	bool dead() const override;
	void save(std::ofstream& fichier) const override;
	void autonomous(Point center) override;
	void afficher_robot(bool link, bool range, int couleur) override;
};

class Transport : public Robot{

public:
	Transport(int uid, double dp, Point pos, Point goal, bool reached, bool retour)
	:Robot(uid, dp, pos, goal, reached), retour(retour) {};

	~Transport() {};
	
	bool getRetour() const override {return retour;};
	void setRetour(bool r) override;
	bool dead() const override;
	void save(std::ofstream& fichier) const override;
	void autonomous(Point center) override;
	void afficher_robot(bool link, bool range, int couleur) override;
		
private:
	bool retour;	
};

class Communication : public Robot{

public:
	Communication(int uid, double dp, Point pos, Point goal, bool reached)
	:Robot(uid, dp, pos, goal, reached) {};

	~Communication() {};	
	
	bool dead() const override;
	void save(std::ofstream& fichier) const override;
	void autonomous(Point center) override;
	void afficher_robot(bool link, bool range, int couleur) override;
};

#endif


