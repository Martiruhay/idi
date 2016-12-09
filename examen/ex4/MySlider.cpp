#include "MySlider.h"
//constructor
MySlider::MySlider(QWidget * parent) : QSlider(parent){
  
}

//implementacio slots
void MySlider::tractaCanvi(){
  emit enviaValor(value());
}