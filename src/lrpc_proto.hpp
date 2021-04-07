#pragma once

#include <cstdint>

using cmd_t = std::uint8_t;
using seq_t = std::uint16_t;

enum class CMD : cmd_t
{
	Add    = 0xAA,
	AddRep = 0xBB,
	Sub    = 0xCC,
	SubRep = 0xDD
};

#pragma pack(push, 1)

struct PHead
{
	cmd_t cmd;
	seq_t seq;
};

struct PAdd
{
	PHead hdr;
	int   num1;
	int   num2;
};

struct PAddRep
{
	PHead hdr;
	int   res;
};

struct PSub
{
	PHead hdr;
	int   num1;
	int   num2;
};

struct PSubRep
{
	PHead hdr;
	int   res;
};

#pragma pack(pop)
