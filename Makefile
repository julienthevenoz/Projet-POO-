# Definitions de macros

CXX     = g++
CXXFLAGS = -g -Wall -std=c++11
CXXFILES = projet.cc geomod.cc simulation.cc base.cc gisement.cc robot.cc message.cc gui.cc graphic.cc
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
OFILES = projet.o geomod.o simulation.o base.o gisement.o robot.o message.o gui.o graphic.o

# Definition de la premiere regle

projet: $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) -o $@ $(LDLIBS)

# Definitions de cibles particulieres

depend:
	@echo " *** MISE A JOUR DES DEPENDANCES ***"
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	  $(CXX) -MM $(CXXFLAGS) $(CXXFILES) | \
	  egrep -v "/usr/include" \
	 ) >Makefile.new
	@mv Makefile.new Makefile

clean:
	@echo " *** EFFACE MODULES OBJET ET EXECUTABLE ***"
	@/bin/rm -f *.o *.x *.cc~ *.h~ projet

#
# -- Regles de dependances generees automatiquement
#	ligne à insérer directement sous chaque dépendance quand on fait make depend
#	"	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)"   sans les guillemets
# DO NOT DELETE THIS LINE
projet.o: projet.cc simulation.h base.h robot.h geomod.h gisement.h gui.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
geomod.o: geomod.cc geomod.h graphic.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
simulation.o: simulation.cc simulation.h base.h robot.h geomod.h \
 gisement.h constantes.h message.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
base.o: base.cc base.h robot.h geomod.h gisement.h message.h constantes.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
gisement.o: gisement.cc geomod.h gisement.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
robot.o: robot.cc robot.h geomod.h constantes.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
message.o: message.cc message.h
gui.o: gui.cc gui.h graphic.h simulation.h base.h robot.h geomod.h \
 gisement.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
graphic.o: graphic.cc graphic.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)
