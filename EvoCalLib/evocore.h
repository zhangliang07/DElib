#ifndef EVOCORE_H
#define EVOCORE_H
#include "evocallib.h"
#include "EvoObjFunction.h"
#include "EvoOperator.h"
#include <Windows.h>
#include <string>
#include <vector>


class EvoCore: public EvoCalLib {
public:
	explicit EvoCore();
	virtual ~EvoCore() override;

	virtual int loadObjFunctionLibrary(const char* libPath) override;
	virtual void releaseObjFunctionLibrary() override;
	virtual const char* getObjFunctionTotalInfo() override;
	virtual int getObjFunctionCount() override;
	virtual int switchObjFunction(int index) override;
	virtual const char* getObjFunctionName(int index) override;
	virtual int getParametersCount() override;

	virtual int loadOperatorLibrary(const char* libPath) override;
	virtual void releaseOperatorLibrary() override;
	virtual const char* getOperatorTotalInfo() override;
	virtual int getOperatorCount() override;
	virtual int switchOperator(int index) override;
	virtual const char* getOperatorName(int index) override;
	virtual int getOperatorConfigCount() override;
	virtual int setOperatorConfigValue(int configId, double value) override;

	virtual void setSeedCount(int count) override;
	virtual int getSeedCount() override;

	//default is 1e-10
	virtual void setStopTolerance(double tolerance) override;
	virtual double getStopTolerance() override;

	//default is maximum = -1, means infinite steps
	virtual void setIterationMaximum(int maximum) override;
	virtual int getIterationMaximum() override;

	//default is 2
	virtual void setThreadCount(int num) override;
	virtual int getThreadCount() override;

	//use for debug
	virtual void registerCallBack(EvoCallBack* preCallback, EvoCallBack* postCallback) override;

	//used to observe the seeds values
	virtual int start() override;
	virtual void stop() override;

	virtual int getCurrentStep() override;
	virtual double getCostTime() override;
	virtual int getBestResultsIndex() override;
	virtual const double* const* getAllSeedsValues() override;
	virtual const double* getAllObjFunctionResults() override;
	virtual const char* getErrorInfo() override;

private:
	std::string theErrorInfo;

	//the libraries
	HINSTANCE theObjFunctionLibrary;
	HINSTANCE theOperatorLibrary;
	EvoObjFunction* theObjFunction;
	EvoOperator* theOperator;

	//the callback functions
	EvoCallBack* thePreCallback;
	EvoCallBack* thePostCallback;

	//the Parameter bounds, first is lowerBound, second is higherBound
	std::vector<std::pair<double, double>> theParameterBoundList;

	//config
	double theTolerance;
	int theIterTimesMaximum;
	int theThreadCount;

	//seeds
	int theSeedCount;
	int theParameterCount;
	double** theSeeds;//first dim is seed id, second dim is parameters
	double* theResults;

	//results
	int theIterStep;
	int theBestSeedIndex;
	double theBestResult;

	double theCostTime;		//unit: second
	bool theStopFlag;
	int theEqualTimes;

	struct ThreadData {
		const int threadId;
		const int startIndex;
		const int endIndex;
		int step;
		double minResult;
		int minIndex;
		std::string errorInfo;
		ThreadData(int id, int i1, int i2):
			threadId(id), startIndex(i1), endIndex(i2) {}
	};

private:
	void clearSeeds();
	int preCheck();
	void prepare();
	int compute();

	//compute by one thread and return the min result and index
	void objectFunctionThread(ThreadData& data);

	void operatorThread(ThreadData& data);

	inline bool checkFinish(const std::vector<ThreadData>& minResultList);

	void log(const char* info);
	void log(const char* info, int errorCode);
};


#endif // EVOCORE_H
