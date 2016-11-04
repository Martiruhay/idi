#include <QLineEdit>

class MyLineEdit: public QlineEdit{
  Q_OBJECT
public:
  MyLineEdit(QWidget *parent);
public slots:
  void tractaReturn();
signals:
  void enviaText(const Qstring &);
};