#include "int_stuff.h"
#include <iostream>
#include <iomanip>

#ifdef N4INT128
	std::ostream &operator<<(std::ostream &strm, unsigned __int128 value)
	{
		const unsigned long long uLongLongMax = 18446744073709551615ULL;
		const unsigned long long uNineteenZeroes = 10000000000000000000ULL;
		if (value > uLongLongMax)
		{
			unsigned __int128 first = value / uNineteenZeroes;
			unsigned long long rest = value % uNineteenZeroes;
			operator<<(strm, first);
			strm << std::setfill ('0') << std::setw (19);
			strm << rest;
		}
		else
		{
			// Printable directly
			unsigned long long shorterValue = value;
			strm << shorterValue;
		}
		return strm;
	}
	
	std::ostream &operator<<(std::ostream &strm, __int128 value)
	{
		const unsigned long long uLongLongMax = 18446744073709551615ULL;
		const unsigned long long uNineteenZeroes = 10000000000000000000ULL;
		if (value < 0)
		{
			strm << "(negative value)";
		}
		else if (value > uLongLongMax)
		{
			unsigned __int128 first = value / uNineteenZeroes;
			unsigned long long rest = value % uNineteenZeroes;
			operator<<(strm, first);
			strm << std::setfill ('0') << std::setw (19);
			strm << rest;
		}
		else
		{
			// Printable directly
			unsigned long long shorterValue = value;
			strm << shorterValue;
		}
		return strm;
	}
#endif