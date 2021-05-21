#ifndef EVOOBJTESTFUNCTION_H
#define EVOOBJTESTFUNCTION_H
#include "../EvoCalLib/EvoObjFunction.h"
#include "vector"


class EvoObjTestFunction : public EvoObjFunction {
public:
	EvoObjTestFunction();
	virtual ~EvoObjTestFunction();

	virtual const char* getTotalInfo() override;
	virtual int getFunctionCount() override;
	virtual void switchFunction(int index) override;
	virtual const char* getFunctionName(int index) override;
	virtual const char* getFunctionInfo(int index) override;

	//plaese write your target function as follow format, and make it return 0 when success.
	//please notice that the program will search the minimum of the function
	virtual int objFunction(const double* parameters, double* results) override;

	//return the count of Parameters
	virtual int getParameterCount() override;
	virtual int getParameterBounds(int index, double* lowerBounds, double* upperBounds) override;

private:
	int theMode;

	//the bounds of parameters
	std::vector<std::pair<double, double>> theParameterBounds0;
	std::vector<std::pair<double, double>> theParameterBounds1;
	std::vector<std::pair<double, double>> theParameterBounds2;

private:
	//the count of parameters is 2
	double sphere(const double* parameters) const;

	//the count of parameters is 2
	double ackley(const double* parameters) const;

	//the count of parameters is 2
	double rastrigin(const double* parameters) const;
};


#endif // EVOOBJTESTFUNCTION_H
