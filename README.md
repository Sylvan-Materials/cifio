# cifio
Crystallographic Information Framework input/output to chemical constitutional graphs



## To build and test

Note: currently working on a new package manager to use on this package.  Other managers should still 
work(as far as they do) as https://github.com/DataDriven-CAM/cnpm.git is based on the package.json npm format.


Build [lua](http://www.lua.org/) and export LUA_HOME=&lt;where you installed it>

ftp://ftp.wwpdb.org/pub/pdb/data/monomers/aa-variants-v1.cif.gz & ftp://ftp.wwpdb.org/pub/pdb/data/monomers/components.cif.gz in your cifio/db folder. Untar
components.cif.gz to components.cif and currently the unit test 'test component db' needs to run once 
to generate a lookup json.
(Note: this will change as this project setup becomes more automated)


Set compiler(>=10)

```
export PATH=~~/Software/gcc-dev/dist/bin:$PATH
export LD_LIBRARY_PATH=`pwd`/../cnpm/cpp_modules/urlcpp:`pwd`/../cnpm/cpp_modules/libgit2/dist/lib:`pwd`:`pwd`/cpp_modules/json-thresher:`pwd`/cpp_modules/xml-thresher:$JAVA_HOME/lib:$JAVA_HOME/lib/server:~/Software/gcc-dev/dist/lib64::`pwd`/cpp_modules/antlr4/runtime/Cpp/run/lib:`pwd`/cpp_modules/urlcpp:`pwd`/cpp_modules/openssl/openssl/lib64:`pwd`/cpp_modules/zlib/dist/lib:$LD_LIBRARY_PATH
```

Generate the c++ lexer/parser code once from the grammars:

```
export CLASSPATH=`pwd`/src:`pwd`/build/src:`pwd`/cpp_modules/antlr4/tool/target/antlr4-4.13.2-SNAPSHOT-complete.jar
cd grammars
java -Xmx500M -cp $CLASSPATH org.antlr.v4.Tool -Dlanguage=Cpp -encoding utf-8 -o ../src/parsing -lib ../grammars -package sylvanmats CIFLexer.g4 CIFParser.g4
java -Xmx500M -cp $CLASSPATH org.antlr.v4.Tool -Dlanguage=Cpp -encoding utf-8 -o ../src/parsing -lib ../grammars -package sylvanmats MOL2Lexer.g4 MOL2Parser.g4
java -Xmx500M -cp $CLASSPATH org.antlr.v4.Tool -Dlanguage=Cpp -encoding utf-8 -o ../src/parsing -lib ../grammars -package sylvanmats SMIRKSLexer.g4 SMIRKSParser.g4
cd ..
```

### cnpm building

```
cnpm  install

cnpm urlcpp
cnpm zlib
cnpm ssl
cnpm antlr4
cnpm lemon
cnpm fmt

cnpm lib

#building and running unit tests
export CIFIO_DB_LOCATION=`pwd`/db
cnpm test

```

### Or yarn building if you prefer

```
yarn config set PATH $PATH
yarn config set LD_LIBRARY_PATH $LD_LIBRARY_PATH
yarn config set CIFIO_DB_LOCATION `pwd`/db
yarn config set JAVA_HOME ${JAVA_HOME}
yarn config set LUA_HOME ${LUA_HOME}
yarn install

#run once
yarn urlcpp
#run once
yarn zlib
#run once
yarn ssl

yarn lib
yarn test
```

## Contact

My twitch stream is https://www.twitch.tv/sylvanmats