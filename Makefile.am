bin_PROGRAMS = funca
funca_SOURCES = main.cpp FuncA.cpp FuncA.h 

check_PROGRAMS = test_FuncA
test_FuncA_SOURCES = test_FuncA.cpp FuncA.cpp
test_FuncA_LDADD = -lgtest -lgtest_main -lpthread

.PHONY: deb

deb:
	@mkdir -p deb/DEBIAN
	echo "Source: funca" > deb/DEBIAN/control
	echo "Maintainer: cheryxso stor8ge18@gmail.com" >> deb/DEBIAN/control
	echo "Package: funca" >> deb/DEBIAN/control
	echo "Architecture: any" >> deb/DEBIAN/control
	echo "Version: 1.0" >> deb/DEBIAN/control
	echo "Description: FuncA " >> deb/DEBIAN/control
