#ifndef EVOMAINDIALOG_H
#define EVOMAINDIALOG_H
#include <QWidget>
class EvoComputeThread;


QT_BEGIN_NAMESPACE
namespace Ui { class EvoMainDialog; }
QT_END_NAMESPACE


class EvoMainDialog : public QWidget {
	Q_OBJECT

public:
	EvoMainDialog(QWidget* parent = nullptr);
	~EvoMainDialog();

protected:
	void timerEvent(QTimerEvent*);

private:
	Ui::EvoMainDialog* ui;

	EvoComputeThread* theCompute;

private:
	void updateValuesFromDll();
	void updateValuesToDll();
	void enableAllButtons(bool enable);
		void loadObjFunction(const QString& dllPath);
		void loadOperator(const QString& dllPath);

private slots:
	void pushString(const QString& str);
	void onFinished();

	void on_button_loadObjFunction_clicked();
	void on_button_loadOperator_clicked();
	void on_button_calculate_clicked();
		void on_button_about_clicked();
};


#endif // EVOMAINDIALOG_H
