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
 * License along with the System; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <cstdint>

namespace System {

enum class TranslatedMode : uint8_t
{
	Unavailable,
	Native,
	Translated,
};

std::chrono::system_clock::time_point GetBootTime();
std::chrono::microseconds GetUpTime();

int32_t GetParentProcessId();
int32_t GetProcessId();
// Get the current process argv
std::vector<std::string> GetProcArgs();
// Get user env variable
std::string GetEnvVar(std::string const& var);
// Set user env variable
bool SetEnvVar(std::string const& key, std::string const& value);
// Unset user env variable
bool UnsetEnvVar(std::string const& key);
// User appdata full path
std::string GetUserdataPath();
// Executable full path
std::string GetExecutablePath();
// .dll, .so or .dylib full path
std::string GetModulePath();
// List all loaded modules
std::vector<std::string> GetModules();
// Set current thread name
bool SetCurrentThreadName(std::string const& thread_name);
// Get if the program is running in translated mode
TranslatedMode GetTranslatedMode();

}
