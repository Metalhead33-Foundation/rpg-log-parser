#ifndef RPGSECTION_H
#define RPGSECTION_H
#include "RpgLog.hpp"
#include <QList>
#include <QJsonArray>

class RpgSection
{
private:
	QString sectionName;
	QList<RpgLog> logs;
public:
	RpgSection();
	RpgSection(RpgSection&& mov);
	RpgSection(const RpgSection& cpy);
	RpgSection& operator=(RpgSection&& mov);
	RpgSection& operator=(const RpgSection& cpy);

	const QList<RpgLog>& getLogs() const;
	QList<RpgLog>& getLogs();
	void setLogs(const QList<RpgLog>& newLogs);
	// Json transforms
	void toJson(QJsonObject& json) const;
	QJsonObject toJson(void) const;
	void fromJson(const QJsonObject& json);
	// String transforms
	void toString(QTextStream& stream) const;
	QString toString() const;
	void fromString(QTextStream& stream);
	void fromString(QString str);

	void streamlineDates();
	const QString& getSectionName() const;
	void setSectionName(const QString& newSectionName);
};
QTextStream& operator<<(QTextStream& left, const RpgSection& right);
QTextStream& operator>>(QTextStream& left, RpgSection& right);

#endif // RPGSECTION_H
