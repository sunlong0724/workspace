#code_list_5-1:code_list_5-1.o
#	g++ code_list_5-1.o -o code_list_5-1
#code_list_5-1.o:code_list_5-1.cc
#	g++ -c code_list_5-1.cc -o code_list_5-1.o
#
#clean:
#	rm *.o code_list_5-1

#OBJS = code_list_5-1.o
#XX = g++
#CFLAGS = -Wall -O -g
#
#code_list_5-1 : $(OBJS)
#	$(XX) $(OBJS) -o code_list_5-1
#code_list_5-1.o : code_list_5-1.cc
#	$(XX) $(CFLAGS) -c code_list_5-1.cc -o code_list_5-1.o
#
#clean:
#	rm -rf *.o code_list_5-1

CC = gcc
XX = g++
CFLAGS = -Wall -O -g
TARGET = code_list_5-1

# $@ $^ $< 都为当前规则的内部变量
#%.cc 就是%.o对应的依靠列表 ($<扩展为依靠列表的第一个, $^为依靠列表, $@为当前规则的目标文件名
#此乃静态模式
%.o : %.cc
	$(XX) $(CFLAGS) -c $< -o $@ 

SOURCES = $(wildcard *.cc) #此处定义变量SOURCES，所以通配符失效，需用wildcared函数
OBJS = $(patsubst %.cc,%.o,$(SOURCES))

$(warning "OBJS is $(OBJS)")

$(TARGET) : $(OBJS)
	$(XX) $(OBJS) -o $(TARGET)
clean:
	rm -rf *.o code_list_5-1


