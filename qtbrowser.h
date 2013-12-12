#ifndef QTBROWSER_H
#define QTBROWSER_H

#include <QObject>
#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkCookie>
#include <QEventLoop>
#include <QTime>
#include <QFile>

#include "useragencontainer.h"

class QtBrowser : public QObject
{
	Q_OBJECT
	protected:
		/* Vars */
		QNetworkAccessManager	*manager;
		QNetworkReply			*reply;
		QNetworkRequest			request;
		QNetworkCookieJar		*cookieJar;
		QStringList visitedUrls;
		QString filename;
		QString page;
		QString srcUrl;
		QString redirectLocation;
		int toWait;
		bool followOnRedirect;
		QByteArray userAgent;
		QString url;

	public:
		/* Methods */
		explicit QtBrowser(QObject *parent = 0);
		QtBrowser(QtBrowser &extBr);

		void					clone(QtBrowser &extBr);
		QNetworkRequest			getRequest();
		QNetworkAccessManager	*getManager();

		/* Methods. Diff */
		int randIt(int low, int high);
		int randIt(int high);

		/* Methods. Cookies */
		QByteArray	getCookie(const QNetworkCookieJar &cookieJar, const char *host, const QString &key);
		QByteArray	getCookie(const char *host, const QString &key);
		bool		setCookie(QNetworkCookieJar *cookieJar, const QUrl &host,	const QString &key, const QString &value);
		bool		setCookie(QNetworkCookieJar *cookieJar, const char *host,	const QString &key, const QString &value);
		bool		setCookie(const QUrl &host,	const QString &key, const QString &value);
		bool		setCookie(const char *host, const QString &key, const QString &value);
		bool		syncCookies(QNetworkCookieJar *cookieJar, const QString &src, const QString &dst);
		bool		syncCookies(const QString &src, const QString &dst);
		void		clearCookies();
		QList<QNetworkCookie> getCookies(QString src);
		QNetworkCookieJar *getCookieJar();

		/* Methods. Handling text from webpage */
		QString		regCap(const QString &where, const QString &what, bool setMin = true, int pos = 1);
		QString		regCap(const QString &what, bool setMin = true, int pos = 1);
		QStringList regCapAll(const QString &where, const QString &regexp, int column = 1, bool setMin = true);
		QStringList regCapAll(const QString &regexp, int column = 1);
		QVector<QVector<QString> > getMatches(const QString &regexp, QString where = "", bool setMin = true);
		QString		getContent();
		void		setContent(QString page);
		bool		contains(QString text, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive);
		bool		contains(QRegExp regexp);
		QString		htmlDecode(QString text);
//		QString		decodeXML(QString text);
		QStringList	getHttpLinks(QString text = "", QString url = "");

		/* Methods. Receiving data */
		void			prepareNetworkRequest(QNetworkRequest &networkRequest);
		QNetworkReply	*getItSync(const QUrl &url, const QByteArray params = QByteArray());
		QString			getPage(const QUrl url, const QByteArray params = QByteArray());
		QString			getPage(const char *url, const QByteArray params = QByteArray());
		void			setFollowRedirects(bool val);
		QNetworkReply	*head(const QUrl url);
		int				getResponseCode();

		/* Methods. JSON */
		QString			jsonVal(QString where, QString name);

		/* Methods. URL */
		void setURL(QString url);
		QString getURL();

		/* Methods. Tags */
		QStringList		getSpecifiedTags(const QString &where, const QString &tag);
		bool			isTagParam(const QString &tag, const QString &param);
		QString			getParamValue(const QString &where, const QString &paramName);
		QString			getNameValue(QString where);
		QStringList		getForms();
		QString			getFormByProperty(const QString &where, const QString &prop, const QString &value);
		QString			getFormByProperty(const QString &property, const QString &value);
		QString			formPutValue(QString form, const QString &name, const QString &value);
		QByteArray		getFormParams(QString where);
		QNetworkReply	*submitForm(QString form);
		void			submit(QString form);
		QString			getFormByHTML(QString text);

		/* Methods. Headers */
		void		generateUserAgent();
		void		setUserAgent(QByteArray);
		QByteArray	getUserAgent();
		QString		getRedirectLocation();
		bool		hasHeader(QByteArray headerName);
		QByteArray	getHeader(QByteArray header);
		void		setHeader(QByteArray name, QByteArray value);
		QtBrowser	clearHeader(QByteArray name);
		QList<QPair<QByteArray, QByteArray> > headers();

		/* Methods. TextEdit */
		QString		normalization(QString str);
		QString		decode(QString str);

		/* Methods. Files */
		bool save(const QString &fileName, QIODevice *data);		// Сохранить данные на локальный носитель
		bool save(const QString &fileName, const QString &data);	// Сохранить текстовые данные на локальный носитель

	signals:

	public slots:

};

#endif // QTBROWSER_H
