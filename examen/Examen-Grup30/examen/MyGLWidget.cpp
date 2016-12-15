#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  angleX = angleY = 0.0;
  perspectiva = true;
  DoingInteractive = NONE;
  //radiEsc = sqrt(10);
  
  ////////////////////////////////
  //Minims - Maxims
  glm::vec3 v = glm::vec3(-4,0, -3) - glm::vec3(2, 2, 3);
  radiEsc = sqrt(v.x*v.x + v.y*v.y + v.z*v.z)/2;
  obs = glm::vec3(radiEsc*1.5, 1.0, 0.0);
  vrp = glm::vec3(-2.0, 0.0, 0.0);
  up = glm::vec3(0.0, 0.1, 0.0);
  float d = 0;
  for (int i = 0; i < 3; i++){
      d += (obs[i] - vrp[i])*(obs[i] - vrp[i]);
  }
  d = sqrt(d);
  ra = 1.0;
  znear = (d - radiEsc)/2.0;
  zfar = d + radiEsc;
  fov = fovi  = 2.0 * asin(radiEsc / d); // (float)M_PI / 2.0f;
  
  legoX = 0;
  segut = false;
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
  createBuffersLegoman();
  createBuffersLegomanSentat();
  createBuffersSofa();
  createBuffersTerra();
  projectTransform ();
  viewTransform ();
}

void MyGLWidget::paintGL () 
{
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Activem el VAO per a pintar el terra 
  glBindVertexArray (VAO_Terra);

  modelTransformIdent ();

  // pintem
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Activem el VAO per a pintar el Legoman
  glBindVertexArray (VAO_Lego);

  modelTransformLegoman ();

  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, lego.faces().size()*3);
  
  glBindVertexArray (VAO_Sofa);

  modelTransformSofa ();

  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, sofa.faces().size()*3);
  
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h) 
{
  
  float newViewport = float(w)/float(h);
  ra = newViewport;
    
  if (newViewport < 1) fov = 2.0 * atan(tan(fovi/2.0)/newViewport);
  
  projectTransform();
  
  glViewport(0, 0, w, h);
}

void MyGLWidget::modelTransformLegoman ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::translate(TG, glm::vec3(1+legoX,0,0));
  TG = glm::rotate(TG, float(-M_PI/2.0), glm::vec3(0,1,0));
  TG = glm::scale(TG, glm::vec3(2.0*escalaLego, 2.0*escalaLego, 2.0*escalaLego));
  TG = glm::translate(TG, -centreBaseLego);
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformSofa ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::translate(TG, glm::vec3(-3.0, 0.0, 0.0));
  TG = glm::scale(TG, glm::vec3(escalaSofa, escalaSofa, escalaSofa));
  TG = glm::translate(TG, -centreBaseSofa);
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformIdent ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform ()
{
  glm::mat4 Proj;  // Matriu de projecció
  if (perspectiva)
    //Proj = glm::perspective(float(M_PI/3.0), 1.0f, radiEsc, 3.0f*radiEsc);
    Proj = glm::perspective(fov, ra, znear, zfar);
  else
    Proj = glm::ortho(-radiEsc, radiEsc, -radiEsc, radiEsc, radiEsc, 3.0f*radiEsc);

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform ()
{
  glm::mat4 View;  // Matriu de posició i orientació
  View = glm::translate(glm::mat4(1.f), glm::vec3(0, -1, -2*radiEsc -1));
  View = glm::rotate(View, -angleY, glm::vec3(0, 1, 0));
  View = glm::rotate(View, angleX, glm::vec3(1, 0, 0));
  View = glm::rotate(View, float(M_PI/2.0), glm::vec3(0, 1, 0));

  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::createBuffersLegoman ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  lego.load("./models/legoman.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel (lego, escalaLego, centreBaseLego);
  
  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Lego);
  glBindVertexArray(VAO_Lego);

  // Creació dels buffers del model patr
  // Buffer de posicions
  glGenBuffers(1, &VBO_LegoPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*lego.faces().size()*3*3, lego.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glGenBuffers(1, &VBO_LegoNorm);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoNorm);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*lego.faces().size()*3*3, lego.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glGenBuffers(1, &VBO_LegoMatamb);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoMatamb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*lego.faces().size()*3*3, lego.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glGenBuffers(1, &VBO_LegoMatdiff);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoMatdiff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*lego.faces().size()*3*3, lego.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glGenBuffers(1, &VBO_LegoMatspec);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoMatspec);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*lego.faces().size()*3*3, lego.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glGenBuffers(1, &VBO_LegoMatshin);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoMatshin);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*lego.faces().size()*3, lego.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}

void MyGLWidget::createBuffersLegomanSentat ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  legosentat.load("./models/legoman-assegut.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel (legosentat, escalaLego2, centreBaseLego2);
  
  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Lego2);
  glBindVertexArray(VAO_Lego2);

  // Creació dels buffers del model patr
  // Buffer de posicions
  glGenBuffers(1, &VBO_LegoPos2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoPos2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*legosentat.faces().size()*3*3, legosentat.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glGenBuffers(1, &VBO_LegoNorm2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoNorm2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*legosentat.faces().size()*3*3, legosentat.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glGenBuffers(1, &VBO_LegoMatamb2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoMatamb2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*lego.faces().size()*3*3, lego.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glGenBuffers(1, &VBO_LegoMatdiff);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoMatdiff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*lego.faces().size()*3*3, lego.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glGenBuffers(1, &VBO_LegoMatspec2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoMatspec2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*legosentat.faces().size()*3*3, lego.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glGenBuffers(1, &VBO_LegoMatshin2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_LegoMatshin2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*legosentat.faces().size()*3, legosentat.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}

void MyGLWidget::createBuffersSofa ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  sofa.load("./models/Sofa.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel (sofa, escalaSofa, centreBaseSofa);
  
  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Sofa);
  glBindVertexArray(VAO_Sofa);

  // Creació dels buffers del model patr
  // Buffer de posicions
  glGenBuffers(1, &VBO_SofaPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_SofaPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*sofa.faces().size()*3*3, sofa.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glGenBuffers(1, &VBO_SofaNorm);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_SofaNorm);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*sofa.faces().size()*3*3, sofa.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glGenBuffers(1, &VBO_SofaMatamb);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_SofaMatamb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*sofa.faces().size()*3*3, sofa.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glGenBuffers(1, &VBO_SofaMatdiff);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_SofaMatdiff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*sofa.faces().size()*3*3, sofa.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glGenBuffers(1, &VBO_SofaMatspec);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_SofaMatspec);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*sofa.faces().size()*3*3, sofa.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glGenBuffers(1, &VBO_SofaMatshin);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_SofaMatshin);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*sofa.faces().size()*3, sofa.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}

void MyGLWidget::createBuffersTerra ()
{
  // Dades del terra
  // VBO amb la posició dels vèrtexs
  glm::vec3 posterra[4] = {
        glm::vec3(-4.0, 0.0, -3.0),
        glm::vec3(-4.0, 0.0,  3.0),
        glm::vec3( 2.0, 0.0, -3.0),
        glm::vec3( 2.0, 0.0,  3.0)
  }; 

  // VBO amb la normal de cada vèrtex
  glm::vec3 normt (0,1,0);
  glm::vec3 normterra[4] = {
	normt, normt, normt, normt
  };

  // Definim el material del terra
  glm::vec3 amb(0,0.1,0);
  glm::vec3 diff(0.6,0.6,0.1);
  glm::vec3 spec(0.5,0.5,0.5);
  float shin = 100;

  // Fem que aquest material afecti a tots els vèrtexs per igual
  glm::vec3 matambterra[4] = {
	amb, amb, amb, amb
  };
  glm::vec3 matdiffterra[4] = {
	diff, diff, diff, diff
  };
  glm::vec3 matspecterra[4] = {
	spec, spec, spec, spec
  };
  float matshinterra[4] = {
	shin, shin, shin, shin
  };

// Creació del Vertex Array Object del terra
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(1, &VBO_TerraPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_TerraNorm);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraNorm);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normterra), normterra, GL_STATIC_DRAW);

  // Activem l'atribut normalLoc
  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glGenBuffers(1, &VBO_TerraMatamb);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatamb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matambterra), matambterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glGenBuffers(1, &VBO_TerraMatdiff);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatdiff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matdiffterra), matdiffterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glGenBuffers(1, &VBO_TerraMatspec);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatspec);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matspecterra), matspecterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glGenBuffers(1, &VBO_TerraMatshin);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraMatshin);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matshinterra), matshinterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
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
  // Obtenim identificador per a l'atribut “normal” del vertex shader
  normalLoc = glGetAttribLocation (program->programId(), "normal");
  // Obtenim identificador per a l'atribut “matamb” del vertex shader
  matambLoc = glGetAttribLocation (program->programId(), "matamb");
  // Obtenim identificador per a l'atribut “matdiff” del vertex shader
  matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
  // Obtenim identificador per a l'atribut “matspec” del vertex shader
  matspecLoc = glGetAttribLocation (program->programId(), "matspec");
  // Obtenim identificador per a l'atribut “matshin” del vertex shader
  matshinLoc = glGetAttribLocation (program->programId(), "matshin");

  // Demanem identificadors per als uniforms del vertex shader
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");
}

void MyGLWidget::calculaCapsaModel (Model &p, float &escala, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = p.vertices()[0];
  miny = maxy = p.vertices()[1];
  minz = maxz = p.vertices()[2];
  for (unsigned int i = 3; i < p.vertices().size(); i+=3)
  {
    if (p.vertices()[i+0] < minx)
      minx = p.vertices()[i+0];
    if (p.vertices()[i+0] > maxx)
      maxx = p.vertices()[i+0];
    if (p.vertices()[i+1] < miny)
      miny = p.vertices()[i+1];
    if (p.vertices()[i+1] > maxy)
      maxy = p.vertices()[i+1];
    if (p.vertices()[i+2] < minz)
      minz = p.vertices()[i+2];
    if (p.vertices()[i+2] > maxz)
      maxz = p.vertices()[i+2];
  }
  escala = 1.0/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_O: { // canvia òptica entre perspectiva i axonomètrica
      perspectiva = !perspectiva;
      projectTransform ();
      break;
    }
    case Qt::Key_Up: { // canvia òptica entre perspectiva i axonomètrica
      if (legoX == -2.5 - 1) seu();
      else{
	legoX -= 0.5;
	break;
      }
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  // Aqui cal que es calculi i s'apliqui la rotacio o el zoom com s'escaigui...
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleX += (e->y() - yClick) * M_PI / 180.0;
    angleY += (e->x() - xClick) * M_PI / 180.0;
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::seu(){
  segut = true;
  //FALTA AFEGIR AL PAINTGL LA CRIDA A PINTAR EL NOU MODEL (JA CARREGAT)
}


