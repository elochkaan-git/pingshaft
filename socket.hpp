#include <cstdint>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

class TCPSocket
{
public:
  TCPSocket()
  {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
      throw std::runtime_error("Creating socket error");
  }

  int32_t Fd() { return fd; }

  ~TCPSocket() { close(fd); }

private:
  int32_t fd;
};

class UDPSocket
{
public:
  UDPSocket()
  {
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
      throw std::runtime_error("Creating socket error");
  }

  int32_t Fd() { return fd; }

  ~UDPSocket() { close(fd); }

private:
  uint32_t fd;
};