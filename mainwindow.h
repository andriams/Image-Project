
#include <QMainWindow>
#include <QGridLayout>
#include <QtGui>
#include <QList>


class WorkArea;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
	
protected:
    void closeEvent(QCloseEvent *event);

private slots:       
    void open();
	void reopen();
    void saveAs();
    void about();
	void resize_image();
	void grayscale_image();
	void add_image();
	//blur image
	void nolinear_filter();
	void linear_filter();
	//sharpen image
	void gradientSobel();
	void laplacian();
	//edge detection
	void gradientSobelED();
	void laplacianED();
	
	void pixel_image();
	//selection an zone of image
	void selection_image();
	//cut and create new image
	void CCNI_image();
	
	//histogram
	void histogram_image();
	void graphe_histogram(QWidget * window, int intensite[256],QString text,int niveau);
	void egaliser();
	
private:
    void createActions();
    void createMenus();
    bool maybeSave();
    bool saveFile(const QByteArray &fileFormat);

	WorkArea *workArea;
	QString fileName_backup;
	QGridLayout *layout ;
	
	//list menu
	QMenu *saveAsMenu;
	QMenu *blurImageMenu;
	QMenu *sharpenImageMenu;
	QMenu *imageEDMenu;
	QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;

    //list action
    QAction *openAct;
	QAction *reopenAct;
    QList<QAction *> saveAsActs;
    QAction *exitAct;
    QAction *aboutAct;
	QAction *resizeAct;
	QAction *grayscaleAct;
	QAction *addImageAct;
	
	//blur image
	QList<QAction *> blurImageActs;
	QAction *linearFilterAct;
	QAction *nolinearFilterAct;
	
	// sharpen image
	QList<QAction *> sharpenImageActs;
	QAction *gradientAct;
	QAction *laplacianAct;
	
	//edge detection
	QList<QAction *> imageEDActs;
	QAction *gradientEDAct;
	QAction *laplacianEDAct;
	
	//selection 
	QAction *selectionAct;
	//cut and create new image
	QAction *CCNIAct;
	
	//histogram
	QAction *histogramAct;
	QAction *egaliseAct;
	//pixel of image
    QAction *pixelAct;
};




