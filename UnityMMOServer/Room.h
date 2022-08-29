#pragma once
#include "JobQueue.h"
#include "Protocol.pb.h"

class Room : public JobQueue
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);
	void BroadcastOtherPlayers(SendBufferRef sendBuffer, uint64 uid);
	bool GetAllPlayerData(OUT Vector<Protocol::PlayerData> &players);

private:
	map<uint64, PlayerRef> _players;
	uint64 roomId;
};

extern shared_ptr<Room> GRoom;