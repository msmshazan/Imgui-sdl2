@echo off
set CompilerFlags= /Z7 /FC /nologo /Od /DINTERNAL=1 -fp:fast -Gm- -GR- -EHa- /Oi -WX -W4 -wd4189 -wd4702 -wd4312 -wd4996 -wd4505 -wd4100 -wd4456 -wd4244 -wd4201
set LinkerFlags=-subsystem:Console 
set bits=x64
set LibraryLocation=..\deps\lib\%bits%\

mkdir build > NUL 2> NUL
pushd build
IF NOT DEFINED vcvars_called (
    call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %bits%
    set vcvars_called=1
)
del *.pdb > NUL 2> NUL
del sqlite3.dll > NUL 2> NUL
del SDL2.dll > NUL 2> NUL
xcopy %LibraryLocation%sqlite3.dll > NUL 2> NUL
xcopy %LibraryLocation%SDL2.dll > NUL 2> NUL
ctime -begin imgui.ctm
cl %CompilerFlags% /MD ..\code\main.cpp /I..\deps\include   /link -incremental:no /LIBPATH:%LibraryLocation% SDL2.lib sqlite3.lib SDL2main.lib user32.lib winmm.lib opengl32.lib  %LinkerFlags% -out:Imgui.exe
set LastError=%ERRORLEVEL%
ctime -end imgui.ctm
popd
