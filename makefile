
VULKAN_SDK_PATH = ./Lib/vulkan/x86_64


CFLAGS 		=-std=c++17 -g -Wall
INCLUDES	:=-I$(VULKAN_SDK_PATH)/include
LDFLAGS 	= -L$(VULKAN_SDK_PATH)/lib -lglfw -lvulkan
OUTPUT		:=./Output/Output.out

all:clean $(OUTPUT)

clean:
	rm -f $(OUTPUT)

$(OUTPUT): ./Src/main.cpp
	g++ $(CFLAGS) $(INCLUDES) $(LDFLAGS) $^ -o $@ 
