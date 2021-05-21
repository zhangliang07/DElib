#ifndef EVOOPERATOR_H
#define EVOOPERATOR_H
#if defined(EVOOPERATOR_LIBRARY)
#  define EVOOPERATOR_EXPORT __declspec(dllexport)
#else
#  define EVOOPERATOR_EXPORT __declspec(dllimport)
#endif


//these APIs are for the seeds operator libaray
//the seeds operator must includs these function, and return 0 if success
class EvoOperator {
public:
	virtual ~EvoOperator() {};

	virtual const char* getTotalInfo() = 0;
	virtual int getOperatorCount() = 0;
	virtual void switchOperator(int index) = 0;
	virtual const char* getOperatorName(int index) = 0;
	virtual const char* getOperatorInfo(int index) = 0;

	virtual int getOperatorConfigCount() = 0;
	virtual int setOperatorConfigValue(int configId, double value) = 0;
	virtual void setParameterBounds(int parameterCount,
		const double* lowerBounds, const double* upperBounds) = 0;
	virtual void setSeedCount(int count) = 0;

	//preprocess and postprocess of seeds in library. step start from 1
	virtual int seedsFirstProcess(const double* results, double** seeds) = 0;
	virtual int seedsProcessThread(int step, const double* results,
		double** seeds, int startIndex, int endIndex) = 0;
};


extern "C" EVOOPERATOR_EXPORT EvoOperator* newEvoOperator();


#endif // EVOOPERATOR_H
