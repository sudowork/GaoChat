# Tie together Makefiles
MAKEFILES = Makefile.cli \
			Makefile.gui

all:
	for i in $(MAKEFILES) ; do \
		(make -f $$i all) ; \
	done

clean:
	for i in $(MAKEFILES) ; do \
		(make -f $$i clean) ; \
	done
