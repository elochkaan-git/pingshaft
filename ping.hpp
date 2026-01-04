#pragma once
#include "ioprotocol.hpp"
#include "server.hpp"
#include "socket.hpp"
#include <arpa/inet.h>
#include <cstdint>
#include <ctime>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

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
    struct addrinfo* address = nullptr;
    struct addrinfo hints{};
    hints.ai_family = AF_INET;
    int error = getaddrinfo(server.hostname.c_str(),
                            std::to_string(server.port).c_str(),
                            &hints,
                            &address);
    if (error)
      throw std::runtime_error("getaddrinfo error");
    
    auto addr = *(struct sockaddr_in*)address->ai_addr;
    if (connect(sock.Fd(), (struct sockaddr*)&addr, sizeof(addr))) {
      throw std::runtime_error("connect error");
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

    send(sock.Fd(), handshakePacket.data(), handshakePacket.size(), 0);
    send(sock.Fd(), statusRequestPacket.data(), statusRequestPacket.size(), 0);

    uint64_t packetLength = ReadVarInt(sock.Fd());
    uint64_t packetID = ReadVarInt(sock.Fd());
    uint64_t jsonLength = ReadVarInt(sock.Fd());

    std::vector<char> buffer(jsonLength);
    uint64_t totalSize = 0;
    char* ptr = buffer.data();

    while (totalSize < jsonLength) {
      ssize_t n = read(sock.Fd(), ptr + totalSize, jsonLength - totalSize);
      if (n <= 0)
        return Response{};
      totalSize += n;
    }

    Response output{};
    output.data = std::string(buffer.begin(), buffer.end());
    output.platform = Platform::Java;
    freeaddrinfo(address);
    return output;
  }
};

class BedrockPing : public Ping
{
public:
  Response ping(Server& server) override
  {
    UDPSocket sock;
    std::time(&now);
    struct addrinfo* address = nullptr;
    struct addrinfo hints{};
    hints.ai_family = AF_INET;
    int error = getaddrinfo(server.hostname.c_str(),
                            std::to_string(server.port).c_str(),
                            &hints,
                            &address);
    if (error)
      throw std::runtime_error("getaddrinfo error");

    auto addr = *(struct sockaddr_in*)address->ai_addr;
    if (connect(sock.Fd(), (struct sockaddr*)&addr, sizeof(addr))) {
      throw std::runtime_error("connect error");
    }

    std::vector<uint8_t> ping = { 1 };
    WriteLong(ping, now);
    ping.insert(ping.end(), magic.begin(), magic.end());
    WriteLong(ping, uid);

    send(sock.Fd(), ping.data(), ping.size(), 0);

    std::vector<uint8_t> buffer(4096);
    recv(sock.Fd(), buffer.data(), 4096, 0);
    if (buffer[0] != 28)
      std::runtime_error("incorrect pong");
    time_t time = ReadLong(buffer, 1);
    long sguid = ReadLong(buffer, 9);
    unsigned short int stringSize = ReadU16(buffer, 33);

    Response output{};
    output.data =
      std::string(buffer.begin() + 49, buffer.begin() + 49 + stringSize);
    output.platform = Platform::Bedrock;
    freeaddrinfo(address);
    return output;
  }

private:
  // magic number for RakNet protocol
  std::vector<uint8_t> magic = { 0,   255, 255, 0,   254, 254, 254, 254,
                                 253, 253, 253, 253, 18,  52,  86,  120 };
  int64_t now = 0;                  // time
  int64_t uid = 0x123456789ABCDEF0; // uid of user, can be any
};