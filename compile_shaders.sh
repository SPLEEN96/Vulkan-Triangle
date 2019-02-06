rm ./Shaders/*.spv
./Lib/vulkan/x86_64/bin/glslangValidator -V ./Shaders/shader.vert -o ./Shaders/vert.spv
./Lib/vulkan/x86_64/bin/glslangValidator -V ./Shaders/shader.frag -o ./Shaders/frag.spv