# cifio
Crystallographic Information Framework input/output to chemical constitutional graphs



## To build and test

Build [antlr4](https://github.com/antlr/antlr4) as sibling project antlr4.  The c++ runtime too.

Build [lemon](http://lemon.cs.elte.hu/hg/lemon-main) as sibling where the install is lemon-main/dist

ftp://ftp.wwpdb.org/pub/pdb/data/monomers/aa-variants-v1.cif.gz & ftp://ftp.wwpdb.org/pub/pdb/data/monomers/components.cif.gz in your cifio/db folder. Untar
components.cif.gz to components.cif and currently the unit test 'test component db' needs to run once 
to generate a lookup json.
(Note: this will change as this project setup becomes more automated)


Set compiler(>=10), antlr and zlib paths

```
export PATH=~/node-v14.9.0-linux-x64/bin:~/Software/gcc-dev/dist/bin:$PATH
export LD_LIBRARY_PATH=`pwd`/../antlr4/runtime/Cpp/run/usr/local/lib:`pwd`/cpp_modules/zlib/dist/lib:$LD_LIBRARY_PATH
```

Generate the c++ lexer/parser code once from the grammar:

```
export CLASSPATH=`pwd`/../antlr4/tool/target/antlr4-4.8-2-SNAPSHOT-complete.jar
cd grammars
java -Xmx500M -cp $CLASSPATH org.antlr.v4.Tool -Dlanguage=Cpp -o ../src/parsing -lib ../grammars -package sylvanmats CIFLexer.g4 CIFParser.g4
cd ..
```


```

yarn config set PATH $PATH
yarn config set LD_LIBRARY_PATH $LD_LIBRARY_PATH
yarn config set CIFIO_DB_LOCATION `pwd`/db
yarn config set JAVA_HOME ${JAVA_HOME}
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