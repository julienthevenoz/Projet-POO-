#include <iostream>
#include <chrono>
#include <thread>
#include "simulation.h"
#include "gui.h"
#include "graphic.h"
#include "constantes.h"

using namespace std;

namespace{
	
	constexpr unsigned taille_dessin(800);
	Simulation* sim_ptr;
	bool link_bool;
	bool range;	
}

namespace Gtk{
	
	template<class T, class... T_Args>
	auto make_managed(T_Args&&... args) -> T*
	{
	 return manage(new T(std::forward<T_Args>(args)...));
	}
}



void set_sim_ptr(){
	
	sim_ptr = get_sim_ptr();
}


MyArea::MyArea(): 
	empty(false)
{}


MyArea::~MyArea()
{}


void MyArea::clear(){
	
	empty = true; 
	refresh();
}


void MyArea::draw(){
	
	empty = false;
	refresh();
}


void MyArea::setWParam(WP x) 
{ 
	prm_area_ref = x; 
	prm_area = prm_area_ref;
} 


void MyArea::refresh(){
	
	auto win = get_window();
	if(win)
	{
		Gdk::Rectangle r(0,0, get_allocation().get_width(),
						 get_allocation().get_height());
		win->invalidate_rect(r,false);
	}
}


bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){

	cr->set_source_rgb(1,1,1);
	cr->paint();
	if(not empty){
			Gtk::Allocation allocation = get_allocation();
			const int width = allocation.get_width();
			const int height = allocation.get_height();
			
			double new_aspect_ratio((double)width/height);
			
			if( new_aspect_ratio > prm_area_ref.ASP)
				largerRatio(new_aspect_ratio);		  	  
			else
				smallerRatio(new_aspect_ratio);	  	  
			
			double xMax(prm_area.XMAX), yMax(prm_area.YMAX), xMin(prm_area.XMIN), yMin(prm_area.YMIN);
			double xMid((xMin + xMax)/2), yMid((yMin + yMax)/2);
			double deltaX(xMax - xMin), deltaY(yMax - yMin);
			
			cr->translate(width/2, height/2);
			cr->scale(width/deltaX, -height/deltaY);
			cr->translate(-xMid, -yMid);
			draw_frame(cr);		
			
			cr->set_line_width(5);	
			setCr(cr);

			if(sim_ptr->get_existe())
				sim_ptr->faire_dessin(link_bool, range);

			return true;		
	}else{
		cout << "Empty !" << endl;
	}
	return true;
}


void MyArea::largerRatio(double new_aspect_ratio){
	
	prm_area.YMAX = prm_area_ref.YMAX ;
	prm_area.YMIN = prm_area_ref.YMIN ;	
				
	double delta(prm_area_ref.XMAX - prm_area_ref.XMIN);
	double mid((prm_area_ref.XMAX + prm_area_ref.XMIN)/2);

	prm_area.XMAX = mid + 0.5*(new_aspect_ratio/prm_area_ref.ASP)*delta ;
	prm_area.XMIN = mid - 0.5*(new_aspect_ratio/prm_area_ref.ASP)*delta ;
}


void MyArea::smallerRatio(double new_aspect_ratio){
	
	prm_area.XMAX = prm_area_ref.XMAX ;
	prm_area.XMIN = prm_area_ref.XMIN ;
					
	double delta(prm_area_ref.YMAX - prm_area_ref.YMIN);
	double mid((prm_area_ref.YMAX + prm_area_ref.YMIN)/2);

	prm_area.YMAX = mid + 0.5*(prm_area_ref.ASP/new_aspect_ratio)*delta ;
	prm_area.YMIN = mid - 0.5*(prm_area_ref.ASP/new_aspect_ratio)*delta ;	
}


void MyArea::draw_frame(const Cairo::RefPtr<Cairo::Context>& cr){
	
	cr->set_line_width(3.0);
	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->move_to(-dim_max,dim_max);
	cr->line_to(dim_max, dim_max);
	cr->line_to(dim_max, -dim_max);
	cr->line_to(-dim_max,-dim_max);
	cr->line_to(-dim_max,dim_max);
	cr->stroke();
}


Gui::Gui():
	m_Box_Big(Gtk::ORIENTATION_VERTICAL), m_Box_High(Gtk::ORIENTATION_HORIZONTAL,10),
	m_Box_Low(Gtk::ORIENTATION_VERTICAL), m_Box_Left(Gtk::ORIENTATION_VERTICAL,10),
	m_Box_General(Gtk::ORIENTATION_VERTICAL,10),
	m_Box_Toggle(Gtk::ORIENTATION_VERTICAL,10),
	m_Box_Area(Gtk::ORIENTATION_HORIZONTAL,10), m_Label_General("General"),	
	m_Label_Toggle("Toggle display"), m_Button_Exit("Exit"), m_Button_Open("Open"),
	m_Button_Save("Save"), m_Button_Start("Start"), m_Button_Step("Step"),
	m_Button_Tog_Link("toggle link"), m_Button_Tog_Range("toggle range"), 
	started(false), step(false)
{
	set_sim_ptr();
	set_title("Planet Donut - DEMO");
	set_border_width(0);
	
	add(m_Box_Big);
	
	packBoxes();
		
	WP prm;
	prm.XMIN = -dim_max;
	prm.XMAX = dim_max;
	prm.YMIN = -dim_max;
	prm.YMAX = dim_max;

	prm.HEIGHT = 400;
	prm.ASP = ( prm.XMAX-prm.XMIN ) / ( prm.YMAX-prm.YMIN );
	
	m_Area.setWParam(prm);	
		
	assignButtons();

	show_all_children();
}


Gui::~Gui()
{}


void Gui::packBoxes(){
	
	m_Box_Big.pack_start(m_Box_High);
	m_Box_Big.pack_start(m_Box_Low);
	
	m_Box_High.pack_start(m_Box_Left,false,false);
	m_Box_High.pack_start(m_Box_Area);
	m_Box_High.set_border_width(0);
	
	m_Box_Low.set_border_width(0);
	m_Box_Low.add(_scrolled_window);
	
	createTreeView();
	
	m_Box_Left.pack_start(m_Box_General,false,false);
	m_Box_Left.pack_start(m_Box_Toggle,false,false);

	m_Box_Area.set_border_width(0);
	m_Box_Area.pack_start(m_Area);
	m_Area.set_size_request(taille_dessin,taille_dessin);
	
	m_Box_General.set_border_width(0);
	m_Box_General.pack_start(m_Label_General);	
	m_Box_General.pack_start(m_Button_Exit,false,false);
	m_Box_General.pack_start(m_Button_Open,false,false);
	m_Box_General.pack_start(m_Button_Save,false,false);
	m_Box_General.pack_start(m_Button_Start,false,false);
	m_Box_General.pack_start(m_Button_Step,false,false);
	
	m_Box_Toggle.set_border_width(0);
	m_Box_Toggle.pack_start(m_Label_Toggle);
	m_Box_Toggle.pack_start(m_Button_Tog_Link,false,false);
	m_Box_Toggle.pack_start(m_Button_Tog_Range,false,false);
}


void Gui::createTreeView(){
	
	count = 0;
	_scrolled_window.set_size_request(-1, 150);
	_scrolled_window.add(_tree_view);
	_scrolled_window.set_policy(Gtk::PolicyType::POLICY_AUTOMATIC,
								Gtk::PolicyType::POLICY_AUTOMATIC);
								
	_scrolled_window.set_hexpand();
	_tree_view.append_column("nbP", _columns._col_nbP);
	_tree_view.append_column("nbF", _columns._col_nbF);
	_tree_view.append_column("nbT", _columns._col_nbT);
	_tree_view.append_column("nbC", _columns._col_nbC);
	_tree_view.append_column_numeric("Amount resource",_columns._col_resource, "%.2f");

	auto cell = Gtk::make_managed<Gtk::CellRendererProgress>();
	int cols_count = _tree_view.append_column("Mission completeness", *cell);
	
	auto progress_col = _tree_view.get_column(cols_count - 1);
	
	if(progress_col)
		progress_col->add_attribute(cell->property_value(),
									_columns._col_resource_percentage);		
}


void Gui::assignButtons(){
	
	m_Button_Exit.signal_clicked().connect(sigc::mem_fun(*this,
					&Gui::on_button_clicked_Exit) );
	m_Button_Open.signal_clicked().connect(sigc::mem_fun(*this,
					&Gui::on_button_clicked_Open) );
	m_Button_Save.signal_clicked().connect(sigc::mem_fun(*this,
					&Gui::on_button_clicked_Save) );
	m_Button_Start.signal_clicked().connect(sigc::mem_fun(*this,
					&Gui::on_button_clicked_Start) );
	 m_Button_Step.signal_clicked().connect(sigc::mem_fun(*this,
					&Gui::on_button_clicked_Step) );
	m_Button_Tog_Link.signal_clicked().connect(sigc::mem_fun(*this,
					&Gui::on_button_clicked_Tog_Link) );
	m_Button_Tog_Range.signal_clicked().connect(sigc::mem_fun(*this,
					&Gui::on_button_clicked_Tog_Range) );
					
	Glib::signal_idle().connect( sigc::mem_fun(*this, &Gui::on_idle) );	
}


void Gui::tree_view_update()
{
	Glib::RefPtr<Gtk::ListStore> ref_tree_model = Gtk::ListStore::create(_columns);
	_tree_view.set_model(ref_tree_model);

	if(sim_ptr->get_existe()) {
		vector<SimData> report (sim_ptr->report_bases());	

		for(size_t i = 0 ; i < count and i <report.size() ; ++i){		
			auto row = *(ref_tree_model->append());
			row[_columns._col_nbP] = report[i].nbP;
			row[_columns._col_nbF] = report[i].nbF;
			row[_columns._col_nbT] = report[i].nbT;
			row[_columns._col_nbC] = report[i].nbC;
			row[_columns._col_resource] = report[i].ressource;
			row[_columns._col_resource_percentage] = report[i].ressource_p;
		}
	}
}


bool Gui::on_idle(){

	++count;			
	tree_view_update();
	
	if(started){
		sim_ptr->update(link_bool,range);
		m_Area.refresh();
		this_thread::sleep_for(chrono::milliseconds(25));
	}else if (step){
		step = false;
		sim_ptr->update(link_bool,range);
		m_Area.refresh();
		this_thread::sleep_for(chrono::milliseconds(25));
	}
	
	return true; 
}


void Gui::on_button_clicked_Exit(){
	
	hide();
}


void Gui::on_button_clicked_Open(){
	
	cout <<"Open"<<endl;
	Gtk::FileChooserDialog dialog("Please choose a file",
			 Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Open", Gtk::RESPONSE_OK);


	int result = dialog.run();


	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
		string filename = dialog.get_filename();
		cout << "File selected: " <<  filename << endl;
		sim_ptr->lecture(filename);
		break;
		}
		case(Gtk::RESPONSE_CANCEL):
		{
		cout << "Cancel clicked." << endl;
		break;
		}
		default:
		{
		cout << "Unexpected button clicked." <<endl;
		break;
		}
	}
	m_Area.refresh();
}


void Gui::on_button_clicked_Save(){
	
	Gtk::FileChooserDialog dialog("Please choose a file",
			 Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Save", Gtk::RESPONSE_OK);

	int result = dialog.run();

	switch(result)
	{
		case(Gtk::RESPONSE_OK):
		{
		string filename = dialog.get_filename();
		cout << "File selected: " <<  filename << endl;
		sim_ptr->sauvegarde(filename);
		break;
		}
	 case(Gtk::RESPONSE_CANCEL):
	 {
		std::cout << "Cancel clicked." << std::endl;
		break;
	 }
	default:
	 {
		std::cout << "Unexpected button clicked." << std::endl;
		break;
	 }
	}
}


void Gui::on_button_clicked_Start(){
	
	cout <<"Start"<<endl;
	started = !started ;
	
	if(started)
		m_Button_Start.set_label("Stop");
	else
		m_Button_Start.set_label("Start");
}


void Gui::on_button_clicked_Step(){
	
	cout <<"Step"<<endl;
	step = true;
}


void Gui::on_button_clicked_Tog_Link(){
	
	link_bool = !link_bool;
	m_Area.refresh();
}


void Gui::on_button_clicked_Tog_Range(){
	
	range = !range;
	m_Area.refresh();
}

bool Gui::on_key_press_event(GdkEventKey * key_event){
	
	if(key_event->type == GDK_KEY_PRESS)
	{
		switch(gdk_keyval_to_unicode(key_event->keyval))
		{
			case '1':
				step = true;
				return true;
			case 'o':
				on_button_clicked_Open();
				return true;
			case 's':
				on_button_clicked_Start();
				return true;
			case 'l' :
				on_button_clicked_Tog_Link();
				return true;
			case 'r' :
				on_button_clicked_Tog_Range();
				return true;
			case ' ' :
				on_button_clicked_Save();
				return true;
			case 'q':
				cout << "Quit" << endl;
				exit(0);
				break;
		}
	}
	
	return Gtk::Window::on_key_press_event(key_event);
}
