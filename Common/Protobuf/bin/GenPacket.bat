pushd %~dp0
protoc.exe -I=./ --cpp_out=./ --csharp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ --csharp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ --csharp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=CS_ --send=SC_
IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../UnityMMOServer"
XCOPY /Y Enum.pb.cc "../../../UnityMMOServer"
XCOPY /Y Struct.pb.h "../../../UnityMMOServer"
XCOPY /Y Struct.pb.cc "../../../UnityMMOServer"
XCOPY /Y Protocol.pb.h "../../../UnityMMOServer"
XCOPY /Y Protocol.pb.cc "../../../UnityMMOServer"
XCOPY /Y ClientPacketHandler.h "../../../UnityMMOServer"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE