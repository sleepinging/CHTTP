#!/bin/sh

sed -i "s/\$(LD_FLAGS_W)/\$(LD_FLAGS_U)/" Makefile
sed -i "s/\tdel/\trm/" Makefile
sed -i "s/myvpn.exe/myvpn/" Makefile

make
