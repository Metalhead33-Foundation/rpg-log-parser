#ifndef RPGPOSTTEMPLATE_HPP
#define RPGPOSTTEMPLATE_HPP
#include <QString>
#include <QJsonObject>
#include <QTextStream>
#include <QMap>
#include <QDirIterator>

class RpgPostTemplate
{
private:
	QString character_name;
	QString characterpic;
	bool isFromFlist;
public:
	RpgPostTemplate();
	const QString& getCharacter_name() const;
	void setCharacter_name(const QString& newCharacter_name);
	const QString& getCharacterpic() const;
	void setCharacterpic(const QString& newCharacterpic);
	bool getIsFromFlist() const;
	void setIsFromFlist(bool newIsFromFlist);
	// Json transforms
	void toJson(QJsonObject& json) const;
	QJsonObject toJson(void) const;
	void fromJson(const QJsonObject& json);
	// String transforms
	void toString(QTextStream& stream) const;
	QString toString() const;
	void fromString(QTextStream& stream);
	void fromString(QString str);
	// Static shit
	static QMap<QString,RpgPostTemplate> mapTemplates(QDirIterator& dirIn);
	static QMap<QString,RpgPostTemplate> mapTemplates(const QString& dirIn);
	static QJsonObject mapTemplates2JSON(QDirIterator& dirIn);
	static QJsonObject mapTemplates2JSON(const QString& dirIn);

};
QTextStream& operator<<(QTextStream& left, const RpgPostTemplate& right);
QTextStream& operator>>(QTextStream& left, RpgPostTemplate& right);

#endif // RPGPOSTTEMPLATE_HPP
