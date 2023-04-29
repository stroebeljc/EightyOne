@echo off
setlocal enabledelayedexpansion
cls

set SourceFolder=%cd%\Source
set SourceBackupFolder=%cd%\SourceBackup
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
echo Source backup folder      = %SourceBackupFolder%
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

set TestFolder=%ComponentsFolder%\ComPort Library
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ComponentsFolder%\DirectDraw
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ComponentsFolder%\DirectSound
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ComponentsFolder%\Office Button 97
if not exist "%TestFolder%" goto :ErrorFolderNotFound

set TestFolder=%ComponentsFolder%\Theme Manager
if not exist "%TestFolder%" goto :ErrorFolderNotFound

rem ---- Check EightyOne executable exists

if not exist "%SourceFolder%\EightyOne.exe" goto :ErrorExecutableNotFound

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

rem ---- Create a backup of the Source folder

rmdir /s /q "%SourceBackupFolder%" > nul 2>&1

if exist "%SourceBackupFolder%" goto :ErrorSourceBackupFailed

xcopy /e /q "%SourceFolder%\*.*" "%SourceBackupFolder%\" > nul 2>&1
if not %errorlevel%==0 goto :ErrorSourceBackupFailed

if not exist "%SourceBackupFolder%" goto :ErrorSourceBackupFailed

rem ---- Clean the source folder

git clean -q -f -d -x "%SourceFolder%"
if not %errorlevel%==0 goto :ErrorCleaningSourceFolder

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
xcopy /e "%SourceFolder%\*.*"            "%OutputDevEnvFolder%\Source\" > nul 2>&1
xcopy /y "%DocumentationFolder%\*.*"     "%OutputDevEnvFolder%\" > nul 2>&1

rem ---- Clean the Development Environment source folder

del /s /q "%ReleaseFilesFolder%\*.bak" > nul 2>&1

rem ---- Create the release folder contents

echo Constructing the Release folder...

xcopy /e "%ReleaseFilesFolder%\*.*"           "%OutputReleaseFolder%\" > nul 2>&1
xcopy /e "%SourceFolder%\ROMs\*.*"            "%OutputReleaseFolder%\ROMs\" > nul 2>&1
xcopy /e "%SourceFolder%\ROM Cartridges\*.*"  "%OutputReleaseFolder%\ROM Cartridges\" > nul 2>&1
xcopy /y "%SourceBackupFolder%\EightyOne.exe" "%OutputReleaseFolder%\" > nul 2>&1

rem ---- Unzip the FatTest.vhd zip file

echo Inflating example file 'FatTest.vhd'...

set fatTestFile=%OutputReleaseFolder%\Examples\Drives\FatTest.zip
call :unzip "%OutputReleaseFolder%\Examples\Drives\" "%fatTestFile%"
del /f /q "%fatTestFile%" > nul 2>&1

rem ---- Unzip the divIDETest.hdf zip file

echo Inflating example file 'divIDETest.vhd'...

set divIDETestFile=%OutputReleaseFolder%\Examples\Drives\divIDETest.zip
call :unzip "%OutputReleaseFolder%\Examples\Drives\" "%divIDETestFile%"
del /f /q "%divIDETestFile%" > nul 2>&1

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

rem ---- Restore from the backup of the Source folder

xcopy /e /q /r /y "%SourceBackupFolder%\*.*" "%SourceFolder%\" > nul 2>&1
if not %errorlevel%==0 goto :ErrorSourceBackupRestorationFailed

rmdir /s /q "%SourceBackupFolder%" > nul 2>&1

if exist "%SourceBackupFolder%" goto :ErrorSourceBackupRestorationFailed

goto :end

:ErrorFolderNotFound
echo Could not find folder:
echo %TestFolder%
goto :ErrorCreationAborted

:ErrorSourceBackupFailed
echo Failed to create backup of the source folder
goto :ErrorCreationAborted

:ErrorSourceBackupRestorationFailed
echo.
echo Failed to restore the source folder from the backup
goto :ErrorCreationAborted

:ErrorCleaningSourceFolder
echo.
echo Failed to clean the source folder
echo.
echo ** Make sure to manually restore the source folder from the backup **
goto :ErrorCreationAborted

:ErrorExecutableNotFound
echo Could not find EightyOne.exe
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
