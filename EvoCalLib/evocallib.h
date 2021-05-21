//起啥名字好呢
//叫 高可配置性且模块化的演化算法工具
//	通用型便携式电机振动特性分析平台


#ifndef EVOCALLIB_H
#define EVOCALLIB_H
#if defined(EVOCALLIB_LIBRARY)
#  define EVOCALLIB_EXPORT __declspec(dllexport)
#else
#  define EVOCALLIB_EXPORT __declspec(dllimport)
#endif


typedef void EvoCallBack();
class EvoCalLib;
extern "C" EVOCALLIB_EXPORT EvoCalLib* newEvoCalLib();


//this class is use for export
class EvoCalLib {
public:
	virtual ~EvoCalLib() {};

	virtual int loadObjFunctionLibrary(const char* libPath) = 0;
	virtual void releaseObjFunctionLibrary() = 0;
	virtual const char* getObjFunctionTotalInfo() {return "none";};
	virtual int getObjFunctionCount() = 0;
	virtual int switchObjFunction(int index) = 0;
	virtual const char* getObjFunctionName(int index) = 0;
	virtual int getParametersCount() = 0;

	virtual int loadOperatorLibrary(const char* libPath) = 0;
	virtual void releaseOperatorLibrary() = 0;
	virtual const char* getOperatorTotalInfo() {return "none";};
	virtual int getOperatorCount() = 0;
	virtual int switchOperator(int index) = 0;
	virtual const char* getOperatorName(int index) = 0;
	virtual int getOperatorConfigCount() = 0;
	virtual int setOperatorConfigValue(int configId, double value) = 0;

	virtual void setSeedCount(int count) = 0;
	virtual int getSeedCount() = 0;

	//default is 1e-10
	virtual void setStopTolerance(double tolerance) = 0;
	virtual double getStopTolerance() = 0;

	//default is maximum = -1, means infinite steps
	virtual void setIterationMaximum(int maximum) = 0;
	virtual int getIterationMaximum() = 0;

	//default is 4
	virtual void setThreadCount(int num) = 0;
	virtual int getThreadCount() = 0;

	//use for debug
	virtual void registerCallBack(EvoCallBack* preCallback, EvoCallBack* postCallback) = 0;

	//used to observe the seeds values
	virtual int start() = 0;
	virtual void stop() = 0;

	virtual const char* getErrorInfo() = 0;
	virtual int getCurrentStep() = 0;
	virtual double getCostTime() = 0;
	virtual int getBestResultsIndex() = 0;
	virtual const double* const* getAllSeedsValues() = 0;
	virtual const double* getAllObjFunctionResults() = 0;
};


#endif // EVOCALLIB_H
