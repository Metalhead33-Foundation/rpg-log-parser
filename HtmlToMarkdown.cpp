#include "HtmlToMarkdown.hpp"
#include <QTextDocument>
#include <QList>
#include <QRegularExpression>

const QString HtmlHeader = QStringLiteral("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:\'Sans Serif\'; font-size:9pt; font-weight:400; font-style:normal;\">\n");
const QString HtmlFooter = QStringLiteral("</body></html>");
const QString UnnecessaryFormatting = QStringLiteral("style=\" margin-top:"
													 "6px; margin-bottom:6"
													 "px; margin-left:0px;"
													 " margin-right:0px; -"
													 "qt-block-indent:0; t"
													 "ext-indent:0px;\"");

static const QList<QString> REAL_NEWLINES = {
	QStringLiteral("</p>"), QStringLiteral("<p>"), QStringLiteral("</p >"), QStringLiteral("<p >"), QStringLiteral("<br>"), QStringLiteral("<br >"), QStringLiteral("</br>"), QStringLiteral("</br >")
};

QString htmlToMarkdown(const QString& html)
{
	QString doc = html;
	doc = doc.replace(QStringLiteral("<q>"),QStringLiteral("\""));
	doc = doc.replace(QStringLiteral("</q>"),QStringLiteral("\""));
	for(const auto& it : REAL_NEWLINES)
	{
		doc = doc.replace(it,QStringLiteral("\x13"));
	}
	QTextDocument document;
	document.setHtml(doc);
	doc = document.toMarkdown(QTextDocument::MarkdownFeature::MarkdownNoHTML).replace('\n',' ').replace('\x13','\n').trimmed();
	return doc;
}

QString markdownToHtml(const QString &markdown)
{
	QTextDocument document;
	document.setMarkdown(markdown);
	return document.toHtml()
			.remove(HtmlHeader)
			.remove(HtmlFooter)
			.remove(UnnecessaryFormatting)
			.replace(QRegularExpression("<a href=\"https://waysofdarkness\\.miraheze\\.org/wiki/([^\">]+)\">([^<]+)</a>"), QStringLiteral("[[\\1|\\2]]"))
			.replace(QRegularExpression("<a\\s+href=\"([^\"]+)\">([^<]+)<\\/a>"), QStringLiteral("[\\1 \\2]"))
			.replace(QRegularExpression("&quot;(.*?)&quot;"), QStringLiteral("<q>\\1</q>"));
}

QString unwikiMarkdown(const QString &markdown)
{
	QString toReturn(markdown);
	toReturn.remove(QStringLiteral("<nowiki>"));
	toReturn.remove(QStringLiteral("</nowiki>"));
	//toReturn = toReturn.replace(QRegularExpression("\\[([^\\]]+)\\s+([^\\]]+)\\]"), QStringLiteral("[\\2](\\1)"));
	//toReturn = toReturn.replace(QRegularExpression("\\[\\[([^|]+)\\|([^\\]]+)\\]\\]"), QStringLiteral("[\\2](https://waysofdarkness.miraheze.org/wiki/\\1)"));
	//toReturn = toReturn.replace(QRegularExpression("\\[\\[(.*?)\\]\\]"), QStringLiteral("[\\1](https://waysofdarkness.miraheze.org/wiki/\\1)"));
	//toReturn = toReturn.replace(QRegularExpression("\\[\\[(.*?)\\|(.*?)\\]\\]"), QStringLiteral("\5\\2\6(https://waysofdarkness.miraheze.org/wiki/\\1)"));
	//toReturn = toReturn.replace(QRegularExpression("\\[\\[(.*?)\\]\\]"), QStringLiteral("\5\\1\6(https://waysofdarkness.miraheze.org/wiki/\\1)"));
	//toReturn = toReturn.replace(QRegularExpression("\\[([^\\]]+)\\s+(.*?)\\]"), QStringLiteral("[\\2](\\1)"));
	toReturn = toReturn.replace(QRegularExpression("\\[\\[([^|]+?)\\]\\]"), QStringLiteral("\5\\1\6(https://waysofdarkness.miraheze.org/wiki/\\1)"));
	toReturn = toReturn.replace(QRegularExpression("\\[\\[([^|]+?)\\|([^\\]]+?)\\]\\]"), QStringLiteral("\5\\2\6(https://waysofdarkness.miraheze.org/wiki/\\1)"));
	toReturn = toReturn.replace(QRegularExpression("\\[([^\\]\\s]+?)\\s+([^\\]]+?)\\]"), QStringLiteral("[\\2](\\1)"));
	toReturn = toReturn.replace('\5','[');
	toReturn = toReturn.replace('\6',']');
	return toReturn;
}
