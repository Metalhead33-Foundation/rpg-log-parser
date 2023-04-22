#ifndef AGNAISTIC_HPP
#define AGNAISTIC_HPP
#include <QString>
#include <QUuid>
#include <QList>
#include <QJsonObject>

class RpgSession;
class AgnaisticMsg {
private:
	QString characterId;
	QString msg;
	QString handle;
	QUuid userId;
public:
	AgnaisticMsg();
	const QString& getCharacterId() const;
	void setCharacterId(const QString& newCharacterId);
	const QString& getMsg() const;
	void setMsg(const QString& newMsg);
	const QString& getHandle() const;
	void setHandle(const QString& newHandle);
	const QUuid& getUserId() const;
	void setUserId(const QUuid& newUserId);
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
};
class AgnaisticRpgScene {
private:
	QString name;
	QString greeting;
	QString sampleChat;
	QString scenario;
	QList<AgnaisticMsg> messages;
public:
	AgnaisticRpgScene();
	const QString& getName() const;
	void setName(const QString& newName);
	const QString& getGreeting() const;
	void setGreeting(const QString& newGreeting);
	const QString& getSampleChat() const;
	void setSampleChat(const QString& newSampleChat);
	const QString& getScenario() const;
	void setScenario(const QString& newScenario);
	const QList<AgnaisticMsg>& getMessages() const;
	QList<AgnaisticMsg>& getMessages();
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	void fromRpgSession(const RpgSession& session, QUuid userId, const QString& playerName = "Stephanus Tavilrond");
	void toRpgSession(RpgSession& session) const;
};

#endif // AGNAISTIC_HPP
