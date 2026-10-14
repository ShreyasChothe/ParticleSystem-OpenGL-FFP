cl.exe /c /EHsc OGL.cpp Particles.cpp imgui/*.cpp
rc.exe OGL.rc
link.exe /out:OGL.exe  *.obj OGL.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
