@echo off
if "%~1"=="" (
  echo Usage: %0 ^<source_file.cu^>
  exit /b 1
)

rem Setup Visual Studio environment if cl.exe not in PATH
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

rem Get the file name without the extension
set "filename=%~n1"

rem Compile the CUDA file
echo Compiling %~1...
nvcc -o "%filename%.exe" "%~1"

rem Check if compilation was successful
if %errorlevel% equ 0 (
  echo Compilation successful. Running %filename%.exe...
  echo.
  rem Execute the binary
  "%filename%.exe"
  echo.
  rem Remove the binary
  del "%filename%.exe"
  del "%filename%.exp"
  del "%filename%.lib"
) else (
  echo Compilation failed.
  exit /b 1
)