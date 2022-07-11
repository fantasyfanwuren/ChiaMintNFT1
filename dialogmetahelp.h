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
    explicit DialogMetaHelp(QWidget *parent = nullptr);
    ~DialogMetaHelp();

private:
    Ui::DialogMetaHelp *ui;
};

#endif // DIALOGMETAHELP_H
