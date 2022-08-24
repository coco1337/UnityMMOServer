#pragma once
#include "Session.h"
#include "Player.h"

class GameSession : public PacketSession
{
public:
	~GameSession()
	{
		if (_currentPlayer == nullptr)
		{
			GConsoleLogger->WriteStdOut(Color::GREEN, L"~GameSession\n");
		}
		else
		{
			GConsoleLogger->WriteStdOut(Color::GREEN, L"~GameSession %s\n",_currentPlayer->name );
		}
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:
	Vector<PlayerRef> _players;

	PlayerRef _currentPlayer;
	weak_ptr<class Room> _room;
};

