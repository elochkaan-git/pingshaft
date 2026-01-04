#include "ioprotocol.hpp"
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <vector>

void
WriteU16(std::vector<uint8_t>& buffer, uint16_t value, bool port)
{
  if (port)
    value = htons(value);
  buffer.push_back(value >> 8);
  buffer.push_back(value & 0xFF);
}

void
WriteLong(std::vector<uint8_t>& buffer, int64_t value)
{
  for (int i = 7; i > -1; --i)
    buffer.push_back((value >> 8 * i) & 0xFF);
}

void
WriteVarInt(std::vector<uint8_t>& buffer, int32_t value)
{
  while (1) {
    unsigned char v = value & 0x7F;
    value >>= 7;
    buffer.push_back(v | (value > 0 ? 0x80 : 0));
    if (!value)
      break;
  }
}

void
WriteVarLong(std::vector<uint8_t>& buffer, int64_t value)
{
  while (1) {
    unsigned char v = value & 0x7F;
    value >>= 7;
    buffer.push_back(v | (value > 0 ? 0x80 : 0));
    if (!value)
      break;
  }
}

void
WriteString(std::vector<uint8_t>& buffer, std::string value)
{
  WriteVarInt(buffer, value.length());
  buffer.insert(buffer.end(), value.begin(), value.end());
}
