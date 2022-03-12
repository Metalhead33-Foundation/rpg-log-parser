#include "RpgSession.hpp"
#include <QLocale>

const QList<RpgLog>& RpgSession::getLogs() const
{
	return logs;
}

QList<RpgLog>& RpgSession::getLogs()
{
	return logs;
}

void RpgSession::setLogs(const QList<RpgLog>& newLogs)
{
	logs = newLogs;
}

void RpgSession::toJson(QJsonArray& json) const
{
	for(const auto& it : logs) {
		json.push_back(it.toJson());
	}
}

QJsonArray RpgSession::toJson() const
{
	QJsonArray tmp;
	toJson(tmp);
	return tmp;
}

void RpgSession::fromJson(const QJsonArray& json)
{
	for(const auto& it : json) {
		RpgLog tmp;
		tmp.fromJson(it.toObject());
		logs.push_back(std::move(tmp));
	}
}

void RpgSession::toString(QTextStream& stream) const
{
	for(const auto& it : logs) {
		stream << it << QChar('\n');
	}
}

QString RpgSession::toString() const
{
	QString tmp;
	QTextStream stream(&tmp, QIODevice::ReadWrite);
	toString(stream);
	return tmp;
}

void RpgSession::fromString(QTextStream& stream)
{
	QString accumulator;
	QString lastRead;
	while(stream.readLineInto(&lastRead)) {
		if(lastRead.startsWith(QStringLiteral("{{RPG Post/"))) {
			accumulator = lastRead.append(QChar('\n'));
		} else if(lastRead.endsWith(QStringLiteral("}}")) && lastRead.size() == 2) {
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

void RpgSession::fromString(QString str)
{
	QTextStream stream(&str, QIODevice::ReadOnly);
	fromString(stream);
}

void RpgSession::streamlineDates()
{
	/*QList<QLocale> locales;
	for(int i = 0; i < 370 ; ++i) {
		locales.push_back(QLocale(QLocale::Language(i)));
	}*/
	for(auto& it : logs) {
		it.attemptToStreamlineDate();
	}
}

RpgSession::RpgSession()
{

}
