#pragma once
#include "raknet/include/BitStream.h"

// Integer that does not use varint encoding for network transmission
template<typename T>
class FixedLengthInteger
{
public:
	FixedLengthInteger() = default;
	FixedLengthInteger(T value) : value(value) {}
	operator T() const { return value; }
	void netWrite(RakNet::BitStream* stream) const { stream->Write(value); }
	void netRead(RakNet::BitStream* stream) { stream->Read(value); }

private:
	T value;
};