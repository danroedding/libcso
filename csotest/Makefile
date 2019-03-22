CC=gcc
RM=rm -f
CFLAGS=-fPIC -Wall -I. -I../cso
LDFLAGS=-L../cso -lcso

SRC_CSOTEST=csotest.c
INC_CSOTEST=
OBJ_CSOTEST=$(subst .c,.o,$(SRC_CSOTEST))
BIN_CSOTEST=csotest

# -----

all: csotest

csotest: $(OBJ_CSOTEST)
	$(CC) $(OBJ_CSOTEST) $(LDFLAGS) -o $(BIN_CSOTEST)

depend: .depend

.depend: $(SRC_CSOTEST)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^>>./.depend;

install: all

clean:
	$(RM) *.o $(BIN_CSOTEST)

distclean: clean
	$(RM) *~ *.swp .*.swp .depend

include .depend

