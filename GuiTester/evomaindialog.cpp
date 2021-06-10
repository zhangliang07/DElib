#include "evomaindialog.h"
#include "ui_evomaindialog.h"
#include "evocomputethread.h"
#include "../EvoCalLib/evocallib.h"
#include "evocomputethread.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#pragma execution_character_set("utf-8")


EvoMainDialog::EvoMainDialog(QWidget* parent):
	QWidget(parent),
	ui(new Ui::EvoMainDialog) {
	ui->setupUi(this);

	//load dll
	theCompute = new EvoComputeThread;
	connect(theCompute, &EvoComputeThread::pushString,
		this, &EvoMainDialog::pushString, Qt::QueuedConnection);
	connect(theCompute, &EvoComputeThread::finished,
		this, &EvoMainDialog::onFinished, Qt::QueuedConnection);
	updateValuesFromDll();


		loadObjFunction("EvoObjTestFunction.dll");
		loadOperator("EvoOperatorDe.dll");
    ui->button_loadObjFunction->hide();
    ui->button_loadOperator->hide();
}


EvoMainDialog::~EvoMainDialog() {
	theCompute->deleteLater();
	delete ui;
}


void EvoMainDialog::timerEvent(QTimerEvent*) {
		if(ui->button_calculate->text() == "start calculation") enableAllButtons(true);
	else ui->button_calculate->setEnabled(true);
}


void EvoMainDialog::updateValuesFromDll() {
	if(!globalDll) return;

	ui->spinBox_thread->setValue(globalDll->getThreadCount());
	ui->spinBox_seed->setValue(globalDll->getSeedCount());
	ui->spinBox_maxIterStep->setValue(globalDll->getIterationMaximum());

	double tolerance = globalDll->getStopTolerance();
	if(tolerance > 0 && tolerance <= 1) {
		ui->checkBox_tolerance->setChecked(true);
		double n = -log(tolerance);
		ui->spinBox_tolerance->setValue(n);
	} else {
		ui->checkBox_tolerance->setChecked(false);
		ui->spinBox_tolerance->setValue(0);
	}
}


void EvoMainDialog::updateValuesToDll() {
	if(!globalDll) return;

	int index = ui->comboBox_objectFunction->currentIndex();
	int error = globalDll->switchObjFunction(index);
	if(error < 0) {
		ui->plainTextEdit->appendPlainText(tr("\nfailed(%1) to switch objective function to: %2")
			.arg(error).arg(globalDll->getObjFunctionName(index)));
	} else {
		ui->plainTextEdit->appendPlainText(tr("\nswitch objective function to: %1")
			.arg(globalDll->getObjFunctionName(index)));
	}

	int count = ui->spinBox_thread->value();
	globalDll->setThreadCount(count);
	ui->plainTextEdit->appendPlainText(tr("set thread count: %1").arg(count));

	globalDll->setSeedCount(ui->spinBox_seed->value());
	globalDll->setIterationMaximum(ui->spinBox_maxIterStep->value());

	if(ui->checkBox_tolerance->isChecked()) {
		double n = ui->spinBox_tolerance->value();
		globalDll->setStopTolerance(exp(-n));
	} else {
		globalDll->setStopTolerance(-1);
	}
}

void EvoMainDialog::enableAllButtons(bool enable) {
	setUpdatesEnabled(false);
	ui->button_calculate->setEnabled(enable);
	ui->button_loadObjFunction->setEnabled(enable);
	ui->button_loadOperator->setEnabled(enable);
	ui->checkBox_tolerance->setEnabled(enable);
	ui->comboBox_objectFunction->setEnabled(enable);
	ui->comboBox_2->setEnabled(enable);
	ui->spinBox_thread->setEnabled(enable);
		setUpdatesEnabled(true);
}


void EvoMainDialog::loadObjFunction(const QString& dllPath){
		int error = globalDll->loadObjFunctionLibrary(dllPath.toLocal8Bit().data());
		if(error < 0) {
				QMessageBox::critical(this, "loading error", tr("error to load objective functions(%1)。").arg(error));
				return;
		}

		//set combo box
		ui->button_loadObjFunction->setToolTip(dllPath);
		ui->comboBox_objectFunction->clear();
		int count = globalDll->getObjFunctionCount();
		for(int i = 0; i < count; ++i) {
				auto* name = globalDll->getObjFunctionName(i);
				ui->comboBox_objectFunction->addItem(name, i);
		}
		ui->plainTextEdit->appendPlainText("load objective functions" + dllPath);
}


void EvoMainDialog::loadOperator(const QString &dllPath){
		int error = globalDll->loadOperatorLibrary(dllPath.toLocal8Bit().data());
		if(error < 0) {
				QMessageBox::critical(this, "loading error", tr("error to load operators(%1)。").arg(error));
				return;
		}

		//set combo box
		ui->button_loadOperator->setToolTip(dllPath);
		ui->comboBox_2->clear();
		int count = globalDll->getOperatorCount();
		for(int i = 0; i < count; ++i) {
				auto* name = globalDll->getOperatorName(i);
				ui->comboBox_2->addItem(name, i);
		}
		ui->plainTextEdit->appendPlainText("load operators" + dllPath);
}


void EvoMainDialog::pushString(const QString& str) {
	ui->plainTextEdit->appendPlainText(str);
}


void EvoMainDialog::onFinished() {
		ui->button_calculate->setText("start calculation");
	enableAllButtons(true);
}



void EvoMainDialog::on_button_loadObjFunction_clicked() {
		QString dllPath = QFileDialog::getOpenFileName(nullptr, "load objective functions", "", "Library (*.dll)");
	if(dllPath.isEmpty()) return;

		loadObjFunction(dllPath);
}


void EvoMainDialog::on_button_loadOperator_clicked() {
		QString dllPath = QFileDialog::getOpenFileName(nullptr, "load operators", "", "Library (*.dll)");
	if(dllPath.isEmpty()) return;

		loadOperator(dllPath);
}


void EvoMainDialog::on_button_calculate_clicked() {
		if(ui->button_calculate->text() == "start calculation") {
		enableAllButtons(false);
		updateValuesToDll();
		theCompute->start();
				ui->button_calculate->setText("stop calculation");
		startTimer(3000);
		} else if(ui->button_calculate->text() == "stop calculation") {
		enableAllButtons(false);
				ui->plainTextEdit->appendPlainText("\nstop calculation manually");
		theCompute->stop();
				ui->button_calculate->setText("start calculation");
		startTimer(3000);
	}
}

void EvoMainDialog::on_button_about_clicked(){
		QMessageBox::about(this, "about",
			"GuiTester of lib of evolutionary computation\n\n"
			"programed by ZHANG Liang\n"
			"mailto: zhangllw@qq.com");
}
