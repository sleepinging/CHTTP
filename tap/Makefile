targetname=main.exe

OBJS=main.o mytap.o myip.o mymac.o mytool.o

GXX=g++

CPP_INCLUDE_PATH="include"
BUILD_FLAG=-g -Wall -Wextra -std=c++11

#$@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。

main:$(OBJS)
	$(GXX) $^ -g -o $(targetname)
main.o:main.cpp
	$(GXX) -c $^ -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
mytap.o:src/mytap.cpp include/mytap.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
myip.o:src/myip.cpp include/myip.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
mymac.o:src/mymac.cpp include/mymac.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
mytool.o:src/mytool.cpp include/mytool.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@

clean:
	del $(OBJS) $(targetname)