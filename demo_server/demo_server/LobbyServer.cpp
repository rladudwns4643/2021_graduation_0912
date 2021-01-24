#include "LobbyServer.h"
#include "protocol.h"

LobbyServer::LobbyServer(short lobby_id)
{
}

LobbyServer::~LobbyServer()
{
}

void LobbyServer::ClinetAccept(int id)
{
}

void LobbyServer::Worker()
{
}

void LobbyServer::ProcessPacket(int id, void* buf)
{
}

void LobbyServer::SendLoginOKPacket(int id)
{
}

void LobbyServer::SendLoginFailPacket(int id)
{
}

void LobbyServer::SendSignUpOKPacket(int id, int mmr)
{
}

void LobbyServer::SendSignUpFailPacket(int id)
{
}

void LobbyServer::SendMatchPacket(int id, short roomNo, char is_host)
{
}

void LobbyServer::SendUserInfoPacket(int id, int mmr, int winCnt)
{
}

void LobbyServer::SendCancelAutoMatchSuccess(int id)
{
}

void LobbyServer::SendPacket(int id, void* buf)
{
}

void LobbyServer::error_display(const char* msg, int err_no)
{
}
