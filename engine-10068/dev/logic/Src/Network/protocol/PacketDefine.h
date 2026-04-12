#ifndef __PACKETDEFINE_H__
#define __PACKETDEFINE_H__

#include <functional>
#include "Network/protocol/DataPacket.h"
#include "Network/protocol/Serializer.h"
#include "TypeTraits/VoidType.h"
#include "Core.h"
#include "raknet/include/BitStream.h"

using namespace LORD;

template<S2CPacketType type>
class S2CPacket;

template<C2SPacketType type>
class C2SPacket;

template<typename Packet>
class PacketBase
{
private:
	template<typename T>
	static void relayToWrite(RakNet::BitStream* stream, const Packet* objPtr, T Packet::* memberPtr)
	{
		Serializer::write(objPtr->*memberPtr, stream);
	}

	template<typename T>
	static void relayToRead(RakNet::BitStream* stream, Packet* objPtr, T Packet::* memberPtr)
	{
		Serializer::read(objPtr->*memberPtr, stream);
	}

public:
	using EncodeFunction = std::function<void(RakNet::BitStream*, const Packet*)>;
	using DecodeFunction = std::function<void(RakNet::BitStream*, Packet*)>;

protected:
	static typename LORD::vector<EncodeFunction>::type encoders;
	static typename LORD::vector<DecodeFunction>::type decoders;
	static bool isInitialized;

	template<typename T>
	static T addMember(T Packet::* memberPtr)
	{
		if (!isInitialized)
		{
			using namespace std::placeholders;
			encoders.push_back(std::bind(&PacketBase::relayToWrite<T>, _1, _2, memberPtr));
			decoders.push_back(std::bind(&PacketBase::relayToRead<T>, _1, _2, memberPtr));
		}
		return T();
	}

	void callEncoders(RakNet::BitStream * stream) const
	{
		for (const auto& encoder : encoders)
		{
			encoder(stream, static_cast<const Packet*>(this));
		}
	}

	void callDecoders(RakNet::BitStream* stream)
	{
		for (const auto& decoder : decoders)
		{
			decoder(stream, static_cast<Packet*>(this));
		}
	}
};

template<typename Packet>
typename LORD::vector<typename PacketBase<Packet>::EncodeFunction>::type PacketBase<Packet>::encoders;
template<typename Packet>
typename LORD::vector<typename PacketBase<Packet>::DecodeFunction>::type PacketBase<Packet>::decoders;
template<typename Packet>
bool PacketBase<Packet>::isInitialized = false;

// PACKET BEGIN
#define _INNER_BEGIN_PACKET(TYPE, CHANNEL, NAME, PACKETNAME)                         \
using PACKETNAME = NAME < TYPE >;                                                    \
template<>                                                                           \
class NAME < TYPE > : public PacketBase< NAME < TYPE > >, public DataPacket          \
{                                                                                    \
private:                                                                             \
    using Packet = NAME < TYPE >;                                                    \
                                                                                     \
public:                                                                              \
	template<typename = void>                                                        \
	NAME(BLOCKMAN::void_t = BLOCKMAN::void_value) { isInitialized = true; }          \
	int  channel() const override { return CHANNEL; }                                \
	ui16 pid() const override { return static_cast<ui16>(TYPE); }                    \
	void encode(RakNet::BitStream* stream) const override { callEncoders(stream); }  \
	void decode(RakNet::BitStream* stream) override { callDecoders(stream); }

// DEF ARG
#define _SERIALIZE_MEMBER(A, a)                                                \
public:                                                                        \
	A a = addMember(&Packet::a);

//PACKET END
#define _END_PACKET };


// S2C PACKET
#define _BEGIN_PACKET_S2C(TYPE, CHANNEL, PACKETNAME)                           \
template<>                                                                     \
class S2CPacket<TYPE>;                                                         \
_INNER_BEGIN_PACKET(TYPE, CHANNEL, S2CPacket, PACKETNAME)


// C2S PACKET
#define _BEGIN_PACKET_C2S(TYPE, CHANNEL, PACKETNAME)                           \
template<>                                                                     \
class C2SPacket<TYPE>;                                                         \
_INNER_BEGIN_PACKET(TYPE, CHANNEL, C2SPacket, PACKETNAME) 


#define DEFINE_NETWORK_SERIALIZER(...)                                         \
void netRead(RakNet::BitStream *dataStream)                                    \
{                                                                              \
    Serializer::readMultiple(dataStream, ##__VA_ARGS__);                       \
}                                                                              \
void netWrite(RakNet::BitStream *dataStream) const                             \
{                                                                              \
    Serializer::writeMultiple(dataStream, ##__VA_ARGS__);                      \
}

#endif // !__PACKETDEFINE_H__

