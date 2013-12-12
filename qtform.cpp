#include "qtform.h"

QtForm::QtForm(QObject *parent) :
	QObject(parent)
{
}

QString QtForm::put(QString form, QString name, QString value)
{
	QRegExp re("name\\s*=\\s*(\"" + name + "\"|'" + name + "')", Qt::CaseInsensitive);
	if (!form.contains(re))
	{
		QString newInput = "\n<input name=\"" + name + "\" value=\"" + value + "\" />\n</form>";
		form.replace(QRegExp("<\\s*/\\s*form\\s*>", Qt::CaseInsensitive), newInput);
	}
	else
	{
		QString input = "name=\"" + name + "\" value=\"" + value + "\" ";
		form.replace(QRegExp("name\\s*=\\s*['\"]" + name + "['\"]", Qt::CaseInsensitive), input);
	}

	return form;
}

QString QtForm::getAction(const QString &form)
{
	return QtBrowser().regCap(form, QString("action\\s*=\\s*\"(.*)\""));
}

QString QtForm::setAction(QString form, QString action)
{
	QRegExp actionRegExp("(action\\s*=\\s*\"(.*)\")", Qt::CaseInsensitive);
	actionRegExp.setMinimal(true);
	if (QtBrowser().regCap(form, QString("(<\\s*form\\s*.*>)")).contains("action"))
	{
		form.replace(actionRegExp, "action=\"" + action + "\"");
	}
	else
	{
		form.replace(QRegExp("<\\s*form"), "<form action=\"" + action + "\"");
	}

	return form;
}

QString QtForm::setMethod(QString form, QString method)
{
	if (form.contains(QRegExp("\\s*method\\s*")) && QtBrowser().regCap(form, QString("method\\s*=\\s*\"(.*)\"")).trimmed().isEmpty())
	{
		form.replace(">", "method=\"" + method + "\">");
	}
	else
	{
		form.replace(QRegExp("<\\s*form"), "<form method=\"" + method + "\"");
	}

	return form;
}

QString QtForm::getMethod(QString form)
{
	return QtBrowser().regCap(form, QString("method\\s*=\\s*['\"](.*)['\"]"));
}
