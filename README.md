# cifio
Crystallographic Information Framework input/output to chemical constitutional graphs



##To build and test

Build [antlr4](https://github.com/antlr/antlr4) as sibling project antlr4.  The c++ runtime too.

Build [lemon](http://lemon.cs.elte.hu/hg/lemon-main) as sibling wher the install is lemon-main/dist

ftp://ftp.wwpdb.org/pub/pdb/data/monomers/aa-variants-v1.cif.gz in your ~/Downloads folder.
https://files.rcsb.org/download/3SGS.cif.gz in your ~/Downloads folder.
(Note: this will change as this project setup becomes mroe automated)

Set compiler(>=10), antlr and zlib paths
```
export PATH=~/node-v14.9.0-linux-x64/bin:~/Software/gcc-dev/dist/bin:$PATH
export LD_LIBRARY_PATH=`pwd`/../antlr4/runtime/Cpp/run/usr/local/lib:`pwd`/node_modules/zlib/dist/lib:$LD_LIBRARY_PATH
```

```

yarn install

#run once
yarn zlib

yarn test
```