#include "evoobjtestfunction.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <ctime>
#include <thread>
#pragma execution_character_set("utf-8")
using namespace std;


EvoObjTestFunction::EvoObjTestFunction():
	theMode(0) {
	theParameterBounds0.push_back(make_pair(-100, 100));
	theParameterBounds0.push_back(make_pair(-100, 100));

	theParameterBounds1.push_back(make_pair(-32.768, 32.768));
	theParameterBounds1.push_back(make_pair(-32.768, 32.768));

	theParameterBounds2.push_back(make_pair(-5.12, 5.12));
	theParameterBounds2.push_back(make_pair(-5.12, 5.12));
}


EvoObjTestFunction::~EvoObjTestFunction() {}


const char* EvoObjTestFunction::getTotalInfo() {
	return nullptr;
}


int EvoObjTestFunction::getFunctionCount() {
	return 3;
}


void EvoObjTestFunction::switchFunction(int index) {
	if(index < 0 || index > 2) return;
	theMode = index;
}


const char* EvoObjTestFunction::getFunctionName(int index) {
	if(theMode < 0 || theMode > 2) return nullptr;

	const char* names[3] = {"sphere", "ackley", "rastrigin"};
	return names[index];
}


const char* EvoObjTestFunction::getFunctionInfo(int index) {
	return nullptr;
}


int EvoObjTestFunction::objFunction(const double* parameters, double* results) {
	switch(theMode) {
	case 0:
		*results  = sphere(parameters);
		break;
	case 1:
		*results  = ackley(parameters);
		break;
	case 2:
		*results  = rastrigin(parameters);
		break;
	default:
		return -1;
	}

	return 0;
}


int EvoObjTestFunction::getParameterCount() {
	if(theMode < 0 || theMode > 2) return -1;

	//all 3 modes have 2 parameters
	return 2;
}


int EvoObjTestFunction::getParameterBounds(int index, double* lowerBounds, double* upperBounds) {
	if(theMode < 0 || theMode > 2) return -1;
	auto& parameterBounds = theParameterBounds0;
	if(theMode == 1) parameterBounds = theParameterBounds1;
	if(theMode == 2) parameterBounds = theParameterBounds2;

	if(index < 0 || index >= int(parameterBounds.size())) return -2;

	*lowerBounds = parameterBounds[index].first;
	*upperBounds = parameterBounds[index].second;
	return parameterBounds.size();
}


double EvoObjTestFunction::sphere(const double* parameters) const {
	const double x1 = parameters[0];
	const double x2 = parameters[1];

	return x1 * x1 + x2 * x2;
}


double EvoObjTestFunction::ackley(const double* parameters) const {
	//space [-32.768, 32.768], the minimum is 0 at (0, 0)
	const double x1 = parameters[0];
	const double x2 = parameters[1];
	const double a = 20, b = 0.2, c = 2 * M_PI;
	const double d = 2;

	const double sum1 = x1 * x1 + x2 * x2;
	const double sum2 = cos(c * x1) + cos(c * x2);
	const double term1 = -a * exp(-b * sqrt(sum1 / d));
	const double term2 = -exp(sum2 / d);

	return term1 + term2 + a + exp(1);

}


double EvoObjTestFunction::rastrigin(const double* parameters) const {
	//space (-5.12, 5.12), the minimum is 0 at (0, 0)
	const double x1 = parameters[0];
	const double x2 = parameters[1];
	const double range = 2;

	return 10 * range + (x1 * x1 - 10 * cos(2 * M_PI * x1)) + (x2 * x2 - 10 * cos(2 * M_PI * x2));
}
