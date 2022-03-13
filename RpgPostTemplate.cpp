#include "RpgPostTemplate.hpp"

const QString& RpgPostTemplate::getCharacter_name() const
{
	return character_name;
}

void RpgPostTemplate::setCharacter_name(const QString& newCharacter_name)
{
	character_name = newCharacter_name;
}

const QString& RpgPostTemplate::getCharacterpic() const
{
	return characterpic;
}

void RpgPostTemplate::setCharacterpic(const QString& newCharacterpic)
{
	characterpic = newCharacterpic;
}

bool RpgPostTemplate::getIsFromFlist() const
{
			  return isFromFlist;
}

 void RpgPostTemplate::setIsFromFlist(bool newIsFromFlist)
{
			  isFromFlist = newIsFromFlist;
}

void RpgPostTemplate::toJson(QJsonObject& json) const
{
	json[QStringLiteral("character_name")] = this->character_name;
	json[QStringLiteral("characterpic")] = this->characterpic;
	json[QStringLiteral("isFromFlist")] = this->isFromFlist;
}

QJsonObject RpgPostTemplate::toJson() const
{
	QJsonObject tmp;
	toJson(tmp);
	return tmp;
}

void RpgPostTemplate::fromJson(const QJsonObject& json)
{
	this->character_name = json[QStringLiteral("character_name")].toString();
	this->characterpic = json[QStringLiteral("characterpic")].toString();
	this->isFromFlist = json[QStringLiteral("isFromFlist")].toBool();
}

static const QString TEMPLATE_INTRO = QStringLiteral("{{RPG Post");
static const QString TEMPLATE_DATE = QStringLiteral("|date={{{date|}}}");
static const QString TEMPLATE_INTRO_PLUS_DATE = QStringLiteral("{{RPG Post\n|date={{{date|}}}");
static const QString TEMPLATE_CHARNAME = QStringLiteral("|character_name=");
static const QString TEMPLATE_CHARPIC = QStringLiteral("|characterpic=");
static const QString TEMPLATE_OUTRO = QStringLiteral("|post={{{post|}}}}}<noinclude>[[Category:RPG Post Templates]]</noinclude>");
static const QString TEMPLATE_OUTRO_ALT = QStringLiteral("|post={{{post|}}}}}<noinclude>[[Category:F-List Archive Templates]]</noinclude>");

void RpgPostTemplate::toString(QTextStream& stream) const
{
	stream << TEMPLATE_INTRO_PLUS_DATE << '\n' << TEMPLATE_CHARNAME << character_name <<
			  '\n' << TEMPLATE_CHARPIC << characterpic << '\n' << (isFromFlist ? TEMPLATE_OUTRO_ALT : TEMPLATE_OUTRO);
}

QString RpgPostTemplate::toString() const
{
	QString temp;
	QTextStream stream(&temp, QIODevice::ReadWrite);
	toString(stream);
	return temp;
}

void RpgPostTemplate::fromString(QTextStream& stream)
{
	QString lastRead;
	while(stream.readLineInto(&lastRead)) {
		if(lastRead.startsWith(TEMPLATE_CHARNAME)) {
			this->character_name = lastRead.mid(TEMPLATE_CHARNAME.size());
		} else if(lastRead.startsWith(TEMPLATE_CHARPIC)) {
			this->characterpic = lastRead.mid(TEMPLATE_CHARPIC.size());
		} else if(lastRead.endsWith(TEMPLATE_OUTRO)) {
			this->isFromFlist = false;
			return;
		} else if(lastRead.endsWith(TEMPLATE_OUTRO_ALT)) {
			this->isFromFlist = true;
			return;
		}
	}
}

void RpgPostTemplate::fromString(QString str)
{
	QTextStream stream(&str, QIODevice::ReadOnly);
	fromString(stream);
}

QMap<QString, RpgPostTemplate> RpgPostTemplate::mapTemplates(QDirIterator& dirIn)
{
	QMap<QString, RpgPostTemplate> tmp;
	while(dirIn.hasNext()) {
		QString path = dirIn.next();
		QFileInfo info(path);
		QFile file(path);
		if(file.open(QFile::ReadOnly | QFile::Text)) {
			QTextStream stream1(&file);
			RpgPostTemplate templated;
			templated.fromString(stream1);
			tmp.insert(info.fileName(),std::move(templated));
		}
	}
	return tmp;
}

QMap<QString, RpgPostTemplate> RpgPostTemplate::mapTemplates(const QString& dirIn)
{
	QDirIterator dirin(dirIn);
	return mapTemplates(dirin);
}

QJsonObject RpgPostTemplate::mapTemplates2JSON(QDirIterator& dirIn)
{
	QJsonObject tmp;
	while(dirIn.hasNext()) {
		QString path = dirIn.next();
		QFileInfo info(path);
		QFile file(path);
		if(file.open(QFile::ReadOnly | QFile::Text)) {
			QTextStream stream1(&file);
			RpgPostTemplate templated;
			templated.fromString(stream1);
			tmp.insert(info.fileName(),templated.toJson());
		}
	}
	return tmp;
}

QJsonObject RpgPostTemplate::mapTemplates2JSON(const QString& dirIn)
{
	QDirIterator dirin(dirIn);
	return mapTemplates2JSON(dirin);
}

RpgPostTemplate::RpgPostTemplate() : isFromFlist(false)
{

}
QTextStream& operator<<(QTextStream& left, const RpgPostTemplate& right) {
	right.toString(left);
	return left;
}
QTextStream& operator>>(QTextStream& left, RpgPostTemplate& right) {
	right.fromString(left);
	return left;
}
