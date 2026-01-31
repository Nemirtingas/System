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

#include <System/SystemDetector.h>
#include <System/Date.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <charconv>

namespace System {
namespace Date {

std::string ClockIso8601(std::chrono::system_clock::time_point datetime, bool withMilliseconds)
{
    auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(datetime);
    auto secs = std::chrono::time_point_cast<std::chrono::seconds>(datetime);

    std::time_t t = std::chrono::system_clock::to_time_t(secs);
    std::tm tm = *std::gmtime(&t); // UTC

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    if (withMilliseconds)
        oss << "." << std::setw(6) << std::setfill('0') << std::chrono::duration_cast<std::chrono::microseconds>(now_us - secs).count();
    
    oss << "Z";

    return oss.str();
}

std::chrono::system_clock::time_point ParseIso8601(std::string_view iso8601)
{
    static const std::regex rgx(
        R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d{1,6}))?Z$)"
    );

    auto str = std::string(iso8601);
    std::smatch m;
    if (!std::regex_match(str, m, rgx)) {
        throw std::invalid_argument("Invalid ISO8601: " + str);
    }

    std::tm tm{};
    tm.tm_year = std::stoi(m[1]) - 1900;
    tm.tm_mon = std::stoi(m[2]) - 1;
    tm.tm_mday = std::stoi(m[3]);
    tm.tm_hour = std::stoi(m[4]);
    tm.tm_min = std::stoi(m[5]);
    tm.tm_sec = std::stoi(m[6]);

#if defined(SYSTEM_OS_WINDOWS)
    std::time_t t = _mkgmtime(&tm);
#else
    std::time_t t = timegm(&tm);
#endif

    auto tp = std::chrono::system_clock::from_time_t(t);
    if (m[7].matched)
    {
        std::string frac = m[7];
        frac.append(6 - frac.size(), '0');
        tp += std::chrono::microseconds(std::stol(frac));
    }

    return tp;
}

}
}
