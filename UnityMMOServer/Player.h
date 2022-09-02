#pragma once
#include "Protocol.pb.h"

class Player
{
public:
	void Init();
	Protocol::MoveData GetMoveData() { return _moveData; }
	void UpdateMoveData(const Protocol::MoveData& moveData);
	// Player() { GConsoleLogger->WriteStdOut(Color::GREEN, L"Player()\n"); }
	// ~Player() { GConsoleLogger->WriteStdOut(Color::GREEN, L"~Player()\n"); }

public:
	uint64 playerId = 0;
	string name;
	weak_ptr<GameSession> ownerSession;

private:
	Protocol::MoveData _moveData;
};

