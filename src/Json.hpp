#pragma once

#include "Value.hpp"

const bool enableExtComment	= true;
const bool enableExtBin		= true;
const bool enableExtOct		= true;
const bool enableExtHex		= true;

Value parse(StringRef src);
Value parse(StringRef src, std::string& error);
