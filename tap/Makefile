targetname=myvpn.exe

OBJPATH=obj

# OBJS=main.o \
# mytap.o myip.o mymac.o mytool.o mymask.o \
# mystring.o myconn.o mysocket.o mycrypto.o \
# mybufconn.o myipnet.o config.o tfile.o \
# test.o

OBJS += $(OBJPATH)/main.o
OBJS += $(OBJPATH)/mytap.o
OBJS += $(OBJPATH)/myip.o
OBJS += $(OBJPATH)/mymac.o
OBJS += $(OBJPATH)/mytool.o
OBJS += $(OBJPATH)/mymask.o
OBJS += $(OBJPATH)/mystring.o
OBJS += $(OBJPATH)/myconn.o
OBJS += $(OBJPATH)/mysocket.o
OBJS += $(OBJPATH)/mycrypto.o
OBJS += $(OBJPATH)/mybufconn.o
OBJS += $(OBJPATH)/myipnet.o
OBJS += $(OBJPATH)/config.o
OBJS += $(OBJPATH)/tfile.o
OBJS += $(OBJPATH)/test.o

GXX=g++

CPP_INCLUDE_PATH=include

LD_FLAGS_W=-lWs2_32
LD_FLAGS_U=-lpthread

LD_FLAGS=$(LD_FLAGS_W)

BUILD_FLAG=-g -Wall -Wextra -std=c++11 #-static

#$@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。

main:$(OBJS)
	$(GXX) $^ $(LD_FLAGS) -o $(targetname)
$(OBJPATH)/main.o:main.cpp
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/mytap.o:src/mytap.cpp include/mytap.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/myip.o:src/myip.cpp include/myip.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/mymac.o:src/mymac.cpp include/mymac.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/mytool.o:src/mytool.cpp include/mytool.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/mymask.o:src/mymask.cpp include/mymask.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/mystring.o:src/mystring.cpp include/mystring.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/packet.o:src/packet.cpp include/packet.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/myconn.o:src/myconn.cpp include/myconn.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/mysocket.o:src/mysocket.cpp include/mysocket.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/mycrypto.o:src/mycrypto.cpp include/mycrypto.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/mybufconn.o:src/mybufconn.cpp include/mybufconn.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/myipnet.o:src/myipnet.cpp include/myipnet.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/config.o:src/config.cpp include/config.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/tfile.o:src/tfile.cpp include/tfile.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
$(OBJPATH)/test.o:src/test.cpp include/test.h
	$(GXX) -c $< -I$(CPP_INCLUDE_PATH) $(BUILD_FLAG) -o $@
	
clean:
	del $(OBJS) $(targetname)