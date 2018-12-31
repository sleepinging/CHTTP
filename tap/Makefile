targetname=myvpn.exe

OBJS=main.o \
mytap.o myip.o mymac.o mytool.o mymask.o \
mystring.o myconn.o mysocket.o mycrypto.o \
mybufconn.o myipnet.o

GXX=g++

CPP_INCLUDE_PATH=include

LD_FLAGS_W=-lWs2_32
LD_FLAGS_U=-lpthread

LD_FLAGS=$(LD_FLAGS_W)

BUILD_FLAG=-g -Wall -Wextra -std=c++11 #-static

#$@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。

main:$(OBJS)
	$(GXX) $^ $(LD_FLAGS) -o $(targetname)
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
mymask.o:src/mymask.cpp include/mymask.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
mystring.o:src/mystring.cpp include/mystring.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
packet.o:src/packet.cpp include/packet.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
myconn.o:src/myconn.cpp include/myconn.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
mysocket.o:src/mysocket.cpp include/mysocket.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
mycrypto.o:src/mycrypto.cpp include/mycrypto.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
mybufconn.o:src/mybufconn.cpp include/mybufconn.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
myipnet.o:src/myipnet.cpp include/myipnet.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@

clean:
	del $(OBJS) $(targetname)