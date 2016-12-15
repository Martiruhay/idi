#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
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
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    // mouse{Press/Release/Move}Event - Són cridades quan es realitza l'event 
    // corresponent de ratolí
    virtual void mousePressEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);

  private:
    void createBuffersLegoman ();
    void createBuffersLegomanSentat();
    void createBuffersSofa ();
    void createBuffersTerra ();
    void carregaShaders ();
    void projectTransform ();
    void viewTransform ();
    void modelTransformIdent ();
    void modelTransformLegoman ();
    void modelTransformSofa ();
    void calculaCapsaModel (Model &p, float &escala, glm::vec3 &CentreBase);
    void seu();

    // VAO i VBO names
    GLuint VAO_Lego, VBO_LegoPos, VBO_LegoNorm, VBO_LegoMatamb, VBO_LegoMatdiff, VBO_LegoMatspec, VBO_LegoMatshin;
    GLuint VAO_Lego2, VBO_LegoPos2, VBO_LegoNorm2, VBO_LegoMatamb2, VBO_LegoMatdiff2, VBO_LegoMatspec2, VBO_LegoMatshin2;
    GLuint VAO_Sofa, VBO_SofaPos, VBO_SofaNorm, VBO_SofaMatamb, VBO_SofaMatdiff, VBO_SofaMatspec, VBO_SofaMatshin;
    GLuint VAO_Terra, VBO_TerraPos, VBO_TerraNorm, VBO_TerraMatamb, VBO_TerraMatdiff, VBO_TerraMatspec, VBO_TerraMatshin;
    // Program
    QOpenGLShaderProgram *program;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // attribute locations
    GLuint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc;

    // model
    Model lego, legosentat, sofa;
    // paràmetres calculats a partir de la capsa contenidora del model
    glm::vec3 centreBaseLego, centreBaseLego2, centreBaseSofa;
    float escalaLego, escalaLego2, escalaSofa;
    // radi de l'escena
    float radiEsc;

    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
    float angleY, angleX, ra, fov, fovi, znear, zfar;
    glm::vec3 vrp, obs, up;
    bool perspectiva;
    float legoX;
    
    bool segut;
    
};

