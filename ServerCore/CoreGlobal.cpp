#include "pch.h"
#include "CoreGlobal.h"
#include "Memory.h"
#include "ConsoleLog.h"

Memory* GMemory = nullptr;
ConsoleLog* GConsoleLogger = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GMemory = new Memory();
		GConsoleLogger = new ConsoleLog();
	}

	~CoreGlobal()
	{
		delete GMemory;
		delete GConsoleLogger;
	}
} GCoreGlobal;