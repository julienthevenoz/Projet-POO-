#ifndef GUI_H
#define GUI_H

#include <gtkmm.h>

struct WP {
	
	 double XMIN; 
	 double XMAX;
	 double YMIN;
	 double YMAX;
	 int HEIGHT; 
	 double ASP; 
};


void set_sim_ptr();


class MyArea : public Gtk::DrawingArea{
	
public:
	MyArea();
	
	virtual ~MyArea();
	
	void clear();
	void draw();
	void setWParam(WP x);
	void refresh();

protected:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
	void largerRatio(double new_aspect_ratio);
	void smallerRatio(double new_aspect_ratio);
	void draw_frame(const Cairo::RefPtr<Cairo::Context>& cr);
	
private:
	bool empty;
	WP prm_area_ref;
	WP prm_area;
};

class Model_columns : public Gtk::TreeModel::ColumnRecord{	
	
public:
		Model_columns(){
		add(_col_id);
		add(_col_nbP);
		add(_col_nbF);
		add(_col_nbT);
		add(_col_nbC);
		add(_col_resource);
		add(_col_resource_percentage);
		}
		
		Gtk::TreeModelColumn<int> _col_id;
		Gtk::TreeModelColumn<int> _col_nbP;
		Gtk::TreeModelColumn<int> _col_nbF;
		Gtk::TreeModelColumn<int> _col_nbT;
		Gtk::TreeModelColumn<int> _col_nbC;
		Gtk::TreeModelColumn<double> _col_resource;
		Gtk::TreeModelColumn<int> _col_resource_percentage;
};


class Gui : public Gtk::Window{
	
public:
	Gui();
	
	virtual ~Gui();
	
	void packBoxes();
	void createTreeView();
	void assignButtons();
	void tree_view_update();		
		
protected:	
	Gtk::Box m_Box_Big;
	Gtk::Box m_Box_High, m_Box_Low, m_Box_Left; 
	Gtk::Box m_Box_General, m_Box_Toggle, m_Box_Area;
	Gtk::Separator m_Separator_G;
	Gtk::Separator m_Separator_T;
	Gtk::Label  m_Label_General;
	Gtk::Label  m_Label_Toggle;
	Gtk::Button m_Button_Exit;
	Gtk::Button m_Button_Open;
	Gtk::Button m_Button_Save;	
	Gtk::Button m_Button_Start;
	Gtk::Button m_Button_Step;	
	Gtk::Button m_Button_Tog_Link;
	Gtk::Button m_Button_Tog_Range;
	MyArea m_Area;
	
	bool on_idle();
	bool on_key_press_event(GdkEventKey * key_event);
	void on_button_clicked_Exit();
	void on_button_clicked_Open();
	void on_button_clicked_Save();
	void on_button_clicked_Start();
	void on_button_clicked_Step();
	void on_button_clicked_Tog_Link();
	void on_button_clicked_Tog_Range();
		
private:
	bool started;
	bool step;
	Model_columns _columns;
	Gtk::ScrolledWindow _scrolled_window;
	Gtk::TreeView _tree_view;
	unsigned count;
};



#endif
