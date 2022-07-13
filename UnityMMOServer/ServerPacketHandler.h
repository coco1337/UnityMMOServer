#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"

enum 
{
  S_TEST = 1
};

class ServerPacketHandler
{
public:
  static void HandlePacket(BYTE* buffer, int32 len);
};

