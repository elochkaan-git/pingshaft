#pragma once
#include <cstdint>
#include <string>

enum class Platform
{
  Java,
  Bedrock
};

struct Server
{
  std::string hostname = "";
  uint16_t port = 0;
  Platform platform = Platform::Java;
};

struct Response
{
  std::string data = "";
  Platform platform = Platform::Java;
  uint16_t latency = 0;
};