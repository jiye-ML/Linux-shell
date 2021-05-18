#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QHeaderView>
#include <QMessageBox>
#include "syetemset.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_flag(false),
    m_auto(false),
    m_isLeft(true),
    m_overNum(0),
    m_pthread(NULL)
{
    ui->setupUi(this);


    QIcon icon(":/Image/people.png");
    setWindowIcon(icon);

    // 加载历史记录
    if(!LoadPersionInfo())
    {
        // 没有历史记录
        GetFileMessage("Config/NameList.txt");
    }
    // 加载问题
    LoadQuestions("Config/question.txt");

    // 显示随机姓名
    qsrand(time(NULL));
    int index = qrand() % m_names.size();
    QString curName = m_names.at(index);
    ui->label_name->setText(curName);
    int count = m_perInfo[curName].blackCount;
    if(count == 0)
    {
        ui->delete_black->setEnabled(false);
    }
    else
    {
        ui->delete_black->setEnabled(true);
    }
    // 随机问题
    index = qrand() % m_questions.size();
    ui->label_question->setText(m_questions.at(index));


    // 初始化
    m_timer = new QTimer(this);
    m_pthread = new QThread(this);
    m_wgInfo = new InfoWidget(this);
    m_randomName = new RandomName(m_names.size(), m_questions.size());
    m_wgInfo->SetSutdentNum(m_names.size());    // 设置总人数
    m_wgInfo->SetBlackListNum(m_perInfo[curName].blackCount);   // 初始化黑名单次数
    ui->statusBar->addWidget(m_wgInfo);
    // RandomName对象移入子线程
    m_randomName->moveToThread(m_pthread);

    // 右侧列表 init
    InitSutdentInfo();
    // 启动子线程
    m_pthread->start();

    // 开始/停止
    connect(ui->start, &QPushButton::clicked, [=]()
    {
        if(!m_flag)
        {
            ui->start->setText("停止");
            ui->automate->setEnabled(false);
            m_timer->start(50);
            if(m_overNum > 0 && m_overNum <= m_questions.size())
            {
                QString text = ui->label_question->text();
                m_questions.removeOne(text);
                m_randomName->SetMaxQuestionNumber(m_questions.size());
            }
            m_overNum++;
        }
        else
        {
            m_timer->stop();
            ui->start->setText("开始");
            ui->automate->setEnabled(true);
        }
        m_flag = !m_flag;
    });

    // 托管
    connect(ui->automate, &QPushButton::clicked, [=]()
    {
        m_auto = true;
        ui->start->setEnabled(false);
        ui->automate->setEnabled(false);
        m_timer->start(50);
        if(m_overNum > 0 && m_overNum <= m_questions.size())
        {
            QString text = ui->label_question->text();
            m_questions.removeOne(text);
            qDebug() << m_questions.size();
            m_randomName->SetMaxQuestionNumber(m_questions.size());
        }
        m_overNum++;

    });

    // 添加到黑名单
    connect(ui->blackList, &QPushButton::clicked, [=]()
    {
        // 当前姓名
        QString curName = ui->label_name->text();
        // 黑名单次数+1
        int count = (m_perInfo[curName].blackCount += 1);
        // 刷新列表
        QString curText = ui->comboBox->currentText();
        slotComboIndexChange(curText);
        m_wgInfo->SetBlackListNum(count);
        ui->delete_black->setEnabled(true);
        QMessageBox::information(this, "提示", "成功添加 @_" + curName + "_@ 到黑名单");
    });

    // 从黑名单中删除
    connect(ui->delete_black, &QPushButton::clicked, [=]()
    {
        QString name = ui->label_name->text();
        m_perInfo[name].blackCount --;
        if(m_perInfo[name].blackCount <= 0)
        {
            m_perInfo[name].blackCount = 0;
            ui->delete_black->setEnabled(false);
            m_wgInfo->SetBlackListNum(0);
        }
        QString text = ui->comboBox->currentText();
        slotComboIndexChange(text);
    });

    // 定时器
    connect(m_timer, &QTimer::timeout, m_randomName, &RandomName::GetName);

    // 处理抽签类发出的信号
    connect(m_randomName, &RandomName::sigName, this, &MainWindow::GetRandomName);

    // 系统设置
    connect(ui->actionSystemSet, &QAction::triggered, [=]()
    {
        SyetemSet dlg(this);
        connect(&dlg, &SyetemSet::sigSysSet, [=](QString path)
        {
            // 数据重置
            // 读文件
            GetFileMessage(path);
            // 初始化列表框
            RefreshTableList(m_perInfo);
            // 初始化随机人名
            // 显示随机姓名
            qsrand(time(NULL));
            int index = qrand() % m_names.size();
            QString curName = m_names.at(index);
            ui->label_name->setText(curName);
            m_randomName->SetMaxNameNumber(m_names.size());
        });
        dlg.exec();
    });

    qDebug() << "main Thread ID: " << this->thread() << endl;
}

MainWindow::~MainWindow()
{
    if(m_pthread != NULL)
    {
        m_pthread->quit();
        m_pthread->wait();
    }
    delete ui;
    delete m_randomName;
}

void MainWindow::LoadQuestions(QString path)
{
    QFile file(path);
    bool bl = file.open(QIODevice::ReadOnly);
    if(!bl)
    {
        return;
    }
    QString text = file.readAll();
    m_questions = text.split("\r\n");
   // m_randomName->SetMaxQuestionNumber(m_questions.size());
    m_overNum = 0;
    file.close();
}

void MainWindow::GetFileMessage(QString path)
{
    // 没有历史记录, 直接读文件
    // 数据初始化
    QFile file(path);
    if(!file.open(QFile::ReadOnly))
    {
        return;
    }
    QTextStream stream(&file);
    QString text = stream.readAll();
    QStringList nameList = text.split(',');

    for(int i=0; i<nameList.size(); ++i)
    {
        PersionInfo info;
        m_perInfo.insert(nameList.at(i), info);
        m_names.push_back(nameList.at(i));
    }
    file.close();
}

void MainWindow::GetRandomName(int index, int idx, int total)
{
    // 获取随机姓名,并显示
    QString curName = m_names.at(index);
    ui->label_name->setText(curName);
    if(m_auto)
    {
        if(total >= OUTTIME)
        {
            // 运行时间大于等于3s停止
            m_timer->stop();
            // 重新初始化
            m_auto = false;
            ui->start->setEnabled(true);
            ui->automate->setEnabled(true);
            m_randomName->clear();
        }
    }
    int count = m_perInfo[curName].blackCount;
    m_wgInfo->SetBlackListNum(count);
    if(count == 0)
    {
        ui->delete_black->setEnabled(false);
    }
    else
    {
        ui->delete_black->setEnabled(true);
    }
    ui->label_question->setText(m_questions.at(idx));
    qDebug() << "++++++++" << this->thread();
}

void MainWindow::InitSutdentInfo()
{
    // 按钮
    ui->arrow->setFixedSize(18, 36);
    ui->arrow->setStyleSheet("QPushButton{border-image: url(:/Image/left.png);}");
    ui->arrow->setToolTip("显示");
    // 隐藏右侧列表
    ui->right_wg->setVisible(false);
    // 右侧列表初始化
    QStringList headers;
    headers << "姓名" << "上黑名单次数";
    ui->tableWidget->setColumnCount(headers.count());
    ui->tableWidget->setRowCount(m_names.count());
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    // 禁止编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 选择整行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction* actionSubtract = new QAction("减1", ui->tableWidget);
    ui->tableWidget->addAction(actionSubtract);
    connect(actionSubtract, &QAction::triggered, [=]()
    {
        QTableWidgetItem* curItem = ui->tableWidget->currentItem();
        int curRow = curItem->row();
        QString name = ui->tableWidget->item(curRow, 0)->text();
        QTableWidgetItem* blackItem = ui->tableWidget->item(curRow, 1);
        int black = blackItem->text().toInt() - 1;
        black = black < 0 ? 0 : black;
        m_perInfo[name].blackCount = black;
        blackItem->setText(QString::number(black));
        if(black == 0)
        {
            slotComboIndexChange("黑名单");
        }
    });


    // 初始化列表
    RefreshTableList(m_perInfo);

    // 下拉框
    ui->comboBox->addItem("全部");
    ui->comboBox->addItem("黑名单");
    ui->comboBox->setCurrentText("全部");

    // 箭头按钮
    connect(ui->arrow, &QPushButton::clicked, [=]()
    {
        if(m_isLeft)
        {
            // 显示右侧列表
            ui->right_wg->setVisible(true);
            ui->arrow->setStyleSheet("QPushButton{border-image: url(:/Image/right.png);}");
            ui->arrow->setToolTip("隐藏");
        }
        else
        {
            ui->right_wg->setVisible(false);
            ui->arrow->setStyleSheet("QPushButton{border-image: url(:/Image/left.png);}");
            ui->arrow->setToolTip("显示");
        }
        m_isLeft = !m_isLeft;
    });

    // 下拉框
    connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotComboIndexChange(QString)));

}

void MainWindow::slotComboIndexChange(QString text)
{
    // 清空列表
    ui->tableWidget->clearContents();
    if(text == "全部")
    {
        ui->tableWidget->setRowCount(m_perInfo.size());
        RefreshTableList(m_perInfo);
    }
    else if(text == "黑名单")
    {
        QMap<QString, PersionInfo> blackList;
        QMap<QString, PersionInfo>::iterator it = m_perInfo.begin();
        for(; it != m_perInfo.end(); ++it)
        {
            if(it.value().blackCount > 0)
            {
                blackList.insert(it.key(), it.value());
            }
        }
        ui->tableWidget->setRowCount(blackList.size());
        RefreshTableList(blackList);
    }
}

void MainWindow::RefreshTableList(QMap<QString, PersionInfo> info)
{
    int row = 0;
    QMap<QString, PersionInfo>::iterator it;
    // 列表添加行
    for(it = info.begin(); it != info.end(); ++it)
    {
        QTableWidgetItem* item1 = new QTableWidgetItem(it.key());
        ui->tableWidget->setItem(row, 0, item1);
        QTableWidgetItem* item2 = new QTableWidgetItem(QString::number(it.value().blackCount));
        ui->tableWidget->setItem(row, 1, item2);
        row ++;
    }
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    CreateXML();
}

bool MainWindow::LoadPersionInfo()
{
    QDomDocument doc;
    QFile file("Config/PersionInfo.xml");
    bool bl = file.open(QIODevice::ReadOnly);
    if(!bl)
    {
        return false;
    }
    if(!doc.setContent(&file))
    {
        return false;
    }

    m_perInfo.clear();
    m_names.clear();
    QDomElement root = doc.documentElement();
    if(root.hasChildNodes())
    {
        QDomNodeList keyList = root.childNodes();
        for(int i=0; i<keyList.size(); ++i)
        {
            // 键值
            QDomElement keyElem = keyList.at(i).toElement();
            QString key = keyElem.attribute("Key");
            // value值
            QDomElement valueElem = keyElem.childNodes().at(0).toElement();
            PersionInfo info;
            info.blackCount = valueElem.text().toInt();
            // 插入新节点
            m_perInfo.insert(key, info);
            m_names.push_back(key);
        }
    }
    file.close();
    return true;
}

bool MainWindow::CreateXML()
{
    QFile file("Config/PersionInfo.xml");
    bool bl = file.open(QIODevice::WriteOnly);
    if(!bl)
    {
        return false;
    }
    QDomDocument doc;
    // 文件头
    QDomProcessingInstruction ins = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
    // 文件头添加到文件中
    doc.appendChild(ins);
    // 添加根节点
    QDomElement root = doc.createElement("人员信息");
    doc.appendChild(root);

    // 写xml
    // xml文件根节点
    QMap<QString, PersionInfo>::iterator it = m_perInfo.begin();
    for(; it != m_perInfo.end(); ++it)
    {
        // 键值节点
        QDomElement key = doc.createElement("键值");
        key.setAttribute("Key", it.key());
        root.appendChild(key);
        // value节点
        QDomElement black = doc.createElement("BlackList");
        QDomText text = doc.createTextNode(QString::number(it.value().blackCount));
        black.appendChild(text);
        key.appendChild(black);
    }


    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();
    return true;
}

