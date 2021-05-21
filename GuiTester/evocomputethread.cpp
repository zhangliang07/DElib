#include "evocomputethread.h"
#include "evocallib.h"
#include <QThread>


EvoCalLib* globalDll = nullptr;
static EvoComputeThread* globalThread = nullptr;


EvoComputeThread::EvoComputeThread() :
	QObject(nullptr) {
	theThread = new QThread;
	this->moveToThread(theThread);
	connect(theThread, &QThread::started, this, &EvoComputeThread::onStart);
	connect(theThread, &QThread::finished, this, &EvoComputeThread::finished);

	globalDll = newEvoCalLib();
	globalThread = this;
}


EvoComputeThread::~EvoComputeThread() {
	theThread->deleteLater();
}


void EvoComputeThread::start() {
	theThread->start();
}


void EvoComputeThread::stop() {
	globalDll->stop();
}


void EvoComputeThread::callback() {
	int step = globalDll->getCurrentStep();
	if(step % 100 == 0) {
		double time = globalDll->getCostTime();
		int bestResultIndex = globalDll->getBestResultsIndex();
		auto result = globalDll->getAllObjFunctionResults();
		double bestResult = result[bestResultIndex];
		emit(globalThread->pushString(
				tr("step: %1, cost time: %2s, best index: %3, best result: %4")
				.arg(step).arg(time).arg(bestResultIndex).arg(bestResult)));
	}
}


void EvoComputeThread::onStart() {
	globalDll->registerCallBack(callback, nullptr);
	emit(pushString(tr("start calculation")));
	int error = globalDll->start();
	if(error < 0) {
		auto* errorInfo = globalDll->getErrorInfo();
		emit(pushString(tr("calcualtion failed(%1): %2").arg(error).arg(errorInfo)));
		theThread->quit();
		emit(finished());
		return;
	}
	double costTime = globalDll->getCostTime();
	int step = globalDll->getCurrentStep();
	emit(pushString(tr("calculation finished at step %1, cost time: %2\n").arg(step).arg(costTime)));

	int bestIndex = globalDll->getBestResultsIndex();
	auto results = globalDll->getAllObjFunctionResults();
	double bestResult = results[bestIndex];

	auto seeds = globalDll->getAllSeedsValues();
	const double* bestSeed = seeds[bestIndex];
	int parameterCount = globalDll->getParametersCount();
	QString str = tr("the best result is %1\nthe best seed(%2) is (").arg(bestResult).arg(bestIndex);
	for(int i = 0; i < parameterCount; ++i) {
		str.append(QString::number(bestSeed[i]) + ", ");
	}
	str.append(")");

	emit(pushString(str));
	theThread->quit();
	emit(finished());
}
