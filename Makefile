all 

zlib:
	cd node_modules/zlib && cmake -DCMAKE_INSTALL_PREFIX=`pwd` . && $(MAKE) && $(MAKE) test && $(MAKE) install
