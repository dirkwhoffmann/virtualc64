// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "StringUtils.h"
#include <sstream>

namespace util {

string createStr(const u8 *buf, isize maxLen)
{
	string result;
	
	for (isize i = 0; i < maxLen && buf[i] != 0; i++) {
		result += char(buf[i]);
	}
	return result;
}

string createAscii(const u8 *buf, isize len, char fill)
{
	string result;
	
	for (isize i = 0; i < len; i++) {
		result += isprint(int(buf[i])) ? char(buf[i]) : fill;
	}
	return result;
}

bool
parseHex(const string &s, isize *result)
{
	try {
		size_t pos = 0;
		auto num = std::stoll(s, &pos, 16);

		if (pos == s.size()) {

			*result = (isize)num;
			return true;
		}
		return false;
	} catch (...) {
		return false;
	}
}

string
lowercased(const string& s)
{
	string result;
	for (auto c : s) { result += (char)std::tolower(c); }
	return result;
}

string
uppercased(const string& s)
{
	string result;
	for (auto c : s) { result += (char)std::toupper(c); }
	return result;
}

string
makePrintable(const string& s)
{
	string result;
	for (auto c : s) {
		if (isprint(c)) {
			result += c;
		} else {
			result += "[" + hexstr<2>(c) + "]";
		}
	}
	return result;
}

string
ltrim(const string &s, const string &characters)
{
	auto pos = s.find_first_not_of(characters);
	return (pos == string::npos) ? "" : s.substr(pos);
}

string
rtrim(const string &s, const string &characters)
{
	auto pos = s.find_last_not_of(characters);
	return (pos == string::npos) ? "" : s.substr(0, pos + 1);
}

string
trim(const string &s, const string &characters)
{
	return ltrim(rtrim(s, characters), characters);
}

std::vector<string>
split(const string &s, char delimiter)
{
	std::stringstream ss(s);
	std::vector<std::string> result;
	string substr;
	
	while(std::getline(ss, substr, delimiter)) {
		result.push_back(substr);
	}
	
	return result;
}

string
concat(std::vector<string> &s, string delimiter)
{
	string result;
	
	isize count = (isize)s.size();
	for (isize i = 0; i < count; i++) {
		
		if (!result.empty()) result += delimiter;
		result += s[i];
	}
	
	return result;
}

template <isize digits> string hexstr(isize number)
{
	char str[digits + 1];
	
	str[digits] = 0;
	for (isize i = digits - 1; i >= 0; i--, number >>= 4) {
		
		auto nibble = number & 0xF;
		str[i] = (char)(nibble < 10 ? '0' + nibble : 'a' + (nibble - 10));
	}
	
	return string(str, digits);
}

template string hexstr <1> (isize number);
template string hexstr <2> (isize number);
template string hexstr <4> (isize number);
template string hexstr <6> (isize number);
template string hexstr <8> (isize number);
template string hexstr <16> (isize number);
template string hexstr <32> (isize number);
template string hexstr <64> (isize number);

string byteCountAsString(isize size)
{
	auto kb = size / 1024;
	auto mb = size / (1024 * 1024);
	auto gb = size / (1024 * 1024 * 1024);
	auto kbfrac = (size * 100 / 1024) % 100;
	auto mbfrac = (size * 100 / (1024 * 1024)) % 100;
	auto gbfrac = (size * 100 / (1024 * 1024 * 1024)) % 100;

	if (size < 1024) {

		return std::to_string(size) + " Bytes";
	}
	if (size < 1024 * 1024) {

		auto frac = kbfrac == 0 ? "" : ("." + std::to_string(kbfrac));
		return std::to_string(kb) + frac + " KB";
	}
	if (size < 1024 * 1024 * 1024) {
		
		auto frac = mbfrac == 0 ? "" : ("." + std::to_string(mbfrac));
		return std::to_string(mb) + frac + " MB";
	}
	
	auto frac = gbfrac == 0 ? "" : ("." + std::to_string(gbfrac));
	return std::to_string(gb) + frac + " GB";
}

string fillLevelAsString(double percentage)
{
	if (percentage < 1.0) {
		return "0." + std::to_string(int(percentage * 100)) + "%";
	} else {
		return std::to_string(int(percentage)) + "%";
	}
}

}
