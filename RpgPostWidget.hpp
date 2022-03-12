#ifndef RPGPOSTWIDGET_HPP
#define RPGPOSTWIDGET_HPP
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include "RpgLog.hpp"

namespace Ui {
class RpgPostWidget;
}

class RpgPostWidget : public QWidget
{
	Q_OBJECT

public:
	explicit RpgPostWidget(QWidget *parent = nullptr);
	~RpgPostWidget();

	QLineEdit* getUserEditor() const;
	QLineEdit* getDateEditor() const;
	QTextEdit* getContentEditor() const;
	void fromRpgLog(const RpgLog& log);
	void toRpgLog(RpgLog& log) const;
	RpgLog toRpgLog(void) const;

private:
	Ui::RpgPostWidget *ui;
};

#endif // RPGPOSTWIDGET_HPP
