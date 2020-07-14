SCADA : How to build
==============

Building SCADA on Windows 7
---------------------------
Qt
---------------------------
Install Qt , see the SCADA tools folder. qt-opensource-windows-x86-mingw482_opengl-5.3.2.exe
Install Qt (including Qt-Creator) to C:\Qt\Qt5.3.2 (the default path prompted by Qt installer). The MinGW tools will be in
C:\Qt\Qt5.3.2\5.3\mingw482_32\bin by default.

Boost
--------------------------
Install boost 1.53 from SCADA tools folder.
unzip the boost files to the directory: C:\boost\, and for the version boost-1.53.0, the boost files will be located in
C:\boost\boost_1_53_0\.

Nsis
--------------------------
Install the installer tool : Nsis. Install it from the SCADA tools folder : nsis-3.0b1-setup.exe

Building SCADA EXE file
--------------------------
• Open Qt creator
• File open file or project select C:\LibreCAD 1\librecad.pro → → “
• Build Build all →
• If everything is OK, then the EXE file will be created on C:\LibreCAD 1\windows\SCADA.exe

Building Windows installer
--------------------------
•press the windows-key and type qt
•select Qt 5.3 for desktop , the a DOS box is opened.
•input: cd "C:\LibreCAD 1\scripts"
•input: build-win-setup.bat
If everything was OK, the installer (SCADA-installer.exe) can be found in the C:\LibreCAD 1\generated\ folder .
