#include "Agnaistic.hpp"
#include <QJsonArray>
#include "RpgSession.hpp"
#include "HtmlToMarkdown.hpp"
#include "HtmlHeader.hpp"
#include <QTextDocument>

const QString& AgnaisticMsg::getMsg() const
{
	return msg;
}

void AgnaisticMsg::setMsg(const QString& newMsg)
{
	msg = newMsg;
}

const QString& AgnaisticMsg::getHandle() const
{
	return handle;
}

void AgnaisticMsg::setHandle(const QString& newHandle)
{
	handle = newHandle;
}

const QUuid& AgnaisticMsg::getUserId() const
{
	return userId;
}

void AgnaisticMsg::setUserId(const QUuid& newUserId)
{
	userId = newUserId;
}

void AgnaisticMsg::toJson(QJsonObject& json) const
{
	if(!characterId.isEmpty()) json[QStringLiteral("characterId")] = characterId;
	if(!msg.isEmpty()) json[QStringLiteral("msg")] = msg;
	if(!handle.isEmpty()) json[QStringLiteral("handle")] = handle;
	if(!userId.isNull()) json[QStringLiteral("userId")] = userId.toString(QUuid::StringFormat::WithoutBraces);
}

QJsonObject AgnaisticMsg::toJson() const
{
	QJsonObject json;
	toJson(json);
	return json;
}

void AgnaisticMsg::fromJson(const QJsonObject& json)
{
	if(json.contains(QStringLiteral("characterId")))
		characterId = json[QStringLiteral("characterId")].toString();
	if(json.contains(QStringLiteral("msg")))
		msg = json[QStringLiteral("msg")].toString();
	if(json.contains(QStringLiteral("handle")))
		handle = json[QStringLiteral("handle")].toString();
	if(json.contains(QStringLiteral("userId")))
		userId = QUuid::fromString(json[QStringLiteral("userId")].toString());
	else
		userId = QUuid();
}

AgnaisticMsg::AgnaisticMsg()
{

}

const QString& AgnaisticMsg::getCharacterId() const
{
	return characterId;
}

void AgnaisticMsg::setCharacterId(const QString& newCharacterId)
{
	characterId = newCharacterId;
}

const QString& AgnaisticRpgScene::getGreeting() const
{
	return greeting;
}

void AgnaisticRpgScene::setGreeting(const QString& newGreeting)
{
	greeting = newGreeting;
}

const QString& AgnaisticRpgScene::getSampleChat() const
{
	return sampleChat;
}

void AgnaisticRpgScene::setSampleChat(const QString& newSampleChat)
{
	sampleChat = newSampleChat;
}

const QString& AgnaisticRpgScene::getScenario() const
{
	return scenario;
}

void AgnaisticRpgScene::setScenario(const QString& newScenario)
{
	scenario = newScenario;
}

const QList<AgnaisticMsg>& AgnaisticRpgScene::getMessages() const
{
	return messages;
}

QList<AgnaisticMsg>& AgnaisticRpgScene::getMessages()
{
	return messages;
}

void AgnaisticRpgScene::toJson(QJsonObject& json) const
{
	json[QStringLiteral("name")] = name;
	json[QStringLiteral("greeting")] = greeting;
	json[QStringLiteral("sampleChat")] = sampleChat;
	json[QStringLiteral("scenario")] = scenario;
	QJsonArray arr;
	for(const auto& it : qAsConst(messages))
	{
		arr.push_back(it.toJson());
	}
	json[QStringLiteral("messages")] = arr;
}

AgnaisticRpgScene::AgnaisticRpgScene()
{

}

const QString& AgnaisticRpgScene::getName() const
{
	return name;
}

void AgnaisticRpgScene::setName(const QString& newName)
{
	name = newName;
}

QJsonObject AgnaisticRpgScene::toJson() const
{
	QJsonObject json;
	toJson(json);
	return json;
}

void AgnaisticRpgScene::fromJson(const QJsonObject& json)
{
	name = json[QStringLiteral("name")].toString();
	greeting = json[QStringLiteral("greeting")].toString();
	sampleChat = json[QStringLiteral("sampleChat")].toString();
	scenario = json[QStringLiteral("scenario")].toString();
	QJsonArray arr = json[QStringLiteral("messages")].toArray();
	for(const auto& it : qAsConst(arr))
	{
		AgnaisticMsg msg;
		msg.fromJson(it.toObject());
		messages.push_back(msg);
	}
}

void AgnaisticRpgScene::fromRpgSession(const RpgSession& session, QUuid userId, const QString& playerName)
{
	name = QStringLiteral("Exported");
	greeting = QStringLiteral("Replace this with whatever you want");
	sampleChat = QStringLiteral("Replace this with whatever you want");
	scenario = QStringLiteral("Replace this with whatever you want");
	for(const auto& section : session.getSections() )
	{
		for(const auto& post : section.getLogs())
		{
			AgnaisticMsg msg;
			const bool isPlayer = post.getUser() == playerName;
			if(isPlayer)
			{
				msg.setHandle(post.getUser());
				msg.setUserId(userId);
			}
			else
			{
				msg.setCharacterId("imported");
			}
			msg.setMsg(htmlToMarkdown(post.getContent()));
			messages.push_back(msg);
		}
	}
}

void AgnaisticRpgScene::toRpgSession(RpgSession& session) const
{
	QTextDocument document;
	RpgSection section;
	section.setSectionName(name);
	for(const auto& it : qAsConst(messages))
	{
		RpgLog log;
		log.setHun(false);
		if(!it.getCharacterId().isEmpty())
			log.setUser(it.getCharacterId());
		else
			log.setUser(it.getHandle());
		log.setDate(QDateTime::currentDateTime());
		document.setMarkdown(it.getMsg());
		log.setContent(document.toHtml().remove(HtmlHeader).remove(HtmlFooter).remove(UnnecessaryFormatting));
		section.getLogs().push_back(log);
	}
	session.getSections().clear();
	session.getSections().push_back(section);
}
