#include "ServerNetworkCore.h"
using namespace std::placeholders;
ServerNetworkCore::ServerNetworkCore(const char *ip, short port)
{
	strcpy(m_ip, ip);
	m_port = port;
	m_serverInstance = LORD::make_shared<RaknetInstance>(ip, port, std::chrono::milliseconds(25));
}


ServerNetworkCore::~ServerNetworkCore(void)
{

}

void ServerNetworkCore::host(ui32 maxSession)
{
	m_serverInstance->listen(maxSession);

}
void ServerNetworkCore::closeSession(const ui64 raknetId)
{
	m_serverInstance->closeSession(raknetId);
}


