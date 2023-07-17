#include "RpgLog.hpp"
#include <QTimeZone>
#include <QIODevice>

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
	json[QStringLiteral("hun")] = this->hun;
	json[QStringLiteral("user")] = this->user;
	json[QStringLiteral("date")] = QJsonValue::fromVariant(this->date);
	json[QStringLiteral("content")] = this->content;
	json[QStringLiteral("streamlinedDate")] = this->streamlinedDate;
	if(this->streamlinedDate) {
		json[QStringLiteral("unixTimestamp")] = this->date.toDateTime().toUTC().toSecsSinceEpoch();
	}
}

QJsonObject RpgLog::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}

void RpgLog::fromJson(const QJsonObject& json)
{
	this->hun = json[QStringLiteral("hun")].toBool();
	this->user = json[QStringLiteral("user")].toString();
	this->content = json[QStringLiteral("content")].toString();
	this->streamlinedDate = json[QStringLiteral("streamlinedDate")].toBool();
	if(this->streamlinedDate && json.contains(QStringLiteral("unixTimestamp"))) {
		this->date = json[QStringLiteral("date")].toVariant().toDateTime();
	} else {
		QString str = json[QStringLiteral("date")].toString();
		QDateTime parsedDate = QDateTime::fromString(str);
		if(parsedDate.isValid()) {
			this->date = parsedDate;
			this->streamlinedDate = true;
		}
		else this->date = str;
	}
}

static const QString POST_START = QStringLiteral("{{RPG Post/");
static const QString DATE_START = QStringLiteral("|date=");
static const QString POST_HUN = QStringLiteral("|hun=yes");
static const QString POST_HUN_N = QStringLiteral("|hun=yes\n");
static const QString CONTENT_START = QStringLiteral("|post=");
static const QString POST_END = QStringLiteral("}}");

void RpgLog::toString(QTextStream& stream) const
{
	//stream << QStringLiteral("{{RPG Post/%1\n|date=%2\n|post=%3\n}}").arg(user).arg(date).arg(content);
	stream << POST_START << user << '\n' << DATE_START << (date.typeId() == QMetaType::QDateTime ? date.toDateTime().toString(Qt::DateFormat::TextDate) : date.toString()) << (hun ? POST_HUN_N : QStringLiteral("\n")) << CONTENT_START << content << '\n' << POST_END;
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
		} else if(lastRead.startsWith(CONTENT_START) && lastRead.endsWith(POST_END)) {
			lastRead = lastRead.mid(CONTENT_START.size());
			this->content = lastRead.left(lastRead.size() - POST_END.size());
			return;
		} else if(lastRead.startsWith(CONTENT_START)) {
			this->content = lastRead.mid(CONTENT_START.size()).append(QChar('\n'));
		} else if(lastRead.endsWith(POST_END)) {
			this->content = this->content.append(lastRead.left(lastRead.size() - POST_END.size()));
			return;
		} else if(!lastRead.compare(POST_HUN)) {
			this->hun = true;
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

static QDateTime validlastDateTime;
void RpgLog::attemptToStreamlineDate()
{
	if(this->date.type() != QVariant::String) return;

	for(int i = Pidgin; i < Invalid ; ++i) {
		QDateTime tmp = attemptToStreamlineDate(DateFormat(i));
		if(tmp.isValid()) {
			validlastDateTime = tmp;
			this->date = tmp;
			this->streamlinedDate = true;
		}
	}

}

static const QString dateFormat1 = QStringLiteral("yyyy-M-d @hh'h' mm'm' ss's' zzz'ms'");
static const QString dateFormat2 = QStringLiteral("yyyy-MM-ddTHH:mm:ss.zzzZ");
static const QString dateFormat3 = QStringLiteral("MMMM d, yyyy h:mmap");
static const QLocale AmericanLocale = QLocale(QLocale::English,QLocale::UnitedStates);

QDateTime RpgLog::attemptToStreamlineDate(DateFormat format)
{
	if(this->date.type() == QVariant::DateTime) return this->date.toDateTime();
	switch (format) {
	case Pidgin: {
		// unknown 22.23.39
		QStringList tmptor = this->date.toString().split(QChar(' '),Qt::SkipEmptyParts);
		if(tmptor.size() == 2 && !tmptor[0].compare(QStringLiteral("unknown"),Qt::CaseInsensitive) && validlastDateTime.isValid()) {
			QTime time = QTime::fromString(tmptor[1].replace(QChar('.'),QChar(':')));
			if(time.isValid()) return QDateTime(validlastDateTime.date(),time,QTimeZone::systemTimeZone());
		}
		// 2022. márc. 12., szombat, 13:11:42
		// 2022. febr. 6., vasárnap, 15:13:50
		QStringList tokens = this->date.toString().replace(QStringLiteral("«"),QChar(' '))
				.replace(QStringLiteral("»"),QChar(' ')).replace(QChar(','),QChar('.')).split(QChar('.'),Qt::SkipEmptyParts);
		if(tokens.size() < 5) return QDateTime();
		for(auto& it : tokens) {
			it = it.trimmed();
		}
		bool okay = true;
		int y = tokens[0].toInt(&okay);
		if(!okay) return QDateTime();
		int m = 0;
		if(tokens[1].startsWith(QStringLiteral("jan"),Qt::CaseInsensitive)) {
			m = 1;
		} else if(tokens[1].startsWith(QStringLiteral("feb"),Qt::CaseInsensitive)) {
			m = 2;
		} else if(tokens[1].startsWith(QStringLiteral("márc"),Qt::CaseInsensitive)) {
			m = 3;
		} else if(tokens[1].startsWith(QStringLiteral("ápr"),Qt::CaseInsensitive)) {
			m = 4;
		} else if(tokens[1].startsWith(QStringLiteral("máj"),Qt::CaseInsensitive)) {
			m = 5;
		} else if(tokens[1].startsWith(QStringLiteral("jún"),Qt::CaseInsensitive)) {
			m = 6;
		} else if(tokens[1].startsWith(QStringLiteral("júl"),Qt::CaseInsensitive)) {
			m = 7;
		} else if(tokens[1].startsWith(QStringLiteral("aug"),Qt::CaseInsensitive)) {
			m = 8;
		} else if(tokens[1].startsWith(QStringLiteral("szept"),Qt::CaseInsensitive)) {
			m = 9;
		} else if(tokens[1].startsWith(QStringLiteral("okt"),Qt::CaseInsensitive)) {
			m = 10;
		} else if(tokens[1].startsWith(QStringLiteral("nov"),Qt::CaseInsensitive)) {
			m = 11;
		} else if(tokens[1].startsWith(QStringLiteral("dec"),Qt::CaseInsensitive)) {
			m = 12;
		} else {
			return QDateTime();
		}
		int d = tokens[2].toInt(&okay);
		if(!okay) return QDateTime();
		QTime time = QTime::fromString(tokens[4].replace(QChar('.'),QChar(':')));
		if(!time.isValid()) return QDateTime();
		return QDateTime(QDate(y,m,d),time,QTimeZone::systemTimeZone());
	}
	case ProBoards: {
		// Jan 8, 2010 at 10:34pm
		// Jan 8, 2010 10:34pm
		QStringList tokens = this->date.toString().replace(QStringLiteral("«"),QChar(' ')).replace(QStringLiteral(" at "),QChar(' '))
				.replace(QStringLiteral("»"),QChar(' ')).replace(QChar(','),QChar(' ')).split(QChar(' '),Qt::SkipEmptyParts);
		if(tokens.size() < 4) return QDateTime();
		for(auto& it : tokens) {
			it = it.trimmed();
		}
		int m = 0;
		if(tokens[0].startsWith("jan",Qt::CaseInsensitive)) {
			m = 1;
		} else if(tokens[0].startsWith("feb",Qt::CaseInsensitive)) {
			m = 2;
		} else if(tokens[0].startsWith("mar",Qt::CaseInsensitive)) {
			m = 3;
		} else if(tokens[0].startsWith("apr",Qt::CaseInsensitive)) {
			m = 4;
		} else if(tokens[0].startsWith("may",Qt::CaseInsensitive)) {
			m = 5;
		} else if(tokens[0].startsWith("jun",Qt::CaseInsensitive)) {
			m = 6;
		} else if(tokens[0].startsWith("jul",Qt::CaseInsensitive)) {
			m = 7;
		} else if(tokens[0].startsWith("aug",Qt::CaseInsensitive)) {
			m = 8;
		} else if(tokens[0].startsWith("sep",Qt::CaseInsensitive)) {
			m = 9;
		} else if(tokens[0].startsWith("oct",Qt::CaseInsensitive)) {
			m = 10;
		} else if(tokens[0].startsWith("nov",Qt::CaseInsensitive)) {
			m = 11;
		} else if(tokens[0].startsWith("dec",Qt::CaseInsensitive)) {
			m = 12;
		} else {
			return QDateTime();
		}
		bool okay = true;
		int d = tokens[1].toInt(&okay);
		if(!okay) return QDateTime();
		int y = tokens[2].toInt(&okay);
		if(!okay) return QDateTime();
		QStringList time = tokens[3].left(tokens[3].size()-2).split(QChar(':'),Qt::SkipEmptyParts);
		if(time.size() < 2) return QDateTime();
		int hr = time[0].toInt(&okay);
		if(!okay) return QDateTime();
		int min = time[1].toInt(&okay);
		if(!okay) return QDateTime();
		if(tokens[3].endsWith(QStringLiteral("pm"),Qt::CaseInsensitive)) hr += 12;
		return QDateTime(QDate(y,m,d),QTime(hr,min,0,0),QTimeZone::systemTimeZone());
	}
	case PhpBB2: {
		// Pént. Jún. 04, 2010 11:25 am
		QStringList tokens = this->date.toString().replace(QStringLiteral("«"),QChar(' '))
				.replace(QStringLiteral("»"),QChar(' ')).replace(QChar(','),QChar('.')).replace(QChar('.'),QChar(' ')).split(QChar(' '),Qt::SkipEmptyParts);
		if(tokens.size() < 6) return QDateTime();
		for(auto& it : tokens) {
			it = it.trimmed();
		}
		int m = 0;
		if(tokens[1].startsWith("jan",Qt::CaseInsensitive)) {
			m = 1;
		} else if(tokens[1].startsWith("feb",Qt::CaseInsensitive)) {
			m = 2;
		} else if(tokens[1].startsWith("márc",Qt::CaseInsensitive)) {
			m = 3;
		} else if(tokens[1].startsWith("ápr",Qt::CaseInsensitive)) {
			m = 4;
		} else if(tokens[1].startsWith("máj",Qt::CaseInsensitive)) {
			m = 5;
		} else if(tokens[1].startsWith("jún",Qt::CaseInsensitive)) {
			m = 6;
		} else if(tokens[1].startsWith("júl",Qt::CaseInsensitive)) {
			m = 7;
		} else if(tokens[1].startsWith("aug",Qt::CaseInsensitive)) {
			m = 8;
		} else if(tokens[1].startsWith("szept",Qt::CaseInsensitive)) {
			m = 9;
		} else if(tokens[1].startsWith("okt",Qt::CaseInsensitive)) {
			m = 10;
		} else if(tokens[1].startsWith("nov",Qt::CaseInsensitive)) {
			m = 11;
		} else if(tokens[1].startsWith("dec",Qt::CaseInsensitive)) {
			m = 12;
		} else {
			return QDateTime();
		}
		bool okay = true;
		int d = tokens[2].toInt(&okay);
		if(!okay) return QDateTime();
		int y = tokens[3].toInt(&okay);
		if(!okay) return QDateTime();
		QStringList time = tokens[4].split(QChar(':'),Qt::SkipEmptyParts);
		if(time.size() < 2) return QDateTime();
		int hr = time[0].toInt(&okay);
		if(!okay) return QDateTime();
		int min = time[1].toInt(&okay);
		if(!okay) return QDateTime();
		if(!tokens[5].compare(QStringLiteral("pm"),Qt::CaseInsensitive)) hr += 12;
		return QDateTime(QDate(y,m,d),QTime(hr,min,0,0),QTimeZone::systemTimeZone());
	}
	case PhpBB2_Alt: {
		// Pént. 4 Jún. 2010 - 8:23
		QStringList tokens = this->date.toString().replace(QStringLiteral("«"),QChar(' '))
				.replace(QStringLiteral("»"),QChar(' ')).replace(QChar('-'),QChar('.')).replace(QChar('.'),QChar(' ')).split(QChar(' '),Qt::SkipEmptyParts);
		if(tokens.size() < 5) return QDateTime();
		for(auto& it : tokens) {
			it = it.trimmed();
		}
		int m = 0;
		if(tokens[2].startsWith("jan",Qt::CaseInsensitive)) {
			m = 1;
		} else if(tokens[2].startsWith("feb",Qt::CaseInsensitive)) {
			m = 2;
		} else if(tokens[2].startsWith("márc",Qt::CaseInsensitive)) {
			m = 3;
		} else if(tokens[2].startsWith("ápr",Qt::CaseInsensitive)) {
			m = 4;
		} else if(tokens[2].startsWith("máj",Qt::CaseInsensitive)) {
			m = 5;
		} else if(tokens[2].startsWith("jún",Qt::CaseInsensitive)) {
			m = 6;
		} else if(tokens[2].startsWith("júl",Qt::CaseInsensitive)) {
			m = 7;
		} else if(tokens[2].startsWith("aug",Qt::CaseInsensitive)) {
			m = 8;
		} else if(tokens[2].startsWith("szept",Qt::CaseInsensitive)) {
			m = 9;
		} else if(tokens[2].startsWith("okt",Qt::CaseInsensitive)) {
			m = 10;
		} else if(tokens[2].startsWith("nov",Qt::CaseInsensitive)) {
			m = 11;
		} else if(tokens[2].startsWith("dec",Qt::CaseInsensitive)) {
			m = 12;
		} else {
			return QDateTime();
		}
		bool okay = true;
		int d = tokens[1].toInt(&okay);
		if(!okay) return QDateTime();
		int y = tokens[3].toInt(&okay);
		if(!okay) return QDateTime();
		QStringList time = tokens[4].split(QChar(':'),Qt::SkipEmptyParts);
		if(time.size() < 2) return QDateTime();
		int hr = time[0].toInt(&okay);
		if(!okay) return QDateTime();
		int min = time[1].toInt(&okay);
		if(!okay) return QDateTime();
		return QDateTime(QDate(y,m,d),QTime(hr,min,0),QTimeZone::systemTimeZone());
	}
	case AoB: {
		// 07/23/2008 02:09:36
		QStringList tokens = this->date.toString().replace(QStringLiteral("«"),QChar(' '))
				.replace(QStringLiteral("»"),QChar(' ')).replace(QChar('/'),QChar(' ')).split(QChar(' '),Qt::SkipEmptyParts);
		if(tokens.size() < 4) return QDateTime();
		for(auto& it : tokens) {
			it = it.trimmed();
		}
		bool okay = true;
		int m = tokens[0].toInt(&okay);
		if(!okay) return QDateTime();
		int d = tokens[1].toInt(&okay);
		if(!okay) return QDateTime();
		int y = tokens[2].toInt(&okay);
		if(!okay) return QDateTime();
		QTime time = QTime::fromString(tokens[3]);
		if(!time.isValid()) return QDateTime();
		return QDateTime(QDate(y,m,d),time,QTimeZone::systemTimeZone());
	}
	case FListPriv: {
		// 2020 May 19 15:10
		QStringList tokens = this->date.toString().replace(QStringLiteral("«"),QChar(' '))
				.replace(QStringLiteral("»"),QChar(' ')).replace(QChar('.'),QChar(' ')).replace(QChar(','),QChar(' ')).split(QChar(' '),Qt::SkipEmptyParts);
		if(tokens.size() < 3) return QDateTime();
		bool okay = true;
		int y = tokens[0].toInt(&okay);
		if(!okay) return QDateTime();
		int d = tokens[2].toInt(&okay);
		if(!okay) return QDateTime();
		int m = 0;
		if(tokens[1].startsWith("jan",Qt::CaseInsensitive)) {
			m = 1;
		} else if(tokens[1].startsWith("feb",Qt::CaseInsensitive)) {
			m = 2;
		} else if(tokens[1].startsWith("mar",Qt::CaseInsensitive)) {
			m = 3;
		} else if(tokens[1].startsWith("apr",Qt::CaseInsensitive)) {
			m = 4;
		} else if(tokens[1].startsWith("may",Qt::CaseInsensitive)) {
			m = 5;
		} else if(tokens[1].startsWith("jun",Qt::CaseInsensitive)) {
			m = 6;
		} else if(tokens[1].startsWith("jul",Qt::CaseInsensitive)) {
			m = 7;
		} else if(tokens[1].startsWith("aug",Qt::CaseInsensitive)) {
			m = 8;
		} else if(tokens[1].startsWith("sept",Qt::CaseInsensitive)) {
			m = 9;
		} else if(tokens[1].startsWith("oct",Qt::CaseInsensitive)) {
			m = 10;
		} else if(tokens[1].startsWith("nov",Qt::CaseInsensitive)) {
			m = 11;
		} else if(tokens[1].startsWith("dec",Qt::CaseInsensitive)) {
			m = 12;
		} else {
			return QDateTime();
		}
		if(tokens.size() < 4) return QDateTime(QDate(y,m,d),QTime(0,0,0,0),QTimeZone::systemTimeZone());
		QStringList time = tokens[3].split(QChar(':'),Qt::SkipEmptyParts);
		if(time.size() < 2) return QDateTime();
		int hr = time[0].toInt(&okay);
		if(!okay) return QDateTime();
		int min = time[1].toInt(&okay);
		if(!okay) return QDateTime();
		return QDateTime(QDate(y,m,d),QTime(hr,min,0,0),QTimeZone::systemTimeZone());
	}
	case Litphoria: // 1/20/2016 1:05:49 PM
	{
		QStringList tokens = this->date.toString().replace(QStringLiteral("«"),QChar(' '))
				.replace(QStringLiteral("»"),QChar(' ')).replace(QChar('/'),QChar(' ')).split(QChar(' '),Qt::SkipEmptyParts);
		if(tokens.size() < 5) return QDateTime();
		bool okay = true;
		int m = tokens[0].toInt(&okay);
		if(!okay) return QDateTime();
		int d = tokens[1].toInt(&okay);
		if(!okay) return QDateTime();
		int y = tokens[2].toInt(&okay);
		if(!okay) return QDateTime();
		QTime time = QTime::fromString(tokens[3]);
		if(!time.isValid()) return QDateTime(QDate(y,m,d),QTime(0,0,0,0),QTimeZone::systemTimeZone());
		if(!tokens[4].compare(QStringLiteral("pm"),Qt::CaseInsensitive)) time.addSecs(60 * 60 * 12);
		return QDateTime(QDate(y,m,d),time,QTimeZone::systemTimeZone());
	}
	case QtHun: // Szo márc. 12 19:01:45 2022
	{
		QStringList tokens = this->date.toString().replace(QStringLiteral("."),QChar(' ')).replace(QStringLiteral(":"),QChar(' ')).split(QChar(' '),Qt::SkipEmptyParts);
		if(tokens.size() < 7) return QDateTime();
		bool okay = true;
		// Day of Week = 0
		// Month = 1
		int m = 0;
		if(tokens[1].startsWith("jan",Qt::CaseInsensitive)) {
			m = 1;
		} else if(tokens[1].startsWith("feb",Qt::CaseInsensitive)) {
			m = 2;
		} else if(tokens[1].startsWith("már",Qt::CaseInsensitive)) {
			m = 3;
		} else if(tokens[1].startsWith("ápr",Qt::CaseInsensitive)) {
			m = 4;
		} else if(tokens[1].startsWith("máj",Qt::CaseInsensitive)) {
			m = 5;
		} else if(tokens[1].startsWith("jún",Qt::CaseInsensitive)) {
			m = 6;
		} else if(tokens[1].startsWith("júl",Qt::CaseInsensitive)) {
			m = 7;
		} else if(tokens[1].startsWith("aug",Qt::CaseInsensitive)) {
			m = 8;
		} else if(tokens[1].startsWith("sze",Qt::CaseInsensitive)) {
			m = 9;
		} else if(tokens[1].startsWith("okt",Qt::CaseInsensitive)) {
			m = 10;
		} else if(tokens[1].startsWith("nov",Qt::CaseInsensitive)) {
			m = 11;
		} else if(tokens[1].startsWith("dec",Qt::CaseInsensitive)) {
			m = 12;
		} else {
			return QDateTime();
		}
		// Day of Month = 2
		int dayOfMonth = tokens[2].toInt(&okay);
		if(!okay) return QDateTime();
		// Hour = 3
		int hour = tokens[3].toInt(&okay);
		if(!okay) return QDateTime();
		// Minute = 4
		int minute = tokens[4].toInt(&okay);
		if(!okay) return QDateTime();
		// Second = 5
		int second = tokens[5].toInt(&okay);
		if(!okay) return QDateTime();
		// Year = 6
		int year = tokens[6].toInt(&okay);
		if(okay) return QDateTime(QDate(year,m,dayOfMonth),QTime(hour,minute,second));
		else return QDateTime();
	}
	case QtRegular:
	{
		return QDateTime::fromString(this->date.toString());
	}
	case Regular:
	{
		return QDateTime::fromString(this->date.toString(), QStringLiteral("yyyy/MM/dd hh:mm:ss"));
	}
	case Dotted:
	{
		return QDateTime::fromString(this->date.toString(), QStringLiteral("yyyy. MM. dd. hh:mm:ss"));
	}
	case SillyTavern1:
	{
		return QDateTime::fromString(this->date.toString(),dateFormat1);
	}
	case SillyTavern2:
	{
		return QDateTime::fromString(this->date.toString(),dateFormat2);
	}
	case SillyTavern3:
	{
		return AmericanLocale.toDateTime(this->date.toString(),dateFormat3);
	}
	default:
		return QDateTime();
	}
}

bool RpgLog::getHun() const
{
	return hun;
}

void RpgLog::setHun(bool newHun)
{
	hun = newHun;
}

RpgLog::RpgLog()
{
	streamlinedDate = false;
	hun = false;
}

RpgLog::RpgLog(const QString& user, const QString& date, const QString& content)
	: user(user), date(date), content(content), streamlinedDate(false), hun(false)
{

}

RpgLog::RpgLog(QString&& user, QString&& date, QString&& content)
	: user(std::move(user)), date(std::move(date)), content(std::move(content)), streamlinedDate(false), hun(false)
{

}

RpgLog::RpgLog(RpgLog&& mov)
	: user(std::move(mov.user)), date(std::move(mov.date)), content(std::move(mov.content)), streamlinedDate(mov.streamlinedDate), hun(mov.hun)
{

}

RpgLog::RpgLog(const RpgLog& cpy)
	: user(cpy.user), date(cpy.date), content(cpy.content), streamlinedDate(cpy.streamlinedDate), hun(cpy.hun)
{

}

RpgLog& RpgLog::operator=(RpgLog&& mov)
{
	this->user = std::move(mov.user);
	this->date = std::move(mov.date);
	this->content = std::move(mov.content);
	this->streamlinedDate = mov.streamlinedDate;
	this->hun = mov.hun;
	return *this;
}
RpgLog& RpgLog::operator=(const RpgLog& cpy)
{
	this->user = cpy.user;
	this->date = cpy.date;
	this->content = cpy.content;
	this->streamlinedDate = cpy.streamlinedDate;
	this->hun = cpy.hun;
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
