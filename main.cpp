#include <QApplication>
#include <QFile>
#include <iostream>
#include <exception>
#include <QJsonDocument>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include "RpgPostTemplate.hpp"
#include "RpgSession.hpp"
#include "QFileException.hpp"

void text2json(QIODevice& in, QIODevice& out) {
	RpgSession sess;
	QTextStream strem(&in);
	sess.fromString(strem);
	QJsonDocument doc(sess.toJson());
	out.write(doc.toJson());
}
static bool windowMode = false;

int handle(const QMap<QString,QString>& args);
void handleException(const std::exception& e);
int consoleTextlog(const QMap<QString,QString>& args);
int consoleTextlogDir(const QMap<QString,QString>& args);
int consoleJsonlog(const QMap<QString,QString>& args);
int consoleJsonlogDir(const QMap<QString,QString>& args);
int consoleTemplateJsonDir(const QMap<QString,QString>& args);
int consoleDateParseTest(const QMap<QString,QString>& args);

static const QString ARG_MODE = QStringLiteral("--mode");
static const QString ARG_IN = QStringLiteral("--in");
static const QString ARG_OUT = QStringLiteral("--out");

static const QString MODE_TEXTLOG2JSON = QStringLiteral("textlog2json");
static const QString MODE_TEXTLOG2JSON_DIR = QStringLiteral("textlog2jsonDIR");
static const QString MODE_JSON2TEXTLOG = QStringLiteral("json2textlog");
static const QString MODE_JSON2TEXTLOG_DIR = QStringLiteral("json2textlogDIR");
static const QString MODE_TEMPLATE2JSON_DIR = QStringLiteral("template2jsonDIR");
static const QString MODE_DATEPARSETEST = QStringLiteral("dateparsetest");

static const QStringList MODES = { MODE_TEXTLOG2JSON, MODE_TEXTLOG2JSON_DIR, MODE_JSON2TEXTLOG, MODE_JSON2TEXTLOG_DIR, MODE_TEMPLATE2JSON_DIR, MODE_DATEPARSETEST };

static QTextStream STDOUT(stdout);

int main(int argc, char *argv[])
{
	QMap<QString,QString> args;
	for(int i = 1; i < (argc - 1); i += 2) {
		args.insert(QString::fromUtf8(argv[i]).trimmed(),QString::fromUtf8(argv[i+1]).trimmed());
	}
	if(!args.contains(ARG_MODE) || !args.contains(ARG_IN)) {
		windowMode = true;
		QApplication a(argc, argv);
		args.insert(ARG_MODE,QInputDialog::getItem(nullptr,QStringLiteral("Select mode"),QStringLiteral("Mode:"),MODES,0,false));
		if(!args.contains(ARG_MODE)) {
			//args.insert(ARG_MODE,MODE_TEXTLOG2JSON);
			args.insert(ARG_MODE,QInputDialog::getItem(nullptr,QStringLiteral("Select mode"),QStringLiteral("Mode:"),MODES,0,false));
		}
		if(!args.contains(ARG_IN)) {
			windowMode = true;
			if(!args[ARG_MODE].compare(MODE_TEXTLOG2JSON,Qt::CaseInsensitive))
				args.insert(ARG_IN,QFileDialog::getOpenFileName(nullptr, QStringLiteral("Open File"),
					QString(), QStringLiteral("Text files (*.txt)")));
			else if(!args[ARG_MODE].compare(MODE_TEXTLOG2JSON_DIR,Qt::CaseInsensitive))
				args.insert(ARG_IN,QFileDialog::getExistingDirectory(nullptr, QStringLiteral("Open Directory (for reading)"),
					QString()));
			else if(!args[ARG_MODE].compare(MODE_JSON2TEXTLOG,Qt::CaseInsensitive))
				args.insert(ARG_IN,QFileDialog::getOpenFileName(nullptr, QStringLiteral("Open File"),
					QString(), QStringLiteral("JSON files (*.json)")));
			else if(!args[ARG_MODE].compare(MODE_JSON2TEXTLOG_DIR,Qt::CaseInsensitive))
				args.insert(ARG_IN,QFileDialog::getExistingDirectory(nullptr, QStringLiteral("Open Directory (for reading)"),
					QString()));
			else if(!args[ARG_MODE].compare(MODE_TEMPLATE2JSON_DIR,Qt::CaseInsensitive))
				args.insert(ARG_IN,QFileDialog::getExistingDirectory(nullptr, QStringLiteral("Open Directory (for reading)"),
					QString()));
			else if(!args[ARG_MODE].compare(MODE_DATEPARSETEST,Qt::CaseInsensitive))
				args.insert(ARG_IN,QFileDialog::getOpenFileName(nullptr, QStringLiteral("Open File"),
					QString(), QStringLiteral("Text files (*.txt)")));
		}
		if(!args.contains(ARG_OUT) && args[ARG_MODE].compare(MODE_DATEPARSETEST,Qt::CaseInsensitive)) {
			windowMode = true;
			if(!args[ARG_MODE].compare(MODE_TEXTLOG2JSON,Qt::CaseInsensitive))
				args.insert(ARG_OUT,QFileDialog::getSaveFileName(nullptr, QStringLiteral("Save File"),
					QString(), QStringLiteral("JSON files (*.json)")));
			else if(!args[ARG_MODE].compare(MODE_TEXTLOG2JSON_DIR,Qt::CaseInsensitive))
				args.insert(ARG_OUT,QFileDialog::getExistingDirectory(nullptr, QStringLiteral("Open Directory (for writing)"),
					QString()));
			else if(!args[ARG_MODE].compare(MODE_JSON2TEXTLOG,Qt::CaseInsensitive))
				args.insert(ARG_OUT,QFileDialog::getSaveFileName(nullptr, QStringLiteral("Save File"),
					QString(), QStringLiteral("Text files (*.txt)")));
			else if(!args[ARG_MODE].compare(MODE_JSON2TEXTLOG_DIR,Qt::CaseInsensitive))
				args.insert(ARG_OUT,QFileDialog::getExistingDirectory(nullptr, QStringLiteral("Open Directory (for writing)"),
					QString()));
			else if(!args[ARG_MODE].compare(MODE_TEMPLATE2JSON_DIR,Qt::CaseInsensitive))
				args.insert(ARG_OUT,QFileDialog::getSaveFileName(nullptr, QStringLiteral("Save File"),
					QString(), QStringLiteral("JSON files (*.json)")));
		}
		try {
			int result = handle(args);
			a.exit(result);
		}  catch (const std::exception& e) {
			QMessageBox::critical(nullptr,QStringLiteral("Exception encountered"),QString::fromUtf8(e.what()));
			a.exit(-1);
		}
	} else {
		return handle(args);
	}
}

int handle(const QMap<QString,QString>& args) {
	if(!args[ARG_MODE].compare(MODE_DATEPARSETEST,Qt::CaseInsensitive)) return consoleDateParseTest(args);
	else if(args.contains(ARG_IN) && args.contains(ARG_OUT)) {
		if(!args[ARG_MODE].compare(MODE_TEXTLOG2JSON,Qt::CaseInsensitive)) return consoleTextlog(args);
		else if(!args[ARG_MODE].compare(MODE_TEXTLOG2JSON_DIR,Qt::CaseInsensitive)) return consoleTextlogDir(args);
		else if(!args[ARG_MODE].compare(MODE_JSON2TEXTLOG,Qt::CaseInsensitive)) return consoleJsonlog(args);
		else if(!args[ARG_MODE].compare(MODE_JSON2TEXTLOG_DIR,Qt::CaseInsensitive)) return consoleJsonlogDir(args);
		else if(!args[ARG_MODE].compare(MODE_TEMPLATE2JSON_DIR,Qt::CaseInsensitive)) return consoleTemplateJsonDir(args);
		else {
			STDOUT << "Unsupported mode!\n";
		}
	}
	return 0;
}
void handleException(const std::exception& e) {
	if(windowMode) {
		QMessageBox::critical(nullptr,QStringLiteral("Exception encountered"),QString::fromUtf8(e.what()));
	} else {
		STDOUT << "Error encountered!\n" << e.what() << "\n";
	}
}

int consoleTextlog(const QMap<QString,QString>& args) {
	try {
	QString in = args[ARG_IN];
	QFile inF(in);
	if(!inF.open(QFile::ReadOnly)) {
		throw QFileException(in, QFile::ReadOnly);
	}
	QString out = args[ARG_OUT];
	QFile outF(out);
	if(!outF.open(QFile::WriteOnly)) {
		throw QFileException(out, QFile::WriteOnly);
	}
	text2json(inF, outF);
	inF.close();
	outF.flush();
	outF.close();
	return 0;
	}  catch (const std::exception& e) {
		handleException(e);
		return -1;
	}
}
int consoleJsonlog(const QMap<QString,QString>& args) {
	try {
	QString in = args[ARG_IN];
	QFile inF(in);
	if(!inF.open(QFile::ReadOnly)) {
		throw QFileException(in, QFile::ReadOnly);
	}
	QString out = args[ARG_OUT];
	QFile outF(out);
	if(!outF.open(QFile::WriteOnly)) {
		throw QFileException(out, QFile::WriteOnly);
	}
	QJsonDocument json = QJsonDocument::fromJson(inF.readAll());
	inF.close();
	RpgSession session;
	session.fromJson(json.array());
	QTextStream stream(&outF);
	session.toString(stream);
	stream.flush();
	outF.close();
	return 0;
	}  catch (const std::exception& e) {
		handleException(e);
		return -1;
	}
}
int consoleTextlogDir(const QMap<QString,QString>& args) {
	try {
	QDirIterator dirIn(args[ARG_IN]);
	QDir dirOut(args[ARG_OUT]);
	while(dirIn.hasNext()) {
		QString path = dirIn.next();
		QFileInfo info(path);
		QString outPath = dirOut.absoluteFilePath(QStringLiteral("%1.json").arg(info.baseName()));
		if(info.isFile()) {
		QFile inF(path);
		if(!inF.open(QFile::ReadOnly)) {
			throw QFileException(path, QFile::ReadOnly);
		}
		QFile outF(outPath);
		if(!outF.open(QFile::WriteOnly)) {
			throw QFileException(outPath, QFile::WriteOnly);
		}
		text2json(inF, outF);
		inF.close();
		outF.flush();
		outF.close();
		}
		}
	}
	catch (const std::exception& e) {
		handleException(e);
		return -1;
	}
	return 0;
}
int consoleJsonlogDir(const QMap<QString,QString>& args) {
	try {
	QDirIterator dirIn(args[ARG_IN]);
	QDir dirOut(args[ARG_OUT]);
	while(dirIn.hasNext()) {
		QString path = dirIn.next();
		QFileInfo info(path);
		QString outPath = dirOut.absoluteFilePath(QStringLiteral("%1.txt").arg(info.baseName()));
		if(info.isFile()) {
		QFile inF(path);
		if(!inF.open(QFile::ReadOnly)) {
			throw QFileException(path, QFile::ReadOnly);
		}
		QFile outF(outPath);
		if(!outF.open(QFile::WriteOnly)) {
			throw QFileException(outPath, QFile::WriteOnly);
		}
		QJsonDocument json = QJsonDocument::fromJson(inF.readAll());
		inF.close();
		RpgSession session;
		session.fromJson(json.array());
		QTextStream stream(&outF);
		session.toString(stream);
		stream.flush();
		outF.close();
		}
		return 0;
		}
	}
	catch (const std::exception& e) {
		handleException(e);
		return -1;
	}
	return 0;
}
int consoleTemplateJsonDir(const QMap<QString,QString>& args) {
	try {
	QDirIterator dirIn(args[ARG_IN]);
	QString out = args[ARG_OUT];
	QFile outF(out);
	if(!outF.open(QFile::WriteOnly)) {
		throw QFileException(out, QFile::WriteOnly);
	}
	QJsonDocument doc(RpgPostTemplate::mapTemplates2JSON(dirIn));
	outF.write(doc.toJson());
	outF.close();
	}
	catch (const std::exception& e) {
		handleException(e);
		return -1;
	}
	return 0;

}
int consoleDateParseTest(const QMap<QString,QString>& args)
{
	try {
		QString in = args[ARG_IN];
		QFile inF(in);
		if(!inF.open(QFile::ReadOnly)) {
			throw QFileException(in, QFile::ReadOnly);
		}
		RpgSession sess;
		QTextStream strem(&inF);
		sess.fromString(strem);
		QStringList unparsedDates;
		for(const auto& section : qAsConst(sess.getSections()))
		{
			for(const auto& post : qAsConst(section.getLogs()))
			{
				if(post.getDate().type() == QVariant::String) unparsedDates.push_back(post.getDate().toString());
			}
		}
		if(unparsedDates.empty())
		{
			if(windowMode) {
				QMessageBox::information(nullptr,QStringLiteral("Success!"),QStringLiteral("No unparsed dates were found."));
			} else {
				STDOUT << "No misformed, unparsed dates found!!\n";
			}
		}
		else {
			QString unparsedList = unparsedDates.join('\n');
			if(windowMode) {
				QMessageBox::warning(nullptr,QStringLiteral("Unparsed dates found"),QStringLiteral("The following dates could not be parsed:\n%1").arg(unparsedList));
			} else {
				STDOUT << "The following dates could not be parsed:\n" << unparsedList << "\n";
			}
		}
		return 0;
	}
	catch (const std::exception& e) {
		handleException(e);
		return -1;
	}
}

