CC=gcc
RM=rm -f
CFLAGS=-fPIC -Wall -I. -I../base -I../../global

SRC_LIB=cso_core.c cso_internals.c cso_serialize.c cso_deserialize.c \
	strtools.c
INC_LIB=cso.h cso_core.h cso_types.h cso_internals.h cso_serialize.h \
	cso_deserialize.h strtools.h
OBJ_LIB=$(subst .c,.o,$(SRC_LIB))

LIB_NAME=libcso.a

INSTALLDIR_INC=../../../build/include/
INSTALLDIR_LIB=../../../build/lib/

# -----

all: csolib

csolib: $(OBJ_LIB)
	ar rcs $(LIB_NAME) $(OBJ_LIB)

depend: .depend

.depend: $(SRC_LIB) ${SRC_TEST}
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^>>./.depend;

install: all
	cp --preserve=timestamps $(INC_LIB) $(INSTALLDIR_INC)
	cp --preserve=timestamps $(LIB_NAME) $(INSTALLDIR_LIB)

clean:
	$(RM) *.o $(LIB_NAME)

distclean: clean
	$(RM) *~ *.swp .*.swp .depend

include .depend

