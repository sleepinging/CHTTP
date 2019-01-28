#!/bin/sh

sed -i "s/\$(LD_FLAGS_W)/\$(LD_FLAGS_U)/" Makefile
sed -i "s/\$(CPP_INCLUDE_PATH_W)/\$(CPP_INCLUDE_PATH_U)/" Makefile
sed -i "s/\$(LD_PATH_W)/\$(LD_PATH_U)/" Makefile
sed -i "s/\tdel/\trm/" Makefile
sed -i "s/myvpn.exe/myvpn/" Makefile

make
