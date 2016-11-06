
#include <QImage>
#include <QWidget>
#include <QtGui>
#include <QPoint>
#include <QRectF>

class WorkArea : public QWidget
{
    Q_OBJECT

public:
    WorkArea(QWidget *parent = 0);

    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    bool isModified() const { return modified; };
	int  imageWidth() {return image.width(); };
	int  imageHeight() {return image.height(); };
	bool imageIsNull() {return image.isNull(); };
	
	bool resizeImage(const int newWidth, const int newHeight);
	bool grayscaleImage();
	bool addImage(const QString &fileName);
	// blur image
	bool medianFilter();
	bool averageFilter();
	//sharpen image
	bool gradient_sobel();
	bool _Laplacian();
	
	//edge detection
	bool gradient_sobel_ED();
	bool laplacian_ED();
	
	//selection image
	bool selectionImage();
	//Cut and Create New Image 
	bool CCNIImage();
	
	bool pixelImage(const int pixel_x, const int pixel_y);
	
	//histogram
	bool histogramImage();
	bool egaliser_histogram();
	int* get_gray() {return gray_image; };
	int* get_red() {return red_image; };
	int* get_green() {return green_image; };
	int* get_blue() {return blue_image; };
	bool imageIsGris() {return image.isGrayscale(); };
	QImage::Format imageFormat() {return image.format(); };
	
	
public slots:


protected:

    void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
    
	QImage scaled_image(int newWidth, int newHeight);
	QRgb get_median_num(QVector<QRgb> filter_area);
	QRgb get_average_num(QVector<QRgb> filter_area);
	QRgb sobel_result(QVector<QRgb> sobelOp_area);
	QRgb laplacian_result(QVector<QRgb> laplacianOp_area);
	QRgb add2RGB(QRgb rgb1, QRgb rgb2, float tc);
	
	void drawRectangle(QPoint point_last);
    
	bool modified;
	bool selection;
	
	QPoint beginPoint;
	QPoint endPoint;
	QPoint lastPoint;

    QImage image;
	QImage image_tmp;
	QImage image_selection;
	
	int red_image[256];
	int green_image[256];
	int blue_image[256];
	int gray_image[256];
	float hist_p[256];
	float hist_c[256];

};


