#ifndef QGLDISPLAYWIDGET_H
#define QGLDISPLAYWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

struct _texture_obj_{
    void init(){
        texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        texture->create();
        id = texture->textureId();
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    QOpenGLTexture* texture;
    uint id;
};

class QGLDisplayWidget : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
    enum enum_pragram_attr_index
    {
        ATTR_VERTEX_IN = 0,
        ATTR_TEXTURE_IN,
    };

//    enum enum_texture_index
//    {
//        TEXTURE_IMG = 0,

//        TEXTURE_MAX
//    };

    enum enum_local
    {
//        Local_Img,
        Local_Mouse,
        Local_Time,
        Local_Resolution,

        Local_Max
    };

public:
    QGLDisplayWidget(QWidget *parent = nullptr);
    virtual ~QGLDisplayWidget();

    void setDisplaySize(int w, int h);
    // QOpenGLWidget interface
signals:
    void modifyFile(const QString&);
    void flushPoint(const QPoint&);
    void playOver();
public slots:
    void setSpeed(int);
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void initProgram(const QString&);
private:
    QOpenGLShader* m_vShader,* m_fShader;
    QOpenGLShaderProgram* m_program;
    int m_location[Local_Max];
    GLfloat m_vertexVertices[8], m_textureVertices[8];
//    _texture_obj_ m_texture[TEXTURE_MAX];
    QPoint m_center;
    QImage m_img;

    long long m_nStart;
    int m_nDelay;
};

#endif // QGLDISPLAYWIDGET_H
