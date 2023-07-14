#include "RpgSection.hpp"
#include <QLocale>
#include <QIODevice>

const QList<RpgLog>& RpgSection::getLogs() const
{
	return logs;
}

QList<RpgLog>& RpgSection::getLogs()
{
	return logs;
}

void RpgSection::setLogs(const QList<RpgLog>& newLogs)
{
	logs = newLogs;
}

void RpgSection::toJson(QJsonObject& json) const
{
	json[QStringLiteral("sectionName")] = this->sectionName;
	QJsonArray arr;
	for(const auto& it : logs) {
		arr.push_back(it.toJson());
	}
	json[QStringLiteral("posts")] = arr;

}

QJsonObject RpgSection::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}

void RpgSection::fromJson(const QJsonObject& json)
{
	this->sectionName = json[QStringLiteral("sectionName")].toString();
	QJsonArray arr = json[QStringLiteral("posts")].toArray();
	for(const auto& it : qAsConst(arr)) {
		RpgLog tmp;
		tmp.fromJson(it.toObject());
		logs.push_back(std::move(tmp));
	}
}

void RpgSection::toString(QTextStream& stream) const
{
	stream << QStringLiteral("== ") << sectionName << QStringLiteral(" ==") << QChar('\n');
	for(const auto& it : logs) {
		stream << it << QChar('\n');
	}
}

QString RpgSection::toString() const
{
	QString tmp;
	QTextStream stream(&tmp, QIODevice::ReadWrite);
	toString(stream);
	return tmp;
}

void RpgSection::fromString(QTextStream& stream)
{
	QString accumulator;
	QString lastRead;
	while(stream.readLineInto(&lastRead)) {
		lastRead = lastRead.trimmed();
		if(lastRead.startsWith(QStringLiteral("==")) && lastRead.endsWith(QStringLiteral("==")) ) {
			this->sectionName = lastRead.replace(QChar('='),QChar(' ')).trimmed();
		}
		else if(lastRead.startsWith(QStringLiteral("{{RPG Post/"))) {
			accumulator = lastRead.append(QChar('\n'));
		} else if(lastRead.endsWith(QStringLiteral("}}"))) {
			accumulator.append(lastRead);
			RpgLog tmp;
			tmp.fromString(accumulator);
			logs.push_back(std::move(tmp));
			accumulator.clear();
		} else {
			accumulator = accumulator.append(lastRead).append(QChar('\n'));
		}
	}
}

void RpgSection::fromString(QString str)
{
	QTextStream stream(&str, QIODevice::ReadOnly);
	fromString(stream);
}

void RpgSection::streamlineDates()
{
	/*QList<QLocale> locales;
	for(int i = 0; i < 370 ; ++i) {
		locales.push_back(QLocale(QLocale::Language(i)));
	}*/
	for(auto& it : logs) {
		it.attemptToStreamlineDate();
	}
}

const QString& RpgSection::getSectionName() const
{
	return sectionName;
}

void RpgSection::setSectionName(const QString& newSectionName)
{
	sectionName = newSectionName;
}

RpgSection::RpgSection()
{

}

RpgSection::RpgSection(RpgSection&& mov)
	: sectionName(std::move(mov.sectionName)), logs(std::move(mov.logs))
{

}

RpgSection::RpgSection(const RpgSection& cpy)
	: sectionName(cpy.sectionName), logs(cpy.logs)
{

}

RpgSection& RpgSection::operator=(RpgSection&& mov)
{
	this->sectionName = std::move(mov.sectionName);
	this->logs = std::move(mov.logs);
	return *this;
}
RpgSection& RpgSection::operator=(const RpgSection& cpy)
{
	this->sectionName = cpy.sectionName;
	this->logs = cpy.logs;
	return *this;
}
QTextStream& operator<<(QTextStream& left, const RpgSection& right) {
	right.toString(left);
	return left;
}
QTextStream& operator>>(QTextStream& left, RpgSection& right) {
	right.fromString(left);
	return left;
}
