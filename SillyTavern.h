#ifndef SILLYTAVERN_H
#define SILLYTAVERN_H
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QList>
namespace SillyTavern {
// Struct for chat metadata objective

struct Extra {
	QString api;
	QString model;
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	static Extra createFromJson(const QJsonObject& json);
};

struct Objective {
	QString objective;
	QStringList tasks;
	QString checkFrequency;
	QString chatDepth;
	bool hideTasks;
	// JSON
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	static Objective createFromJson(const QJsonObject& json);
};

// Struct for chat metadata
struct ChatMetadata {
	QString note_prompt;
	int note_interval;
	int note_position;
	int note_depth;
	Objective objective;
	// JSON
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	static ChatMetadata createFromJson(const QJsonObject& json);
};

// Struct for the user
struct Header {
	QString user_name;
	QString character_name;
	QDateTime create_date;
	ChatMetadata chat_metadata;
	// JSON
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	static Header createFromJson(const QJsonObject& json);
};

// Struct for swipe info
struct SwipeInfo {
	QDateTime send_date;
	QDateTime gen_started;
	QDateTime gen_finished;
	Extra extra;
	// JSON
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	static SwipeInfo createFromJson(const QJsonObject& json);
};

// Struct for swipes
struct Message {
	Extra extra;
	QString name;
	bool is_user;
	bool is_name;
	QString force_avatar;
	QDateTime send_date;
	QString mes;
	QDateTime gen_started;
	QDateTime gen_finished;
	QList<SwipeInfo> swipe_info;
	int swipe_id;
	QStringList swipes;
	// JSON
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	static Message createFromJson(const QJsonObject& json);
};
}

class RpgSession;
class SillyTavernConversaiton
{
private:
	SillyTavern::Header header;
	QList<SillyTavern::Message> messages;
public:
	SillyTavernConversaiton();
	const SillyTavern::Header& getHeader() const;
	SillyTavern::Header& getHeader();
	const QList<SillyTavern::Message>& getMessages() const;
	QList<SillyTavern::Message>& getMessages();
	// JSON and other stuff
	void toJson(QJsonObject& json) const;
	QJsonObject toJson() const;
	void fromJson(const QJsonObject& json);
	void toJsonL(QTextStream& strm) const;
	QString toJsonL() const;
	void fromJsonL(const QStringList& strlist);
	void fromJsonL(const QString& str);
	void fromJsonL(QTextStream& strm);
	void fromRpgSession(const RpgSession& session, const QString& playerName = "Stephanus Tavilrond", const QString forceAvatarForPlayer = "User Avatars/1689330632925.png", bool rawMode = false);
	void toRpgSession(RpgSession& session) const;
};

#endif // SILLYTAVERN_H
