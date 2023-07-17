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
	bool hun;
public:
	enum DateFormat {
		Pidgin = 0,
		ProBoards,
		PhpBB2,
		PhpBB2_Alt,
		AoB,
		FListPriv,
		Litphoria,
		QtHun,
		QtRegular,
		Regular,
		Dotted,
		SillyTavern1,
		SillyTavern2,
		SillyTavern3,
		Invalid
	};
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
	bool getHun() const;
	void setHun(bool newHun);
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
	void attemptToStreamlineDate();
	QDateTime attemptToStreamlineDate(DateFormat format);
};

QTextStream& operator<<(QTextStream& left, const RpgLog& right);
QTextStream& operator>>(QTextStream& left, RpgLog& right);

#endif // RPGLOG_HPP
