#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include <tchar.h>
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "DBSynchronizer.h"
#include "Job.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임로직
		service->GetIocpCore()->Dispatch(10);

		ThreadManager::DistributeReservedJobs();

		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=192.168.50.164,1433;Database=GameDB;Uid=mmoServer;Pwd=Coco1337!;"));
	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");
	GDBConnectionPool->Push(dbConn);

	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"192.168.50.254", 8888),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100
		);

	ASSERT_CRASH(service->Start());

	wcout << L"Server start" << '\n';

	for (int i = 0; i < 5; ++i)
	{
		GThreadManager->Launch([&service]()
			{
				while (true)
				{
					//service->GetIocpCore()->Dispatch();
					DoWorkerJob(service);
				}
			});
	}
	
	DoWorkerJob(service);

	// SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
	// GSessionManager.Broadcast(sendBuffer);

	GThreadManager->Join();
}