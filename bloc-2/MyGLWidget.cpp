#include "MyGLWidget.h"
#include <string>
#include <iostream>
//using namespace std;

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  scale = 1.0f;
  rotation = 0.0f;
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  createBuffers();
  initCamera();
}

void MyGLWidget::initCamera(){
    radiEsfera();
    
  OBS = glm::vec3(0.0, 0.0, radi*1.5);
  VRP = glm::vec3(0.0, 0.0, 0.0);
  UP = glm::vec3(0.0, 0.1, 0.0);
  phy = psi = theta = 0.0;
  xClick     = 0;
    yClick     = 0;
    delta     = M_PI / 180.0;
    interaccio = NOINTERACCIO;
  
  float d = 0;
  for (int i = 0; i < 3; i++){
      d += (OBS[i] - VRP[i])*(OBS[i] - VRP[i]);
  }
  d = sqrt(d);
  ra = 1.0;
  znear = (d - radi)/2.0;
  zfar = d + radi;
  FOV = fovi  = 2.0 * asin(radi / d); // (float)M_PI / 2.0f;
  
  viewTransform();
  projectTransform();
}

void MyGLWidget::radiEsfera(){
    float xmin, xmax, ymin, ymax, zmin, zmax;
    xmin = xmax = model.vertices()[0];
    ymin = ymax = model.vertices()[1];
    zmin = zmax = model.vertices()[2];
    for (unsigned int i = 3; i < model.vertices().size(); i += 3){
        if (model.vertices()[i] < xmin) xmin = model.vertices()[i];
        if (model.vertices()[i] > xmax) xmax = model.vertices()[i];
        if (model.vertices()[i+1] < ymin) ymin = model.vertices()[i+1];
        if (model.vertices()[i+1] > ymax) ymax = model.vertices()[i+1];
        if (model.vertices()[i+2] < zmin) zmin = model.vertices()[i+2];
        if (model.vertices()[i+2] > zmax) zmax = model.vertices()[i+2];
    }
    
    float dx = xmax - xmin;
    float dy = ymax - ymin;
    float dz = zmax - zmin;
    
    radi = sqrt(dx*dx + dy*dy + dz*dz)/2.0;
    centre[0] = (xmax + xmin)/2.0;
    centre[1] = (ymax + ymin)/2.0;
    centre[2] = (zmax + zmin)/2.0;
}

void MyGLWidget::paintGL () 
{
  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Carreguem la transformació de model
  modelTransform ();
  viewTransform();
  projectTransform();

  // Activem el VAO per a pintar la caseta 
  glBindVertexArray (VAO_Homer);

  // pintem
  glDrawArrays(GL_TRIANGLES, 0, 3 * model.faces().size());
  
  //TERRA
  /*terraTransform();
  glBindVertexArray(VAO_Terra);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);*/

  glBindVertexArray (0);
}

void MyGLWidget::modelTransform () 
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  transform = glm::rotate(transform, rotation, glm::vec3(0,1,0));
  transform = glm::translate(transform, -centre);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::terraTransform(){
  glm::mat4 transform (1.0f);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform(){
  glm::mat4 proj = glm::perspective(FOV, ra, znear, zfar);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);
}

void MyGLWidget::viewTransform(){
  //glm::mat4 view = glm::lookAt(OBS, VRP, UP);
    glm::mat4 view(1.0f);
    view = glm::translate(view, -OBS);
    view = glm::rotate(view, psi, glm::vec3(0,1,0));
    view = glm::rotate(view, -theta, glm::vec3(1,0,0));
    view = glm::rotate(view, phy, glm::vec3(0,0,1));
    view = glm::translate(view, -VRP);
    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
  
}

void MyGLWidget::resizeGL (int w, int h) 
{
    //w i h són els del nou viewport
    
    float newViewport = float(w)/float(h);
    ra = newViewport;
    
    if (newViewport < 1) FOV = 2.0 * atan(tan(fovi/2.0)/newViewport);
    
    projectTransform();
    
  glViewport(0, 0, w, h);
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_S: { // escalar a més gran
      scale += 0.05;
      break;
    }
    case Qt::Key_D: { // escalar a més petit
      scale -= 0.05;
      break;
    }
    case Qt::Key_R: { // Rotar
      rotation += (float)M_PI/4.0f;
      break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::mousePressEvent(QMouseEvent * e){
    makeCurrent();
    xClick = e->x();
    yClick = e->y();
    if (e->button() && Qt::LeftButton) 
        interaccio = ROTACIO;
    else interaccio = NOINTERACCIO;
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent * e){
    makeCurrent();
    interaccio = NOINTERACCIO;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent * e){
    makeCurrent();
    int dx = abs(e->x() - xClick);
    int dy = abs(e->y() - yClick);
    
    if (dx > dy){
        if (e->x() > xClick)
            psi += dx*delta;
        else psi -= dx*delta;
    }
    else {
        if (e->y() > yClick)
            theta -= dy*delta;
        else theta += dy*delta;
    }
    update();
    xClick = e->x();
    yClick = e->y();
}

void MyGLWidget::createBuffers () 
{ 
  model.load("./models/Patricio.obj");
  glm::vec3 posicio[4] = {
        glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3( 1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, -1.0),
        glm::vec3( 1.0, -1.0, -1.0)
  };
  
  glm::vec3 color[4] = {
      glm::vec3(0.5, 0.25, 0),
      glm::vec3(0.5, 0.25, 0),
      glm::vec3(0.5, 0.25, 0),
      glm::vec3(0.5, 0.25, 0)
};
  
  //////////////////////////
  
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(1, &VBO_terraPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_terraPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posicio), posicio, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);
  
  glGenBuffers(1, &VBO_terraCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_terraCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
  
  /*glGenBuffers(1, &VBO_CasaCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CasaCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray (0);*/
  
  
  
  ///////////////////////////
  
  

  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Homer);
  glBindVertexArray(VAO_Homer);

  glGenBuffers(1, &VBO_vertexs);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_vertexs);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model.faces().size() * 3 * 3,
	       model.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_color);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model.faces().size() * 3 * 3,
                      model.VBO_matdiff(), GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag");
  vs.compileSourceFile("shaders/vertshad.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc = glGetUniformLocation(program->programId(), "PT");
  viewLoc = glGetUniformLocation(program->programId(), "VT");
}

