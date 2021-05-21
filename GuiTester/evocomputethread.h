#ifndef EVOCOMPUTETHREAD_H
#define EVOCOMPUTETHREAD_H
#include <QObject>
class QThread;
class EvoCalLib;


class EvoComputeThread : public QObject {
	Q_OBJECT

signals:
	void pushString(const QString& str);
	void finished();

public:
	explicit EvoComputeThread();
	~EvoComputeThread();

	void start();
	void stop();

private:
	QThread* theThread;

private:
	static void callback();

	void onStart();
};


extern EvoCalLib* globalDll;


#endif // EVOCOMPUTETHREAD_H
