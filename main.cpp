#include "QFileException.hpp"
#include "RpgPostTemplate.hpp"
#include "RpgSession.hpp"
#include "TavernAI.hpp"
#include "Agnaistic.hpp"
#include "SillyTavern.h"
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QMessageBox>
#include <exception>
#include <functional>
#include <iostream>
#include <QTextDocument>
#include "HtmlToMarkdown.hpp"

void text2json( QIODevice &in, QIODevice &out ) {
	RpgSession sess;
	QTextStream strem( &in );
	sess.fromString( strem );
	QJsonDocument doc( sess.toJson( ) );
	out.write( doc.toJson( ) );
}
static bool windowMode = false;

typedef QMap< QString, QString > StringMap;
typedef std::function< int( const StringMap & ) > RpgHandler;

struct Mode {
	RpgHandler function;
	bool needsOutput;
	bool worksWithDirectories;
	bool needsPlayerName;
	bool needsGuid;
	bool needsChid;
	QString inPattern;
	QString outPattern;
};

int handle( QMap< QString, QString > &args );
void handleException( const std::exception &e );
int consoleTextlog( const QMap< QString, QString > &args );
int consoleTextlogDir( const QMap< QString, QString > &args );
int consoleJsonlog( const QMap< QString, QString > &args );
int consoleJsonlogDir( const QMap< QString, QString > &args );
int consoleTemplateJsonDir( const QMap< QString, QString > &args );
int consoleDateParseTest( const QMap< QString, QString > &args );
int discord2textlog( const QMap< QString, QString > &args );
int discord2json( const QMap< QString, QString > &args );
// new AI stuff
int textlog2agnaistic( const QMap< QString, QString > &args );
int json2agnaistic( const QMap< QString, QString > &args );
int agnaistic2textlog( const QMap< QString, QString > &args );
int agnaistic2json( const QMap< QString, QString > &args );
int textlog2tavernai( const QMap< QString, QString > &args );
int json2tavernai( const QMap< QString, QString > &args );
int tavernai2textlog( const QMap< QString, QString > &args );
int tavernai2json( const QMap< QString, QString > &args );
int textlog2sillytavern( const QMap< QString, QString > &args );
int json2sillytavern( const QMap< QString, QString > &args );
int sillytavern2textlog( const QMap< QString, QString > &args );
int sillytavern2json( const QMap< QString, QString > &args );
int textlog2aitemplate( const QMap< QString, QString > &args );
int json2aitemplate( const QMap< QString, QString > &args );

static const QString ARG_MODE = QStringLiteral( "--mode" );
static const QString ARG_IN = QStringLiteral( "--in" );
static const QString ARG_OUT = QStringLiteral( "--out" );
static const QString ARG_PLAYERNAME = QStringLiteral( "--playername" );
static const QString ARG_PLAYERUID = QStringLiteral( "--playeruid" );
static const QString ARG_CHID = QStringLiteral( "--chid" );

static const QString MODE_TEXTLOG2JSON = QStringLiteral( "textlog2json" );
static const QString MODE_TEXTLOG2JSON_DIR =
	QStringLiteral( "textlog2jsonDIR" );
static const QString MODE_JSON2TEXTLOG = QStringLiteral( "json2textlog" );
static const QString MODE_JSON2TEXTLOG_DIR =
	QStringLiteral( "json2textlogDIR" );
static const QString MODE_TEMPLATE2JSON_DIR =
	QStringLiteral( "template2jsonDIR" );
static const QString MODE_DATEPARSETEST = QStringLiteral( "dateparsetest" );
static const QString MODE_DISCORD2TEXTLOG = QStringLiteral( "discord2textlog" );
static const QString MODE_DISCORD2JSONLOG = QStringLiteral( "discord2jsonlog" );
// The new AI stuff
static const QString MODE_TEXTLOG2AGNAISTIC =
	QStringLiteral( "textlog2agnaistic" );
static const QString MODE_JSON2AGNAISTIC = QStringLiteral( "json2agnaistic" );
static const QString MODE_AGNAISTIC2TEXTLOG =
	QStringLiteral( "agnaistic2textlog" );
static const QString MODE_AGNAISTIC2JSON = QStringLiteral( "agnaistic2json" );
static const QString MODE_TEXTLOG2TAVERNAI =
	QStringLiteral( "textlog2tavernai" );
static const QString MODE_JSON2TAVERNAI = QStringLiteral( "json2tavernai" );
static const QString MODE_TAVERNAI2TEXTLOG =
	QStringLiteral( "tavernai2textlog" );
static const QString MODE_TAVERNAI2JSON = QStringLiteral( "tavernai2json" );
static const QString MODE_TEXTLOG2AITEMPLATE = QStringLiteral( "textlog2aitemplate" );
static const QString MODE_JSON2AITEMPLATE = QStringLiteral( "json2aitemplate" );
static const QString MODE_TEXTLOG2SILLYTAVERN =
	QStringLiteral( "textlog2sillytavern" );
static const QString MODE_JSON2SILLYTAVERN = QStringLiteral( "json2sillytavern" );
static const QString MODE_SILLYTAVERN2TEXTLOG =
	QStringLiteral( "sillytavern2textlog" );
static const QString MODE_SILLYTAVERN2JSON = QStringLiteral( "sillytavern2json" );
static const QString TEXT_FILE = QStringLiteral("Text files (*.txt)");
static const QString JSON_FILE = QStringLiteral("JSON files (*.json)");
static const QString JSONL_FILE = QStringLiteral("JSONL files (*.jsonl)");

static const QMap< QString, Mode > ModeMap = {
	{ MODE_TEXTLOG2JSON,
	  { .function = consoleTextlog,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
	  .inPattern = TEXT_FILE,
	  .outPattern = JSON_FILE }},
	{ MODE_TEXTLOG2JSON_DIR,
	  { .function = consoleTextlogDir,
	.needsOutput = true,
	.worksWithDirectories = true,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false } },
	{ MODE_JSON2TEXTLOG,
	  { .function = consoleJsonlog,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSON_FILE,
		.outPattern = TEXT_FILE } },
	{ MODE_JSON2TEXTLOG_DIR,
	  { .function = consoleJsonlogDir,
	.needsOutput = true,
	.worksWithDirectories = true,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false } },
	{ MODE_TEMPLATE2JSON_DIR,
	  { .function = consoleTemplateJsonDir,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false } },
	{ MODE_DATEPARSETEST,
	  { .function = consoleDateParseTest,
	.needsOutput = false,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = TEXT_FILE } },
	{ MODE_DISCORD2TEXTLOG,
	  { .function = discord2textlog,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSON_FILE,
		.outPattern = TEXT_FILE } },
	{ MODE_DISCORD2JSONLOG,
	  { .function = discord2json,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSON_FILE,
		.outPattern = JSON_FILE } },
	// New AI stuff
	{ MODE_TEXTLOG2AGNAISTIC,
	  { .function = textlog2agnaistic,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = true,
	.needsGuid = true,
	.needsChid = false,
		.inPattern = TEXT_FILE,
		.outPattern = JSON_FILE } },
	{ MODE_JSON2AGNAISTIC,
	  { .function = json2agnaistic,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = true,
	.needsGuid = true,
	.needsChid = false,
		.inPattern = JSON_FILE,
		.outPattern = JSON_FILE } },
	{ MODE_AGNAISTIC2TEXTLOG,
	  { .function = agnaistic2textlog,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSON_FILE,
		.outPattern = TEXT_FILE } },
	{ MODE_AGNAISTIC2JSON,
	  { .function = agnaistic2json,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSON_FILE,
		.outPattern = JSON_FILE } },
	{ MODE_TEXTLOG2TAVERNAI,
	  { .function = textlog2tavernai,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = true,
	.needsGuid = false,
	.needsChid = true,
		.inPattern = TEXT_FILE,
		.outPattern = JSONL_FILE } },
	{ MODE_JSON2TAVERNAI,
	  { .function = json2tavernai,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = true,
	.needsGuid = false,
	.needsChid = true,
		.inPattern = JSON_FILE,
		.outPattern = JSONL_FILE } },
	{ MODE_TAVERNAI2TEXTLOG,
	  { .function = tavernai2textlog,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSONL_FILE,
		.outPattern = TEXT_FILE } },
	{ MODE_TAVERNAI2JSON,
	  { .function = tavernai2json,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSONL_FILE,
		.outPattern = JSON_FILE } },
	{ MODE_TEXTLOG2AITEMPLATE,
	  { .function = textlog2aitemplate,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = true,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = TEXT_FILE,
		.outPattern = TEXT_FILE } },
	{ MODE_JSON2AITEMPLATE,
	  { .function = json2aitemplate,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = true,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSON_FILE,
		.outPattern = TEXT_FILE } },
	{ MODE_TEXTLOG2SILLYTAVERN,
	  { .function = textlog2sillytavern,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = true,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = TEXT_FILE,
		.outPattern = JSONL_FILE } },
	{ MODE_JSON2SILLYTAVERN,
	  { .function = json2sillytavern,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = true,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSON_FILE,
		.outPattern = JSONL_FILE } },
	{ MODE_SILLYTAVERN2TEXTLOG,
	  { .function = sillytavern2textlog,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSONL_FILE,
		.outPattern = TEXT_FILE } },
	{ MODE_SILLYTAVERN2JSON,
	  { .function = sillytavern2json,
	.needsOutput = true,
	.worksWithDirectories = false,
	.needsPlayerName = false,
	.needsGuid = false,
	.needsChid = false,
		.inPattern = JSONL_FILE,
		.outPattern = JSON_FILE } }};

static QTextStream STDOUT( stdout );

int main( int argc, char *argv[] ) {
	QApplication a( argc, argv );
	try {
	QMap< QString, QString > args;
	for ( int i = 1; i < ( argc - 1 ); i += 2 ) {
	args.insert( QString::fromUtf8( argv[i] ).trimmed( ),
			 QString::fromUtf8( argv[i + 1] ).trimmed( ) );
	}
	if ( !args.contains( ARG_MODE ) ) {
		windowMode = true;
		QStringList modeList;
		for(auto it = std::begin(ModeMap); it != std::end(ModeMap); ++it)
		{
			modeList.push_back(it.key());
		}
		args.insert( ARG_MODE,
			 QInputDialog::getItem(
				 nullptr, QStringLiteral( "Select mode" ),
				 QStringLiteral( "Mode:" ), modeList, 0, false ) );
	}
	auto foundMode = ModeMap.find(args[ARG_MODE].toLower());
	if(foundMode == std::end(ModeMap))
		throw  std::runtime_error("Unsupported mode!");
	const auto& mode = foundMode.value();
	if(!args.contains( ARG_IN ))
	{
		if(mode.worksWithDirectories)
			args.insert(ARG_IN,QFileDialog::getExistingDirectory(nullptr, QStringLiteral("Open Directory (for reading)"),
								QString()));
		else
			args.insert(ARG_IN,QFileDialog::getOpenFileName(nullptr, QStringLiteral("Open File"),
								QString(), mode.inPattern));
	}
	if(!args.contains( ARG_OUT ) && mode.needsOutput )
	{
		if(mode.worksWithDirectories)
			args.insert(ARG_OUT,QFileDialog::getExistingDirectory(nullptr, QStringLiteral("Open Directory (for reading)"),
								QString()));
		else
			args.insert(ARG_OUT,QFileDialog::getSaveFileName(nullptr, QStringLiteral("Save File"),
								QString(), mode.outPattern));
	}
	if(!args.contains( ARG_PLAYERNAME ) && mode.needsPlayerName )
	{
		args.insert(ARG_PLAYERNAME,QInputDialog::getText(nullptr,QStringLiteral("Insert Player Name"),QStringLiteral("Player Name")));
	}
	if(!args.contains( ARG_PLAYERUID ) && mode.needsGuid )
	{
		args.insert(ARG_PLAYERUID,QInputDialog::getText(nullptr,QStringLiteral("Insert Player UUID"),QStringLiteral("Player UUID")));
	}
	if(!args.contains( ARG_CHID ) && mode.needsChid )
	{
		args.insert(ARG_CHID,QString::number(QInputDialog::getInt(nullptr,QStringLiteral("Insert Character ID"),QStringLiteral("Character ID"))));
	}
	mode.function(args);
	} catch ( const std::exception &e ) {
		if(windowMode)
			QMessageBox::critical( nullptr,
				   QStringLiteral( "Exception encountered" ),
				   QString::fromUtf8( e.what( ) ) );
		else
			STDOUT << "Exception encountered\n" << e.what() << "\n";
		a.exit( -1 );
	}
}

void handleException( const std::exception &e ) {
	if ( windowMode ) {
	QMessageBox::critical( nullptr,
				   QStringLiteral( "Exception encountered" ),
				   QString::fromUtf8( e.what( ) ) );
	} else {
	STDOUT << "Error encountered!\n" << e.what( ) << "\n";
	}
}

int consoleTextlog( const QMap< QString, QString > &args ) {
	try {
	QString in = args[ARG_IN];
	QFile inF( in );
	if ( !inF.open( QFile::ReadOnly ) ) {
		throw QFileException( in, QFile::ReadOnly );
	}
	QString out = args[ARG_OUT];
	QFile outF( out );
	if ( !outF.open( QFile::WriteOnly ) ) {
		throw QFileException( out, QFile::WriteOnly );
	}
	text2json( inF, outF );
	inF.close( );
	outF.flush( );
	outF.close( );
	return 0;
	} catch ( const std::exception &e ) {
	handleException( e );
	return -1;
	}
}
int consoleJsonlog( const QMap< QString, QString > &args ) {
	try {
	QString in = args[ARG_IN];
	QFile inF( in );
	if ( !inF.open( QFile::ReadOnly ) ) {
		throw QFileException( in, QFile::ReadOnly );
	}
	QString out = args[ARG_OUT];
	QFile outF( out );
	if ( !outF.open( QFile::WriteOnly ) ) {
		throw QFileException( out, QFile::WriteOnly );
	}
	QJsonDocument json = QJsonDocument::fromJson( inF.readAll( ) );
	inF.close( );
	RpgSession session;
	session.fromJson( json.array( ) );
	QTextStream stream( &outF );
	session.toString( stream );
	stream.flush( );
	outF.close( );
	return 0;
	} catch ( const std::exception &e ) {
	handleException( e );
	return -1;
	}
}
int consoleTextlogDir( const QMap< QString, QString > &args ) {
	try {
	QDirIterator dirIn( args[ARG_IN] );
	QDir dirOut( args[ARG_OUT] );
	while ( dirIn.hasNext( ) ) {
		QString path = dirIn.next( );
		QFileInfo info( path );
		QString outPath = dirOut.absoluteFilePath(
		QStringLiteral( "%1.json" ).arg( info.baseName( ) ) );
		if ( info.isFile( ) ) {
		QFile inF( path );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( path, QFile::ReadOnly );
		}
		QFile outF( outPath );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( outPath, QFile::WriteOnly );
		}
		text2json( inF, outF );
		inF.close( );
		outF.flush( );
		outF.close( );
		}
	}
	} catch ( const std::exception &e ) {
	handleException( e );
	return -1;
	}
	return 0;
}
int consoleJsonlogDir( const QMap< QString, QString > &args ) {
	try {
	QDirIterator dirIn( args[ARG_IN] );
	QDir dirOut( args[ARG_OUT] );
	while ( dirIn.hasNext( ) ) {
		QString path = dirIn.next( );
		QFileInfo info( path );
		QString outPath = dirOut.absoluteFilePath(
		QStringLiteral( "%1.txt" ).arg( info.baseName( ) ) );
		if ( info.isFile( ) ) {
		QFile inF( path );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( path, QFile::ReadOnly );
		}
		QFile outF( outPath );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( outPath, QFile::WriteOnly );
		}
		QJsonDocument json = QJsonDocument::fromJson( inF.readAll( ) );
		inF.close( );
		RpgSession session;
		session.fromJson( json.array( ) );
		QTextStream stream( &outF );
		session.toString( stream );
		stream.flush( );
		outF.close( );
		}
		return 0;
	}
	} catch ( const std::exception &e ) {
	handleException( e );
	return -1;
	}
	return 0;
}
int consoleTemplateJsonDir( const QMap< QString, QString > &args ) {
	try {
	QDirIterator dirIn( args[ARG_IN] );
	QString out = args[ARG_OUT];
	QFile outF( out );
	if ( !outF.open( QFile::WriteOnly ) ) {
		throw QFileException( out, QFile::WriteOnly );
	}
	QJsonDocument doc( RpgPostTemplate::mapTemplates2JSON( dirIn ) );
	outF.write( doc.toJson( ) );
	outF.close( );
	} catch ( const std::exception &e ) {
	handleException( e );
	return -1;
	}
	return 0;
}
int consoleDateParseTest( const QMap< QString, QString > &args ) {
	try {
	QString in = args[ARG_IN];
	QFile inF( in );
	if ( !inF.open( QFile::ReadOnly ) ) {
		throw QFileException( in, QFile::ReadOnly );
	}
	RpgSession sess;
	QTextStream strem( &inF );
	sess.fromString( strem );
	QStringList unparsedDates;
	for ( const auto &section : qAsConst( sess.getSections( ) ) ) {
		for ( const auto &post : qAsConst( section.getLogs( ) ) ) {
		if ( post.getDate( ).type( ) == QVariant::String )
			unparsedDates.push_back( post.getDate( ).toString( ) );
		}
	}
	if ( unparsedDates.empty( ) ) {
		if ( windowMode ) {
		QMessageBox::information(
			nullptr, QStringLiteral( "Success!" ),
			QStringLiteral( "No unparsed dates were found." ) );
		} else {
		STDOUT << "No misformed, unparsed dates found!!\n";
		}
	} else {
		QString unparsedList = unparsedDates.join( '\n' );
		if ( windowMode ) {
		QMessageBox::warning(
			nullptr, QStringLiteral( "Unparsed dates found" ),
			QStringLiteral(
			"The following dates could not be parsed:\n%1" )
			.arg( unparsedList ) );
		} else {
		STDOUT << "The following dates could not be parsed:\n"
			   << unparsedList << "\n";
		}
	}
	return 0;
	} catch ( const std::exception &e ) {
	handleException( e );
	return -1;
	}
}
struct DiscordMessage {
	int sender;
	QString message;
};
QMap<QDateTime,DiscordMessage> getDiscordMessageMap(const QJsonObject& data)
{
	QMap<QDateTime,DiscordMessage> DiscordMessageMap;
	for(const auto it : data) {
		QJsonObject objects = it.toObject();
		for(const auto zt : objects) {
			QJsonObject message = zt.toObject();
			DiscordMessage msg;
			msg.sender = message["u"].toInt();
			msg.message = markdownToHtml(message["m"].toString());
			QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(message["t"].toVariant().toLongLong());
			DiscordMessageMap.insert(timestamp,msg);
		}
	}
	return DiscordMessageMap;
}
void discordMessagesToSession(const QMap<QDateTime,DiscordMessage>& discordMap, RpgSession& session)
{
	RpgSection sect;
	for(auto it = std::begin(discordMap); it != std::end(discordMap); ++it)
	{
		RpgLog log;
		log.setDate( it.key() );
		log.setHun( false );
		log.setUser( QString::number(it.value().sender) );
		log.setContent( it.value().message );
		sect.getLogs().push_back(log);
	}
	session.getSections().clear();
	session.getSections().push_back(sect);
}

int discord2textlog( const QMap< QString, QString > &args ) {
	try {
	QString in = args[ARG_IN];
	QFile inF( in );
	if ( !inF.open( QFile::ReadOnly ) ) {
		throw QFileException( in, QFile::ReadOnly );
	}
	QString out = args[ARG_OUT];
	QFile outF( out );
	if ( !outF.open( QFile::WriteOnly ) ) {
		throw QFileException( out, QFile::WriteOnly );
	}
	QJsonDocument json = QJsonDocument::fromJson( inF.readAll( ) );
	inF.close( );
	QJsonObject obj = json.object();
	QJsonObject data = obj["data"].toObject();
	RpgSession session;
	discordMessagesToSession(getDiscordMessageMap(data),session);
	QTextStream stream( &outF );
	session.toString( stream );
	stream.flush( );
	outF.close( );
	return 0;
	} catch ( const std::exception &e ) {
	handleException( e );
	return -1;
	}
}
int discord2json( const QMap< QString, QString > &args ) {
	try {
	QString in = args[ARG_IN];
	QFile inF( in );
	if ( !inF.open( QFile::ReadOnly ) ) {
		throw QFileException( in, QFile::ReadOnly );
	}
	QString out = args[ARG_OUT];
	QFile outF( out );
	if ( !outF.open( QFile::WriteOnly ) ) {
		throw QFileException( out, QFile::WriteOnly );
	}
	QJsonDocument json = QJsonDocument::fromJson( inF.readAll( ) );
	inF.close( );
	QJsonObject obj = json.object();
	QJsonObject data = obj["data"].toObject();
	RpgSession session;
	discordMessagesToSession(getDiscordMessageMap(data),session);
	QJsonDocument doc( session.toJson( ) );
	outF.write( doc.toJson( ) );
	outF.close( );
	return 0;
	} catch ( const std::exception &e ) {
	handleException( e );
	return -1;
	}
}
int textlog2agnaistic( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		QTextStream strem( &inF );
		sess.fromString( strem );
		AgnaisticRpgScene agn;
		agn.fromRpgSession(sess, QUuid::fromString(args[ARG_PLAYERUID]), args[ARG_PLAYERNAME]);
		QJsonDocument doc(agn.toJson());
		outF.write( doc.toJson() );
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int json2agnaistic( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		QJsonDocument injson = QJsonDocument::fromJson( inF.readAll() );
		sess.fromJson(injson.array() );
		AgnaisticRpgScene agn;
		agn.fromRpgSession(sess, QUuid::fromString(args[ARG_PLAYERUID]), args[ARG_PLAYERNAME]);
		QJsonDocument doc(agn.toJson());
		outF.write( doc.toJson() );
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int agnaistic2textlog( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		AgnaisticRpgScene agn;
		RpgSession sess;
		QJsonDocument injson = QJsonDocument::fromJson( inF.readAll() );
		agn.fromJson(injson.object());
		agn.toRpgSession(sess);
		QTextStream strem( &outF );
		sess.toString(strem);
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int agnaistic2json( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		AgnaisticRpgScene agn;
		RpgSession sess;
		QJsonDocument injson = QJsonDocument::fromJson( inF.readAll() );
		agn.fromJson(injson.object());
		agn.toRpgSession(sess);
		QJsonDocument outJsonDocument(sess.toJson());
		outF.write(outJsonDocument.toJson());
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int textlog2tavernai( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		QTextStream strem( &inF );
		sess.fromString( strem );
		TavernAiConversation tavernai;
		tavernai.fromRpgSession(sess, args[ARG_PLAYERNAME], args[ARG_CHID].toInt() );
		QTextStream strem2( &outF );
		tavernai.toJsonL(strem2);
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int json2tavernai( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		QJsonDocument injson = QJsonDocument::fromJson( inF.readAll() );
		sess.fromJson(injson.array() );
		TavernAiConversation tavernai;
		tavernai.fromRpgSession(sess, args[ARG_PLAYERNAME], args[ARG_CHID].toInt() );
		QTextStream strem2( &outF );
		tavernai.toJsonL(strem2);
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int tavernai2textlog( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		TavernAiConversation tavernai;
		QTextStream strem( &inF );
		tavernai.fromJsonL( strem );
		tavernai.toRpgSession(sess);
		QTextStream strem2( &outF );
		sess.toString(strem2);
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int tavernai2json( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		TavernAiConversation tavernai;
		QTextStream strem( &inF );
		tavernai.fromJsonL( strem );
		tavernai.toRpgSession(sess);
		QJsonDocument outJsonDocument(sess.toJson());
		outF.write(outJsonDocument.toJson());
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}

void rpgLogToTemplate(QTextStream& strem2, const RpgSession& sess, const QStringList& playerNames )
{
	QStringList characterNames;
	bool firstIter = false;
	for(const auto& section : qAsConst(sess.getSections()))
	{
		if(firstIter) break;
		for(const auto& post : qAsConst(section.getLogs()))
		{
			if(firstIter) break;
			if(post.getUser() != playerNames[0] )
			{
				QString charname = post.getUser();
				QString characterFirstname = charname.split(' ')[0];
				if(!characterNames.contains(charname)) characterNames.push_back(charname);
				if(!characterNames.contains(characterFirstname)) characterNames.push_back(characterFirstname);
				firstIter = true;
				break;
			}
		}
	}
	strem2 << "<START>\n";
	for(const auto& section : qAsConst(sess.getSections()))
	{
		for(const auto& post : qAsConst(section.getLogs()))
		{
			bool isPlayerPost = post.getUser() == playerNames[0];
			QString content = htmlToMarkdown(post.getContent());
			for(const auto& it : qAsConst(playerNames))
				content = content.replace(it,QStringLiteral("{{user}}"));
			for(const auto& it : qAsConst(characterNames))
				content = content.replace(it,QStringLiteral("{{char}}"));
			content = content.replace('\n',' ');
			strem2 << (isPlayerPost ? "{{user}}" : "{{char}}") << ": " << content << '\n';
		}
	}
	strem2.flush();
}

int textlog2aitemplate( const QMap< QString, QString > &args )
{
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		QStringList playerNames = { args[ARG_PLAYERNAME], args[ARG_PLAYERNAME].split(' ')[0] };
		RpgSession sess;
		QTextStream strem( &inF );
		sess.fromString( strem );
		QTextStream strem2( &outF );
		rpgLogToTemplate(strem2,sess,playerNames);
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int json2aitemplate( const QMap< QString, QString > &args )
{
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		QStringList playerNames = { args[ARG_PLAYERNAME], args[ARG_PLAYERNAME].split(' ')[0] };
		RpgSession sess;
		QJsonDocument injson = QJsonDocument::fromJson( inF.readAll() );
		sess.fromJson(injson.array() );
		QTextStream strem2( &outF );
		rpgLogToTemplate(strem2,sess,playerNames);
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}

int sillytavern2textlog( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		SillyTavernConversaiton sillytavern;
		QTextStream strem( &inF );
		sillytavern.fromJsonL( strem );
		sillytavern.toRpgSession(sess);
		QTextStream strem2( &outF );
		sess.toString(strem2);
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int sillytavern2json( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		SillyTavernConversaiton sillytavern;
		QTextStream strem( &inF );
		sillytavern.fromJsonL( strem );
		sillytavern.toRpgSession(sess);
		QJsonDocument outJsonDocument(sess.toJson());
		outF.write(outJsonDocument.toJson());
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int textlog2sillytavern( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		QTextStream strem( &inF );
		sess.fromString( strem );
		SillyTavernConversaiton sillytavern;
		sillytavern.fromRpgSession(sess, args[ARG_PLAYERNAME]);
		QTextStream strem2( &outF );
		sillytavern.toJsonL(strem2);
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
int json2sillytavern( const QMap< QString, QString > &args ) {
	try {
		QString in = args[ARG_IN];
		QFile inF( in );
		if ( !inF.open( QFile::ReadOnly ) ) {
			throw QFileException( in, QFile::ReadOnly );
		}
		QString out = args[ARG_OUT];
		QFile outF( out );
		if ( !outF.open( QFile::WriteOnly ) ) {
			throw QFileException( out, QFile::WriteOnly );
		}
		RpgSession sess;
		QJsonDocument injson = QJsonDocument::fromJson( inF.readAll() );
		sess.fromJson(injson.array() );
		SillyTavernConversaiton sillytavern;
		sillytavern.fromRpgSession(sess, args[ARG_PLAYERNAME]);
		QTextStream strem2( &outF );
		sillytavern.toJsonL(strem2);
		return 0;
	} catch ( const std::exception &e )
	{
		handleException( e );
		return -1;
	}
}
