
#include <QtGui>
#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QAction>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QMenu>
#include<QMenuBar>
#include "workarea.h"

/////////////MainWindows()/////////////
MainWindow::MainWindow()
{
    workArea = new WorkArea;
    setCentralWidget(workArea);	
	
    createActions();
    createMenus();
    (void)statusBar();

    setWindowTitle(tr("Projet image"));

    resize(500, 500);
}

////////////////closeEvent()//////////////
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

////////////open file///////////////////
void MainWindow::open()
{	
	if (maybeSave()) 
	{
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
        fileName_backup = fileName;
		if (!fileName.isEmpty())
            workArea->openImage(fileName);
    }
}
/////////////reopen file//////////
void MainWindow::reopen()
{
	if (!fileName_backup.isEmpty())
		workArea->openImage(fileName_backup);
	else 
		QMessageBox::warning(this, tr("Reopen Image "), tr("Must open an image first"));	
}
/////////////save ///////////////
void MainWindow::saveAs()
{
    if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Save as Image "), tr("Must open an image first"));
		return;
	}
	QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

//////////about()/////////////
void MainWindow::about()
{
   QMessageBox::about(this, tr("About"), tr(" projet d'image 01/2008-02/2008 \n 1.Resize \n" 
						" 2.Fusion\n 3.Niveaux de gris\n 4.Flou image\n 5.Filtrage"
						"\n Reference:\n "
						"Rafael C.Gonzalez -- Digital Image Processing(Second Edition)"
						));
}
//////////////resize()/////////
void MainWindow::resize_image()
{
	bool ok_W ,ok_H;
    if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Resize Image "), tr("Must open an image first"));
		return;
	}
	
    int newWidth = QInputDialog::getInt(this, tr("Resize"),tr("Select new width:"),
                                            workArea->imageWidth(),
                                            1, 4096, 1, &ok_W);
    int newHeight = QInputDialog::getInt(this, tr("Resize"),tr("Select new height:"),
                                            workArea->imageHeight(),
                                            1, 2048, 1, &ok_H);
    
	if (ok_W && ok_H)
	{
        if(!workArea->resizeImage(newWidth, newHeight))
			QMessageBox::warning(this, tr("Resize Image"), tr(" Can't resize image! "));
	}
	
}

//////////grayscale_image()/////////
void MainWindow::grayscale_image()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Grayscale Image "), tr("Must open an image first!"));
		return;
	}
	
	if(!workArea->grayscaleImage())
		QMessageBox::warning(this, tr("Grayscale Image"), tr(" Can't  make grayscale image! "));

}

//////////add_image()///////////
void MainWindow::add_image()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Add Image "), tr("Must open an image first!"));
		return;
	}
		
	QString fileName = QFileDialog::getOpenFileName(this, tr("Add Image Open File"), QDir::currentPath());
    if (!fileName.isEmpty())
		workArea->addImage(fileName);
	
}

//////////blur_image()///////////
/////////nolinear_filter()////////
void MainWindow::nolinear_filter()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Blur Image_filter median "), tr("Must open an image first!"));
		return;
	}
	
	if(!workArea->medianFilter())
		QMessageBox::warning(this, tr("Blur image_filter median "), tr(" Can't  make blur image! "));
		
}
/////////linear_filter()////////
void MainWindow::linear_filter()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Blur Image_filter average "), tr("Must open an image first!"));
		return;
	}
	
	if(!workArea->averageFilter())
		QMessageBox::warning(this, tr("Blur image_filter average "), tr(" Can't  make blur image! "));
		
}

/////////////sharpen image/////////////////
////////////gradient()/////////////////////
void MainWindow::gradientSobel()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Sharpen Image_gradient sobel "), tr("Must open an image first!"));
		return;
	}
	if(!workArea->gradient_sobel())
		QMessageBox::warning(this, tr("Sharpen Image_gradient sobel "), tr(" Can't  make the gradient image! "));	

}
////////////laplacian()/////////////////////
void MainWindow::laplacian()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Sharpen Image_laplacian "), tr("Must open an image first!"));
		return;
	}
	if(!workArea->_Laplacian())
		QMessageBox::warning(this, tr("Sharpen image_laplacian "), tr(" Can't  make the laplacian image! "));

}
////////////edge detection////////////////
////////////gradientED()/////////////////////
void MainWindow::gradientSobelED()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Edge Detection_gradient sobel "), tr("Must open an image first!"));
		return;
	}
	if(!workArea->gradient_sobel_ED())
		QMessageBox::warning(this, tr("Edge Detection_gradient sobel "), tr(" Can't  make the gradient image! "));	

}
////////////laplacianED()/////////////////////
void MainWindow::laplacianED()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Edge Detection_laplacian "), tr("Must open an image first!"));
		return;
	}
	if(!workArea->laplacian_ED())
		QMessageBox::warning(this, tr("Edge Detection_laplacian "), tr(" Can't  make the laplacian image! "));

}

////////////pixel_image()//////////////////
void MainWindow::pixel_image()
{
	bool ok_W ,ok_H;
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Pixel Image "), tr("Must open an image first!"));
		return;
	}
	
    int pixel_x = QInputDialog::getInt(this, tr("Pixel_x"),tr("Select x pos :"),
                                            workArea->imageWidth()/2, 1, workArea->imageWidth(), 1, &ok_W);
    int pixel_y = QInputDialog::getInt(this, tr("Pixel_y"),tr("Select y pos :"),
                                            workArea->imageHeight()/2, 1, workArea->imageHeight(), 1, &ok_H);
											
	if(ok_W && ok_H)
	{
		if( !workArea->pixelImage(pixel_x, pixel_y) )
			QMessageBox::warning(this, tr("Pixel Image"), tr(" Can't get pixel of image! "));	
	}
}
//////////selection_image()///////
void MainWindow::selection_image()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Selection image "), tr("Must open an image first!"));
		return;
	}
	if(!workArea->selectionImage())
		QMessageBox::warning(this, tr("Selection Image"), tr(" Can't select image! "));

}
/////////Cut and Create New Image_image()//////////
void MainWindow::CCNI_image()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Cut and Create New Image "), tr("Must open an image first!"));
		return;
	}
	if(!workArea->CCNIImage())
		QMessageBox::warning(this, tr("Cut and Create New Image"), tr(" Can't cut and create new image! "));
		
}
///////////histogram_image ( )///////////////////
void MainWindow::histogram_image()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Histogram of image "), tr("Must open an image first!"));
		return;
	}
	if(workArea->histogramImage())
	{	
		QWidget *window = new QWidget;
		layout = new QGridLayout(window);
		if( workArea->imageFormat() == QImage::Format_Indexed8)
		{
			graphe_histogram(window, workArea->get_gray(),QString("Gray"),1);
		}
		else
		if(workArea->imageFormat() > QImage::Format_Indexed8)
		{
			graphe_histogram(window, workArea->get_red(),QString("Red"),1);
			graphe_histogram(window, workArea->get_green(),QString("Green"),4);
			graphe_histogram(window, workArea->get_blue(),QString("Blue"),7);
		}
		window->setLayout(layout);
		window->show();	
	}
}
void MainWindow::graphe_histogram(QWidget * window, int intensite[256],QString text,int niveau)
{	
		//tracage de graphe 
		QPushButton *button1 = new QPushButton(text);
		QGraphicsScene *scene= new QGraphicsScene;
		QGraphicsView *vue= new QGraphicsView(scene,window);
		QLabel *max_lab = new QLabel;
		int max=0;
		for (int i=0;i<256;i++)
			if( max < intensite[i] ) 
				max=intensite[i];
		
		max_lab->setNum(max);
			
		int j=255;
		for(int i=0;i<=255;i++)
		{
			scene->addLine(QLineF(j, intensite[i],j,0));
			j--;
		}

		vue->rotate(180);
		//scene->setSceneRect(0,0,255,max);
		vue->scale(1.0,(qreal)(100.0/(qreal)max));
		vue->setFixedSize(255,100);
		vue->setScene(scene);
		
		layout->addWidget(button1,niveau-1,0);
		layout->addWidget(vue,niveau,0);
		layout->addWidget(max_lab,niveau+1,0);
		
}
/////////egaliser()///////////////////////
void MainWindow::egaliser()
{
	if(workArea->imageIsNull())
	{
		QMessageBox::warning(this, tr("Egalise Image "), tr("Must open an image first!"));
		return;
	}
	if(!workArea->egaliser_histogram())
		QMessageBox::warning(this, tr("Egalise Image "), tr("ne egalise pas image "));
}
///////////////////////////////////
///////////createMenus()////////////
void MainWindow::createMenus()
{
    //save as menu
	saveAsMenu = new QMenu(tr("&Save As"), this);
    foreach (QAction *action, saveAsActs)
		saveAsMenu->addAction(action);
		
	//file menu
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
	fileMenu->addAction(reopenAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    
	//blurImage menu
	blurImageMenu = new QMenu(tr("&Blur Image"), this);
	blurImageMenu->addAction(linearFilterAct);
	blurImageMenu->addAction(nolinearFilterAct);
	
	//sharpenImage menu
	sharpenImageMenu = new QMenu(tr("&Sharpen Image"), this);
	sharpenImageMenu->addAction(gradientAct);
	sharpenImageMenu->addAction(laplacianAct);
	
	//image edge detection
	imageEDMenu = new QMenu(tr("&Edge Detection"), this);
	imageEDMenu->addAction(gradientEDAct);
	imageEDMenu->addAction(laplacianEDAct);
	
    //edit menu
    editMenu = new QMenu(tr("&Edit"), this);	
	editMenu->addAction(selectionAct);
	editMenu->addAction(CCNIAct);
	////////////////////////////////////
	editMenu->addSeparator();
	editMenu->addAction(pixelAct);
	editMenu->addAction(histogramAct);
	editMenu->addAction(egaliseAct);
	editMenu->addAction(grayscaleAct);
	editMenu->addAction(addImageAct);
	////////////////////////////////////
	editMenu->addSeparator();
	editMenu->addMenu(blurImageMenu);
	editMenu->addMenu(sharpenImageMenu);
	editMenu->addMenu(imageEDMenu);
	////////////////////////////////////
	editMenu->addSeparator();
	editMenu->addAction(resizeAct);
    
    //help menu
    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
	
	//add menus
	menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(helpMenu);
}

///////////////////createActions()/////////////
void MainWindow::createActions()
{
	//open action
    openAct = new QAction(tr("Open..."), this);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	
	//reopen action
    reopenAct = new QAction(tr("Reopen..."), this);
    reopenAct->setStatusTip(tr("Reopen an existing file"));
    connect(reopenAct, SIGNAL(triggered()), this, SLOT(reopen()));

	//save as action
    foreach (QByteArray format, QImageWriter::supportedImageFormats()) 
	{
        QString text = tr("%1...").arg(QString(format).toUpper());
        QAction *action = new QAction(text, this);
        action->setData(format);
		action->setStatusTip("save an " + text + " image");
        connect(action, SIGNAL(triggered()), this, SLOT(saveAs()));
        saveAsActs.append(action);
    }

	//exit action
    exitAct = new QAction(tr("Exit"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	//about action
    aboutAct = new QAction(tr("About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
	
	//resize action
	resizeAct = new QAction(tr("Resize"), this);
    resizeAct->setStatusTip(tr("Show the application's Resize"));
    connect(resizeAct, SIGNAL(triggered()), this, SLOT(resize_image()));

	//grayscale action
	grayscaleAct = new QAction(tr("Convert to Grayscale "), this);
    grayscaleAct->setStatusTip(tr("Show the application of grayscale image"));
    connect(grayscaleAct, SIGNAL(triggered()), this, SLOT(grayscale_image()));
	
	//addImage action
	addImageAct = new QAction(tr("Add an image "), this);
    addImageAct->setStatusTip(tr("Show the application of mixed image"));
    connect(addImageAct, SIGNAL(triggered()), this, SLOT(add_image()));
	
	//blurImage action
	linearFilterAct = new QAction(tr("Linear filter (filter average 3x3) "), this);
	linearFilterAct->setStatusTip(tr("Show the application of linear filter"));
	connect(linearFilterAct , SIGNAL(triggered()), this, SLOT(linear_filter()));
	
	nolinearFilterAct = new QAction(tr("Nolinear filter (filter median 3x3) "), this);
	nolinearFilterAct->setStatusTip(tr("Show the application of nolinear filter"));
	connect(nolinearFilterAct , SIGNAL(triggered()), this, SLOT(nolinear_filter()));
	
	blurImageActs.append(linearFilterAct);
	blurImageActs.append(nolinearFilterAct);
	
	//sharpenImage action
	gradientAct = new QAction(tr("Grandient (3x3 Sobel)"), this);
	gradientAct->setStatusTip(tr("Show the application of gradient method of sharpen"));
	connect(gradientAct , SIGNAL(triggered()), this, SLOT(gradientSobel()));
	
	laplacianAct = new QAction(tr("Laplacian (3x3) "), this);
	laplacianAct->setStatusTip(tr("Show the application of laplacian method of sharpen"));
	connect(laplacianAct , SIGNAL(triggered()), this, SLOT(laplacian()));
	
	sharpenImageActs.append(gradientAct);
	sharpenImageActs.append(laplacianAct);
	
	// edge detection
	gradientEDAct = new QAction(tr("Grandient (3x3 Sobel)"), this);
	gradientEDAct->setStatusTip(tr("Show the application of gradient methodof edge detection"));
	connect(gradientEDAct , SIGNAL(triggered()), this, SLOT(gradientSobelED()));
	
	laplacianEDAct = new QAction(tr("Laplacian (3x3) "), this);
	laplacianEDAct->setStatusTip(tr("Show the application of laplacian methodof edge detection"));
	connect(laplacianEDAct , SIGNAL(triggered()), this, SLOT(laplacianED()));
	
	imageEDActs.append(gradientEDAct);
	imageEDActs.append(laplacianEDAct);
	
	//pixel action
	pixelAct = new QAction(tr("Pixel Color"), this);
    pixelAct->setStatusTip(tr("Show the application of pixel"));
    connect(pixelAct, SIGNAL(triggered()), this, SLOT(pixel_image()));
	
	//selection action
	selectionAct = new QAction(tr("Selection an zone of image"), this);
    selectionAct->setStatusTip(tr("Show the application of selection"));
    connect(selectionAct, SIGNAL(triggered()), this, SLOT(selection_image()));
	
	//cut and create new image
	CCNIAct = new QAction(tr("Cut and create new image"), this);
    CCNIAct->setStatusTip(tr("Show the application of cut and create new image"));
    connect(CCNIAct, SIGNAL(triggered()), this, SLOT(CCNI_image()));
	
	//histogram action
	histogramAct = new QAction(tr("Histogramme"), this);
    histogramAct->setStatusTip(tr("Show the application of histogram"));
    connect(histogramAct, SIGNAL(triggered()), this, SLOT(histogram_image()));
	
	//egalise action
	egaliseAct = new QAction(tr("Egalisation"), this);
    egaliseAct->setStatusTip(tr("Show the application of egalisation"));
    connect(egaliseAct, SIGNAL(triggered()), this, SLOT(egaliser()));
	
	
}

//////////maybeSave()////////////
bool MainWindow::maybeSave()
{
    if (workArea->isModified()) 
	{
       QMessageBox::StandardButton ret;
       ret = QMessageBox::warning(this, tr("projet_image"),tr("The image has been modified.\n Do you want to save your changes?"),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes) 
		{
            return saveFile("png");
        } 
		else if (ret == QMessageBox::Cancel) 
		{
            return false;
        }
    }
    return true;
}

///////////saveFile(const QByteArray &fileFormat)////////////////
bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString(fileFormat.toUpper()))
                               .arg(QString(fileFormat)));
    if (fileName.isEmpty()) 
	{
        return false;
    } 
	else 
	{
        return workArea->saveImage(fileName, fileFormat);
    }
}




