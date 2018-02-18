@echo off
setlocal enabledelayedexpansion
cls

set SourceFolder=%cd%\Source
set ComponentsFolder=%cd%\Components
set InstallationFilesFolder=%cd%\Installation Files
set ReleaseFilesFolder=%cd%\Release Files
set DocumentationFolder=%cd%\Documentation
set OutputFolder=%cd%\Output Files

rem ---- Output title

echo EightyOne Distributable Files Creator
echo =====================================
echo.
echo Source folder             = %SourceFolder%
echo Components folder         = %ComponentsFolder%
echo Installation files folder = %InstallationFilesFolder%
echo Documentation folder      = %DocumentationFolder%
echo Release Files folder      = %ReleaseFilesFolder%
echo.

rem ---- Check that all folders exists

set TestFolder=%SourceFolder%
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%InstallationFilesFolder%
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%DocumentationFolder%
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ReleaseFilesFolder%
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ComponentsFolder%
if not exist "%TestFolder%" goto :ErrorFolderNotFound

rem ---- Check component subfolders exists

set TestFolder=%ComponentsFolder%\Animation-Timer
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ComponentsFolder%\ComPort Library
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ComponentsFolder%\DirectDraw
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ComponentsFolder%\Office Button 97
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ComponentsFolder%\Theme Manager
if not exist "%TestFolder%" goto :ErrorFolderNotFound

rem ---- Query whether the release history file has been updated

set /p answer="Have you remembered to update the 'Release history.txt' file? (Y/N) "

if '%answer%'=='y' goto :cont
if '%answer%'=='Y' goto :cont

goto :ErrorCreationAborted

:cont
echo.

rem ---- Request the revision number from the user

set /p revision="Build version number 1.x (e.g. enter 10 for version 1.10): "

set /a val=%revision% 2>nul
if not %val%==%revision% goto :InvalidVersionNumber

rem ---- Create the output folder structure

rmdir /s /q "%OutputFolder%" > nul 2>&1
mkdir "%OutputFolder%" > nul 2>&1

set eoDevEnv=EightyOne V1.%revision% Development Environment
set eoDistribution=EightyOne V1.%revision% Distribution Files
set eoRelease=EightyOne V1.%revision% Release

set OutputDevEnvFolder=%OutputFolder%\%eoDevEnv%
set OutputDistributionFolder=%OutputFolder%\%eoDistribution%
set OutputReleaseFolder=%OutputFolder%\%eoRelease%

mkdir "%OutputDevEnvFolder%" > nul 2>&1
mkdir "%OutputDistributionFolder%" > nul 2>&1
mkdir "%OutputReleaseFolder%" > nul 2>&1

rem ---- Create the development environment folder contents

echo.
echo Constructing the Development Environment folder...

xcopy /e "%InstallationFilesFolder%\*.*" "%OutputDevEnvFolder%\Installation Files\" > nul 2>&1
xcopy /e "%SourceFolder%\*.*"        "%OutputDevEnvFolder%\Source\" > nul 2>&1
xcopy /y "%DocumentationFolder%\*.*" "%OutputDevEnvFolder%\" > nul 2>&1

rem ---- Clean the Development Environment source folder

set OutputDevEnvSourceFolder=%OutputFolder%\%eoDevEnv%\Source

del /s /q "%OutputDevEnvSourceFolder%\*.exe" > nul 2>&1
del /s /q "%OutputDevEnvSourceFolder%\*.lib" > nul 2>&1
del /s /q "%OutputDevEnvSourceFolder%\*.tds" > nul 2>&1
del /s /q "%OutputDevEnvSourceFolder%\*.obj" > nul 2>&1
del /s /q "%OutputDevEnvSourceFolder%\*.bak" > nul 2>&1
del /s /q "%OutputDevEnvSourceFolder%\*.~*" > nul 2>&1
del /s /q "%OutputDevEnvSourceFolder%\*.*$$$" > nul 2>&1
del /s /q "%OutputDevEnvSourceFolder%\*.gitattributes" > nul 2>&1
del /s /q "%OutputDevEnvSourceFolder%\*.gitignore" > nul 2>&1
rmdir /s /q "%OutputDevEnvSourceFolder%\.git" > nul 2>&1

rem ---- Create the release folder contents

echo Constructing the Release folder...

xcopy /e "%ReleaseFilesFolder%\*.*"     "%OutputReleaseFolder%\" > nul 2>&1
xcopy /e "%SourceFolder%\ROM\*.*"       "%OutputReleaseFolder%\ROM\" > nul 2>&1
xcopy /y "%SourceFolder%\EightyOne.exe" "%OutputReleaseFolder%\" > nul 2>&1

rem ---- Unzip the FatTest.vhd zip file

echo Inflating example file 'FatTest.vhd'...

set fatTestFile=%OutputReleaseFolder%\Examples\Drives\FatTest.zip
call :unzip "%OutputReleaseFolder%\Examples\Drives\" "%fatTestFile%"
del /f /q "%fatTestFile%" > nul 2>&1

rem ---- Create the distribution files folder contents

echo Creating Development Environment distributable zip file...
call :zip "%OutputDevEnvFolder%" "%OutputDistributionFolder%\EightyOne V1.%revision% Source.zip"

echo Creating Release distributable zip file...
call :zip "%OutputReleaseFolder%" "%OutputDistributionFolder%\EightyOne V1.%revision%.zip"

rem Remove temporary files that get created
for %%f in ("%OutputDistributionFolder%"\*) do if not %%~xf==.zip del /q "%%f" > nul 2>&1

echo.
echo The distributable zip files have been created here:
echo %OutputDistributionFolder%
echo.
echo ** Check that the distributable zip files have been created correctly **

goto :end

:ErrorFolderNotFound
echo Could not find folder:
echo %TestFolder%
goto :ErrorCreationAborted

:InvalidVersionNumber
echo.
echo Invalid version number.
goto :ErrorCreationAborted

:ErrorCreationAborted
echo.
echo Creation of distributable zip files aborted.
goto :end

:zip
set zipScriptFile="%OutputFolder%\_zipIt.vbs"

echo Set objArgs = WScript.Arguments > %zipScriptFile%
echo InputFolder = objArgs(0) >> %zipScriptFile%
echo ZipFile = objArgs(1) >> %zipScriptFile%
echo CreateObject("Scripting.FileSystemObject").CreateTextFile(ZipFile, True).Write "PK" ^& Chr(5) ^& Chr(6) ^& String(18, vbNullChar) >> %zipScriptFile%
echo Set objShell = CreateObject("Shell.Application") >> %zipScriptFile%
echo Set source = objShell.NameSpace(InputFolder).Items >> %zipScriptFile%
echo objShell.NameSpace(ZipFile).CopyHere(source) >> %zipScriptFile%
echo wScript.Sleep 10000 >> %zipScriptFile%

CScript //nologo %zipScriptFile% %1 %2 > nul 2>&1

del /q %zipScriptFile% > nul 2>&1

goto :eof

:unzip
set unzipScriptFile="%OutputFolder%\_unzipIt.vbs"

echo set objShell = CreateObject("Shell.Application") > %unzipScriptFile%
echo set FilesInZip=objShell.NameSpace(%2).items >> %unzipScriptFile%
echo objShell.NameSpace(%1).CopyHere(FilesInZip) >> %unzipScriptFile%
echo Set objShell = Nothing >> %unzipScriptFile%

cscript //nologo %unzipScriptFile% > nul 2>&1

del /q %unzipScriptFile% > nul 2>&1

goto :eof
    
:end
echo.
pause
