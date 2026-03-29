@echo off
if "%~1"=="" (
  echo Usage: %0 ^<source_file.c^>
  exit /b 1
)

rem Get the file name without the extension
set "filename=%~n1"

rem Compile the C file
clang -o "%filename%.exe" "%~1"

rem Check if compilation was successful
if %errorlevel% equ 0 (
  rem Execute the binary
  "%filename%.exe"
  rem Remove the binary
  del "%filename%.exe"
) else (
  echo Compilation failed.
  exit /b 1
)