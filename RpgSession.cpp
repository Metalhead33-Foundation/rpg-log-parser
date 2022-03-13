#include "RpgSession.hpp"

const QList<RpgSection>& RpgSession::getSections() const
{
	return sections;
}

QList<RpgSection>& RpgSession::getSections()
{
	return sections;
}

void RpgSession::setSections(const QList<RpgSection>& newSections)
{
	sections = newSections;
}

void RpgSession::toJson(QJsonArray& json) const
{
	for(const auto& it : sections) {
		if(!it.getLogs().isEmpty()) json.push_back(it.toJson());
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
		RpgSection tmp;
		tmp.fromJson(it.toObject());
		sections.push_back(std::move(tmp));
	}
}

void RpgSession::toString(QTextStream& stream) const
{
	for(const auto& it : sections) {
		stream << it;
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
		if(lastRead.startsWith(QStringLiteral("==")) && lastRead.endsWith(QStringLiteral("==")) ) {
			if(accumulator.startsWith(QStringLiteral("=="))) {
				RpgSection sect;
				sect.fromString(accumulator.replace(QStringLiteral("{{Str}}"),QStringLiteral("<nowiki>*</nowiki>")));
				sect.streamlineDates();
				sections.push_back(std::move(sect));
			}
			accumulator.clear();
			accumulator = lastRead.append(QChar('\n'));
		} else {
			accumulator = accumulator.append(lastRead).append(QChar('\n'));
		}
	}
	if(accumulator.startsWith(QStringLiteral("=="))) {
		RpgSection sect;
		sect.fromString(accumulator.replace(QStringLiteral("{{Str}}"),QStringLiteral("<nowiki>*</nowiki>")));
		sect.streamlineDates();
		sections.push_back(std::move(sect));
	}
}

void RpgSession::fromString(QString str)
{
	QString tmp;
	QTextStream stream(&tmp, QIODevice::ReadWrite);
	fromString(stream);
}

RpgSession::RpgSession()
{

}
QTextStream& operator<<(QTextStream& left, const RpgSession& right) {
	right.toString(left);
	return left;
}
QTextStream& operator>>(QTextStream& left, RpgSession& right) {
	right.fromString(left);
	return left;
}
