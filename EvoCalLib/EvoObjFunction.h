#ifndef EVOOBJFUNCTION_H
#define EVOOBJFUNCTION_H
#if defined(EVOOBJFUNCTION_LIBRARY)
#  define EVOOBJFUNCTION_EXPORT __declspec(dllexport)
#else
#  define EVOOBJFUNCTION_EXPORT __declspec(dllimport)
#endif


//these APIs are for the objective function libaray
class EvoObjFunction {
public:
	virtual ~EvoObjFunction() {};

	virtual const char* getTotalInfo() = 0;
	virtual int getFunctionCount() = 0;
	virtual void switchFunction(int index) = 0;
	virtual const char* getFunctionName(int index) = 0;
	virtual const char* getFunctionInfo(int index) = 0;

	//plaese write your objective function as follow format, and make it return 0 when success.
	//please notice that the program will search the minimum of the function
	virtual int objFunction(const double* parameters, double* results) = 0;

	//return the count of Parameters
	virtual int getParameterCount() = 0;
	virtual int getParameterBounds(int index, double* lowerBounds, double* upperBounds) = 0;
};


extern "C" EVOOBJFUNCTION_EXPORT EvoObjFunction* newEvoObjFunction();


#endif // EVOOBJFUNCTION_H
