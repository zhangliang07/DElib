#include "evocore.h"
#include <cmath>
#include <ctime>
#include <thread>
using namespace std;


EvoCore::EvoCore():
	theObjFunctionLibrary(nullptr),
	theOperatorLibrary(nullptr),
	theObjFunction(nullptr),
	theOperator(nullptr),
	thePreCallback(nullptr),
	thePostCallback(nullptr),
	theTolerance(-1),
	theIterTimesMaximum(10000),
	theThreadCount(4),
	theSeedCount(10000),
	theParameterCount(-1),
	theSeeds(nullptr),
	theResults(nullptr),
	theIterStep(-1),
	theBestSeedIndex(-1),
	theBestResult(-1),
	theStopFlag(false) {
//	AllocConsole();
//	AttachConsole(ATTACH_PARENT_PROCESS); // 将当前程序附着到父进程上，因为是从控制台启动的，所以当前程序的父进程就是那个控制台。
//	freopen("CONOUT$", "w", stdout);   // 重定向STDOUT
//	printf("debug\n");
}


EvoCore::~EvoCore() {
	clearSeeds();
	releaseObjFunctionLibrary();
	releaseOperatorLibrary();
}


int EvoCore::loadObjFunctionLibrary(const char* libPath) {
	theObjFunctionLibrary = LoadLibraryA(libPath);
	if(!theObjFunctionLibrary) return -1;

	typedef EvoObjFunction* NewFunction();
	NewFunction* newFunction = (NewFunction*)GetProcAddress(theObjFunctionLibrary, "newEvoObjFunction");
	if(!newFunction) return -2;

	theObjFunction = newFunction();
	if(!theObjFunction) return -3;

	return switchObjFunction(0);
}


void EvoCore::releaseObjFunctionLibrary() {
	delete theObjFunction;
	theObjFunction = nullptr;

	FreeLibrary(theObjFunctionLibrary);
	theObjFunctionLibrary = nullptr;
}


const char* EvoCore::getObjFunctionTotalInfo() {
	if(!theObjFunction) return "the ObjFunctionLibrary hasn't be loaded.";
	return theObjFunction->getTotalInfo();
}


int EvoCore::getObjFunctionCount() {
	if(!theObjFunction) return -1;
	return theObjFunction->getFunctionCount();
}


int EvoCore::switchObjFunction(int index) {
	if(!theObjFunction) return -1;
	int count = theObjFunction->getFunctionCount();
	if(index < 0 || index >= count) return -2;

	theObjFunction->switchFunction(index);

	//load the parameter config
	theParameterCount = theObjFunction->getParameterCount();
	if(theParameterCount < 1) return -3;

	theParameterBoundList.clear();
	for(int i = 0; i < theParameterCount; ++i) {
		double lowerBounds;
		double upperBounds;
		int error = theObjFunction->getParameterBounds(i, &lowerBounds, &upperBounds);
		if(error < 1) return -4;

		theParameterBoundList.push_back(make_pair(lowerBounds, upperBounds));
	}

	return 0;
}


const char* EvoCore::getObjFunctionName(int index) {
	if(!theObjFunction) return nullptr;
	return theObjFunction->getFunctionName(index);
}


int EvoCore::getParametersCount() {
	if(!theObjFunction) return -1;
	return theParameterCount;
}


int EvoCore::loadOperatorLibrary(const char* libPath) {
	theOperatorLibrary = LoadLibraryA(libPath);
	if(!theOperatorLibrary) return -1;

	typedef EvoOperator* NewFunction();
	NewFunction* newFunction = (NewFunction*)GetProcAddress(theOperatorLibrary, "newEvoOperator");
	if(!newFunction) return -2;

	theOperator = newFunction();
	if(!theOperator) return -3;

	return 0;
}


void EvoCore::releaseOperatorLibrary() {
	delete theOperator;
	theOperator = nullptr;

	FreeLibrary(theOperatorLibrary);
	theOperatorLibrary = nullptr;
}


const char* EvoCore::getOperatorTotalInfo() {
	if(!theOperator) return "the OperatorLibrary hasn't be loaded.";
	return theOperator->getTotalInfo();
}


int EvoCore::getOperatorCount() {
	if(!theOperator) return -1;
	return theOperator->getOperatorCount();
}


int EvoCore::switchOperator(int index) {
	if(!theOperator) return -1;
	int count = theOperator->getOperatorCount();
	if(index < 0 || index >= count) return -2;

	theOperator->switchOperator(index);
	return 0;
}


const char* EvoCore::getOperatorName(int index) {
	if(!theOperator) return nullptr;
	return theOperator->getOperatorName(index);
}


int EvoCore::getOperatorConfigCount() {
	if(!theOperator) return -1;
	return theOperator->getOperatorConfigCount();
}


int EvoCore::setOperatorConfigValue(int configId, double value) {
	if(!theOperator) return -1;
	return theOperator->setOperatorConfigValue(configId, value);
}


void EvoCore::setSeedCount(int count) {
	//must clear the seeds before
	clearSeeds();

	if(count <= 0) count = 10000;
	theSeedCount = count;
}


int EvoCore::getSeedCount() {
	return theSeedCount;
}


void EvoCore::setStopTolerance(double tolerance) {
	theTolerance = tolerance;
}


double EvoCore::getStopTolerance() {
	return theTolerance;
}


void EvoCore::setIterationMaximum(int maximum) {
	if(maximum <= 0) maximum = 100000;
	theIterTimesMaximum = maximum;
}


int EvoCore::getIterationMaximum() {
	return theIterTimesMaximum;
}


void EvoCore::setThreadCount(int num) {
	theThreadCount = num;
}


int EvoCore::getThreadCount() {
	return theThreadCount;
}


void EvoCore::registerCallBack(EvoCallBack* preCallback, EvoCallBack* postCallback) {
	thePreCallback = preCallback;
	thePostCallback = postCallback;
}


int EvoCore::start() {
	//to avoid blocking the main thread, this function must runs in another thread

	//check and prepare seeds
	int error = preCheck();
	if(error < 0) {
		theErrorInfo += "\npreCheck " + to_string(error);
		return -1;
	}

	prepare();

	error = compute();
	if(error < 0) {
		theErrorInfo += "\ncompute " + to_string(error);
		return -2;
	}
	return 0;
}


void EvoCore::stop() {
	theStopFlag = true;
}


int EvoCore::getCurrentStep() {
	return theIterStep;
}


double EvoCore::getCostTime() {
	return theCostTime;
}


int EvoCore::getBestResultsIndex() {
	return theBestSeedIndex;
}


const double* const* EvoCore::getAllSeedsValues() {
	return theSeeds;
}


const double* EvoCore::getAllObjFunctionResults() {
	return theResults;
}


const char* EvoCore::getErrorInfo() {
	return theErrorInfo.data();
}


inline bool EvoCore::checkFinish(const std::vector<ThreadData>& minResultList) {
	//search the min result
	int minIndex = 0;
	double minResult = theResults[minIndex];
	for(size_t i = 0; i < minResultList.size(); ++i) {
		if(minResultList[i].minResult < minResult) {
			minResult = minResultList[i].minResult;
			minIndex = minResultList[i].minIndex;
		}
	}
	double lastBestResult = theBestResult;
	theBestSeedIndex = minIndex;
	theBestResult = minResult;

	if(minResult == lastBestResult
		|| abs(minResult - lastBestResult) < theTolerance)
		++theEqualTimes;
	else
		theEqualTimes = 0;

	if(theEqualTimes > 100) {
		//notice this number
//		printf("theEqualTimes > 100\n");
		return true;
	}

	if(theStopFlag)
		return true;
	if(theIterStep > theIterTimesMaximum)
		return true;
	return false;
}


int EvoCore::preCheck() {
	if(!theObjFunction) return -1;
	if(!theOperator) return -2;

	if(theIterTimesMaximum <= 0) return -3;

	if(theSeedCount <= 0) return -4;
	if(theParameterCount <= 0) return -5;
	if(theParameterBoundList.size() != size_t(theParameterCount)) return -6;
	for(auto& it : theParameterBoundList) {
		if(it.first >= it.second) return -7;
	}

	return 0;
}


void EvoCore::prepare() {
	theIterStep = 0;
	theBestSeedIndex = -1;
	theBestResult = DBL_MAX;
	theEqualTimes = 0;
	theStopFlag = false;
	theErrorInfo.clear();

	//the bounds have been got when switch the objFunction, here set the parameters'bounds
	double* lowerBounds = new double[theParameterCount];
	double* higherBounds = new double[theParameterCount];
	for(int i = 0; i < theParameterCount; ++i) {
		lowerBounds[i] = theParameterBoundList[i].first;
		higherBounds[i] = theParameterBoundList[i].second;
	}
	theOperator->setParameterBounds(theParameterCount, lowerBounds, higherBounds);
	delete[] lowerBounds;
	delete[] higherBounds;

	//create seeds
	clearSeeds();
	theSeeds = new double*[theSeedCount];
	for(int i = 0; i < theSeedCount; ++i) {
		theSeeds[i] = new double[theParameterCount];
	}
	theResults = new double[theSeedCount];
	theOperator->setSeedCount(theSeedCount);

	//initial seeds values
	srand(clock());
	for(int j = 0; j < theParameterCount; ++j) {
		const double minBound = theParameterBoundList[j].first;
		//		const double maxBound = parameterInfoList[j].maxBound;
		const double span = theParameterBoundList[j].second - minBound;
		for(int i = 0; i < theSeedCount; ++i) {
			theSeeds[i][j] = minBound + double(rand()) / double(RAND_MAX) * span;
		}
	}
	for(int i = 0; i < theSeedCount; ++i) {
		theResults[i] = 0;
	}
}


void EvoCore::clearSeeds() {
	if(theSeeds) {
		for(int i = 0; i < theSeedCount; ++i) {
			delete [] theSeeds[i];
		}
		delete [] theSeeds;
		theSeeds = nullptr;
	}

	if(theResults) {
		delete [] theResults;
		theResults = nullptr;
	}
}


int EvoCore::compute() {
	//split the seeds
	vector<ThreadData> threadDataList;
	vector<thread> threads;
	{
		const int count = theSeedCount / theThreadCount;
		int endIndex = 0;
		for(int i = 0;  i < theThreadCount; ++i) {
			threadDataList.push_back(ThreadData(i, endIndex, endIndex + count));
			endIndex += count;
			threads.push_back(thread());
		}
		const_cast<int&>(threadDataList.back().endIndex) = theSeedCount;
	}

	//start
	const clock_t startTime = clock();
	for(theIterStep = 1; ; ++theIterStep) {
		//use multi-thread
		for(int i = 0; i < theThreadCount; ++i) {
			threadDataList[i].step = theIterStep;
			threadDataList[i].minResult = 0.0;
			threadDataList[i].minIndex = -1;
			threads[i] = thread(&EvoCore::objectFunctionThread, this, ref(threadDataList[i]));
		}
		for(int i = 0; i < theThreadCount; ++i) {
			threads[i].join();
			if(!threadDataList[i].errorInfo.empty()) {
				theErrorInfo = threadDataList[i].errorInfo;
				return -1;
			}
		}

		if(checkFinish(threadDataList)) break;

		if(thePreCallback) {
			theCostTime = double(clock() - startTime) / double(CLOCKS_PER_SEC);
			thePreCallback();
		}

		//use multi-thread
		if(theIterStep != 1) {
			for(int i = 0; i < theThreadCount; ++i) {
				threadDataList[i].step = theIterStep;
				threadDataList[i].minResult = 0.0;
				threadDataList[i].minIndex = -1;
				threads[i] = thread(&EvoCore::operatorThread, this, ref(threadDataList[i]));
//				operatorThread(threadDataList[i]);
			}
			for(int i = 0; i < theThreadCount; ++i) {
				threads[i].join();
				if(!threadDataList[i].errorInfo.empty()) {
					theErrorInfo = threadDataList[i].errorInfo;
					return -2;
				}
			}
		} else {
			int error =	theOperator->seedsFirstProcess(theResults, theSeeds);
			if(error < 0) {
				theErrorInfo = "theOperator seedsPreProcess: " + to_string(error);
				return -3;
			}
		}

		if(thePostCallback) {
			theCostTime = double(clock() - startTime) / double(CLOCKS_PER_SEC);
			thePostCallback();
		}
	}
	theCostTime = double(clock() - startTime) / double(CLOCKS_PER_SEC);
	return 0;
}


void EvoCore::objectFunctionThread(ThreadData& data) {
	for(int i = data.startIndex; i < data.endIndex; ++i) {
		int error = theObjFunction->objFunction(theSeeds[i], &theResults[i]);
		if(error < 0) {
			data.errorInfo = "the objFunction: " + to_string(error);
			return;
		}
	}

	//search the min result
	int minIndexTemp = data.startIndex;
	double minResultTemp = theResults[minIndexTemp];
	for(int i = data.startIndex; i < data.endIndex; ++i) {
		if(theResults[i] < minResultTemp) {
			minResultTemp = theResults[i];
			minIndexTemp = i;
		}
	}
	data.minIndex = minIndexTemp;
	data.minResult = minResultTemp;
	data.errorInfo.clear();
}


void EvoCore::operatorThread(ThreadData& data) {
	int error = theOperator->seedsProcessThread(data.step,
			theResults, theSeeds, data.startIndex, data.endIndex);
	if(error < 0) {
		data.errorInfo = "the objFunction: " + to_string(error);
		return;
	}
}


void EvoCore::log(const char* info) {
	char str[30];
	auto t = time(nullptr);
	ctime_s(str, 30, &t);
	theErrorInfo = string("time: ") + str + '\n' + info + "\n\n";
	printf("%s", theErrorInfo.data());

	FILE* file;
	int error = fopen_s(&file, "EvoErrorInfo.log", "a");
	if(error != 0) {
		printf("can't open the log file.\n");
		return;
	}

	fputs(theErrorInfo.data(), file);
	fclose(file);
}


void EvoCore::log(const char* info, int errorCode) {
	char str[30];
	auto t = time(nullptr);
	ctime_s(str, 30, &t);
	theErrorInfo = string("time: ") + str + " error: " + to_string(errorCode) + '\n' + info + "\n\n";
	printf("%s", theErrorInfo.data());

	FILE* file;
	int error = fopen_s(&file, "EvoErrorInfo.log", "a");
	if(error != 0) {
		printf("can't open the log file.\n");
		return;
	}

	fputs(theErrorInfo.data(), file);
	fclose(file);
}
