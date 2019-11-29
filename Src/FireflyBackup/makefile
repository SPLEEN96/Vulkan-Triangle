#Resources: 
#https://stackoverflow.com/questions/40288915/makefile-compiling-every-time
#http://itscompiling.eu/2017/01/12/precompiled-headers-cpp-compilation/
#############CHEAT SHEET##################
#$@ name of the target
#$^ all items in dependency list
#$< dependency of the object (first item in dependency list)
#-MD -MP automatically generates dependency files (.d) (has to be included in makefile)
#all: main.cpp --> target: dependencies
#############CHEAT SHEET##################
VULKAN_SDK_PATH	:=./Lib/vulkan/x86_64

CC			:=gcc
CXX			:=g++
OUTPUT		:=Engine.out
INCLUDES	:=-I./Src -I./Lib/ -I./Lib/spdlog/include/  -I$(VULKAN_SDK_PATH)/include/
CFLAGS 		:=-g -Wall
CXXFLAGS	:=-std=c++11 -g -Wall
LFLAGS		:=-lglfw -L$(VULKAN_SDK_PATH)/lib -lvulkan
DEFINES		:=-DDFFLY_LINUX -DGLFW_INCLUDE_NONE
CXXFLAGS	+=$(DEFINES)

SOURCES			:=$(shell find ./Src -name '*.cpp')
SOURCES_CXXDEP	:=Lib/imgui/imgui.cpp Lib/imgui/imgui_demo.cpp Lib/imgui/imgui_draw.cpp Lib/imgui/imgui_widgets.cpp
SOURCES_CDEP	:=
OBJS			:=$(SOURCES:.cpp=.o)
OBJS_CXXDEP		:=$(SOURCES_CXXDEP:.cpp=.o)
OBJS_CDEP		:=$(SOURCES_CDEP:.c=.o)
PCH_SOURCE		:=./Src/PCH_CORE.h
PCH_OUT			:=$(PCH_SOURCE).gch

output_dir	:=./Output


#Compiles everything without cleanup
default: $(OUTPUT)
#Compiles everything and clean
fresh: $(OUTPUT) clean cleandep
#Removes all .o and .d files of firefly source
clean: 
	rm -f $(OBJS) $(OBJS:.o=.d)
#Removes all .o and .d files of firefly dependencies
cleandep:
	rm -f $(OBJS_CXXDEP) $(OBJS_CXXDEP:.o=.d) $(OBJS_CDEP) $(OBJS_CDEP:.o=.d)


#Links and Compiles Objects into Executable
$(OUTPUT): $(OBJS) $(OBJS_CXXDEP) $(OBJS_CDEP)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $^ -o $(output_dir)/$@

#Compiles every SourceFiles into Objects with pattern rule
$(OBJS): %.o: %.cpp $(PCH_OUT)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(LFLAGS) -MD -MP -c $< -o $@

#Compiles every c++ libraries into Objects with pattern rule
$(OBJS_CXXDEP): %.o: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(LFLAGS) -MD -MP -c $< -o $@

#Compiles every c libraries into Objects with pattern rule
$(OBJS_CDEP): %.o: %.c
	$(CXX) $(INCLUDES) $(CFLAGS) $(LFLAGS) -MD -MP -c $< -o $@

#Precompiles the PCH source files
$(PCH_OUT): $(PCH_SOURCE)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $< -o $(PCH_OUT)


-include $(OBJS:.o=.d) #replaces the .o by .d
