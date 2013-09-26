##---------------------------------------------------
## Author:		Gonzalo Abella
## Update:		25/01/2012 by Gonzalo Abella
##---------------------------------------------------

buildDir=build

all: build

cmake: CMakeLists.txt
	cd $(buildDir) && cmake ..

build: cmake
	$(MAKE) -C $(buildDir)

clean:
	$(MAKE) -C $(buildDir) clean
	
cleanup_cache:
	cd $(buildDir) && rm -rf *
