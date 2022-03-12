#include "RpgLog.hpp"
#include <QTimeZone>

const QString& RpgLog::getUser() const
{
	return user;
}

void RpgLog::setUser(const QString& newUser)
{
	user = newUser;
}

const QVariant& RpgLog::getDate() const
{
	return date;
}

void RpgLog::setDate(const QVariant& newDate)
{
	date = newDate;
	streamlinedDate = (this->date.type() == QVariant::DateTime);
}

const QString& RpgLog::getContent() const
{
	return content;
}

void RpgLog::setContent(const QString& newContent)
{
	content = newContent;
}

void RpgLog::toJson(QJsonObject& json) const
{
	json[QStringLiteral("user")] = this->user;
	json[QStringLiteral("date")] = QJsonValue::fromVariant(this->date);
	json[QStringLiteral("content")] = this->content;
	json[QStringLiteral("streamlinedDate")] = this->streamlinedDate;
}

QJsonObject RpgLog::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}

void RpgLog::fromJson(const QJsonObject& json)
{
	this->user = json[QStringLiteral("user")].toString();
	this->date = json[QStringLiteral("date")].toVariant();
	this->content = json[QStringLiteral("content")].toString();
	this->streamlinedDate = json[QStringLiteral("streamlinedDate")].toBool();
}

static const QString POST_START = QStringLiteral("{{RPG Post/");
static const QString DATE_START = QStringLiteral("|date=");
static const QString CONTENT_START = QStringLiteral("|post=");
static const QString POST_END = QStringLiteral("}}");

void RpgLog::toString(QTextStream& stream) const
{
	//stream << QStringLiteral("{{RPG Post/%1\n|date=%2\n|post=%3\n}}").arg(user).arg(date).arg(content);
	stream << POST_START << user << '\n' << DATE_START << date.toString() << '\n' << CONTENT_START << content << '\n' << POST_END;
}

QString RpgLog::toString() const
{
	//return QStringLiteral("{{RPG Post/%1\n|date=%2\n|post=%3\n}}").arg(user).arg(date).arg(content);
	QString tmp;
	QTextStream stream(&tmp, QIODevice::ReadWrite);
	toString(stream);
	return tmp;
}

void RpgLog::fromString(QTextStream& stream)
{
	QString lastRead;
	while(stream.readLineInto(&lastRead)) {
		if(lastRead.startsWith(POST_START)) {
			this->user = lastRead.mid(POST_START.size());
		} else if(lastRead.startsWith(DATE_START)) {
			this->date = lastRead.mid(DATE_START.size());
		} else if(lastRead.startsWith(CONTENT_START)) {
			this->content = lastRead.mid(CONTENT_START.size()).append(QChar('\n'));
		} else if(!lastRead.compare(POST_END)) {
			return;
		} else {
			this->content = this->content.append(QChar('\n')).append(lastRead);
		}
		lastRead.clear();
	}
}

void RpgLog::fromString(QString str)
{
	QTextStream stream(&str, QIODevice::ReadOnly);
	fromString(stream);
}

void RpgLog::attemptToStreamlineDate(const QList<QLocale>& localeList)
{
	if(this->date.type() == QVariant::DateTime)
	{
		streamlinedDate = true;
		return;
	} else if(this->date.type() == QVariant::String) {
		QDateTime initialParse = QDateTime::fromString(this->date.toString());
		if(initialParse.isValid()) {
			streamlinedDate = true;
			this->date =initialParse;
			return;
		}
		for(const auto& it : localeList) {
			QDateTime initialParse = it.toDateTime(this->date.toString(), QLocale::LongFormat);
			if(initialParse.isValid()) {
				streamlinedDate = true;
				this->date =initialParse;
				return;
			}
			initialParse = it.toDateTime(this->date.toString(), QLocale::ShortFormat);
			if(initialParse.isValid()) {
				streamlinedDate = true;
				this->date =initialParse;
				return;
			}
		}
	}
}

void RpgLog::attemptToStreamlineDate()
{
	// 2022. márc. 12., szombat, 13:11:42
	if(this->date.type() != QVariant::String) return;
	QStringList tokens = this->date.toString().replace(QChar(','),QChar('.')).split(QChar('.'),Qt::SkipEmptyParts);
	if(tokens.size() < 5) return;
	for(auto& it : tokens) {
		it = it.trimmed();
	}
	bool okay = true;
	int y = tokens[0].toInt(&okay);
	if(!okay) return;
	int m = 0;
	if(!tokens[1].compare(QStringLiteral("jan"),Qt::CaseInsensitive)) {
		m = 1;
	} else if(!tokens[1].compare(QStringLiteral("febr"),Qt::CaseInsensitive)) {
		m = 2;
	} else if(!tokens[1].compare(QStringLiteral("márc"),Qt::CaseInsensitive)) {
		m = 3;
	} else if(!tokens[1].compare(QStringLiteral("ápr"),Qt::CaseInsensitive)) {
		m = 4;
	} else if(!tokens[1].compare(QStringLiteral("máj"),Qt::CaseInsensitive)) {
		m = 5;
	} else if(!tokens[1].compare(QStringLiteral("jún"),Qt::CaseInsensitive)) {
		m = 6;
	} else if(!tokens[1].compare(QStringLiteral("júl"),Qt::CaseInsensitive)) {
		m = 7;
	} else if(!tokens[1].compare(QStringLiteral("aug"),Qt::CaseInsensitive)) {
		m = 8;
	} else if(!tokens[1].compare(QStringLiteral("szept"),Qt::CaseInsensitive)) {
		m = 9;
	} else if(!tokens[1].compare(QStringLiteral("okt"),Qt::CaseInsensitive)) {
		m = 10;
	} else if(!tokens[1].compare(QStringLiteral("nov"),Qt::CaseInsensitive)) {
		m = 11;
	} else if(!tokens[1].compare(QStringLiteral("dec"),Qt::CaseInsensitive)) {
		m = 12;
	} else {
		return;
	}
	int d = tokens[2].toInt(&okay);
	if(!okay) return;
	QTime time = QTime::fromString(tokens[4]);
	if(!time.isValid()) return;
	QDateTime tmp(QDate(y,m,d),time,QTimeZone::systemTimeZone());
	if(tmp.isValid()) {
		this->date = tmp;
		this->streamlinedDate = true;
	}
}

RpgLog::RpgLog()
{
	streamlinedDate = false;
}

RpgLog::RpgLog(const QString& user, const QString& date, const QString& content)
	: user(user), date(date), content(content), streamlinedDate(false)
{

}

RpgLog::RpgLog(QString&& user, QString&& date, QString&& content)
	: user(std::move(user)), date(std::move(date)), content(std::move(content)), streamlinedDate(false)
{

}

RpgLog::RpgLog(RpgLog&& mov)
	: user(std::move(mov.user)), date(std::move(mov.date)), content(std::move(mov.content)), streamlinedDate(mov.streamlinedDate)
{

}

RpgLog::RpgLog(const RpgLog& cpy)
	: user(cpy.user), date(cpy.date), content(cpy.content), streamlinedDate(cpy.streamlinedDate)
{

}

RpgLog& RpgLog::operator=(RpgLog&& mov)
{
	this->user = std::move(mov.user);
	this->date = std::move(mov.date);
	this->content = std::move(mov.content);
	this->streamlinedDate = mov.streamlinedDate;
	return *this;
}
RpgLog& RpgLog::operator=(const RpgLog& cpy)
{
	this->user = cpy.user;
	this->date = cpy.date;
	this->content = cpy.content;
	this->streamlinedDate = cpy.streamlinedDate;
	return *this;
}
QTextStream& operator<<(QTextStream& left, const RpgLog& right) {
	right.toString(left);
	return left;
}
QTextStream& operator>>(QTextStream& left, RpgLog& right) {
	right.fromString(left);
	return left;
}
