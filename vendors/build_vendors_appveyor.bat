cd vendors

cd cpp-libface\libface-project
qmake "CONFIG+=%configuration%" libface.pro
nmake.exe
copy %configuration%\face.* ..\..\..\libs
cd ..\..

cd ssdll\src\ssdll
qmake "CONFIG+=%configuration% appveyor" ssdll.pro
nmake.exe
copy %configuration%\ssdll.* ..\..\..\..\libs

rem cd ..\..\..\..\src
