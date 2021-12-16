#ifndef SODA_13_CALC_H
	#define SODA_13_CALC_H

	#include "int_stuff.h"
	#include "triplet_calc.h"
	#include "quartet_calc.h"
	#include "hdt.h"
	#ifdef doStopwatch
		#include "stopwatch.h"
	#endif

	using namespace std;

	class Soda13Calc : public TripletCalc, public QuartetCalc
	{
		public:
			Soda13Calc();
			~Soda13Calc();
			INTTYPE_REST calculateTripletDistance(RootedTree *t1, RootedTree *t2);
			INTTYPE_N4 calculateQuartetDistance(UnrootedTree *t1, UnrootedTree *t2);
			#ifdef doCounter
				static long long runtimeThingy;
				#define INC_RUNTIME_COUNTER { Soda13Calc::runtimeThingy++; }
			#else
			#define INC_RUNTIME_COUNTER {}
			#endif

		private:
			HDTFactory *dummyHDTFactory;
			void calcUnresolved(RootedTree *t);
			void count(RootedTree *v);
			//RootedTree* contract(RootedTree *v, int markedColor);
			RootedTree *t1, *t2;
			HDT *hdt;
			INTTYPE_REST resolvedTriplets, unresolvedTriplets;
#ifdef quartetsToo
			INTTYPE_N4 resolvedQuartetsAgree, resolvedQuartetsAgreeDiag, resolvedQuartetsDisagree, resolvedQuartetsDisagreeDiag;
#ifdef quartetsNoSwap
			INTTYPE_N4 resolvedQuartetsAgreeUpper, resolvedQuartetsDisagreeUpper;
#endif
#ifdef calcE
			INTTYPE_N4 unresolvedQuartets;
#endif
#endif
			#ifdef doStopwatch
				Stopwatch stopwatchTotal, stopwatchColor, stopwatchCounting, stopwatchContract;
				#define START_STOPWATCH(NAME)		\
				{									\
					NAME.start();					\
				}
				#define STOP_STOPWATCH(NAME)		\
				{									\
					NAME.stop();					\
				}
			#else
				#define START_STOPWATCH(NAME) {}
				#define STOP_STOPWATCH(NAME) {}
			#endif
	};
#endif
