@echo off 
for /r %%i in (*.frag, *.vert) do %VULKAN_SDK%/Bin32/glslangValidator.exe -V %%i