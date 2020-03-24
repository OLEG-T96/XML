#include "checker.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QRegExp"

Checker::Checker(QWidget *parent)
        :QWidget(parent)
{
  QHBoxLayout *hbox = new QHBoxLayout();

 QLabel *InputTitle  = new QLabel("XML Document",this);
 QLabel *ErrorsTitle = new QLabel("Errors",this);

  Check_B = new QPushButton ("Check");
  hbox->addStretch(300);
  hbox->addWidget(Check_B);

  QVBoxLayout *vbox = new QVBoxLayout();
  InFile_E = new QTextEdit();
  InFile_E->setMinimumSize(400,300);
  InFile_E->setSizePolicy(QSizePolicy::MinimumExpanding,
                          QSizePolicy::MinimumExpanding);
  Log_E = new QTextEdit();
  Log_E->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
  Log_E->setReadOnly(true);
  vbox->addWidget(InputTitle);
  vbox->addWidget(InFile_E);
  vbox->addSpacing(5);
  vbox->addWidget(ErrorsTitle);
  vbox->addWidget(Log_E);
  vbox->addSpacing(10);
  vbox->addLayout(hbox);
  setLayout(vbox);
  connect(Check_B, &QPushButton::clicked, this, &Checker::OnCheck);
}

int Checker::XML_AttrFind(const QString &str)
{
    if(str.isEmpty()){return 0;}

    QRegExp XML_Attr("\\s[a-zA-Z]+\\w*=\"\\w+\"\\s+");
    int AttrIndex{0};
    int AttrCounter{0};
    while((AttrIndex = XML_Attr.indexIn(str + " ",AttrIndex)) != -1 )
    {
      AttrIndex++;
      AttrCounter++;
    }
    return AttrCounter;
}

int Checker::XML_Root(const QString &str)
{
    if(str.isEmpty())
    {
        Log_E->append("Empty Root");
        return -1;
    }

    RootStart  = str.indexOf("<");
    int closeTag = str.indexOf(">");

    if((RootStart == -1) || (closeTag == -1))
    {
        Log_E->append("Root tag not found");
        return -1;
    }
    if(XML_CheckTag(str.mid((RootStart),(closeTag - RootStart))).m_Body == " ")
    {
        Log_E->append("Wrong Root");
        return -1;
    }

    RootName = XML_GetTagName(str.mid((RootStart + 1),
                               (closeTag - RootStart) - 1));
    if(RootName  == " ")
    {
        Log_E->append("Root Error");
       return -1;
    }
    if(str.mid(closeTag-2,3) == " />")
    {
        Log_E->append("Root is Closed!");
        return -1;
    }
    QString CloseRootName = ("</" + RootName + ">");
    RootEnd = str.indexOf(CloseRootName);
    if(RootEnd == -1)
    {
        Log_E->append("Can't find root end!");
        return -1;
    }
    if(RootEnd != str.lastIndexOf(CloseRootName))
    {
        Log_E->append("Root tag closed more than once!");
        return -1;
    }

    QString OutRoot;
    OutRoot = str.left(RootStart) + "\r";
    int closeTagPos = RootEnd + CloseRootName.length();
    OutRoot.append(str.right(str.length() - closeTagPos));
    if(OutRoot.contains(QRegExp("\\S+")))
      {
        Log_E->append("ERR: Unknown: " + OutRoot);
        return -1;
      }
    Log_E->append(RootName);
    Log_E->append(CloseRootName);
    return 1;
}

Checker::Tag_Struct Checker::XML_CheckTag(const QString &str)
{
    Tag_Struct Tag;
    if(str.isEmpty()){return Tag;}

    int OpenTagIndex  = str.indexOf("<");
    int CloseTagIndex = str.indexOf(">");
    Tag.m_Body   = str.mid((OpenTagIndex + 1),
                                (CloseTagIndex - OpenTagIndex) - 1);
    Tag.m_Name = XML_GetTagName(Tag.m_Body);

    if(Tag.m_Name  == " ")
    {
        Tag.m_Body = " ";
        Tag.m_ErrorMsg = "Wrong name";
        return Tag;
    }

    Tag.m_AttrNum = XML_AttrFind(Tag.m_Body);

    if(XML_WordsFind(Tag.m_Body) != (Tag.m_AttrNum + 1))

    {
        Tag.m_ErrorMsg = "Wrong attr";
        Tag.m_Body = " ";
    }
    return Tag;
}

int Checker::XML_WordsFind (const QString &str, bool inTag )
{
    if(!str.contains(QRegExp("\\S+")))
    {
        return 0;
    }
   int WordsNum = str.split(QRegExp("\\s+")).count();
   if(str.right(1) == " "){WordsNum--;}
   if(inTag)
   {
     if(str.right(2) == " /"){WordsNum --;}
   }
   return WordsNum;
}

int Checker::OutTagCheck(const QString &XML_file)
{
    int prevTag{0};
    int nextTag = XML_file.indexOf("<");
    int words{0};
    QString str  = XML_file.left(nextTag);
    if(str.contains(QRegExp("\\S+")))
    {
      if((words = str.split(QRegExp("\\s+")).count()) > 0)
      {
          Log_E->append("Unknown: " + str);
      }
    }

    while ((prevTag != -1)&&(nextTag != -1))
    {
        prevTag = XML_file.indexOf(">",nextTag);
        nextTag = XML_file.indexOf("<",prevTag);
        str  = XML_file.mid(prevTag+1,(nextTag - prevTag) - 1);
        if(str.contains(QRegExp("\\S+")))
        {
          words += str.split(QRegExp("\\s+")).count();
          Log_E->append("Unknown: " + str);
        }
    }
   return words;
}

int Checker::XML_FindAllTags(const QString &XML_file, int lastTagPos)
{
    if(XML_file.isEmpty())      // <-- check if File is empty
    {
        Log_E->append("EMPTY");
        return -1;//"EMPTY";
    }

    int OpenTagIndex  = XML_file.indexOf(QRegExp("<[^/]"),lastTagPos);
    int CloseTagIndex = XML_file.indexOf(">",OpenTagIndex);

    if((OpenTagIndex == -1) || (CloseTagIndex == -1))  // <-- check for available tags

    {
         //Log_E->append("No Tags Found");
        return -1;//"No Tags Found";
    }
    if(OpenTagIndex == RootEnd)
    {
        Log_E->append("Finished!");
        return 0;
    }

    QString str = XML_file.mid((OpenTagIndex),
                               (CloseTagIndex - OpenTagIndex)+1);
    Tag_Struct Tag = XML_CheckTag(str);
    QString StatusMessage{": "};
    StatusMessage.prepend("<" + Tag.m_Name + ">");

    if(Tag.m_Body == " ")
    {
        StatusMessage.prepend(QString::number(lastTagPos));
        Log_E->append(StatusMessage + Tag.m_ErrorMsg);
        return -1;//StatusMessage + Tag.m_ErrorMsg;
    }

    QString EndTag ="</" + Tag.m_Name + ">";
    int EndTagPos = XML_file.indexOf(EndTag);

    if(EndTagPos == -1)
    {
        if(Tag.m_Body.right(2) == " /")
        {
          return XML_FindAllTags(XML_file,CloseTagIndex);
        }
        StatusMessage.prepend(QString::number(EndTagPos));
        Log_E->append(StatusMessage + "EndTagErr");
        return -1;//StatusMessage + "EndTagErr";
    }
    if(Tag.m_Body.right(2) == " /")
    {
        StatusMessage.prepend(QString::number(EndTagPos));
         Log_E->append(StatusMessage + "Err: tag closed twice");
        return -1;//StatusMessage + "Err: tag closed twice";
    }

    //int nextTag = XML_file.indexOf("<",CloseTagIndex);
    int nextTag = XML_file.indexOf(QRegExp("<[^/]"),CloseTagIndex);

    if(((nextTag !=-1) && (nextTag < EndTagPos)))
    {

       nextTag = XML_FindAllTags(XML_file,nextTag);
    }

    while(((nextTag !=-1) && (nextTag < EndTagPos)))
    {

       nextTag = XML_FindAllTags(XML_file,nextTag + 1);
    }

    return EndTagPos;
}

QString Checker::XML_GetTagName(const QString &tag)
{
    QString TagName;
    int FirstSpaceIndex  = tag.indexOf(" ");
    if(FirstSpaceIndex == -1)
    {
        TagName = tag;
        return TagName;
    }
    else if(FirstSpaceIndex == 0)
    {
        return " ";
    }
    else if(FirstSpaceIndex > 0)
    {
        TagName  = tag.left(FirstSpaceIndex);
        return TagName;
    }
    return " ";
}

void Checker::OnCheck()
{
     Log_E->clear();
     QString str = InFile_E->toPlainText();
     if(OutTagCheck(str) > 0){return;}
     XML_Root(str);
     int pos = XML_FindAllTags(str,RootStart + 1);
     if(pos == -1)
     {
         Log_E->append("Wrong XML");
         return;
     }
     while(pos > 0)
     {
       pos = XML_FindAllTags(str,pos+1);
     }

}
