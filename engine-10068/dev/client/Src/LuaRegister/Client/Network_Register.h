#ifndef _NETWORK_REGISTER_H__
#define _NETWORK_REGISTER_H__

#include "Network/ClientNetwork.h"
#include "Network/ClientHttpRequest.h"
#include "LuaRegister/Template/LuaRegister.h"
#include "GameAnalytics/GameAnalyticsRequest.h"

using namespace LUA_REGISTER;

_BEGIN_REGISTER_CLASS(ClientHttpRequest)
_CLASSREGISTER_AddStaticMember(asyncGet, ClientHttpRequest::asyncLuaGet)
_CLASSREGISTER_AddStaticMember(asyncPost, ClientHttpRequest::asyncLuaPost)
_CLASSREGISTER_AddStaticMember(asyncPut, ClientHttpRequest::asyncLuaPut)
_CLASSREGISTER_AddStaticMember(asyncDelete, ClientHttpRequest::asyncLuaDelete)
_CLASSREGISTER_AddStaticMember(friendOperation, ClientHttpRequest::friendOperation)
_END_REGISTER_CLASS()


_BEGIN_REGISTER_CLASS(ClientNetwork)
_CLASSREGISTER_AddStaticMember(Instance, ClientNetwork::Instance)
_CLASSREGISTER_AddMember(getLuaSender, ClientNetwork::getLuaSender)
_CLASSREGISTER_AddMember(getLuaCommonSender, ClientNetwork::getLuaCommonSender)
_CLASSREGISTER_AddMember(getLuaDataCache, ClientNetwork::getLuaDataCache)
_CLASSREGISTER_AddMember(isAlive, ClientNetwork::isAlive)
_CLASSREGISTER_AddMember(isConnected, ClientNetwork::isConnected)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(ClientPacketSender)
_CLASSREGISTER_AddMember(sendCallOnManor, ClientPacketSender::sendCallOnManor)
_CLASSREGISTER_AddMember(sendRebirth, ClientPacketSender::sendRebirth)
_CLASSREGISTER_AddMember(sendChangeCurrentItemInfo, ClientPacketSender::sendChangeCurrentItemInfo)
_CLASSREGISTER_AddMember(sendBindEntity, ClientPacketSender::sendBindEntity)
_CLASSREGISTER_AddMember(sendUnbindEntity, ClientPacketSender::sendUnbindEntity)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(ClientCommonDataSender)
_CLASSREGISTER_AddMember(sendClientCommonData, ClientCommonDataSender::sendClientCommonData)
_CLASSREGISTER_AddMember(sendLuaCommonData, ClientCommonDataSender::sendLuaCommonData)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(DataCache)
_CLASSREGISTER_AddMember(getClientId, DataCache::getClientId)
_CLASSREGISTER_AddMember(getServerId, DataCache::getServerId)
_END_REGISTER_CLASS()

_BEGIN_REGISTER_CLASS(GameAnalyticsRequest)
_CLASSREGISTER_AddStaticMember(Instance, GameAnalyticsRequest::Instance)
_CLASSREGISTER_AddMember(asyncEvents, GameAnalyticsRequest::asyncEvents)
_END_REGISTER_CLASS()

#endif