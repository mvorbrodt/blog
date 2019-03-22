#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>

#define TCHAR char
#define TEXT(x) x
#define BYTE unsigned char

namespace base64
{
	inline static const TCHAR encodeLookup[] = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	inline static const TCHAR padCharacter = TEXT('=');

	inline std::basic_string<TCHAR> encode(std::vector<BYTE> inputBuffer)
	{
		std::basic_string<TCHAR> encodedString;
		encodedString.reserve(((inputBuffer.size()/3) + (inputBuffer.size() % 3 > 0)) * 4);
		uint32_t temp;
		std::vector<BYTE>::iterator cursor = inputBuffer.begin();
		for(size_t idx = 0; idx < inputBuffer.size()/3; idx++)
		{
			temp  = (*cursor++) << 16;
			temp += (*cursor++) << 8;
			temp += (*cursor++);
			encodedString.append(1,encodeLookup[(temp & 0x00FC0000) >> 18]);
			encodedString.append(1,encodeLookup[(temp & 0x0003F000) >> 12]);
			encodedString.append(1,encodeLookup[(temp & 0x00000FC0) >> 6 ]);
			encodedString.append(1,encodeLookup[(temp & 0x0000003F)      ]);
		}
		switch(inputBuffer.size() % 3)
		{
		case 1:
			temp  = (*cursor++) << 16;
			encodedString.append(1,encodeLookup[(temp & 0x00FC0000) >> 18]);
			encodedString.append(1,encodeLookup[(temp & 0x0003F000) >> 12]);
			encodedString.append(2,padCharacter);
			break;
		case 2:
			temp  = (*cursor++) << 16;
			temp += (*cursor++) << 8;
			encodedString.append(1,encodeLookup[(temp & 0x00FC0000) >> 18]);
			encodedString.append(1,encodeLookup[(temp & 0x0003F000) >> 12]);
			encodedString.append(1,encodeLookup[(temp & 0x00000FC0) >> 6 ]);
			encodedString.append(1,padCharacter);
			break;
		}
		return encodedString;
	}

	std::vector<BYTE> decode(const std::basic_string<TCHAR>& input)
	{
		if (input.length() % 4)
			throw std::runtime_error("Non-Valid base64!");
		size_t padding = 0;
		if (input.length())
		{
			if (input[input.length()-1] == padCharacter)
				padding++;
			if (input[input.length()-2] == padCharacter)
				padding++;
		}
		std::vector<BYTE> decodedBytes;
		decodedBytes.reserve(((input.length()/4)*3) - padding);
		uint32_t temp=0;
		std::basic_string<TCHAR>::const_iterator cursor = input.begin();
		while (cursor < input.end())
		{
			for (size_t quantumPosition = 0; quantumPosition < 4; quantumPosition++)
			{
				temp <<= 6;
				if       (*cursor >= 0x41 && *cursor <= 0x5A)
					temp |= *cursor - 0x41;
				else if  (*cursor >= 0x61 && *cursor <= 0x7A)
					temp |= *cursor - 0x47;
				else if  (*cursor >= 0x30 && *cursor <= 0x39)
					temp |= *cursor + 0x04;
				else if  (*cursor == 0x2B)
					temp |= 0x3E;
				else if  (*cursor == 0x2F)
					temp |= 0x3F;
				else if  (*cursor == padCharacter)
				{
					switch( input.end() - cursor )
					{
					case 1:
						decodedBytes.push_back((temp >> 16) & 0x000000FF);
						decodedBytes.push_back((temp >> 8 ) & 0x000000FF);
						return decodedBytes;
					case 2:
						decodedBytes.push_back((temp >> 10) & 0x000000FF);
						return decodedBytes;
					default:
						throw std::runtime_error("Invalid Padding in Base 64!");
					}
				}  else
					throw std::runtime_error("Non-Valid Character in Base 64!");
				cursor++;
			}
			decodedBytes.push_back((temp >> 16) & 0x000000FF);
			decodedBytes.push_back((temp >> 8 ) & 0x000000FF);
			decodedBytes.push_back((temp      ) & 0x000000FF);
		}
		return decodedBytes;
	}
}
