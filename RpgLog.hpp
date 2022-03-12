#ifndef RPGLOG_HPP
#define RPGLOG_HPP
#include <QString>
#include <QJsonObject>
#include <QTextStream>
#include <QLocale>
#include <QList>
#include <QVariant>

class RpgLog
{
private:
	QString user;
	QVariant date;
	QString content;
	bool streamlinedDate;
public:
	RpgLog();
	RpgLog(const QString& user, const QString& date, const QString& content);
	RpgLog(QString&& user, QString&& date, QString&& content);
	RpgLog(RpgLog&& mov);
	RpgLog(const RpgLog& cpy);
	RpgLog& operator=(RpgLog&& mov);
	RpgLog& operator=(const RpgLog& cpy);
	const QString& getUser() const;
	void setUser(const QString& newUser);
	const QVariant& getDate() const;
	void setDate(const QVariant& newDate);
	const QString& getContent() const;
	void setContent(const QString& newContent);
	// Json transforms
	void toJson(QJsonObject& json) const;
	QJsonObject toJson(void) const;
	void fromJson(const QJsonObject& json);
	// String transforms
	void toString(QTextStream& stream) const;
	QString toString() const;
	void fromString(QTextStream& stream);
	void fromString(QString str);
	// Streamlining date?
	void attemptToStreamlineDate(const QList<QLocale>& localeList);
	void attemptToStreamlineDate();
};

QTextStream& operator<<(QTextStream& left, const RpgLog& right);
QTextStream& operator>>(QTextStream& left, RpgLog& right);

#endif // RPGLOG_HPP
