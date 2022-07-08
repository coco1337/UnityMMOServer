#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"

Listener::~Listener()
{
}

bool Listener::StartAccept(ServerServiceRef service)
{
	return false;
}

void Listener::CloseSocket()
{
}

HANDLE Listener::GetHandle()
{
	return HANDLE();
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
}
