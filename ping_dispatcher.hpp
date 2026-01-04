#pragma once
#include "ping.hpp"
#include "server.hpp"

class PingDispatcher
{
public:
  PingDispatcher(Ping* javaPing, Ping* bedrockPing)
  {
    jp = javaPing;
    bp = bedrockPing;
  }

  Response ping(Server& server)
  {
    if (server.platform == Platform::Java)
      return jp->ping(server);
    else
      return bp->ping(server);
  }

private:
  Ping* jp = nullptr;
  Ping* bp = nullptr;
};