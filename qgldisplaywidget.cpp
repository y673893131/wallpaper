#include "qgldisplaywidget.h"
#include <QDebug>
#include <time.h>
#include <QTimer>

QGLDisplayWidget::QGLDisplayWidget(QWidget *parent)
    :QOpenGLWidget(parent)
    ,m_nDelay(1000)
    ,m_vShader(nullptr),m_fShader(nullptr)
    ,m_program(nullptr)
{
    m_nStart = clock();
    connect(this, &QGLDisplayWidget::modifyFile, this, [this](const QString& sFile)
    {
        if(!sFile.isEmpty())
        {
            initProgram(sFile);
            qDebug() << "local:" << m_location[Local_Mouse] << m_location[Local_Time] << m_location[Local_Resolution];
        }
        update();
    }, Qt::ConnectionType::QueuedConnection);


    auto fun = [this](/*const QPoint& pt*/)
    {
        m_center = QCursor::pos();
        if(isVisible())
            update();
    };

    auto timer = new QTimer(this);
    timer->setInterval(50);
    timer->start();
    connect(timer, &QTimer::timeout, this, fun);
    connect(this, &QGLDisplayWidget::flushPoint, this, fun, Qt::ConnectionType::QueuedConnection);
}

QGLDisplayWidget::~QGLDisplayWidget()
{
    qDebug() << "gl video widget quit.";
}

void QGLDisplayWidget::setDisplaySize(int w, int h)
{
    resize(w, h);
}

void QGLDisplayWidget::setSpeed(int speed)
{
    if(speed)
        m_nDelay = speed;
}

void QGLDisplayWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    initProgram(":/custom.fsh");
}

void QGLDisplayWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void QGLDisplayWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0, 0);

    auto time = clock() - m_nStart;
    m_program->bind();
    if(m_location[Local_Time] >= 0) glUniform1f(m_location[Local_Time], time * 1.0 / m_nDelay);
    if(m_location[Local_Mouse] >= 0) glUniform2f(m_location[Local_Mouse], m_center.x() / width(), m_center.y() / height());
    if(m_location[Local_Resolution] >= 0) glUniform2f(m_location[Local_Resolution], width(), height());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
    m_program->release();
}

void QGLDisplayWidget::initProgram(const QString & fragment)
{
    if(m_program) {m_program->release(); delete m_program;}
    // program
    if(!m_vShader)
    {
        m_vShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
        auto bCompile = m_vShader->compileSourceFile(":/img.vsh");
        if(!bCompile){
            qWarning() << "vertex compile failed.";
        }
    }

    if(m_fShader) delete m_fShader;
    m_fShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    auto bCompile = m_fShader->compileSourceFile(fragment);
    if(!bCompile){
        qWarning() <<  "fragment compile failed.";
    }

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShader(m_fShader);
    m_program->addShader(m_vShader);
    m_program->bindAttributeLocation("position", ATTR_VERTEX_IN);
    m_program->bindAttributeLocation("surfacePosAttrib", ATTR_TEXTURE_IN);
    m_program->link();

    m_location[Local_Mouse] = m_program->uniformLocation("mouse");
    m_location[Local_Time] = m_program->uniformLocation("time");
    m_location[Local_Resolution] = m_program->uniformLocation("resolution");

    // vertex/texture vertices value
    GLfloat vertexVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         -1.0f, 1.0f,
         1.0f, 1.0f,
    };

    memcpy(m_vertexVertices, vertexVertices, sizeof(vertexVertices));
    glVertexAttribPointer(ATTR_VERTEX_IN, 2, GL_FLOAT, 0, 0, m_vertexVertices);
    glEnableVertexAttribArray(ATTR_VERTEX_IN);

    GLfloat textureVertices[] = {
        0.0f,  1.0f,
        1.0f,  1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
    };

    memcpy(m_textureVertices, textureVertices, sizeof(textureVertices));
    glVertexAttribPointer(ATTR_TEXTURE_IN, 2, GL_FLOAT, 0, 0, m_textureVertices);
    glEnableVertexAttribArray(ATTR_TEXTURE_IN);
    // texture obj
//    for (int n = 0; n < TEXTURE_MAX; ++n)
//        m_texture[n].init();
}
