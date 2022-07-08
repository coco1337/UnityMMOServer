#pragma once
class Player
{
public:
	uint64 playerId = 0;
	string name;
	GameSessionRef ownerSession; // cycle 문제 생길 수 있음
};

