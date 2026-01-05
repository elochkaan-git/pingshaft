#pragma once
#include "ioprotocol.hpp"
#include "server.hpp"
#include "socket.hpp"
#include <arpa/inet.h>
#include <chrono>
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>
#include <vector>

struct sockaddr_in
GetAddr(std::string hostname, uint16_t port)
{
  struct addrinfo* address = nullptr;
  struct addrinfo hints{};
  hints.ai_family = AF_INET;
  int error = getaddrinfo(
    hostname.c_str(), std::to_string(port).c_str(), &hints, &address);
  if (error)
    throw std::system_error(error, std::generic_category(), "getaddrinfo");

  auto output = *(struct sockaddr_in*)address->ai_addr;
  freeaddrinfo(address);
  return output;
}

class Ping
{
public:
  Ping() = default;
  virtual ~Ping() = default;
  virtual Response ping(Server& server) = 0;
};

class JavaPing : public Ping
{
public:
  Response ping(Server& server) override
  {
    TCPSocket sock;
    auto addr = GetAddr(server.hostname, server.port);
    if (connect(sock.Fd(), (struct sockaddr*)&addr, sizeof(addr))) {
      throw std::system_error(errno, std::system_category(), "connect");
    }

    std::vector<uint8_t> handshake;
    WriteVarInt(handshake, 0);
    WriteVarInt(handshake, 0);
    WriteString(handshake, server.hostname);
    WriteU16(handshake, server.port, true);
    WriteVarInt(handshake, 1);

    std::vector<uint8_t> handshakePacket;
    WriteVarLong(handshakePacket, handshake.size());
    handshakePacket.insert(
      handshakePacket.end(), handshake.begin(), handshake.end());

    std::vector<uint8_t> statusRequest;
    WriteVarInt(statusRequest, 0);

    std::vector<uint8_t> statusRequestPacket;
    WriteVarLong(statusRequestPacket, (int32_t)statusRequest.size());
    statusRequestPacket.insert(
      statusRequestPacket.end(), statusRequest.begin(), statusRequest.end());

    if (send(sock.Fd(), handshakePacket.data(), handshakePacket.size(), 0) <
          0 ||
        send(sock.Fd(),
             statusRequestPacket.data(),
             statusRequestPacket.size(),
             0) < 0)
      throw std::system_error(errno, std::system_category(), "send");

    uint64_t packetLength = ReadVarInt(sock.Fd());
    uint64_t packetID = ReadVarInt(sock.Fd());
    uint64_t jsonLength = ReadVarInt(sock.Fd());

    std::vector<char> buffer(jsonLength);
    uint64_t totalSize = 0;
    char* ptr = buffer.data();

    ssize_t n = 1;
    while (totalSize < jsonLength) {
      n = read(sock.Fd(), ptr + totalSize, jsonLength - totalSize);
      if (!n)
        throw std::runtime_error("unexpected EOF");
      if (n < 0)
        throw std::system_error(errno, std::system_category(), "read");
      totalSize += n;
    }

    // Getting latency
    auto now = std::chrono::steady_clock::now();
    std::vector<uint8_t> latencyPacket = { 9, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
    if (send(sock.Fd(), latencyPacket.data(), latencyPacket.size(), 0) < 0)
      throw std::system_error(errno, std::system_category(), "send");
    packetLength = ReadVarInt(sock.Fd());
    packetID = ReadVarInt(sock.Fd());
    if (read(sock.Fd(), latencyPacket.data(), 8) <= 0)
      throw std::system_error(errno, std::system_category(), "read");
    long fut = ReadLong(latencyPacket, 0);
    std::string latency =
      ",\"latency\":" +
      std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - now)
                       .count());
    buffer.insert(buffer.end() - 1, latency.begin(), latency.end());

    Response output{};
    output.data = std::string(buffer.begin(), buffer.end());
    output.platform = Platform::Java;
    return output;
  }
};

class BedrockPing : public Ping
{
public:
  Response ping(Server& server) override
  {
    UDPSocket sock;
    auto addr = GetAddr(server.hostname, server.port);
    if (connect(sock.Fd(), (struct sockaddr*)&addr, sizeof(addr))) {
      throw std::system_error(errno, std::system_category(), "connect");
    }

    auto now = std::chrono::steady_clock::now();
    std::vector<uint8_t> ping = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };
    ping.insert(ping.end(), magic.begin(), magic.end());
    WriteLong(ping, uid);

    if (send(sock.Fd(), ping.data(), ping.size(), 0) < 0)
      throw std::system_error(errno, std::system_category(), "send");

    std::vector<uint8_t> buffer(4096);
    recv(sock.Fd(), buffer.data(), 4096, 0);
    if (buffer[0] != 28)
      std::runtime_error("wrong pong for bedrock");
    time_t time = ReadLong(buffer, 1);
    long sguid = ReadLong(buffer, 9);
    unsigned short int stringSize = ReadU16(buffer, 33);
    std::string latency =
      std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - now)
                       .count()) +
      ";";
    buffer.insert(buffer.begin() + 49 + stringSize, latency.begin(), latency.end());

    Response output{};
    output.data = std::string(
      buffer.begin() + 49, buffer.begin() + 49 + stringSize + latency.size());
    output.platform = Platform::Bedrock;
    return output;
  }

private:
  // magic number for RakNet protocol
  std::vector<uint8_t> magic = { 0,   255, 255, 0,   254, 254, 254, 254,
                                 253, 253, 253, 253, 18,  52,  86,  120 };
  int64_t uid = 0x123456789ABCDEF0; // uid of user, can be any
};