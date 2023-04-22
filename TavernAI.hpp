#ifndef TAVERNAI_HPP
#define TAVERNAI_HPP
#include <QString>
#include <QJsonObject>
#include <QList>
#include <QTextStream>

class RpgSession;
struct TavernAiConversationHeader
{
	QString user_name;
	QString character_name;
	qint64 create_date;
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
};
struct TavernAiConversationMsg
{
	QString name;
	bool is_user;
	bool is_name;
	qint64 send_date;
	QString mes;
	int chid;
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
};

class TavernAiConversation
{
private:
	TavernAiConversationHeader header;
	QList<TavernAiConversationMsg> messages;
public:
	TavernAiConversation();
	TavernAiConversationHeader& getHeader();
	const TavernAiConversationHeader& getHeader() const;
	QList<TavernAiConversationMsg>& getMessages();
	const QList<TavernAiConversationMsg>& getMessages() const;
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	void toJsonL(QTextStream& strm) const;
	QString toJsonL() const;
	void fromJsonL(const QStringList& strlist);
	void fromJsonL(const QString& str);
	void fromJsonL(QTextStream& strm);
	void fromRpgSession(const RpgSession& session, const QString& playerName = "Stephanus Tavilrond", int chid = 6);
	void toRpgSession(RpgSession& session) const;
};


#endif // TAVERNAI_HPP
