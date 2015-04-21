# Wenn openCV local installiert ist (pkg-config --cflags opencv muss Ergebnis melden)
OPENCVCFLAGS = `pkg-config --cflags opencv`   
OPENCVLINK = `pkg-config --libs opencv`

# Im Uni FB Netz (Installlation auf /home/ufrese/opencv/)
# Dann /home/ufrese/opencv/lib/ zu LD_LIBRARY_PATH hinzufuegen
# mit export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/ufrese/opencv/lib/
# OPENCVCFLAGS = -I/home/ufrese/opencv/include/
# OPENCVLINK = -L/home/ufrese/opencv/lib/  -lcxcore -lcv -lhighgui -lcvaux

#Im eigenen home Verzeichnis unter ~/packages/opencv
#Dann muss $(HOME)/packages/opencv/lib/ auch im LD_LIBRARY_PATH enthalten sein
# OPENCVCFLAGS = -I$(HOME)/packages/opencv/include/
# OPENCVLINK = -L$(HOME)/packages/opencv/lib/  -lcxcore -lcv -lhighgui -lcvaux

peas:

MKS = ./stripSolution.py

%.sample.cc: %.cc
	$(MKS) $< $@

# Achtung: Wenn pkg-config opencv nicht findet. Muss oft /usr/lib/pkgconfig/ und /usr/local/lib/pkgconfig in den
# Pfad PKG_CONFIG_PATH eingebunden werden.
# export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/lib/pkgconfig/:/usr/local/lib/pkgconfig
#
peas: peas.cc Makefile
	g++ -ggdb $(OPENCVCFLAGS) -D_LINUX -o peas peas.cc $(OPENCVLINK)

ebvtutorial01.zip: Makefile peas.sample.cc pics/*.jpg
	zip ebvtutorial01.zip peas.sample.cc Makefile pics/*.jpg

ebvtutorial01solution.zip: Makefile peas.cc pics/*.jpg
	zip ebvtutorial01solution.zip Makefile peas.cc pics/*.jpg
