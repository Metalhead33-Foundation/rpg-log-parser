#include "TavernAI.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QTextStream>
#include <QTextDocument>
#include "HtmlHeader.hpp"
#include "RpgSession.hpp"
#include "HtmlToMarkdown.hpp"

void TavernAiConversationHeader::toJson(QJsonObject& json) const
{
	json[QStringLiteral("user_name")] = user_name;
	json[QStringLiteral("character_name")] = character_name;
	json[QStringLiteral("create_date")] = int(create_date);
}

QJsonObject TavernAiConversationHeader::toJson() const
{
	QJsonObject json;
	toJson(json);
	return json;
}

void TavernAiConversationHeader::fromJson(const QJsonObject& json)
{
	this->user_name = json[QStringLiteral("user_name")].toString();
	this->character_name = json[QStringLiteral("character_name")].toString();
	this->create_date = json[QStringLiteral("create_date")].toInt();
}

void TavernAiConversationMsg::toJson(QJsonObject& json) const
{
	json[QStringLiteral("name")] = name;
	json[QStringLiteral("is_user")] = is_user;
	json[QStringLiteral("is_name")] = is_name;
	json[QStringLiteral("send_date")] = double(send_date);
	json[QStringLiteral("mes")] = mes;
	if(chid > 0) json[QStringLiteral("chid")] = chid;
}

QJsonObject TavernAiConversationMsg::toJson() const
{
	QJsonObject json;
	toJson(json);
	return json;
}

void TavernAiConversationMsg::fromJson(const QJsonObject& json)
{
	this->name = json[QStringLiteral("name")].toString();
	this->is_user = json[QStringLiteral("is_user")].toBool();
	this->is_name = json[QStringLiteral("is_name")].toBool();
	this->send_date = json[QStringLiteral("send_date")].toDouble();
	this->mes = json[QStringLiteral("mes")].toString();
	this->chid = json[QStringLiteral("chid")].toInt(-1);
}

const QList<TavernAiConversationMsg>& TavernAiConversation::getMessages() const
{
	return messages;
}

void TavernAiConversation::toJson(QJsonObject& json) const
{
	json[QStringLiteral("header")] = header.toJson();
	QJsonArray arr;
	for(const auto& it : qAsConst(messages))
	{
		arr.push_back(it.toJson());
	}
	json[QStringLiteral("messages")] = arr;
}

QJsonObject TavernAiConversation::toJson() const
{
	QJsonObject json;
	toJson(json);
	return json;
}

void TavernAiConversation::fromJson(const QJsonObject& json)
{
	header.fromJson(json[QStringLiteral("header")].toObject());
	QJsonArray arr = json[QStringLiteral("messages")].toArray();
	for(const auto& it : qAsConst(arr))
	{
		TavernAiConversationMsg msg;
		msg.fromJson(it.toObject());
		messages.push_back(msg);
	}
}

void TavernAiConversation::toJsonL(QTextStream& strm) const
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

QString TavernAiConversation::toJsonL() const
{
	QString str;
	{
		QTextStream strm(&str);
		toJsonL(strm);
	}
	if(str.endsWith('\n')) str.chop(1);
	return str;
}

void TavernAiConversation::fromJsonL(const QStringList& strlist)
{
	if(strlist.empty()) return;
	{
		QJsonDocument hdr = QJsonDocument::fromJson(strlist[0].toUtf8());
		header.fromJson(hdr.object());
	}
	for(int i = 1; i < strlist.length(); ++i)
	{
		QJsonDocument doc = QJsonDocument::fromJson(strlist[i].toUtf8());
		TavernAiConversationMsg msg;
		msg.fromJson(doc.object());
		messages.push_back(msg);
	}
}

void TavernAiConversation::fromJsonL(const QString& str)
{
	fromJsonL(str.split('\n',Qt::SkipEmptyParts));
}

void TavernAiConversation::fromJsonL(QTextStream& strm)
{
	QStringList lst;
	while(!strm.atEnd())
	{
		lst.push_back(strm.readLine());
	}
	fromJsonL(lst);
}

void TavernAiConversation::fromRpgSession(const RpgSession& session, const QString& playerName, int chid)
{
	for(const auto& section : session.getSections() )
	{
		for(const auto& post : section.getLogs())
		{
			TavernAiConversationMsg msg;
			msg.name = post.getUser();
			if(post.getUser() == playerName)
			{
				header.user_name = msg.name;
				msg.is_user = true;
				msg.is_name = false;
			}
			else
			{
				header.character_name = msg.name;
				msg.is_user = false;
				msg.is_name = true;
			}
			msg.send_date = post.getDate().toDateTime().toUTC().toMSecsSinceEpoch();
			msg.chid = msg.is_name ? chid : -1;
			msg.mes = htmlToMarkdown(post.getContent());
			messages.push_back(msg);
		}
	}
	if(!messages.empty())
	{
		header.create_date = messages[0].send_date;
	}
}



void TavernAiConversation::toRpgSession(RpgSession& session) const
{
	QTextDocument document;
	document.setTextWidth(-1);
	RpgSection section;
	section.setSectionName("AI scene");
	for(const auto& it : qAsConst(messages))
	{
		RpgLog log;
		log.setHun(false);
		log.setUser(it.name);
		log.setDate(QDateTime::fromMSecsSinceEpoch(it.send_date));
		document.setMarkdown(it.mes);
		document.setTextWidth(-1);
		log.setContent(document.toHtml().remove(HtmlHeader).remove(HtmlFooter).remove(UnnecessaryFormatting));
		section.getLogs().push_back(log);
	}
	session.getSections().clear();
	session.getSections().push_back(section);
}

TavernAiConversation::TavernAiConversation()
{

}

TavernAiConversationHeader& TavernAiConversation::getHeader()
{
	return header;
}

const TavernAiConversationHeader& TavernAiConversation::getHeader() const
{
	return header;
}

QList<TavernAiConversationMsg>& TavernAiConversation::getMessages()
{
	return messages;
}
