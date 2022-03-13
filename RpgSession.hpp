#ifndef RPGSESSION_HPP
#define RPGSESSION_HPP
#include "RpgSection.hpp"

class RpgSession
{
private:
	QList<RpgSection> sections;
public:
	RpgSession();
	const QList<RpgSection>& getSections() const;
	QList<RpgSection>& getSections();
	void setSections(const QList<RpgSection>& newSections);
	// JSON
	void toJson(QJsonArray& json) const;
	QJsonArray toJson(void) const;
	void fromJson(const QJsonArray& json);
	// String
	void toString(QTextStream& stream) const;
	QString toString(void) const;
	void fromString(QTextStream& stream);
	void fromString(QString str);
};
QTextStream& operator<<(QTextStream& left, const RpgSession& right);
QTextStream& operator>>(QTextStream& left, RpgSession& right);

#endif // RPGSESSION_HPP
