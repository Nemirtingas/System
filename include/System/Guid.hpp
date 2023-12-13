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
	union
	{
		struct
		{
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
		struct
		{
			uint64_t Qword1;
			uint64_t Qword2;
		};
		uint8_t Bytes[16];
	};

	inline constexpr GuidData() noexcept : Bytes{} {}
	inline constexpr GuidData(uint32_t dword, uint16_t short1, uint16_t short2, uint16_t short3, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6) noexcept :
		Dword(dword), Short1(short1), Short2(short2), Short3(short3), Byte1(byte1), Byte2(byte2), Byte3(byte3), Byte4(byte4), Byte5(byte5), Byte6(byte6)
	{}
	inline constexpr GuidData(uint64_t qword1, uint64_t qword2) noexcept :
		Qword1(qword1), Qword2(qword2)
	{}
	inline constexpr GuidData(
		uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4,
		uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8,
		uint8_t byte9, uint8_t byte10, uint8_t byte11, uint8_t byte12,
		uint8_t byte13, uint8_t byte14, uint8_t byte15, uint8_t byte16) noexcept :
		Bytes{byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10, byte11, byte12, byte13, byte14, byte15, byte16}
	{}
	inline constexpr GuidData(GuidData const&) = default;
	inline constexpr GuidData(GuidData &&) noexcept = default;
	inline constexpr GuidData& operator=(GuidData const&) = default;
	inline constexpr GuidData& operator=(GuidData&&) noexcept = default;
};
#pragma pack(pop)

inline constexpr bool operator==(System::GuidData const& l, System::GuidData const& r)
{
	return l.Qword1 == r.Qword1 && l.Qword2 == r.Qword2;
}

inline constexpr bool operator!=(System::GuidData const& l, System::GuidData const& r) { return !(l == r); }

inline constexpr bool operator<(System::GuidData const& l, System::GuidData const& r)
{
	if (l.Dword != r.Dword) return l.Dword < r.Dword;
	if (l.Short1 != r.Short1) return l.Short1 < r.Short1;
	if (l.Short2 != r.Short2) return l.Short2 < r.Short2;
	if (l.Short3 != r.Short3) return l.Short3 < r.Short3;
	if (l.Byte1 != r.Byte1) return l.Byte1 < r.Byte1;
	if (l.Byte2 != r.Byte2) return l.Byte2 < r.Byte2;
	if (l.Byte3 != r.Byte3) return l.Byte3 < r.Byte3;
	if (l.Byte4 != r.Byte4) return l.Byte4 < r.Byte4;
	if (l.Byte5 != r.Byte5) return l.Byte5 < r.Byte5;
	return l.Byte6 < r.Byte6;
}

inline constexpr bool operator<=(System::GuidData const& l, System::GuidData const& r)
{
	if (l.Dword != r.Dword) return l.Dword < r.Dword;
	if (l.Short1 != r.Short1) return l.Short1 < r.Short1;
	if (l.Short2 != r.Short2) return l.Short2 < r.Short2;
	if (l.Short3 != r.Short3) return l.Short3 < r.Short3;
	if (l.Byte1 != r.Byte1) return l.Byte1 < r.Byte1;
	if (l.Byte2 != r.Byte2) return l.Byte2 < r.Byte2;
	if (l.Byte3 != r.Byte3) return l.Byte3 < r.Byte3;
	if (l.Byte4 != r.Byte4) return l.Byte4 < r.Byte4;
	if (l.Byte5 != r.Byte5) return l.Byte5 < r.Byte5;
	return l.Byte6 <= r.Byte6;
}

inline constexpr bool operator>(System::GuidData const& l, System::GuidData const& r) { return r < l; }
inline constexpr bool operator>=(System::GuidData const& l, System::GuidData const& r) { return r <= l; }

class Guid
{
	GuidData _Data;

public:
	inline constexpr Guid() {}
	inline constexpr Guid(Guid const& other) { *this = other; }
	inline constexpr Guid(Guid&& other) noexcept { *this = other; }
	inline constexpr Guid(GuidData const& other) { *this = other; }
	inline constexpr Guid(GuidData&& other) noexcept { *this = other; }
	inline explicit Guid(std::string const& stringGuid) { FromString(stringGuid); }
	inline constexpr Guid& operator=(Guid const& other) { _Data = other._Data; return *this; }
	inline constexpr Guid& operator=(Guid&& other) noexcept { _Data = other._Data; return *this; }
	inline constexpr Guid& operator=(GuidData const& other) { _Data = other; return *this; }
	inline constexpr Guid& operator=(GuidData&& other) noexcept { _Data = other; return *this; }
	bool FromString(std::string const& stringGuid);
	std::string ToString(bool upperCaseHex = false) const;
	inline constexpr GuidData GetRawGuid() const { return _Data; }
	inline constexpr void Clear() { _Data = GuidData{}; }

	inline constexpr bool operator==(Guid const& other) const { return _Data == other._Data; }
	inline constexpr bool operator!=(Guid const& other) const { return _Data != other._Data; }
	inline constexpr bool operator <(Guid const& other) const { return _Data < other._Data; }
	inline constexpr bool operator<=(Guid const& other) const { return _Data <= other._Data; }
	inline constexpr bool operator >(Guid const& other) const { return _Data > other._Data; }
	inline constexpr bool operator>=(Guid const& other) const { return _Data >= other._Data; }

	inline constexpr bool operator==(GuidData const& other) const { return _Data == other; }
	inline constexpr bool operator!=(GuidData const& other) const { return _Data != other; }
	inline constexpr bool operator <(GuidData const& other) const { return _Data < other; }
	inline constexpr bool operator<=(GuidData const& other) const { return _Data <= other; }
	inline constexpr bool operator >(GuidData const& other) const { return _Data > other; }
	inline constexpr bool operator>=(GuidData const& other) const { return _Data >= other; }
};

}