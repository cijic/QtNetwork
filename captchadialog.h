#ifndef CAPTCHA_DIALOG_H
#define CAPTCHA_DIALOG_H

#include <QObject>
#include <QDialog>
#include <QtGui>
#include <QDebug>

#include "qtbrowser.h"


/* Class for show dialog window and to get entered data */
class CaptchaDialog : public QDialog
{
	Q_OBJECT
	private:
		/* vars */
		QLineEdit *input;		// Line for user input

	public:
		/* methods */
		CaptchaDialog(const char *path, const char *title = "Enter the code:", QWidget* pwgt = 0);
		QString getCode();	// Method for return user input.
};
#endif

//CAPTCHA_DIALOG_H
