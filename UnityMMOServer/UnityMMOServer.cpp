#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include <iostream>
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"

int main()
{
	Protocol::S_TEST pkt;
	pkt.set_id(1000);
	pkt.set_hp(100);
	pkt.set_attack(10);
	
	{
		Protocol::BuffData* data = pkt.add_buffs();
		data->set_buffid(100);
		data->set_remaintime(1.2f);
		data->add_victims(4000);
	}
	
	{
		Protocol::BuffData* data = pkt.add_buffs();
		data->set_buffid(200);
		data->set_remaintime(1.2f);
		data->add_victims(5000);
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	GSessionManager.Broadcast(sendBuffer);
}