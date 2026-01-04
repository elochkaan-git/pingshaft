#include "ioprotocol.hpp"
#include <cstdint>
#include <unistd.h>

uint16_t
ReadU16(std::vector<uint8_t>& buffer, size_t pos)
{
  uint16_t output = 0;
  output |= buffer[pos] << 8;
  output |= buffer[pos + 1];
  return output;
}

int64_t
ReadLong(std::vector<uint8_t>& buffer, size_t pos)
{
  int64_t output = 0;
  for (int i = 0; i < 8; ++i)
    output |= (buffer[pos + i] << (8 * (7 - i)));
  return output;
}

int32_t
ReadVarInt(int32_t socket)
{
  uint64_t output = 0;
  uint8_t temp = 0;
  uint8_t i = 0;

  while (1) {
    int status = read(socket, &temp, 1);
    if (status <= 0)
      return -1;
    output |= (uint64_t)(temp & 0x7F) << (7 * i);
    if (!(temp & 0x80))
      break;

    if (++i > 5)
      return -1;
  }

  return output;
}