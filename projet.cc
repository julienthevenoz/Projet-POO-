//fichier projet.cc, THEVENOZ Julien et MANEFF Lucas

#include <gtkmm.h>
#include "simulation.h"
#include "gui.h"

using namespace std;

int main(int argc, char* argv[]){
	
	
	auto app = Gtk::Application::create();		

	Gui gui;
	
	if(argc==2)
		lecture_directe(argv[1]);
		
	return app->run(gui);
}
