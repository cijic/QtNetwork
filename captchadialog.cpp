#include "captchadialog.h"


CaptchaDialog::CaptchaDialog(const char *path, const char *title, QWidget *pwgt)
	:QDialog(pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::MSWindowsFixedSizeDialogHint)
{
	QString _path(path);
	if (_path.trimmed().startsWith("http"))
	{
		QTemporaryFile captchaFile;
		captchaFile.setAutoRemove(false);
		if (!captchaFile.open())
		{
			qDebug() << "captchaFile can't be open" << captchaFile.errorString();
			return;
		}
		else
		{
			QtBrowser br(this);
			captchaFile.write(br.getItSync(_path)->readAll());
			_path = captchaFile.fileName();
		}
	}

	if (!QFile::exists(QString(_path)))
	{
		QMessageBox::warning(this, " ", "Image can't be displayed.");
		return;
	}

	unsigned btnWidth	= 75;
	unsigned btnHeight	= 23;
	unsigned margin		= 5;

	QPixmap pixmap(_path);

	this->setWindowTitle(QString::fromUtf8(title));
	this->resize(pixmap.width() + margin*2, 0);

	this->input		= new QLineEdit(this);
	this->input->setGeometry(QRect(margin, pixmap.height() + margin*2, this->width() - margin*2, btnHeight));

	QLabel *image	= new QLabel(this);
	image->setPixmap(pixmap);
	image->show();
	image->setBuddy(this->input);
	image->setGeometry(QRect(margin, margin, pixmap.width(), pixmap.height()));

	QPushButton *cancel;
	cancel = new QPushButton(QString::fromUtf8("Cancel"), this);
	cancel->setGeometry(QRect(this->width() - btnWidth - margin, input->geometry().y() + btnHeight + margin*2, btnWidth, btnHeight));

	QPushButton *ok;
	ok = new QPushButton(QString::fromUtf8("ОК"), this);
	ok->setGeometry(QRect(cancel->geometry().x() - btnWidth - margin, cancel->geometry().y(), btnWidth, btnHeight));
	this->connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
	
	this->resize(pixmap.width() + margin*2, ok->geometry().y() + btnHeight + margin);
}

QString CaptchaDialog::getCode()
{
	return this->input->text(); 
}

