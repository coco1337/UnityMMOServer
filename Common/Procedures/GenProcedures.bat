pushd %~dp0

GenProcs.exe --path=../../UnityMMOServer/GameDB.xml --output=GenProcedures.h

IF ERRORLEVEL 1 PAUSE

XCOPY /Y GenProcedures.h "../../UnityMMOServer"

DEL /Q /F *.h

PAUSE