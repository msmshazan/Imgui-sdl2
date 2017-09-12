
IF NOT DEFINED vcvars_called (
    call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %bits%
    set vcvars_called=1
)
msdev build/Imgui.exe
