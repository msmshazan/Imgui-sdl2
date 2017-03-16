@echo off
set CompilerFlags= /Z7 /FC /nologo /Od /DINTERNAL=1 -fp:fast -Gm- -GR- -EHa- /Oi -WX -W4 -wd4189 -wd4702 -wd4312 -wd4996 -wd4505 -wd4100 -wd4456 -wd4244 -wd4201
set LinkerFlags=-subsystem:Console 
set bits=x64
set LibraryLocation= ..\deps\lib\%bits%\

mkdir build > NUL 2> NUL
pushd build
IF NOT DEFINED vcvars_called (
    call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %bits%
    set vcvars_called=1
)
del *.pdb > NUL 2> NUL
ctime -begin imgui.ctm
REM cl %CompilerFlags% /MTd ..\code\gamecode.cpp -Fmgamecode.map -LD /link -incremental:no -opt:ref -PDB:handmade_%RANDOM%.pdb -EXPORT:GameUpdateAndRender -out:GameCode.dll
cl %CompilerFlags% /MD ..\code\main.cpp /I..\deps\include   /link -incremental:no %LibraryLocation%SDL2.lib %LibraryLocation%SDL2main.lib comdlg32.lib opengl32.lib  %LinkerFlags% -out:Imgui.exe
ctime -end imgui.ctm
popd
