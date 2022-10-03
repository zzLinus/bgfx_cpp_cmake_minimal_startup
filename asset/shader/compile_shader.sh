shaderc -f cs_update.sc     -o cs_update.bin     -p spirv --type compute --platform linux &&
shaderc -f fs_update.sc     -o fs_update.bin     -p spirv --type fragment --platform linux &&
shaderc -f fs_update_3d.sc  -o fs_update_3d.bin  -p spirv --type fragment --platform linux &&
shaderc -f fs_update_cmp.sc -o fs_update_cmp.bin -p spirv --type fragment --platform linux &&
shaderc -f vs_update.sc     -o vs_update.bin     -p spirv --type vertex --platform linux &&
mv *.bin ../../build/shaders/spirv/
