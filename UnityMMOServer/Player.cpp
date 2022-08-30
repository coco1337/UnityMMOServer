#include "pch.h"
#include "Player.h"

void Player::Init()
{
	_moveData.mutable_movedir()->set_x(0);
	_moveData.mutable_movedir()->set_y(0);
	_moveData.mutable_movedir()->set_z(0);
	_moveData.mutable_position()->set_x(0);
	_moveData.mutable_position()->set_y(0);
	_moveData.mutable_position()->set_z(0);
	_moveData.mutable_rotation()->set_x(0);
	_moveData.mutable_rotation()->set_y(0);
	_moveData.mutable_rotation()->set_z(0);
}

void Player::UpdateMoveData(const Protocol::MoveData& moveData)
{
	_moveData.mutable_movedir()->CopyFrom(moveData.movedir());
	_moveData.mutable_position()->CopyFrom(moveData.position());
	_moveData.mutable_rotation()->CopyFrom(moveData.rotation());
}
