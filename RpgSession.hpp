#ifndef RPGSESSION_HPP
#define RPGSESSION_HPP
#include "RpgLog.hpp"
#include <QList>
#include <QJsonArray>

class RpgSession
{
private:
	QList<RpgLog> logs;
public:
	RpgSession();
	const QList<RpgLog>& getLogs() const;
	QList<RpgLog>& getLogs();
	void setLogs(const QList<RpgLog>& newLogs);
	// Json transforms
	void toJson(QJsonArray& json) const;
	QJsonArray toJson(void) const;
	void fromJson(const QJsonArray& json);
	// String transforms
	void toString(QTextStream& stream) const;
	QString toString() const;
	void fromString(QTextStream& stream);
	void fromString(QString str);

	void streamlineDates();
};

#endif // RPGSESSION_HPP
