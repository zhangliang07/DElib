#include "evooperatorde.h"
//#include <math.h>
#include <time.h>
#pragma execution_character_set("utf-8")
using namespace std;


EvoOperatorDe::EvoOperatorDe():
	theSeedCount(0),
	theParameterCount(0),
	theOldSeeds(nullptr),
	theNewSeeds(nullptr),
	theOldResults(nullptr) {
	theConfigDe.DEzoom = 0.3;
	theConfigDe.DEcr = 0.3;
	theConfigDe.DErenew = 0.02;
}


EvoOperatorDe::~EvoOperatorDe() {
	clearSeeds();
}


const char* EvoOperatorDe::getTotalInfo() {
	return nullptr;
}


int EvoOperatorDe::getOperatorCount() {
	return 2;
}


void EvoOperatorDe::switchOperator(int index) {
	if(index < 0 || index > 1) return;
	theMode = index;
}


const char* EvoOperatorDe::getOperatorName(int index) {
	if(index < 0 || index > 1) return nullptr;

	const char* names[3] = {"difference", "PSO"};
	return names[index];
}


const char* EvoOperatorDe::getOperatorInfo(int index) {
	return nullptr;
}


int EvoOperatorDe::getOperatorConfigCount() {
	if(theMode < 0 || theMode > 1) return -1;

	int configCount[] = {2, 2};
	return configCount[theMode];
}


int EvoOperatorDe::setOperatorConfigValue(int configId, double value) {
	if(theMode == 0) {
		if(configId == 0) theConfigDe.DEzoom = value;
		else if(configId == 1) theConfigDe.DEcr = value;
		else return 0;

	} else if(theMode == 1) {

	} else return -1;

	return 0;
}


void EvoOperatorDe::setParameterBounds(int parameterCount,
	const double* lowerBounds, const double* upperBounds) {
	theParameterBoundList.clear();
	for(int i = 0; i < parameterCount; ++i) {
		theParameterBoundList.push_back(make_pair(lowerBounds[i], upperBounds[i]));
	}
	theParameterCount = parameterCount;
}


void EvoOperatorDe::setSeedCount(int count) {
	theSeedCount = count;
}


int EvoOperatorDe::seedsFirstProcess(const double* results, double** seeds) {
	createSeeds(seeds, results);
	return 0;
}


int EvoOperatorDe::seedsProcessThread(int step, const double* results,
	double** seeds, int startIndex, int endIndex) {
	if(!theOldResults) return -1;

	srand(clock() * (startIndex + endIndex));
	//chosen
	standardDE_chosen(results, startIndex, endIndex);

	//variety
	standardDE_variety(seeds, startIndex, endIndex);

	//copy results
	memcpy(theOldResults + startIndex, results + startIndex,
		(endIndex - startIndex)*sizeof(double));
//	for(int i = startIndex; i < endIndex; ++i) {
//		theOldResults[i] = results[i];
//	}
	return 0;
}


void EvoOperatorDe::createSeeds(const double* const* seeds, const double* results) {
	clearSeeds();

	theOldSeeds = new double*[theSeedCount];
	theNewSeeds = new double*[theSeedCount];
	theOldResults = new double[theSeedCount];

	for(int i = 0; i < theSeedCount; ++i) {
		theOldSeeds[i] = new double[theParameterCount];
		theNewSeeds[i] = new double[theParameterCount];
		memcpy(theOldSeeds[i], seeds[i], theParameterCount * sizeof(double));
		memcpy(theNewSeeds[i], seeds[i], theParameterCount * sizeof(double));
//		for(int j = 0; j < theParameterCount; ++j) {
//			theOldSeeds[i][j] = seeds[i][j];
//			theNewSeeds[i][j] = seeds[i][j];
//		}
		theOldResults[i] = results[i];
	}
}


void EvoOperatorDe::clearSeeds() {
	if(theOldSeeds) {
		for(int i = 0; i < theSeedCount; ++i) {
			delete [] theOldSeeds[i];
		}
		delete [] theOldSeeds;
		theOldSeeds = nullptr;
	}

	if(theNewSeeds) {
		for(int i = 0; i < theSeedCount; ++i) {
			delete [] theNewSeeds[i];
		}
		delete [] theNewSeeds;
		theNewSeeds = nullptr;
	}

	if(theOldResults) {
		delete [] theOldResults;
		theOldResults = nullptr;
	}
}


void EvoOperatorDe::standardDE_variety(double** seeds, int startIndex, int endIndex) {
	//变异
	const double zoom = theConfigDe.DEzoom;
	const double cr = theConfigDe.DEcr;
	const double renew = theConfigDe.DErenew;

	for(int i = startIndex; i < endIndex; ++i) {
		const int id1 = rand() % theSeedCount;
		const int id2 = rand() % theSeedCount;		//随机选三个点
		const int id3 = rand() % theSeedCount;
		for(int j = 0; j < theParameterCount; ++j) {
			const double random = double(rand()) / double(RAND_MAX);
			const double& minBound = theParameterBoundList[j].first;
			const double& maxBound = theParameterBoundList[j].second;
			if(random < renew) {
				const double span = maxBound - minBound;
				theNewSeeds[i][j] = minBound + double(rand()) / double(RAND_MAX) * span;
			} else if(random < cr) {
				theNewSeeds[i][j] = theOldSeeds[id1][j] + zoom * (theOldSeeds[id2][j] - theOldSeeds[id3][j]);

				//检查是否越界。如果越界就重新赋一个随机值
				if(theNewSeeds[i][j] < minBound || theNewSeeds[i][j] > maxBound) {
					const double span = maxBound - minBound;
					theNewSeeds[i][j] = minBound + double(rand()) / double(RAND_MAX) * span;
				}
			} else {
				theNewSeeds[i][j] = theOldSeeds[i][j];
			}
//			seeds[i][j] = theNewSeeds[i][j];
		}
		memcpy(seeds[i], theNewSeeds[i], theParameterCount * sizeof(double));
	}
}


void EvoOperatorDe::standardDE_chosen(const double* result, int startIndex, int endIndex) {
	//选择
	for(int i = startIndex; i < endIndex; ++i) {
		if(result[i] < theOldResults[i]) {
			memcpy(theOldSeeds[i], theNewSeeds[i], theParameterCount * sizeof(double));
//			for(int j = 0; j < theParameterCount; ++j)
//				theOldSeeds[i][j] = theNewSeeds[i][j];
		}
	}
}
