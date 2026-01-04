#include <cstdint>
#include <sys/socket.h>
#include <unistd.h>

class TCPSocket
{
public:
  TCPSocket() { fd = socket(AF_INET, SOCK_STREAM, 0); }

  int32_t Fd() { return fd; }

  ~TCPSocket() { close(fd); }

private:
  int32_t fd;
};

class UDPSocket
{
public:
  UDPSocket() { fd = socket(AF_INET, SOCK_DGRAM, 0); }

  int32_t Fd() { return fd; }

  ~UDPSocket() { close(fd); }

private:
  uint32_t fd;
};