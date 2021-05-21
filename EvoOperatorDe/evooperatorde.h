#ifndef EVOOPERATORDE_H
#define EVOOPERATORDE_H
#include "../EvoCalLib/EvoOperator.h"
#include <string>
#include <vector>


class EvoOperatorDe: public EvoOperator {
public:
	EvoOperatorDe();
	virtual ~EvoOperatorDe();

	virtual const char* getTotalInfo() override;
	virtual int getOperatorCount() override;
	virtual void switchOperator(int index) override;
	virtual const char* getOperatorName(int index) override;
	virtual const char* getOperatorInfo(int index) override;

	virtual int getOperatorConfigCount() override;
	virtual int setOperatorConfigValue(int configId, double value) override;
	virtual void setParameterBounds(int parameterCount,
		const double* lowerBounds, const double* upperBounds) override;
	virtual void setSeedCount(int count) override;

	//preprocess and postprocess of seeds in library
	virtual int seedsFirstProcess(const double* results, double** seeds) override;
	virtual int seedsProcessThread(int step, const double* results,
		double** seeds, int startIndex, int endIndex) override;

private:
	int theMode;

	//the Parameter bounds, first is lowerBound, second is higherBound
	std::vector<std::pair<double, double>> theParameterBoundList;

	int theSeedCount;
	int theParameterCount;
	double** theOldSeeds;
	double** theNewSeeds;
	double* theOldResults;

	struct ConfigDe {
		double DEzoom, DEcr;  //DE算法中的放大系数与突变概率系数
		double DErenew;				//重新生成种子的概率
	} theConfigDe;

private:
	void createSeeds(const double* const* seeds, const double* results);
	void clearSeeds();

	//from oldseeds to modify newseeds
	void standardDE_variety(double** seeds, int startIndex, int endIndex);

	//from newseeds to modify oldseeds
	void standardDE_chosen(const double* result, int startIndex, int endIndex);
};


#endif // EVOOPERATORDE_H
