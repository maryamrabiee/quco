#ifndef INT_STUFF_H
#define INT_STUFF_H

#include <iostream>

#ifdef N4INT128
	#define INTTYPE_N4 __int128
	#ifdef NRESETINT128
		#define INTTYPE_REST __int128
	#else
		#define INTTYPE_REST long long
	#endif
	
	std::ostream &operator<<(std::ostream &strm, unsigned __int128 value);
	std::ostream &operator<<(std::ostream &strm, __int128 value);
#else
	#define INTTYPE_N4 long long
	#define INTTYPE_REST long long
#endif

#endif