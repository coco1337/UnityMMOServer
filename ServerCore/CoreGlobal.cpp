#include "pch.h"
#include "CoreGlobal.h"
#include "ConsoleLog.h"

ConsoleLog* GConsoleLogger = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GConsoleLogger = new ConsoleLog();
	}

	~CoreGlobal()
	{
		delete GConsoleLogger;
	}
} GCoreGlobal;