#ifndef HTMLTOMARKDOWN_HPP
#define HTMLTOMARKDOWN_HPP
#include <QString>

QString htmlToMarkdown(const QString& html);
QString markdownToHtml(const QString& markdown);
QString unwikiMarkdown(const QString& markdown);

#endif // HTMLTOMARKDOWN_HPP
