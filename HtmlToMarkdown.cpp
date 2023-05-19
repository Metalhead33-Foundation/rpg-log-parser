#include "HtmlToMarkdown.hpp"
#include <QTextDocument>
#include <QList>

static const QList<QString> REAL_NEWLINES = {
	QStringLiteral("</p>"), QStringLiteral("<p>"), QStringLiteral("</p >"), QStringLiteral("<p >"), QStringLiteral("<br>"), QStringLiteral("<br >"), QStringLiteral("</br>"), QStringLiteral("</br >")
};

QString htmlToMarkdown(const QString& html)
{
	QString doc = html;
	for(const auto& it : REAL_NEWLINES)
	{
		doc = doc.replace(it,QStringLiteral("\x13"));
	}
	QTextDocument document;
	document.setHtml(doc);
	doc = document.toMarkdown(QTextDocument::MarkdownFeature::MarkdownNoHTML).replace('\n',' ').replace('\x13','\n').trimmed();
	return doc;
}
