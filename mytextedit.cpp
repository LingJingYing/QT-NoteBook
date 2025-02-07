#include "mytextedit.h"



MyTextEdit::MyTextEdit(QWidget *parent):QTextEdit(parent)
{

}

void MyTextEdit::wheelEvent(QWheelEvent *event)
{
    if(ctrlKeyPressed){
        if(event->angleDelta().y() > 0){
            zoomIn();
        }
        else{
            zoomOut();
        }
        event->accept();
    }
    else{
        QTextEdit::wheelEvent(event);
    }
}
void MyTextEdit::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
        ctrlKeyPressed = true;
    }
    QTextEdit::keyPressEvent(event);
}

void MyTextEdit::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
        ctrlKeyPressed = false;
    }
    QTextEdit::keyReleaseEvent(event);
}
