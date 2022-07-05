#pragma once
class SendBufferChunk;

class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer; }
	uint32 AllocSize() { return _allocSize; }
	uint32 WriteSize() { return _writeSize; }
	void Close(uint32 writeSize);

private:
	BYTE* _buffer;
	uint32 _allocSize = 0;
	uint32 _writeSize = 0;
	SendBufferChunkRef _owner; // Ref Counting 유지
};

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void Reset();
	SendBufferRef Open(uint32 allocSize);	// 할당할 공간
	void Close(uint32 writeSize); // 실질적으로 사용한 공간

	bool IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	uint32 FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer = {};
	bool _open = false;
	uint32 _usedSize = 0;
};


/*------------
* SendBufferManager
-------------*/
class SendBufferManager
{
public:
	SendBufferRef Open(uint32 size);

private:
	SendBufferChunkRef Pop();
	void Push(SendBufferChunkRef buffer);

	static void PushGlobal(SendBufferChunk* buffer);

private:
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks;
};