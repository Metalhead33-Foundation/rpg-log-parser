#include "SillyTavern.h"
#include "RpgSession.hpp"
#include "HtmlToMarkdown.hpp"
#include <QTextDocument>

namespace SillyTavern {
/*
	QString objective;
	QStringList tasks;
	QString checkFrequency;
	QString chatDepth;
	bool hideTasks;
*/
void Objective::toJson(QJsonObject &json) const
{
	json[QStringLiteral("objective")] = this->objective;
	json[QStringLiteral("checkFrequency")] = this->checkFrequency;
	json[QStringLiteral("chatDepth")] = this->chatDepth;
	json[QStringLiteral("hideTasks")] = this->hideTasks;
	QJsonArray tasks;
	for(const auto& it : qAsConst(this->tasks))
	{
		tasks.push_back(it);
	}
	json[QStringLiteral("tasks")] = tasks;
}

QJsonObject Objective::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}

void Objective::fromJson(const QJsonObject &json)
{
	this->objective = json[QStringLiteral("objective")].toString();
	this->checkFrequency = json[QStringLiteral("checkFrequency")].toString();
	this->chatDepth = json[QStringLiteral("chatDepth")].toString();
	this->hideTasks = json[QStringLiteral("hideTasks")].toBool();
	auto tasks = json[QStringLiteral("tasks")].toArray();
	this->tasks.clear();
	for(const auto& it : qAsConst(tasks))
	{
		this->tasks.push_back(it.toString());
	}
}

Objective Objective::createFromJson(const QJsonObject &json)
{
	Objective tmp;
	tmp.fromJson(json);
	return tmp;
}
/*
	QString note_prompt;
	int note_interval;
	int note_position;
	int note_depth;
	Objective objective;
*/
void ChatMetadata::toJson(QJsonObject &json) const
{
	json[QStringLiteral("note_prompt")] = this->note_prompt;
	json[QStringLiteral("note_interval")] = this->note_interval;
	json[QStringLiteral("note_position")] = this->note_position;
	json[QStringLiteral("note_depth")] = this->note_depth;
	json[QStringLiteral("objective")] = this->objective.toJson();
}

QJsonObject ChatMetadata::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}
void ChatMetadata::fromJson(const QJsonObject &json)
{
	this->note_depth = json[QStringLiteral("note_depth")].toInt();
	this->note_position = json[QStringLiteral("note_position")].toInt();
	this->note_interval = json[QStringLiteral("note_interval")].toInt();
	this->note_prompt = json[QStringLiteral("note_prompt")].toString();
	this->objective.fromJson(json[QStringLiteral("objective")].toObject());
}

ChatMetadata ChatMetadata::createFromJson(const QJsonObject &json)
{
	ChatMetadata tmp;
	tmp.fromJson(json);
	return tmp;
}
/*
	QString user_name;
	QString character_name;
	QString create_date;
	ChatMetadata chat_metadata;
*/
static const QString dateFormat1 = QStringLiteral("yyyy-M-d @hh'h' mm'm' ss's' zzz'ms'");
void Header::toJson(QJsonObject &json) const
{
	json[QStringLiteral("user_name")] = this->user_name;
	json[QStringLiteral("character_name")] = this->character_name;
	json[QStringLiteral("create_date")] = this->create_date.toString(dateFormat1);
	json[QStringLiteral("chat_metadata")] = this->chat_metadata.toJson();
}

QJsonObject Header::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}

void Header::fromJson(const QJsonObject &json)
{
	this->user_name = json[QStringLiteral("user_name")].toString();
	this->character_name = json[QStringLiteral("character_name")].toString();
	this->create_date = QDateTime::fromString(json[QStringLiteral("create_date")].toString(),dateFormat1).toLocalTime();
	this->chat_metadata.fromJson(json[QStringLiteral("chat_metadata")].toObject());
}

Header Header::createFromJson(const QJsonObject &json)
{
	Header tmp;
	tmp.fromJson(json);
	return tmp;
}
/*
	QString send_date;
	QString gen_started;
	QString gen_finished;
*/
static const QString dateFormat2 = QStringLiteral("yyyy-MM-ddTHH:mm:ss.zzzZ");
static const QString dateFormat3 = QStringLiteral("MMMM d, yyyy h:mmap");
static const QLocale AmericanLocale = QLocale(QLocale::English,QLocale::UnitedStates);
void SwipeInfo::toJson(QJsonObject &json) const
{
	json[QStringLiteral("send_date")] = AmericanLocale.toString(this->send_date,dateFormat3);
	json[QStringLiteral("gen_started")] = this->gen_started.toString(dateFormat2);
	json[QStringLiteral("gen_finished")] = this->gen_finished.toString(dateFormat2);
}

QJsonObject SwipeInfo::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}

void SwipeInfo::fromJson(const QJsonObject &json)
{
	this->send_date = AmericanLocale.toDateTime(json[QStringLiteral("send_date")].toString(),dateFormat3);
	this->gen_started = QDateTime::fromString(json[QStringLiteral("gen_started")].toString(),dateFormat2);
	this->gen_started.setTimeSpec(Qt::UTC);
	this->gen_finished = QDateTime::fromString(json[QStringLiteral("gen_finished")].toString(),dateFormat2);
	this->gen_finished.setTimeSpec(Qt::UTC);
}

SwipeInfo SwipeInfo::createFromJson(const QJsonObject &json)
{
	SwipeInfo tmp;
	tmp.fromJson(json);
	return tmp;
}
/*
	QVariant extra;
	QString name;
	bool is_user;
	bool is_name;
	QString send_date;
	QString mes;
	QString gen_started;
	QString gen_finished;
	QList<SwipeInfo> swipe_info;
	int swipe_id;
	QStringList swipes;
*/
void Message::toJson(QJsonObject &json) const
{
	json[QStringLiteral("extra")] = QJsonValue::fromVariant(this->extra);
	json[QStringLiteral("name")] = this->name;
	json[QStringLiteral("is_user")] = this->is_user;
	json[QStringLiteral("is_name")] = this->is_name;
	json[QStringLiteral("send_date")] = AmericanLocale.toString(this->send_date,dateFormat3);
	json[QStringLiteral("force_avatar")] = this->force_avatar;
	json[QStringLiteral("mes")] = this->mes;
	json[QStringLiteral("gen_started")] = this->gen_started.toString(dateFormat2);
	json[QStringLiteral("gen_finished")] = this->gen_finished.toString(dateFormat2);
	json[QStringLiteral("swipe_id")] = this->swipe_id;
	QJsonArray swipes;
	for(const auto& it : qAsConst(this->swipes))
	{
		swipes.push_back(it);
	}
	json[QStringLiteral("swipes")] = swipes;
	QJsonArray swipe_info;
	for(const auto& it : qAsConst(this->swipe_info))
	{
		swipes.push_back(it.toJson());
	}
	json[QStringLiteral("swipe_info")] = swipe_info;
}

QJsonObject Message::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}

void Message::fromJson(const QJsonObject &json)
{
	this->extra = json[QStringLiteral("extra")].toVariant();
	this->name = json[QStringLiteral("name")].toString();
	this->is_user = json[QStringLiteral("is_user")].toBool();
	this->is_name = json[QStringLiteral("is_name")].toBool();
	this->send_date = AmericanLocale.toDateTime(json[QStringLiteral("send_date")].toString(),dateFormat3);
	this->force_avatar = json[QStringLiteral("force_avatar")].toString();
	this->mes = json[QStringLiteral("mes")].toString();
	this->gen_started = QDateTime::fromString(json[QStringLiteral("gen_started")].toString(),dateFormat2);
	this->gen_started.setTimeSpec(Qt::UTC);
	this->gen_finished = QDateTime::fromString(json[QStringLiteral("gen_finished")].toString(),dateFormat2);
	this->gen_finished.setTimeSpec(Qt::UTC);
	this->swipe_id = json[QStringLiteral("swipe_id")].toInt();
	auto swipes = json[QStringLiteral("swipes")].toArray();
	this->swipes.clear();
	for(const auto& it : qAsConst(swipes))
	{
		this->swipes.push_back(it.toString());
	}
	auto swipe_info = json[QStringLiteral("swipe_info")].toArray();
	this->swipe_info.clear();
	for(const auto& it : qAsConst(swipe_info))
	{
		this->swipe_info.push_back(SwipeInfo::createFromJson(it.toObject()));
	}
}

Message Message::createFromJson(const QJsonObject &json)
{
	Message tmp;
	tmp.fromJson(json);
	return tmp;
}
}

const QList<SillyTavern::Message> &SillyTavernConversaiton::getMessages() const
{
	return messages;
}

QList<SillyTavern::Message> &SillyTavernConversaiton::getMessages()
{
	return messages;
}

void SillyTavernConversaiton::toJson(QJsonObject &json) const
{
	json[QStringLiteral("header")] = this->header.toJson();
	QJsonArray messages;
	for(const auto& it : qAsConst(this->messages))
	{
		messages.push_back(it.toJson());
	}
	json[QStringLiteral("messages")] = messages;
}

QJsonObject SillyTavernConversaiton::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}

void SillyTavernConversaiton::fromJson(const QJsonObject &json)
{
	this->header.fromJson(json[QStringLiteral("header")].toObject());
	this->messages.clear();
	auto messages = json[QStringLiteral("messages")].toArray();
	for(const auto& it : qAsConst(messages))
	{
		this->messages.push_back(SillyTavern::Message::createFromJson(it.toObject()));
	}
}

void SillyTavernConversaiton::toJsonL(QTextStream &strm) const
{
	{
		QJsonDocument hdr = QJsonDocument(header.toJson());
		strm << hdr.toJson(QJsonDocument::Compact) << '\n';
	}
	for(const auto& it : qAsConst(messages))
	{
		QJsonDocument doc = QJsonDocument(it.toJson());
		strm << doc.toJson(QJsonDocument::Compact) << '\n';
	}
	strm.flush();
}

QString SillyTavernConversaiton::toJsonL() const
{
	QString str;
	{
		QTextStream strm(&str);
		toJsonL(strm);
	}
	if(str.endsWith('\n')) str.chop(1);
	return str;
}

void SillyTavernConversaiton::fromJsonL(const QStringList &strlist)
{
	{
		QJsonDocument hdr = QJsonDocument::fromJson(strlist[0].toUtf8());
		header.fromJson(hdr.object());
	}
	for(int i = 1; i < strlist.length(); ++i)
	{
		QJsonDocument doc = QJsonDocument::fromJson(strlist[i].toUtf8());
		this->messages.push_back(SillyTavern::Message::createFromJson(doc.object()));
	}
}

void SillyTavernConversaiton::fromJsonL(const QString &str)
{
	fromJsonL(str.split('\n',Qt::SkipEmptyParts));
}

void SillyTavernConversaiton::fromJsonL(QTextStream &strm)
{
	QStringList lst;
	while(!strm.atEnd())
	{
		lst.push_back(strm.readLine());
	}
	fromJsonL(lst);
}

void SillyTavernConversaiton::fromRpgSession(const RpgSession &session, const QString &playerName, const QString forceAvatarForPlayer, bool rawMode)
{
	int chatDepth = 0;
	header.user_name = playerName;
	// First iteration
	for(const auto& section : qAsConst(session.getSections()))
	{
		for(const auto& log : section.getLogs())
		{
			++chatDepth;
		}
	}
	header.chat_metadata.note_depth = 4;
	header.chat_metadata.note_interval = 1;
	header.chat_metadata.note_position = 1;
	header.chat_metadata.note_prompt = "";
	header.chat_metadata.objective.chatDepth = QString::number(chatDepth);
	header.chat_metadata.objective.checkFrequency = QString::number(2);
	header.chat_metadata.objective.hideTasks = false;
	for(const auto& section : qAsConst(session.getSections()))
	{
		for(const auto& log : section.getLogs())
		{
			--chatDepth;
			SillyTavern::Message msg;
			msg.mes = rawMode ? unwikiMarkdown(log.getContent()) : htmlToMarkdown(log.getContent());
			msg.name = log.getUser();
			msg.is_user = msg.name == playerName;
			msg.is_name = !msg.is_user;
			msg.force_avatar = msg.is_user ? forceAvatarForPlayer : QString();
			msg.send_date = (log.getDate().toDateTime().isValid()) ? log.getDate().toDateTime() : QDateTime::currentDateTime().addSecs(-240 * chatDepth);
			if(msg.is_name)
			{
				msg.gen_finished = msg.send_date;
				msg.gen_started = msg.send_date;
				msg.swipe_id = 0;
				msg.swipes = QStringList() << msg.mes;
				msg.swipe_info.push_back(SillyTavern::SwipeInfo{ .send_date = msg.send_date, .gen_started = msg.send_date, .gen_finished = msg.send_date });
			}
			messages.push_back(msg);
		}
	}
	if(messages.size()) header.create_date = messages[0].send_date;
}

/*
	QString user;
	QVariant date;
	QString content;
	bool streamlinedDate;
	bool hun;
*/

void SillyTavernConversaiton::toRpgSession(RpgSession &session) const
{
	QList<RpgSection> sections;
	{
		RpgSection section;
		section.setSectionName(QStringLiteral("Silly Tavern"));
		QDateTime latest;
		for(const auto& it : qAsConst(messages))
		{
			RpgLog log;
			log.setHun(false);
			log.setUser(it.name);
			log.setContent(markdownToHtml(it.mes));
			if(it.gen_finished.isValid()) {
				log.setDate(it.gen_finished.toLocalTime());
				latest = it.gen_finished.toLocalTime();
			}
			else if(it.send_date.isValid())
			{
				log.setDate(it.send_date);
			}
			if(latest.isValid())
			{
				if(latest >= log.getDate().toDateTime())
				{
					log.setDate(latest.addSecs(360));
				}
			} else latest = log.getDate().toDateTime();
			section.getLogs().push_back(log);
		}
		sections.push_back(section);
	}
	session.setSections(sections);
}

SillyTavernConversaiton::SillyTavernConversaiton()
{

}

const SillyTavern::Header &SillyTavernConversaiton::getHeader() const
{
	return header;
}

SillyTavern::Header &SillyTavernConversaiton::getHeader()
{
	return header;
}
