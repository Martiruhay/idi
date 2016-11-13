#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del context grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    
    virtual void mousePressEvent   (QMouseEvent * e);
    virtual void mouseReleaseEvent (QMouseEvent * e);
    virtual void mouseMoveEvent    (QMouseEvent * e);

  private:
    void createBuffers ();
    void carregaShaders ();
    void modelTransform ();
    void projectTransform();
    void viewTransform();
    void terraTransform();
    
    void initCamera();
    
    void radiEsfera();

    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // VAO i VBO names
    GLuint VAO_Homer, VBO_vertexs, VBO_color, VAO_Terra, VBO_terraPos, VBO_terraCol;
    // Program
    QOpenGLShaderProgram *program;
    // Internal vars
    float scale, rotation;
    glm::vec3 pos, OBS, VRP, UP;
    float FOV, fovi, ra, znear, zfar;
    Model model;
    float radi, psi, theta, phy, delta;
    glm::vec3 centre;
    typedef enum {NOINTERACCIO, ROTACIO} Interaccio;
    Interaccio interaccio;
    int xClick, yClick;
};

