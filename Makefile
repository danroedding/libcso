all:
	cd cso; make
	cd csotest; make

clean:
	cd cso; make clean
	cd csotest; make clean

distclean:
	cd cso; make distclean
	cd csotest; make distclean


