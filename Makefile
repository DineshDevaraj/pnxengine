
HDR = rtree.h \
		interpreter.h

SRC = pnlex.cpp \
		pnrule.tab.c \
		rtree.cpp \
		pnrule.cpp \
		interpreter.cpp

pngen : $(HDR) $(SRC)
	g++ -g $(SRC) -o pngen

debug : $(HDR) $(SRC)
	g++ -g -DDEBUG $(SRC) -o pngen

pnrule.tab.c : pnrule.y
	bison -d pnrule.y;

clean :
	rm pnrule.tab.h pnrule.tab.c pngen
