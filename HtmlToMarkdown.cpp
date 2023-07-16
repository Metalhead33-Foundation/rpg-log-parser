#include "HtmlToMarkdown.hpp"
#include <QTextDocument>
#include <QList>

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
	return document.toHtml().remove(HtmlHeader).remove(HtmlFooter).remove(UnnecessaryFormatting);
}
