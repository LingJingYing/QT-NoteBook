#include "widget.h"
#include "./ui_widget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    textEditModify = false;
    //this->setLayout(ui->verticalLayout);
    //ui->widgetBottom->setLayout(ui->horizontalLayout);
    // connect(ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &Widget::onCursorPositionChanged);
    connect(ui->textEdit, &QTextEdit::textChanged, [=](){textEditModify=true;});

    QShortcut *shortcutOpen = new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")), this);
    QShortcut *shortcutSave = new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save")), this);
    QShortcut *shortcutZoomIn = new QShortcut(QKeySequence(tr("Ctrl+Shift+=", "File|ZoomIn")), this);
    QShortcut *shortcutZoomOut = new QShortcut(QKeySequence(tr("Ctrl+Shift+-", "File|ZoomOut")), this);

    connect(shortcutOpen, &QShortcut::activated, [=](){
        on_btnOpen_clicked();
    });

    connect(shortcutSave, &QShortcut::activated, [=](){
        on_btnSave_clicked();
    });

    connect(shortcutZoomIn, &QShortcut::activated, [=](){
        zoomIn();
    });

    connect(shortcutZoomOut, &QShortcut::activated, [=](){
        zoomOut();
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),
                                                    "E:/QtProject/", tr("Text (*.txt)"));

    ui->textEdit->clear();

    file.setFileName(fileName);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug() << "file open error";
        return;
    }

    this->setWindowTitle(fileName+"-NoteBook");

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Encoding::Utf8);

    while(!in.atEnd()){
        QString context = in.readLine();
        ui->textEdit->append(context);
    }

    QTextCursor cursor(ui->textEdit->textCursor());
    cursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveAnchor);
    ui->textEdit->setTextCursor(cursor);

    textEditModify = false;
}


void Widget::on_btnSave_clicked()
{
    if(textEditModify){
        QString context = ui->textEdit->toPlainText();

        if(!context.isEmpty()){
            if(!file.isOpen()){
                QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"),
                                                                "E:/QtProject/untitled.txt", tr("Text (*.txt)"));


                file.setFileName(fileName);
                if(!file.open(QIODevice::ReadWrite | QIODevice::Text)){
                    qDebug() << "file open error";
                    return;
                }
            }

            // file.seek(0);
            QTextStream out(&file);
            out.seek(0);
            out.setEncoding(QStringConverter::Encoding::Utf8);
            out << context;
            textEditModify = false;
            this->setWindowTitle(file.fileName()+"-NoteBook");
        }
    }
}


void Widget::on_btnClose_clicked()
{
    if(textEditModify){
        if(file.isOpen() || !ui->textEdit->document()->isEmpty()){
            int ret = QMessageBox::warning(this, tr("NoteBook Notice:"),
                                           tr("The document has been modified.\nDo you want to save your changes?"),
                                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                           QMessageBox::Save);

            switch(ret){
            case QMessageBox::Save:
                on_btnSave_clicked();
                break;
            case QMessageBox::Discard:
                ui->textEdit->clear();
                if(file.isOpen()){
                    file.close();
                    this->setWindowTitle("NoteBook");
                    textEditModify = false;
                }
                break;
            case QMessageBox::Cancel:
                break;
            default:
                break;
            }
        }
    }
    else{
        ui->textEdit->clear();
        if(file.isOpen()){
            file.close();
            this->setWindowTitle("NoteBook");
            textEditModify = false;
        }
    }
}

void Widget::onCursorPositionChanged()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QString blockNum = QString::number(cursor.blockNumber()+1);
    QString columnNum = QString::number(cursor.columnNumber()+1);
    const QString labelMes = "第" + blockNum + "行，第" + columnNum + "列";
    ui->labelPosition->setText(labelMes);

    //设置高亮
    QList<QTextEdit::ExtraSelection> extraSelectList;
    QTextEdit::ExtraSelection ext;

    ext.cursor = cursor;

    QBrush qBrush(Qt::lightGray);

    ext.format.setBackground(qBrush);
    ext.format.setProperty(QTextFormat::Property::FullWidthSelection, true);

    extraSelectList.append(ext);
    ui->textEdit->setExtraSelections(extraSelectList);
}

void Widget::zoomIn()
{
    QFont font = ui->textEdit->font();

    int fontSize = font.pointSize();
    if(-1 == fontSize) return;

    int newFontSize = fontSize + 1;
    font.setPointSize(newFontSize);
    ui->textEdit->setFont(font);
}

void Widget::zoomOut()
{
    QFont font = ui->textEdit->font();

    int fontSize = font.pointSize();
    if(-1 == fontSize) return;

    int newFontSize = fontSize - 1;
    font.setPointSize(newFontSize);
    ui->textEdit->setFont(font);
}


