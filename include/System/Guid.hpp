/*
 * Copyright (C) Nemirtingas
 * This file is part of System.
 *
 * System is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * System is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with System; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>
#include <string>

namespace System {


#pragma pack(push, 1)
struct GuidData
{
	union {
		struct {
			uint32_t Dword;
			uint16_t Short1;
			uint16_t Short2;
			uint16_t Short3;
			uint8_t Byte1;
			uint8_t Byte2;
			uint8_t Byte3;
			uint8_t Byte4;
			uint8_t Byte5;
			uint8_t Byte6;
		};
		uint8_t Bytes[16];
	};
};
#pragma pack(pop)

inline bool operator==(System::GuidData const& l, System::GuidData const& r)
{
	return *reinterpret_cast<const uint64_t*>(&l.Bytes[0]) == *reinterpret_cast<const uint64_t*>(&r.Bytes[0]) &&
		*reinterpret_cast<const uint64_t*>(&l.Bytes[8]) == *reinterpret_cast<const uint64_t*>(&r.Bytes[8]);
}

inline bool operator!=(System::GuidData const& l, System::GuidData const& r) { return !(l == r); }

class Guid
{
	GuidData _Data;

public:
	inline Guid() :_Data{} {}
	inline Guid(Guid const& other) { *this = other; }
	inline Guid(Guid&& other) noexcept { *this = other; }
	inline explicit Guid(std::string const& stringGuid) { FromString(stringGuid); }
	inline Guid& operator=(Guid const& other) { _Data = other._Data; return *this; }
	inline Guid& operator=(Guid&& other) noexcept { _Data = other._Data; return *this; }
	bool FromString(std::string const& stringGuid);
	std::string ToString(bool upperCaseHex = false) const;
	inline GuidData GetRawGuid() const { return _Data; }
	inline void Clear() { _Data = GuidData{}; }

	inline bool operator==(Guid const& other) const { return _Data == other._Data; }
	inline bool operator!=(Guid const& other) const { return _Data != other._Data; }

	inline bool operator==(GuidData const& other) const { return _Data == other; }
	inline bool operator!=(GuidData const& other) const { return _Data != other; }
};

}