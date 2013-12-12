#include "qtbrowser.h"

QtBrowser::QtBrowser(QObject *parent) :
	QObject(parent)
{
	this->cookieJar = new QNetworkCookieJar(this);
	this->manager = new QNetworkAccessManager(this);
	this->manager->setCookieJar(this->cookieJar);
	this->prepareNetworkRequest(this->request);
}

QtBrowser::QtBrowser(QtBrowser &extBr)
{
	this->clone(extBr);
}

/*
	Clone current browser.
*/
void QtBrowser::clone(QtBrowser &extBr)
{
	this->request = QNetworkRequest(extBr.getRequest());
	this->manager = extBr.getManager();
	this->page    = extBr.getContent();
	this->url	  = extBr.getURL();
//	qDebug() << __FILE__ << __LINE__;
	this->setUserAgent(extBr.getUserAgent());
}

QNetworkRequest QtBrowser::getRequest()
{
	return this->request;
}

QNetworkAccessManager *QtBrowser::getManager()
{
	return this->manager;
}

/*
	Receive random integer number in range.
*/
int QtBrowser::randIt(int low, int high)
{
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
	return qrand() % ((high + 1) - low) + low;
}

/*
	Receive random integer number that is greater than 0.
*/
int QtBrowser::randIt(int high)
{
	return this->randIt(0, high);
}


/*
	Get cookie from host with key
*/
QByteArray QtBrowser::getCookie(const QNetworkCookieJar &cookieJar, const char *host, const QString &key)
{
	QList<QNetworkCookie> cookies = cookieJar.cookiesForUrl(QUrl(host));
	foreach(QNetworkCookie c, cookies)
	{
		if (QString(host).contains(c.domain()) && c.name() == key)
		{
			return c.value();
		}
	}

	host = QString(host).replace(QUrl(QString(host)).scheme() + "://", ".").toUtf8().data();
	foreach(QNetworkCookie c, cookies)
	{
		if (QString(host).contains(c.domain()) && c.name() == key)
		{
			return c.value();
		}
	}
	return "";
}

QByteArray QtBrowser::getCookie(const char *host, const QString &key)
{
	return this->getCookie(*this->manager->cookieJar(), host, key);
}

/*
	Method for setting Cookie for the specified domain.
*/
bool QtBrowser::setCookie(QNetworkCookieJar *cookieJar, const QUrl &host, const QString &key, const QString &value)
{
	QByteArray localKey;
	localKey.append(key);
	QByteArray localValue;
	localValue.append(value);

	QList<QNetworkCookie> cookies;
	cookies.append(QNetworkCookie(localKey, localValue));
	if (!cookieJar->setCookiesFromUrl(cookies, host))
	{
		return false;
	}
	else
	{
		return true;
	}
}

/*
	Reimpelemted method setCookie(QNetworkCookieJar &cookieJar, const QUrl &host, const QString &key, const QString &value)
*/
bool QtBrowser::setCookie(QNetworkCookieJar *cookieJar, const char *host, const QString &key, const QString &value)
{
	return this->setCookie(cookieJar, QUrl(host), key, value);
}

bool QtBrowser::setCookie(const QUrl &host, const QString &key, const QString &value)
{
	return this->setCookie(this->cookieJar, host, key, value);
}

bool QtBrowser::setCookie(const char *host, const QString &key, const QString &value)
{
	return this->setCookie(this->cookieJar, QUrl(host), key, value);
}

/*
	Methods for sync all cookies from source host with destination host
*/
bool QtBrowser::syncCookies(QNetworkCookieJar *cookieJar, const QString &src, const QString &dst)
{
	QList<QNetworkCookie> cookies = cookieJar->cookiesForUrl(QUrl(src));
	foreach(QNetworkCookie c, cookies)
	{
		if (!this->setCookie(cookieJar, dst, c.name(), c.value()))
		{
			return false;
		}
	}

	return true;
}

bool QtBrowser::syncCookies(const QString &src, const QString &dst)
{
	QString localSrc;
	QString localDst;
	if (!src.startsWith("http"))
	{
		localSrc.append(src).prepend("http://");
	}

	if (!dst.startsWith("http"))
	{
		localDst.append(dst).prepend("http://");
	}

	return this->syncCookies(this->cookieJar, localSrc, localDst);
}

void QtBrowser::clearCookies()
{
	this->cookieJar->deleteLater();
	this->cookieJar = new QNetworkCookieJar(this);
	this->manager->setCookieJar(this->cookieJar);
}

QList<QNetworkCookie> QtBrowser::getCookies(QString src)
{
	return cookieJar->cookiesForUrl(QUrl(src));
}

QNetworkCookieJar *QtBrowser::getCookieJar()
{
	return this->cookieJar;
}

/*
	Method for receiving text conform regExp.
*/
QString QtBrowser::regCap(const QString &where, const QString &what, bool setMin, int pos)
{
//	qDebug() << "regCap" << what;
	QRegExp r(what);
	r.setMinimal(setMin);
	r.setCaseSensitivity(Qt::CaseInsensitive);
	if (r.indexIn(where) != -1)
	{
//		qDebug() << r.cap(pos);
		return r.cap(pos);
	}
	else
	{
		return "";
	}
}

QString QtBrowser::regCap(const QString &what, bool setMin, int pos)
{
	return this->regCap(this->page, what, setMin, pos);
}

/*
	Method for receiving all occurancies of regExp in text.
*/
QStringList QtBrowser::regCapAll(const QString &where, const QString &regexp, int column, bool setMin)
{
	QRegExp rx(regexp, Qt::CaseInsensitive, QRegExp::RegExp2);
	QStringList list;
	int pos = 0;
	rx.setMinimal(setMin);
	QString cap;
	while ((pos = rx.indexIn(where, pos)) != -1)
	{
		cap = rx.cap(column);
		pos = rx.pos(column) + cap.size();
		list << cap;
	}

	return list;
}

QStringList QtBrowser::regCapAll(const QString &regexp, int column)
{
	return this->regCapAll(this->page, regexp, column);
}

/*
	Поиск всех совпадений регулярных выражений в заданном тексте.
*/
QVector< QVector<QString> > QtBrowser::getMatches(const QString &regexp, QString where, bool setMin)
{
	if (where.isEmpty())
	{
		where = this->getContent();
	}

	QVector< QVector<QString> > result;

	// Количество выражений.
	unsigned matchesCount = QString(regexp).replace("\\(", "(").count("(", Qt::CaseInsensitive);

//	for (unsigned i = 1; i <= matchesCount; i++)
//	{
//		QRegExp rx(regexp, Qt::CaseInsensitive, QRegExp::RegExp2);
//		QVector<QString> list;
//		int pos = 0;
//		rx.setMinimal(true);
//		QString cap;
//		while ((pos = rx.indexIn(where, pos)) != -1)
//		{
//			cap = rx.cap(i);
//			pos = rx.pos(i) + cap.size();
//			list << cap;
//		}

//		result.append(list);
//	}

	QRegExp rx(regexp, Qt::CaseInsensitive, QRegExp::RegExp2);
	QVector<QString> list;
	int pos = 0;
	rx.setMinimal(setMin);
	QString cap;
	while ((pos = rx.indexIn(where, pos)) != -1)
	{
		list.clear();
		for (unsigned i = 1; i <= matchesCount; i++)
		{
			cap = rx.cap(i);
			pos = rx.pos(i) + cap.size();
			list << cap;
		}
		result.append(list);
	}

	return result;
}

/*
	Transfer content of current webpage.
*/
QString QtBrowser::getContent()
{
	return this->page;
}

void QtBrowser::setContent(QString page)
{
	this->page = page;
}

bool QtBrowser::contains(QString text, Qt::CaseSensitivity sensitivity)
{
	return this->getContent().contains(text, sensitivity);
}

bool QtBrowser::contains(QRegExp regexp)
{
	return this->getContent().contains(regexp);
}

QString QtBrowser::htmlDecode(QString text)
{
	QMap<QString, QString> htmlSpecialEntities;
	htmlSpecialEntities.insert("&quot;", "\"");
	htmlSpecialEntities.insert("&amp;", "&");
	htmlSpecialEntities.insert("&lt;", "<");
	htmlSpecialEntities.insert("&gt;", ">");
	htmlSpecialEntities.insert("&OElig;", "Œ");
	htmlSpecialEntities.insert("&ndash;", "–");
	htmlSpecialEntities.insert("&mdash;", "—");
	htmlSpecialEntities.insert("&lsquo;", "‘");
	htmlSpecialEntities.insert("&rsquo;", "’");
	htmlSpecialEntities.insert("&sbquo;", "‚");
	htmlSpecialEntities.insert("&ldquo;", "“");
	htmlSpecialEntities.insert("&rdquo;", "”");
	foreach (QString entity, htmlSpecialEntities.keys())
	{
		text.replace(entity, htmlSpecialEntities.value(entity));
	}

	return text;
}

//QString QtBrowser::decodeXML(QString text)
//{
//	if (text.trimmed().isDetached())
//	{
//		text = this->getContent();
//	}

//	QString pattern("(\\&#\\d{4};)");
//	QStringList toDecode = this->regCapAll(text, pattern, 1, false);
//	foreach(QString tc, toDecode)
//	{
//		uint num10 = this->regCap(tc, QString("(\\d{4})"), false).toInt();
//		QString hex;
//		hex.setNum(num10, 16);
//		text.replace(tc, this->normalization(hex.prepend("\\u0")));
//	}

//	return text;
//}

QStringList QtBrowser::getHttpLinks(QString text, QString url)
{
	if (text.trimmed().isEmpty())
	{
		text = this->getContent();
	}

	if (url.trimmed().isEmpty())
	{
		url = this->getURL();
	}

	text = text.trimmed();
	if (text.isEmpty())
	{
		return QStringList();
	}

	text.replace("&lt;", ">");
	text.replace("&gt;", "<");
	text.replace("&amp;", "&");
	text.replace("&quot;", "\"");

	QRegExp re("<br.*>");
	re.setMinimal(true);
	text.replace(re, "\r\n");

	text.remove("<wbr>");
	text.remove(QRegExp("</?(i|b|u|s)>"));

	QStringList links;
	links.append(this->regCapAll(text, QString("href\\s*=\"(.*)\"")));
	links.append(this->regCapAll(text, QString("<script type\\s*=\\s*\"text/javascript\" src=\"(.*)\">")));
	links.append(this->regCapAll(text, QString("(https?://?([\\da-z\\.-]+)\\.([a-z\\.]{2,6})([\\/\\w \\.-]*)*\\/?)"), 1, false));

	for (int i = 0; i < links.size(); i++)
	{
		// Если ссылка начинается с "/", значит она относительна корня, а значит стоит добавить домен и протокол впереди.
		if (links.at(i).startsWith("/"))
		{
			QString link(links.at(i));
			link.prepend(QUrl::fromUserInput(QUrl(url).host()).toString());
			links.replace(i, link);
		}
	}

	return links;
}

void QtBrowser::prepareNetworkRequest(QNetworkRequest &networkRequest)
{
	networkRequest.setRawHeader("User-Agent", "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.0.10) Gecko/2009042523 Ubuntu/9.04 (jaunty) Firefox/3.0.10");
	networkRequest.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	networkRequest.setRawHeader("Accept-Language", "de, en-gb;q=0.9, en;q=0.8");
	networkRequest.setRawHeader("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
	networkRequest.setRawHeader("Cache-Control", "no-cache");
	networkRequest.setRawHeader("Connection", "close");
	networkRequest.setRawHeader("Pragma", "no-cache");
}

/*
	Method for synchronous receive data from Internet.
*/
QNetworkReply *QtBrowser::getItSync(const QUrl &url, const QByteArray params)
{
	if (!this->visitedUrls.isEmpty())
	{
		this->request.setRawHeader("Referer", this->visitedUrls.last().toAscii().data());
	}
	this->visitedUrls.append(url.toString());
	this->request.setUrl(url);
	this->url = url.toString();

	if (params.isEmpty())
	{
		this->reply = this->manager->get(this->request);
	}
	else
	{
		QNetworkRequest rtmp = this->request;
		this->request.setRawHeader("Content-type", "application/x-www-form-urlencoded");
		this->reply = this->manager->post(request, params);
		this->request = rtmp;
	}

	QEventLoop loop;
	connect(this->reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	this->redirectLocation = this->reply->rawHeader("Location");
	if (this->followOnRedirect)
	{
		if (!this->redirectLocation.isEmpty())
		{
			if (!this->redirectLocation.startsWith("http"))
			{
				bool startedWithSlash = true;
				if (!this->redirectLocation.startsWith("/"))
				{
					startedWithSlash = false;
				}
				QString toPrepend(this->regCap(this->getURL(), QString("(https?://(www\\.)?.*)/(.+)")));

				if (!startedWithSlash)
				{
					toPrepend = this->getURL();
				}

				this->redirectLocation.prepend(toPrepend);
			}
			qDebug() << "Request redirected to:" << this->redirectLocation;
			return this->getItSync(this->redirectLocation, params);
		}
	}

	return this->reply;
}


/*
	Method for receiving pair of values from JSON-string.
*/
QString QtBrowser::jsonVal(QString where, QString name)
{
	return this->regCap(where, "'\\s?" + name + "\\s?':\\s?'(.*)'");
}

void QtBrowser::setURL(QString url)
{
	this->url = url;
}

QString QtBrowser::getURL()
{
	if (this->visitedUrls.size())
	{
		return this->visitedUrls.last();
	}
	else
	{
		return this->url;
	}
}

/*
	Method for receiving all occurancies of specified tag.
*/
QStringList QtBrowser::getSpecifiedTags(const QString &where, const QString &tag)
{
	QStringList singleTagsList;
	singleTagsList.append("input");
	singleTagsList.append("a");
	singleTagsList.append("p");
	singleTagsList.append("image");

	QStringList tags;
	QString localWhere = where;
	QString localTag;
	int pos = 0;

	QString singleTagClosed	=	"(<[\\s]*"+tag+".*/>)";
	QString singleTagUnclosed = "(<[\\s]*"+tag+".*>)";
	QString doubleTag =			"(<[\\s]*"+tag+"[\\s]*.*>.*</"+tag+">)";

	while (!this->regCap(localWhere, doubleTag).isEmpty() || !this->regCap(localWhere, singleTagClosed).isEmpty() || !this->regCap(localWhere, singleTagUnclosed).isEmpty())
	{
		if (singleTagsList.contains(tag, Qt::CaseInsensitive))
		{
			localTag = regCap(localWhere, singleTagUnclosed);
			if (localTag.isEmpty())
			{
				localTag = regCap(localWhere, singleTagClosed);
			}
		}
		else
		{
			localTag = regCap(localWhere, doubleTag);
		}

		pos = localWhere.indexOf(localTag) + localTag.size();
		tags.append(localTag);
		localWhere.remove(0, pos);
	}

	return tags;
}

/*
	Check if tag is parameters
*/
bool QtBrowser::isTagParam(const QString &tag, const QString &param)
{
	bool result = this->regCap(tag, "[\\s< ]?[^\"'](" + param + ")[^\"'][\\s]?").isEmpty();
	return !result;
}

/*
	Get value of the param.
*/
QString QtBrowser::getParamValue(const QString &where, const QString &paramName)
{
	QString result = this->regCap(where, paramName + "=\"(.*)\"[\\s>]").trimmed();
	if (result.trimmed().isEmpty())
	{
		result = this->regCap(where, paramName + "='(.*)'[\\s>]").trimmed();
	}
	if (result.trimmed().isEmpty())
	{
		result = this->regCap(where, paramName + "=(.*)[\\s>]").trimmed();
	}
	if (result.trimmed().isEmpty())
	{
		result = this->regCap(where, paramName + "\\s*=\\s*['\"](.*)['\"]");
	}

	result.replace("\"\"", "");
	result.replace("''", "");

	return result;
}

/*
	Method for parameter name and it's value, which will be used for form submitting.
*/
QString QtBrowser::getNameValue(QString where)
{
	QString result;

	QString name	= this->getParamValue(where, "name");
	QString value	= this->getParamValue(where, "value");

	result = name + "=" + value;
	result.replace(" ", "+");
	result.replace("\"\"", "");
	result.replace("''", "");

	return result;
}

QStringList QtBrowser::getForms()
{
	return this->getSpecifiedTags(this->getContent(), "form");
}

/*
	Find first form, that has specified properties.
*/
QString QtBrowser::getFormByProperty(const QString &where, const QString &prop, const QString &value)
{
	foreach(QString form, this->getSpecifiedTags(where, "form"))
	{
		QString initTag = this->regCap(form, QString("(<.*>)"));
		if (!this->regCap(initTag, "("+prop+"\\s*=\\s*\""+value+"\")").trimmed().isEmpty())
		{
			return form;
		}
	}
	return "";
}

QString QtBrowser::getFormByProperty(const QString &property, const QString &value)
{
	return this->getFormByProperty(this->page, property, value);
}

/*
	Add value to the form.
*/
QString QtBrowser::formPutValue(QString form, const QString &name, const QString &value)
{
	QString n = name;
	QString v = value;
	n.replace("\"", "");
	v.replace("\"", "");
	if (!form.contains(n, Qt::CaseInsensitive))
	{
		form.replace("</form>", "<input name=\"" + n + "\" value=\"" + v + "\" /></form>");
	}
	else
	{
		form.replace("name=\"" + n + "\"", "name=\"" + n + "\" value=\"" + v + "\" ");
	}
	return form;
}

/*
	Methods for getting all form parameters.
*/
QByteArray QtBrowser::getFormParams(QString where)
{
	QByteArray localParams;
	QStringList tags;
	tags.append("input");
	tags.append("textarea");
	foreach(QString t, tags)
	{
		foreach(QString tag, this->getSpecifiedTags(where, t))
		{
			if (!this->isTagParam(tag, "name"))
			{
				continue;
			}
			QString toAppend = this->getNameValue(tag) + "&";
			localParams.append(toAppend);
		}
	}

	localParams.remove(localParams.size()-1, 1);

	return localParams;
}

/*
	Submit form.
*/
QNetworkReply *QtBrowser::submitForm(QString form)
{
	QString url = regCap(form, QString("action=\"(.*)\""));
	url.replace("///", "//");

	if (url.trimmed().isEmpty())
	{
		url = this->getURL();
	}

	if (!this->visitedUrls.isEmpty() && !url.contains(QUrl(this->visitedUrls.last()).host()) && QUrl(url).isRelative())
	{
		url = url.prepend(QUrl(this->visitedUrls.last()).host());
	}

	if (!url.contains("://"))
	{
		url = url.prepend("http://");
	}

	if (url.isEmpty())
	{
		return 0;
	}
	else
	{
		const QString method = this->getParamValue(form, "method").toLower();

		if (method == QString("post").toLower())
		{
			this->request.setRawHeader("Content-type", "application/x-www-form-urlencoded");
			QByteArray params = this->getFormParams(form);
			return this->getItSync(url, params);
		}
		else if (method == QString("get").toLower())
		{
			this->request.setRawHeader("Content-type", 0);
			return this->getItSync(url);
		}
		else
		{
			qDebug() << trUtf8("Unknow type of form submit.");
			return 0;
		}
	}
}

void QtBrowser::submit(QString form)
{
	this->setContent(this->submitForm(form)->readAll());
}

/**
 * @brief QtBrowser::getFormByHTML - Поиск формы по содержимому.
 * @param text - Текст, который искать.
 * @return - нужная форма.
 */
QString QtBrowser::getFormByHTML(QString text)
{
	QStringList forms = this->getSpecifiedTags(this->getContent(), "form");
	foreach (QString form, forms)
	{
		if (form.contains(text))
		{
			return form;
		}
	}
	return "";
}

/*
	Generate new User-Agent header.
*/
void QtBrowser::generateUserAgent()
{
//	qDebug() << __FILE__ << __LINE__;
	UserAgenContainer container;
	this->setUserAgent(container.getRandomUserAgent());
}

/*
	Set User-Agent.
*/
void QtBrowser::setUserAgent(QByteArray userAgent)
{
//	qDebug() << __FILE__ << __LINE__ << userAgent;
	this->userAgent = userAgent;
	this->request.setRawHeader("User-Agent", this->userAgent);
}

/*
	Get User-Agent
*/
QByteArray QtBrowser::getUserAgent()
{
	if (this->userAgent.trimmed().isEmpty())
	{
		this->userAgent = this->request.rawHeader("User-Agent");
	}
//	qDebug() << __FILE__ << __LINE__ << this->userAgent;
	return this->userAgent;
}

QString QtBrowser::getRedirectLocation()
{
	return this->redirectLocation;
}

bool QtBrowser::hasHeader(QByteArray headerName)
{
	if (this->request.hasRawHeader(headerName))
	{
		return true;
	}
	else if (this->request.hasRawHeader(headerName))
	{
		return true;
	}

	return false;
}

QByteArray QtBrowser::getHeader(QByteArray header)
{
	if (this->request.hasRawHeader(header))
	{
		return this->request.rawHeader(header);
	}
	else
	{
		if (this->hasHeader(header))
		{
			return this->reply->rawHeader(header);
		}
	}

	return "";
}

void QtBrowser::setHeader(QByteArray name, QByteArray value)
{
	this->request.setRawHeader(name, value);
}

QtBrowser QtBrowser::clearHeader(QByteArray name)
{
	this->request.setRawHeader(name, 0);
	return *this;
}

QList<QPair<QByteArray, QByteArray> > QtBrowser::headers()
{
	return this->reply->rawHeaderPairs();
}

/*
	Нормализация строки, содержащей юникод-символы.
	https://ru.wikipedia.org/wiki/Юникод
*/
QString QtBrowser::normalization(QString str)
{
	QStringList unicodeSymbols = this->regCapAll(str, "(\\\\u[0-9a-f]{4}|\\\\x[0-9a-f]{2})", 1, false);
	for (int i = 0; i < unicodeSymbols.size(); i++)
	{
		// Удаляем первые 2 символа - это либо \u, либо \x.
		QString curr = QString(unicodeSymbols.at(i)).remove(0, 2);

		// Просто переменная, куда вернётся результат успешности преобразования строки.
		bool ok;

		// Заменяем юникод-символ на его ASCII-представление.
		str.replace(unicodeSymbols.at(i), QString(char(curr.toInt(&ok, 16))));
	}

	return str;
}

/*
	Декодирование строки.
*/
QString QtBrowser::decode(QString str)
{
	QStringList hexSymbols = this->regCapAll(str, "(%[0-9a-f]{2})");
	for (int i = 0; i < hexSymbols.size(); i++)
	{
		// Удаляем %
		QString curr = QString(hexSymbols.at(i)).remove(0, 1);

		// Просто переменная, куда вернётся результат успешности преобразования строки.
		bool ok;

		// Заменяем символ на его ASCII-представление.
		str.replace(hexSymbols.at(i), QString(char(curr.toInt(&ok, 16))));
	}

	return str;
}

bool QtBrowser::save(const QString &fileName, QIODevice *data)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Couldn't open" << fileName << "for writing" << file.errorString();
		return false;
	}

	file.write(data->readAll());
	file.close();

	return true;
}

bool QtBrowser::save(const QString &fileName, const QString &data)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Couldn't open" << fileName << "for writing" << file.errorString();
		return false;
	}

	QByteArray localdata;
	localdata.append(data);
	file.write(localdata);
	file.close();

	return true;
}

/*
	Receive webpage as text.
*/
QString QtBrowser::getPage(const QUrl url, const QByteArray params)
{
	QUrl _url(url);
	if (!this->visitedUrls.isEmpty() && !url.toString().contains(QUrl(this->visitedUrls.last()).host()) && url.isRelative())
	{
		_url.setUrl(QUrl::fromUserInput(url.toString().prepend(QUrl(this->visitedUrls.last()).host())).toString());
		qDebug() << __FILE__ << __LINE__ << _url;
	}
	this->setURL(_url.toString());
	this->page = this->getItSync(_url, params)->readAll();
	return this->page;
}

QString QtBrowser::getPage(const char *url, const QByteArray params)
{
	return this->getPage(QUrl(url), params);
}

/*
	To redirect or not to redirect.
*/
void QtBrowser::setFollowRedirects(bool val)
{
	this->followOnRedirect = val;
}

/*
	HEAD-запрос.
*/
QNetworkReply *QtBrowser::head(const QUrl url)
{
	QNetworkReply *reply;
	QNetworkAccessManager *manager = new QNetworkAccessManager();
	reply = manager->head(QNetworkRequest(url));

	QEventLoop *loop = new QEventLoop(this);
	connect(reply, SIGNAL(finished()), loop, SLOT(quit()));
	loop->exec();
	loop->deleteLater();

	return reply;
}

int QtBrowser::getResponseCode()
{
	return this->reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}
