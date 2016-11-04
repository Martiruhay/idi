#include "MyLineEdit.h"
//constructor
MyLineEdit::MyLineEdit(QWidget * parent) : QLineEdit(parent){
  
}

//implementacio slots
void MyLineEdit::tractaReturn(){
  emit enviaText(text());
}