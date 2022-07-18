pushd %~dp0
protoc.exe -I=./ --cpp_out=./  ./Enum.proto
protoc.exe -I=./ --cpp_out=./  ./Struct.proto
protoc.exe -I=./ --cpp_out=./  ./Protocol.proto
IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../UnityMMOServer"
XCOPY /Y Enum.pb.cc "../../../UnityMMOServer"
XCOPY /Y Struct.pb.h "../../../UnityMMOServer"
XCOPY /Y Struct.pb.cc "../../../UnityMMOServer"
XCOPY /Y Protocol.pb.h "../../../UnityMMOServer"
XCOPY /Y Protocol.pb.cc "../../../UnityMMOServer"