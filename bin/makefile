#---------------------------------------------------------------------------#
#     makefile.exe for prawda                                               #
#     Anissimow D.W.                                     1 Martz 1999.      #
#---------------------------------------------------------------------------#

ROOTDIR="/usr/share/prawda"
HOMEDIR=`(cd ; pwd)`
ENG = ../grammar/english_russian
ESP = ../grammar/esperanto_russkij

all:
	cd ../src/bin;\
	make -f makefile.exe
	g++ -E -I$(ENG)/ -o $(ENG)/lang.txt $(ENG)/lang.cpp
	g++ -E -I$(ESP)/ -o $(ESP)/lang.txt $(ESP)/lang.cpp

rpm-image:
	cd ../src/bin;\
	make -f makefile.exe
	g++ -E -I$(ENG)/ -o $(ENG)/lang.txt $(ENG)/lang.cpp
	g++ -E -I$(ESP)/ -o $(ESP)/lang.txt $(ESP)/lang.cpp

install:
	mkdir -p /usr/local/share/prawda/dicts
	mkdir -p /usr/local/share/prawda/grammar
	mkdir -p /usr/local/share/prawda/fonts
	mkdir -p /usr/local/share/doc/prawda/help
	mkdir -p /usr/local/share/doc/prawda/latex
	mkdir -p /usr/local/share/doc/prawda/html_grammar	
	cp ../src/bin/prawda		/usr/local/bin
	cp ../src/bin/prawda_x		/usr/local/bin
	cp -r ../dicts/*		/usr/local/share/prawda/dicts
	cp -r ../grammar/*		/usr/local/share/prawda/grammar
	cp ../fonts/*			/usr/local/share/prawda/fonts
	/usr/sbin/chkfontpath -q -a     /usr/local/share/prawda/fonts
	cp ../language.rc               /usr/local/share/prawda
	cp ../prawdarc.local            /usr/local/share/prawda
	cp ../prawdarc.root             /usr/local/share/prawda
	cp ../version                   /usr/local/share/prawda
	cp ../z_man.rc                  /usr/local/share/prawda
	cp ../z_preproc.rc              /usr/local/share/prawda
	cp ../doc/*.txt			/usr/local/share/doc/prawda
	cp ../doc/help/*		/usr/local/share/doc/prawda/help
	cp ../doc/latex/*.ps		/usr/local/share/doc/prawda/latex
	cp -r ../doc/html_grammar/*	/usr/local/share/doc/prawda/html_grammar


uninstall:
	rm -rf	/usr/local/share/prawda
	rm -rf	/usr/local/share/doc/prawda
	rm -f	/usr/local/bin/prawda
	rm -f	/usr/local/bin/prawda_x
	/usr/sbin/chkfontpath -q -r /usr/local/share/prawda/fonts

install-rpm:
	mkdir -p $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/usr/share/prawda/dicts
	mkdir -p $(DESTDIR)/usr/share/prawda/fonts
	mkdir -p $(DESTDIR)/usr/share/prawda/grammar
	mkdir -p $(DESTDIR)/usr/share/doc/prawda/help
	mkdir -p $(DESTDIR)/usr/share/doc/prawda/latex
	mkdir -p $(DESTDIR)/usr/share/doc/prawda/html_grammar
	cp ../src/bin/prawda		$(DESTDIR)/usr/bin
	cp ../src/bin/prawda_x		$(DESTDIR)/usr/bin
	cp -r ../dicts/*		$(DESTDIR)/usr/share/prawda/dicts
	cp -r ../grammar/*		$(DESTDIR)/usr/share/prawda/grammar
	cp ../fonts/*			$(DESTDIR)/usr/share/prawda/fonts
	cp ../language.rc               $(DESTDIR)/usr/share/prawda
	cp ../prawdarc.local            $(DESTDIR)/usr/share/prawda
	cp ../prawdarc.root             $(DESTDIR)/usr/share/prawda
	cp ../version                   $(DESTDIR)/usr/share/prawda
	cp ../z_man.rc                  $(DESTDIR)/usr/share/prawda
	cp ../z_preproc.rc              $(DESTDIR)/usr/share/prawda
	cp ../doc/*.txt			$(DESTDIR)/usr/share/doc/prawda
	cp ../doc/help/*		$(DESTDIR)/usr/share/doc/prawda/help
	cp ../doc/latex/*.ps		$(DESTDIR)/usr/share/doc/prawda/latex
	cp -r ../doc/html_grammar/*	$(DESTDIR)/usr/share/doc/prawda/html_grammar

# ----------------------------- EOF -------------------------------------



