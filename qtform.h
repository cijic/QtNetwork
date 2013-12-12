#ifndef QTFORM_H
#define QTFORM_H

#include <QObject>
#include <QDebug>
#include <QRegExp>
#include "qtbrowser.h"

class QtForm : public QObject
{
	Q_OBJECT
public:
	explicit QtForm(QObject *parent = 0);
	static QString put(QString form, QString name, QString value);
	static QString getAction(const QString &form);
	static QString setAction(QString form, QString action);
	static QString setMethod(QString form, QString method);
	static QString getMethod(QString form);

signals:
	
public slots:
	
};

#endif // QTFORM_H
