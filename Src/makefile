VULKAN_SDK_PATH = /home/spleen/Documents/PROJECTS/Rendering/vulkan/1.1.97.0/x86_64


CFLAGS 		=-std=c++17 -g -Wall
INCLUDES	:=-I$(VULKAN_SDK_PATH)/include
LDFLAGS 	= -L$(VULKAN_SDK_PATH)/lib -lglfw -lvulkan
CFLAGS      +=$(INCLUDES)
OUTPUT		:=Output.out

all:clean $(OUTPUT)

$(OUTPUT): main.cpp
	g++ $(CFLAGS) main.cpp $(LDFLAGS) -o $@ 

.PHONY: test clean

#test: VulkanTest
#	LD_LIBRARY_PATH=$(VULKAN_SDK_PATH)/lib VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/explicit_layer.d ./VulkanTest

clean:
	rm -f $(OUTPUT)
