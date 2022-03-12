#include "RpgPostWidget.hpp"
#include "ui_RpgPostWidget.h"

RpgPostWidget::RpgPostWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RpgPostWidget)
{
	ui->setupUi(this);
}

RpgPostWidget::~RpgPostWidget()
{
	delete ui;
}

QLineEdit* RpgPostWidget::getUserEditor() const
{
	return ui->posterEdit;
}

QLineEdit* RpgPostWidget::getDateEditor() const
{
	return ui->dateEdit;
}

QTextEdit* RpgPostWidget::getContentEditor() const
{
	return ui->contentEdit;
}

void RpgPostWidget::fromRpgLog(const RpgLog& log)
{
	ui->posterEdit->setText(log.getUser());
	ui->dateEdit->setText(log.getDate().toString());
	ui->contentEdit->setHtml(log.getContent());
}

void RpgPostWidget::toRpgLog(RpgLog& log) const
{
	log.setUser(ui->posterEdit->text());
	log.setDate(ui->dateEdit->text());
	log.setContent(ui->contentEdit->toHtml());
}

RpgLog RpgPostWidget::toRpgLog() const
{
	RpgLog tmp;
	toRpgLog(tmp);
	return tmp;
}
