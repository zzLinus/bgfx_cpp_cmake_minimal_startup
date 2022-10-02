shaderc -f fs_bump.sc -o fs_bump.bin -p spirv --type fragment --platform linux &&
shaderc -f vs_bump.sc -o vs_bump.bin -p spirv --type vertex --platform linux &&
shaderc -f vs_bump_instanced.sc -o vs_bump_instanced.bin -p spirv --type vertex --platform linux &&
mv *.bin ../../build/shaders/spirv/
