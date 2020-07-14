call set-windows-env.bat

pushd ..
qmake.exe librecad.pro -r -spec win32-g++
if not _%1==_NoClean (
	mingw32-make.exe clean
)
mingw32-make.exe -j4
if NOT exist windows\SCADA.exe (
	echo "Building windows\SCADA.exe failed!"
	exit /b /1
)
windeployqt.exe windows\SCADA.exe
popd
call build-win-setup.bat
