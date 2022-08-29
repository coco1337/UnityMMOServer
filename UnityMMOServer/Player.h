#pragma once
class Player
{
public:
	void Init();
	tuple<float, float, float> GetPos() { return _position; }
	tuple<float, float, float> GetRot() { return _rotation; }
	Player() { GConsoleLogger->WriteStdOut(Color::GREEN, L"Player()\n"); }
	~Player() { GConsoleLogger->WriteStdOut(Color::GREEN, L"~Player()\n"); }

public:
	uint64 playerId = 0;
	string name;
	weak_ptr<GameSession> ownerSession;

private:
	tuple<float, float, float> _position;
	tuple<float, float, float> _rotation;
};

