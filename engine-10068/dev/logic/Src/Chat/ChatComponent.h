#pragma once
#ifndef __CHAT_COMPONENT_HEADER__
#define __CHAT_COMPONENT_HEADER__
#include <list>
#include "Network/protocol/PacketDefine.h"
#include "raknet/include/BitStream.h"

struct ChatComponent : public ObjectAlloc
{

	list<std::shared_ptr<ChatComponent>>::type siblings;

	virtual ~ChatComponent() = default;
	virtual void netRead(RakNet::BitStream *dataStream)
	{
		Serializer::read(siblings, dataStream);
	}

	virtual void netWrite(RakNet::BitStream *dataStream) const
	{
		Serializer::write(siblings, dataStream);
	}
};

struct ChatComponentString : public ChatComponent
{

	i32 sendId = 0;
	String msg = "";
	String payerNickName = "";

	ChatComponentString() {

	}

	ChatComponentString(int sendId ,String msg) 
	{
		this->sendId = sendId;
		this->msg = msg;
	}

	ChatComponentString(int sendId, String msg, String nickName)
	{
		this->sendId = sendId;
		this->msg = msg;
		this->payerNickName = nickName;
	}

	void netRead(RakNet::BitStream *dataStream) override
	{
		ChatComponent::netRead(dataStream);
		Serializer::read(sendId, dataStream);
		Serializer::read(msg, dataStream);
		Serializer::read(payerNickName, dataStream);
		
	}

	void netWrite(RakNet::BitStream *dataStream) const  override
	{
		ChatComponent::netWrite(dataStream);
		Serializer::write(sendId, dataStream);
		Serializer::write(msg, dataStream);
		Serializer::write(payerNickName, dataStream);
	}
};

class FormatArgs : public ObjectAlloc
{
public:
	enum class Type : ui8 
	{
		INT,
		STRING,
		PLAYER_ID,
		CHAT_COMPONENT,
		CHAT_COMPONENT_STRING,
		CHAT_COMPONENT_TRANSLATION
	};

	Type type = Type::STRING;
	i32 intValue = 0;
	i32 playerId = 0;
	String strValue = "";
	std::shared_ptr<ChatComponent> chatValue;
	
public:
	FormatArgs() = default;
	~FormatArgs() {};
	FormatArgs(const FormatArgs&) = default;
	FormatArgs(int value) : type(Type::PLAYER_ID), playerId(value) {}
	FormatArgs(int value , Type type) :  intValue(value) , type(type) {}
	FormatArgs(String value) : type(Type::STRING), strValue(value) {}
	FormatArgs(ChatComponent* value) :type(Type::CHAT_COMPONENT), chatValue(value) {}

	void netRead(RakNet::BitStream *dataStream)
	{
		Serializer::read(type, dataStream);
		switch (type)
		{
		case Type::INT:
			Serializer::read(intValue, dataStream);
			break;
		case Type::PLAYER_ID:
			Serializer::read(playerId, dataStream);
			break;
		case Type::STRING:
			Serializer::read(strValue, dataStream);
			break;
		case Type::CHAT_COMPONENT:
			Serializer::read(chatValue, dataStream);
			break;
		}
	}

	void netWrite(RakNet::BitStream *dataStream) const
	{
		Serializer::write(type, dataStream);
		switch (type)
		{
		case Type::INT:
			Serializer::write(intValue, dataStream);
			break;
		case Type::PLAYER_ID:
			Serializer::write(playerId, dataStream);
			break;
		case Type::STRING:
			Serializer::write(strValue, dataStream);
			break;
		case Type::CHAT_COMPONENT:
			Serializer::write(chatValue, dataStream);
			break;
		}
	}
};

struct ChatComponentTranslation : public ChatComponent
{
	String key;
	vector<FormatArgs>::type formatArgs;

	void netRead(RakNet::BitStream *dataStream) override
	{
		ChatComponent::netRead(dataStream);
		Serializer::read(key, dataStream);
		Serializer::read(formatArgs, dataStream);
	}

	void netWrite(RakNet::BitStream *dataStream) const  override
	{
		ChatComponent::netWrite(dataStream);
		Serializer::write(key, dataStream);
		Serializer::write(formatArgs, dataStream);
	}
};

struct ChatComponentNotification : public ChatComponent
{
public:
	enum class Type : ui8
	{
		NATURE,
		TIMING_TOP,
		TIMING_BOTTOM,
		TIMING_CENTER,
		TEAM_RESOURCES_NOTIFICATION,
		TIMING_RIGHT
	};

public:

	Type type = Type::NATURE;
	i32 keepTime = 0;
	String msgContext = "";
	ChatComponentNotification() {};
	ChatComponentNotification(String msgValue) : type(Type::NATURE), msgContext(msgValue) {}
	ChatComponentNotification(String msgValue, i32  timeValue , Type typeValue) : type(typeValue), msgContext(msgValue), keepTime(timeValue) {}

	void netRead(RakNet::BitStream *dataStream) override
	{
		ChatComponent::netRead(dataStream);
		Serializer::read(type, dataStream);
		Serializer::read(keepTime, dataStream);
		Serializer::read(msgContext, dataStream);
	}

	void netWrite(RakNet::BitStream *dataStream) const  override
	{
		ChatComponent::netWrite(dataStream);
		Serializer::write(type, dataStream);
		Serializer::write(keepTime, dataStream);
		Serializer::write(msgContext, dataStream);
	}

};

#endif