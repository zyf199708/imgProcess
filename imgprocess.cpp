#include "imgprocess.h"
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QMatrix>
#include <QIcon>
#include <QLabel>
#include <QDebug>
ImgProcess::ImgProcess(QWidget *parent)
    : QMainWindow(parent)
{
    imgLabel=new QLabel(this);
   // imgLabel->move(0,0);

    img=new QImage;
    oldImage = new QImage;
    fileName="D:/QtWork/ImgProcess/a.jpg";
    img->load(fileName);
    *oldImage = *img;

    m_width=img->width();
    m_height=img->height();
    imgLabel->setFixedSize(m_width,m_height);

    setFixedSize(m_width,m_height);
    imgLabel->setPixmap(QPixmap::fromImage(*img));

    statusBar();
    createAction();
    createMenu();
}

ImgProcess::~ImgProcess()
{

}
void ImgProcess::createAction()
 {
        open=new QAction(QIcon("D:/QtWork/ImgProcess/open.png"),"打开图片");
        open->setStatusTip("打开一个图片");
        open->setShortcut(tr("Ctrl+O"));
        connect(open,SIGNAL(triggered(bool)),this,SLOT(openfile()));

        close=new QAction("关闭文件");
        connect(close,SIGNAL(triggered(bool)),this,SLOT(closefile()));

        save=new QAction("保存");
        connect(save,SIGNAL(triggered(bool)),this,SLOT(savefile()));
        reshow=new QAction(QIcon("D:/QtWork/ImgProcess/new.png"),tr("重新显示"));
        connect(reshow,SIGNAL(triggered(bool)),this,SLOT(Rshow()));

        oppo=new QAction("反相");
        oppo->setStatusTip("图像反相");
        connect(oppo,SIGNAL(triggered(bool)),this,SLOT(oppoimg()));

        lastImg = new QAction("返回上一步");
        connect(lastImg,SIGNAL(triggered(bool)),this,SLOT(toLast()));

        roll=new QAction(QIcon("D:/QtWork/ImgProcess/rotate90.png"),"顺时针旋转");
        roll->setShortcut(tr("Ctrl+Left"));
        connect(roll,SIGNAL(triggered(bool)),this,SLOT(rollimg()));

        backRoll=new QAction(QIcon("D:/QtWork/ImgProcess/rotate180.png"),"逆时针旋转");
        backRoll->setShortcut(tr("Ctrl+Right"));
        connect(backRoll,SIGNAL(triggered(bool)),this,SLOT(backroll()));

        edge=new QAction("查找边缘");
        connect(edge,SIGNAL(triggered(bool)),this,SLOT(findedge()));

        gray=new QAction("灰度化");
        connect(gray,SIGNAL(triggered(bool)),this,SLOT(graying()));
        binImg=new QAction("二值化");
        connect(binImg,SIGNAL(triggered(bool)),this,SLOT(bining()));
}

void ImgProcess::createMenu()
 {
        file=menuBar()->addMenu("文件");
        file->addAction(open);
        file->addAction(close);
        file->addAction(save);
        file->addAction(reshow);

        edit=menuBar()->addMenu("编辑");
        edit->addAction(lastImg);
        edit->addAction(roll);
        edit->addAction(backRoll);

        imgProcess=menuBar()->addMenu("灰度图处理");
        imgProcess->addAction(oppo);
        imgProcess->addAction(edge);

        colorimg=menuBar()->addMenu("彩图处理");
        colorimg->addAction(gray);
        colorimg->addAction(roll);
        colorimg->addAction(backRoll);
        colorimg->addAction(binImg);
}

void ImgProcess::openfile()
{
    fileName=QFileDialog::getOpenFileName(this,tr("Open Image"), "F:/", tr("Image Files (*.bmp *.jpg)"));
   if(!fileName.isEmpty())
   {
    img->load(fileName);
    scaleImg();
    *oldImage = *img;
    imgLabel->setPixmap(QPixmap::fromImage(*img));
   }
}

void ImgProcess::scaleImg()
{
    m_width=img->width();
    m_height=img->height();

    if(m_width>this->width() || m_height>this->height())
    {
        if((m_width/this->m_width)>(m_height/this->m_height))
        {
            *img = img->scaled(this->width(), m_height*((float)this->width()/m_width));
            m_width=img->width();
            m_height=img->height();
        }
     else
        {
            *img = img->scaled(m_width*((float)this->height()/m_height), this->height());
            m_width=img->width();
            m_height=img->height();
        }
    }
}

void ImgProcess::oppoimg()
{
    *oldImage = *img;
    if(!fileName.isEmpty())
    {
    for (int i=0;i<m_width;i++)
    {
       for (int j=0;j<m_height;j++)
       {
           int red=qRed(img->pixel(i,j));
           int green=qGreen(img->pixel(i,j));
           int blue=qBlue(img->pixel(i,j));

           QColor color( 255-red,255-green,255-blue);
           img->setPixel(i,j,color.rgb());
       }
    }

}
    else
        QMessageBox::warning(this,"注意","未打开灰度图片");

    imgLabel->setPixmap(QPixmap::fromImage(*img));
}

void ImgProcess::rollimg()
{
    *oldImage = *img;
    QMatrix matrix;
    matrix.rotate(270);
    QImage *newimg=new QImage;
    *newimg=img->transformed(matrix);
    *img=*newimg;
    imgLabel->setPixmap(QPixmap::fromImage(*img));
    delete newimg;
}

void ImgProcess::backroll()
{
    *oldImage = *img;
    QMatrix matrix;
    matrix.rotate(90);
    QImage *newimg=new QImage;
    *newimg=img->transformed(matrix);
    *img=*newimg;
    imgLabel->setPixmap(QPixmap::fromImage(*img));
    delete newimg;
}

void ImgProcess::closefile()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("关闭图片");
    msgBox.setText("你正在关闭这个文件");
    msgBox.setInformativeText("是否关闭？");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret=msgBox.exec();
     switch (ret)
     {
     case QMessageBox::Cancel:
         msgBox.close();break;
     case QMessageBox::Ok:
         imgLabel->clear();
         delete img;
         break;
     }
}

void ImgProcess::findedge()
{
    *oldImage = *img;
    if(!fileName.isEmpty()&&img->allGray())
    {
      int r=0;
      EdgeNum *num=new EdgeNum();
      r=num->exec();

    if(r==1)
      {
      int number=num->getNum();
      showedge(number);
      }
    }
    else
        QMessageBox::warning(this,"注意","未打开灰度图片");
}
void ImgProcess::showedge(int number)
{
    *oldImage = *img;
    for (int i=0;i<m_width-1;i++)
    {
       for (int j=0;j<m_height;j++)
       {
           int grayL = qGray(img->pixel(i,j));
           int grayR = qGray(img->pixel(i,j+1));
           int gray;
           if(grayL-grayR > number)
             gray = 255;
       else
             gray = 0;

           QColor color(gray, gray, gray);
           img->setPixel(i,j,color.rgb());
       }
    }

    imgLabel->setPixmap(QPixmap::fromImage(*img));

}

void ImgProcess::Rshow()
{
    if(!fileName.isEmpty())
    {
    img->load(fileName);
    scaleImg();
    imgLabel->setPixmap(QPixmap::fromImage(*img));
    }
    else
        QMessageBox::warning(this,"注意","未打开图片");
}

void ImgProcess::graying()
{
    *oldImage = *img;
    QImage::Format f=img->format();

    if(f!=QImage::Format_Invalid)
    {
        for(int i=0;i<m_width;i++)
        {
            for(int j=0;j<m_height;j++)
            {
                int gray=qGray(img->pixel(i,j));
                QColor color(gray, gray, gray);
                img->setPixel(i,j,color.rgb());
            }
        }
    imgLabel->setPixmap(QPixmap::fromImage(*img));
    }
}

void ImgProcess::bining()
{
    *oldImage = *img;
    QImage::Format f=img->format();
    int value = Otsu(img);
    if(f!=QImage::Format_Invalid)
    {
        for(int i=0;i<m_width;i++)
        {
            for(int j=0;j<m_height;j++)
            {
                int gray=qGray(img->pixel(i,j));
                if(gray>=value)
                    gray=255;
                if(gray<value)
                    gray=0;
                QColor color(gray, gray, gray);
                img->setPixel(i,j,color.rgb());
            }
        }
    imgLabel->setPixmap(QPixmap::fromImage(*img));
    }
}

int ImgProcess::Otsu(QImage* img)
{
    if (NULL == img)
        {
            return -1;
        }
        std::vector<int> histogram = Histogram(img);
        int total = 0;
        for (int i = 0; i != histogram.size(); ++i)
        {
            total += histogram[i];
        }

        double sum = 0.0;
        for (unsigned int i = 1; i < histogram.size(); ++i)
            sum += i * histogram[i];
        double sumB = 0.0;
        double wB = 0.0;
        double wF = 0.0;
        double mB = 0.0;
        double mF = 0.0;
        double max = 0.0;
        double between = 0.0;
        double threshold = 0.0;
        for (int i = 0; i != 256; ++i)
        {
            wB += histogram[i];
            if (wB == 0)
                continue;
            wF = total - wB;
            if (wF == 0)
                break;
            sumB += i * histogram[i];
            mB = sumB / wB;
            mF = (sum - sumB) / wF;
            between = wB * wF * (mB - mF) * (mB - mF);
            if ( between > max )
            {
                threshold = i;
                max = between;
            }
        }
        return threshold;
}

std::vector<int> ImgProcess::Histogram( QImage* img)
{
    unsigned char* graydata = img->bits();
    std::vector<int> hist(256);    // 256色
    for (int i = 0; i != img->width(); ++i)
    {
        for (int j = 0; j != img->height(); ++j)
        {
            int index = int(*graydata);
            hist[index] += 1;
            graydata += 1;    // step
        }
    }
    graydata = NULL;
    return hist;
}

void ImgProcess::savefile()
{
    QMessageBox box;
    box.setWindowTitle("保存图片");
    box.setInformativeText("你要保存这个图片?");
    box.setStandardButtons(QMessageBox::Save|QMessageBox::Cancel);
    box.setDefaultButton(QMessageBox::Save);
    int ret = box.exec();
     if(ret==QMessageBox::Save)
     {
    fileName=QFileDialog::getSaveFileName(this,tr("保存图片"), "F:/", tr("(*.bmp);;(*.jpg)"));
    imgLabel->pixmap()->save(fileName);
    //img->save(fileName,0);
     }

}

void ImgProcess::toLast()
{
    *oldImage = *img;
    imgLabel->setPixmap(QPixmap::fromImage(*img));
}
