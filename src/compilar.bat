taskkill /f /im main.exe
cl /EHsc main.cpp recursos.res /link /subsystem:windows /INCREMENTAL:NO
mt.exe -manifest main.exe.manifest -outputresource:main.exe;1
