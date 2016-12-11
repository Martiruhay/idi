#include <QSlider>

class MySlider: public QSlider{
  Q_OBJECT
public:
  MySlider(QWidget *parent);
public slots:
  void tractaCanvi();
signals:
  void enviaValor(const int &);
};
