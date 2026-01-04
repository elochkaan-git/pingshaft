#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

void
WriteU16(std::vector<uint8_t>& buffer, uint16_t value, bool port = true);
void
WriteLong(std::vector<uint8_t>& buffer, int64_t value);
void
WriteVarInt(std::vector<uint8_t>& buffer, int32_t value);
void
WriteVarLong(std::vector<uint8_t>& buffer, int64_t value);
void
WriteString(std::vector<uint8_t>& buffer, std::string value);

uint16_t
ReadU16(std::vector<uint8_t>& buffer, size_t pos);
int64_t
ReadLong(std::vector<uint8_t>& buffer, size_t pos);
int32_t
ReadVarInt(int32_t socket);