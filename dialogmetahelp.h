#ifndef DIALOGMETAHELP_H
#define DIALOGMETAHELP_H

#include <QDialog>

namespace Ui {
class DialogMetaHelp;
}

class DialogMetaHelp : public QDialog
{
    Q_OBJECT
public:
    void setText(const QString &text);
    void setTitle(const QString &title);
public:
    explicit DialogMetaHelp(QWidget *parent = nullptr);
    ~DialogMetaHelp();

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::DialogMetaHelp *ui;
};

#endif // DIALOGMETAHELP_H
