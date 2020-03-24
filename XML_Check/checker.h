#pragma once
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>


class Checker : public QWidget
{
   // Q_OBJECT

public:
    Checker(QWidget *parent = 0);
    virtual ~Checker() {};

private:
    QPushButton *Check_B;
    QTextEdit   *InFile_E;
    QTextEdit   *Log_E;

private:
    QString RootName;
    int     RootStart{0};
    int     RootEnd{0};

private:
    struct  Tag_Struct
    {
        QString  m_Body     {" "};
        QString  m_Name     {" "};
        int      m_AttrNum  {0};
        QString  m_ErrorMsg {" "};
    };

private:
    int XML_AttrFind         (const QString &str);
    int XML_Root             (const QString &str);
    int XML_WordsFind        (const QString &str, bool inTag = true);
    int OutTagCheck          (const QString &str);
    Tag_Struct XML_CheckTag  (const QString &str);
    QString XML_GetTagName   (const QString &tag);
    int XML_FindAllTags  (const QString &XML_file,int lastTagPos);

private slots:
void OnCheck();

};

