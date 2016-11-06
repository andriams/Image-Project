
#include <QtWidgets>
#include <QMessageBox>
#include "workarea.h"

WorkArea::WorkArea(QWidget *parent): QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
	modified = false;
	selection = false;
	
}

bool WorkArea::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
	{
		QMessageBox::information(this, tr("Open Image "), tr("Cannot load %1.").arg(fileName));
        return false;
	}
	
	image = loadedImage;
    modified = false;
    update();
	
    return true;
}

bool WorkArea::saveImage(const QString &fileName, const char *fileFormat)
{

	QImage visibleImage = image;
    if (visibleImage.save(fileName, fileFormat)) 
	{
        modified = false;
        return true;
    } else 
	{
        return false;
    }
}

void WorkArea::paintEvent(QPaintEvent * )
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), image);
}

bool WorkArea::resizeImage(int newWidth, int newHeight)
{
	QImage newImage;
	//method best
	newImage = scaled_image(newWidth, newHeight);
	//method good
	//newImage = image.scaled(newWidth, newHeight, Qt::IgnoreAspectRatio, Qt::FastTransformation );
	if( newImage.isNull() )
		return false;
	if( !(newWidth == imageWidth() && newHeight == imageHeight()))
		modified = true;
	
	image = newImage;
	update();
	return true;
}

QImage WorkArea::scaled_image(int newWidth, int newHeight)
{
	QImage newImage(newWidth, newHeight, image.format ());
	
	int sw = imageWidth()- 1, sh = imageHeight() - 1, dw = newWidth - 1, dh = newHeight - 1;
    int B, N, x, y;
	int nPixelSize = 0;
	
	if(newImage.format() == QImage::Format_Mono || newImage.format() == QImage::Format_MonoLSB )
		nPixelSize = 1;
	else if (newImage.format() == QImage::Format_Indexed8)
	{
		newImage.setColorTable(image.colorTable());
		nPixelSize = 1;
	}
	else if (newImage.format() > QImage::Format_Indexed8)
		nPixelSize = 4;

    uchar *pLinePrev, *pLineNext;
    uchar *pDest;
    uchar *pA, *pB, *pC, *pD;
	
    for ( int i = 0; i <= dh; ++i )
    {
        pDest = newImage.scanLine(i);

        y = i * sh / dh;
        N = dh - i * sh % dh;
        pLinePrev = image.scanLine(y++);
        pLineNext = ( N == dh ) ? pLinePrev : image.scanLine(y);

        for ( int j = 0; j <= dw; ++j )
        {
            x = j * sw / dw * nPixelSize;
            B = dw - j * sw % dw;
            pA = pLinePrev + x;
            pB = pA + nPixelSize;
            pC = pLineNext + x;
            pD = pC + nPixelSize;
            if ( B == dw )
            {
                pB = pA;
                pD = pC;
            }

            for ( int k = 0; k < nPixelSize; ++k )

                *pDest++ = ( uchar )( int )(
                    ( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
                    + dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
                    + dw * dh / 2 ) / ( dw * dh )
                );
        }
    }
    return newImage;

}

bool WorkArea::grayscaleImage()
{
	if( image.format() == QImage::Format_Indexed8)
		return true;
	
	QImage new_image(imageWidth(), imageHeight(), QImage::Format_Indexed8 );
	QColor *tmp1;
	int val;
	
	// color table
	QVector<QRgb> color_table(0);
	QRgb val_rgb;
	for(int k = 0; k < 256; ++k)
	{
		val_rgb = qRgb(k,k,k);
		color_table.append(val_rgb);
	}
	new_image.setColorTable(color_table);
	
	//grayscale image
	for(int i=0; i<imageWidth(); i++)
		for(int j=0; j<imageHeight(); j++)
		{
			tmp1 = new QColor(image.pixel(i, j));
			val = (int) ((tmp1->red()*30 + tmp1->green()*59 + tmp1->blue()*11  + 50 )  / 100 );
			new_image.setPixel(i, j, val );
		}
	if(new_image.isNull())
		return false;
	else
	{
		image = new_image;
		modified = true;
		update();
	}
	return true;
}


bool WorkArea::addImage(const QString &fileName)
{
	QImage image1,image2,image_tmp;
	image1 = image;
    if (!image2.load(fileName))
	{
		QMessageBox::information(this, tr("Open Image "), tr("Cannot load %1.").arg(fileName));
        return false;
	}
	
	int image1_w = image1.width();
	int image1_h = image1.height();
	int image2_w = image2.width();
	int image2_h = image2.height();
	int mixedimage_w = 0;
	int mixedimage_h = 0;
	QImage::Format format_mixed ;
	
	mixedimage_w = (image1_w >= image2_w) ? image1_w : image2_w;
	mixedimage_h = (image1_h >= image2_h) ? image1_h : image2_h;
	
	// 32bit+   --> 8bit
	if(image1.format() > QImage::Format_Indexed8 && image2.format() == QImage::Format_Indexed8)
	{
		format_mixed = image2.format(); 
		image1  = image1.convertToFormat(image2.format(), image2.colorTable(), Qt::AutoColor);
	}else if(image2.format() > QImage::Format_Indexed8 && image1.format() == QImage::Format_Indexed8)
	{
		format_mixed = image1.format();
		image2  = image2.convertToFormat(image1.format(), image1.colorTable(), Qt::AutoColor);
	}
	//32bit+   -->  32bits+
	else if (image1.format() >= image2.format() && image2.format() > QImage::Format_Indexed8 )
	{
		format_mixed = image2.format();
		image1 = image1.convertToFormat(image2.format(), Qt::AutoColor );	
	}else if (image2.format() > image1.format() && image1.format() > QImage::Format_Indexed8 )
	{
		format_mixed = image1.format();
		image2  = image2.convertToFormat(image1.format(), Qt::AutoColor );
	}
	// 8 bit --> 8 bit
	else if (image2.format() == image1.format() && image1.format() == QImage::Format_Indexed8)
	{
		format_mixed = image1.format();
	}
	
	QImage mixedImage(mixedimage_w, mixedimage_h, format_mixed );
	QColor *tmp1, *tmp2, *tmp3;
	int tmp7, tmp8;
	uint tmp9;
	
	if( format_mixed > QImage::Format_Indexed8 )
	{
		for(int i=0; i<mixedimage_w; i++)
			for(int j=0; j<mixedimage_h; j++)
			{
				if(i >= image1_w || j >= image1_h && i <= image2_w && j <= image2_h)
					tmp1 = new QColor(image2.pixel(i, j));
				else 
					tmp1 = new QColor(image1.pixel(i, j));
			
				if(i >= image2_w || j >= image2_h && i <= image1_w && j <= image1_h)
					tmp2 = new QColor(image1.pixel(i, j));
				else
					tmp2 = new QColor(image2.pixel(i, j));
				
				if((i >= image2_w && j >= image1_h) || (i >= image1_w && j >= image2_h))
					tmp1 = tmp2 = new QColor;

				tmp3 = new QColor(  (int) ((tmp1->red() + tmp2->red())/2), 
									(int) ((tmp1->green() + tmp2->green())/2), 
									(int) ((tmp1->blue() + tmp2->blue())/2), 255);	
				mixedImage.setPixel(i, j, tmp3->rgb () );
			}
	}else if ( format_mixed == QImage::Format_Indexed8)	
	{
		mixedImage.setColorTable(image1.colorTable());
		for(int i=0; i<mixedimage_w; i++)
			for(int j=0; j<mixedimage_h; j++)
			{
				if(i >= image1_w || j >= image1_h && i <= image2_w && j <= image2_h)
					tmp7 = image2.pixelIndex(i, j);
				else 
					tmp7 = image1.pixelIndex(i, j);
			
				if(i >= image2_w || j >= image2_h && i <= image1_w && j <= image1_h)
					tmp8 = image1.pixelIndex(i, j);
				else
					tmp8 = image2.pixelIndex(i, j);
				
				if((i >= image2_w && j >= image1_h) || (i >= image1_w && j >= image2_h))
					tmp7 = tmp8 = 0;

				tmp9 = (uint) ((tmp7 + tmp8)/2);	
				mixedImage.setPixel(i, j, tmp9 );
			}	
	}
	
	image = mixedImage;
    modified = true;
	update();
    return true;
	
}

///////////////////////////////////////
bool WorkArea::medianFilter()
{
	QImage imageSrc,imageDet;
	imageSrc = imageDet = image;
	int imageDet_w = imageWidth();
	int imageDet_h = imageHeight();
	
	//filter 3 X 3
	int filter_w = 3;
	int filter_h = 3;
	int filter_middle_w = 1;
	int filter_middle_h = 1;
	
	QVector<QRgb> filter_area(filter_w * filter_h);
	QRgb tmp_rgb;
	QColor color_tmp;
	int count_tmp = 0;
	for(int i = filter_middle_h; i < imageDet_h - filter_h + filter_middle_h + 1; i++)
		for(int j = filter_middle_w; j < imageDet_w - filter_w + filter_middle_w + 1; j++)
		{
			//get 3 X 3 pixels
			for(int k = 0 ; k < filter_h ; k++)
				for(int l = 0; l < filter_w; l++ )
				{
					tmp_rgb = imageSrc.pixel( j - filter_middle_w + l , i - filter_middle_h + k  ); 
					filter_area.replace(count_tmp, tmp_rgb);
					count_tmp ++;
				}
			count_tmp = 0;	
			
			if( imageDet.format() > QImage::Format_Indexed8 )
				imageDet.setPixel(j, i, get_median_num(filter_area));
			else
			// if format = Format_Indexed8 
			{
				color_tmp = QColor(get_median_num(filter_area));
				imageDet.setPixel( j, i, color_tmp.red() );
			}
		}
		
	if( imageDet.isNull() || imageDet == imageSrc )
		return false;
	image = imageDet;
    modified = true;
	update();
    return true;
}

QRgb WorkArea::get_median_num(QVector<QRgb> filter_area)
{
	
	QColor color, color_median;
	int R, G, B;
	int tmp;
	QVector<int> red(0), green(0), blue(0);
	for(int k = 0; k < filter_area.size(); k++)
	{
		color = QColor(filter_area.value(k));
		red.append( color.red() );
		green.append( color.green() );
		blue.append( color.blue() );
	}
	
	for(int i = 0; i < filter_area.size()-1; i++)    
		for(int j = 0; j < filter_area.size()-i-1; j++)  
		{  
			
			if( red.value(j) > red.value(j+1) )
			{
				tmp = red.value(j);
				red.replace( j, red.value(j+1) );
				red.replace( j + 1,tmp );
			}
			if( green.value(j) > green.value(j+1) )
			{
				tmp = green.value(j);
				green.replace( j, green.value(j+1) );
				green.replace( j + 1,tmp );
			}
			if( blue.value(j) > blue.value(j+1) )
			{
				tmp = blue.value(j);
				blue.replace( j, blue.value(j+1) );
				blue.replace( j + 1,tmp );
			}	
		}
	if( (filter_area.size() & 1) > 0)
	{
		R = red.value( (filter_area.size() +1 )/2);
		G = green.value( (filter_area.size() +1 )/2);
		B = blue.value( (filter_area.size() +1 )/2);
	}
	else
	{
		R = ( red.value( filter_area.size()/2 + 1 ) + 
				red.value( filter_area.size()/2) ) / 2 ;
		G = ( green.value( filter_area.size()/2 + 1 ) + 
				green.value( filter_area.size()/2) ) / 2 ;	
		B = ( blue.value( filter_area.size()/2 + 1 ) + 
				blue.value( filter_area.size()/2) ) / 2 ;	
	}
				
	color_median = QColor(R, G, B, 255);
	return color_median.rgb() ;

}
////////////////////////////////////////
bool WorkArea::averageFilter()
{
	QImage imageSrc,imageDet;
	imageSrc = imageDet = image;
	int imageDet_w = imageWidth();
	int imageDet_h = imageHeight();
	
	//filter 3 X 3
	int filter_w = 3;
	int filter_h = 3;
	int filter_middle_w = 1;
	int filter_middle_h = 1;
	
	QVector<QRgb> filter_area(filter_w * filter_h);
	QRgb tmp_rgb;
	QColor color_tmp;
	int count_tmp = 0;
	for(int i = filter_middle_h; i < imageDet_h - filter_h + filter_middle_h + 1; i++)
		for(int j = filter_middle_w; j < imageDet_w - filter_w + filter_middle_w + 1; j++)
		{
			//get 3 X 3 pixels
			for(int k = 0 ; k < filter_h ; k++)
				for(int l = 0; l < filter_w; l++ )
				{
					tmp_rgb = imageSrc.pixel( j - filter_middle_w + l , i - filter_middle_h + k  ); 
					filter_area.replace(count_tmp, tmp_rgb);
					count_tmp ++;
				}
			count_tmp = 0;	
			
			if( imageDet.format() > QImage::Format_Indexed8 )
				imageDet.setPixel(j, i, get_average_num(filter_area));
			else
			// if format = Format_Indexed8 
			{
				color_tmp = QColor(get_average_num(filter_area));
				imageDet.setPixel( j, i, color_tmp.red() );
			}
		}
		
	if( imageDet.isNull() || imageDet == imageSrc )
		return false;
	image = imageDet;
    modified = true;
	update();
    return true;

}

QRgb WorkArea::get_average_num(QVector<QRgb> filter_area)
{
	QColor color, color_average;
	int R=0, G=0, B=0;
	QVector<int> red(0), green(0), blue(0);
	for(int k = 0; k < filter_area.size(); k++)
	{
		color = QColor(filter_area.value(k));
		red.append( color.red() );
		green.append( color.green() );
		blue.append( color.blue() );
	}
	for(int i = 0; i < filter_area.size(); i++)
	{
		R = R + red.value(i);
		G = G + green.value(i);
		B = B + blue.value(i);
	}
	R = R/filter_area.size();
	G = G/filter_area.size();
	B = B/filter_area.size();
	color_average = QColor(R, G, B, 255);
	return color_average.rgb() ;
}

/////////////////////////////////////////
bool WorkArea::gradient_sobel()
{
	QImage imageSrc,imageDet;
	imageSrc = imageDet = image;
	int imageDet_w = imageWidth();
	int imageDet_h = imageHeight();
	
	// sobel operateur 3 X 3
	int sobelOp_w = 3;
	int sobelOp_h = 3;
	int sobelOp_middle_w = 1;
	int sobelOp_middle_h = 1;

	QVector<QRgb> sobelOp_area(sobelOp_w * sobelOp_h);
	QRgb tmp_rgb;
	QColor color_tmp,color_tmpSrc ;
	int count_tmp = 0 ,tmp = 0;
	
	for(int i = sobelOp_middle_h; i < imageDet_h - sobelOp_h + sobelOp_middle_h + 1; i++)
		for(int j = sobelOp_middle_w; j < imageDet_w - sobelOp_w + sobelOp_middle_w + 1; j++)
		{
			//get 3 X 3 pixels
			for(int k = 0 ; k < sobelOp_h ; k++)
				for(int l = 0; l < sobelOp_w; l++ )
				{
					tmp_rgb = imageSrc.pixel( j - sobelOp_middle_w + l , i - sobelOp_middle_h + k  ); 
					sobelOp_area.replace(count_tmp, tmp_rgb);
					count_tmp ++;
				}
			count_tmp = 0;	
			
			if( imageDet.format() > QImage::Format_Indexed8 )
			{
				tmp_rgb = add2RGB( imageSrc.pixel(j,i), sobel_result(sobelOp_area), 0.1);
				imageDet.setPixel(j, i, tmp_rgb );
			}
			else
			// if format = Format_Indexed8 
			{
				
				color_tmp = QColor(sobel_result(sobelOp_area));
				color_tmpSrc = QColor( imageSrc.pixel(j,i));
				tmp = color_tmp.red()/10 + color_tmpSrc.red();
				if(tmp > 255) tmp = 255;
				if(tmp < 0) tmp=0;
 				imageDet.setPixel( j, i,  (uint)tmp );
			}
		}
		//
	if( imageDet.isNull()  || imageDet == imageSrc)
		return false;	
	image = imageDet;
    modified = true;
	update();
	return true;
}
//matice 3x3
//[ -1   -2    -1 ]    [-1    0    1]
//[ 0     0    0 ]   [-2    0    2]
//[ 1     2     1 ]    [-1     0    1]  
/*
QRgb WorkArea::sobel_result(QVector<QRgb> sobelOp_area)
{
	QRgb z1 = sobelOp_area.value(0);
	QRgb z2 = sobelOp_area.value(1);
	QRgb z3 = sobelOp_area.value(2);
	QRgb z4 = sobelOp_area.value(3);
	//QRgb z5 = sobelOp_area.value(4);
	QRgb z6 = sobelOp_area.value(5);
	QRgb z7 = sobelOp_area.value(6);
	QRgb z8 = sobelOp_area.value(7);
	QRgb z9 = sobelOp_area.value(8);
	QRgb z789 = z7 + 2*z8 + z9;
	QRgb z123 = z1 + 2*z2 + z3;
	QRgb z369 = z3 + 2*z6 + z9;
	QRgb z147 = z1 + 2*z4 + z7;
	
	QRgb Gx_abs = (z789 > z123) ? (z789 - z123) : (z123 - z789);
	QRgb Gy_abs = (z369 > z147) ? (z369 - z147) : (z147 - z369);

	QRgb val_sobel = Gx_abs + Gy_abs;
	return val_sobel ;	
}
*/

QRgb WorkArea::sobel_result(QVector<QRgb> sobelOp_area)
{
	QColor color, color_result;
	int Rx, Gx, Bx, Rx_tmp, Gx_tmp, Bx_tmp, Rx_abs, Gx_abs, Bx_abs;
	int Ry, Gy, By, Ry_tmp, Gy_tmp, By_tmp, Ry_abs, Gy_abs, By_abs;
	int Rxy, Gxy, Bxy;
	QVector<int> red(0), green(0), blue(0);
	for(int k = 0; k < sobelOp_area.size(); k++)
	{
		color = QColor(sobelOp_area.value(k));
		red.append( color.red() );
		green.append( color.green() );
		blue.append( color.blue() );
	}
	Rx_tmp = red.value(6) + 2*red.value(7) + red.value(8) - (red.value(0) + 2*red.value(1) + red.value(2));
	Gx_tmp = green.value(6) + 2*green.value(7) + green.value(8) - (green.value(0) + 2*green.value(1) + green.value(2));
	Bx_tmp = blue.value(6) + 2*blue.value(7) + blue.value(8) - (blue.value(0) + 2*blue.value(1) + blue.value(2));	
	Rx_abs = qAbs(Rx_tmp);
	Gx_abs = qAbs(Gx_tmp);
	Bx_abs = qAbs(Bx_tmp);
	Rx = ( Rx_abs > 255) ? 255 : ( (Rx_abs < 0) ? 0 : Rx_abs ) ;
	Gx = ( Gx_abs > 255) ? 255 : ( (Gx_abs < 0) ? 0 : Gx_abs ) ;
	Bx = ( Bx_abs > 255) ? 255 : ( (Bx_abs < 0) ? 0 : Bx_abs ) ;
	
	Ry_tmp = red.value(2) + 2*red.value(5) + red.value(8) - (red.value(0) + 2*red.value(3) + red.value(6));
	Gy_tmp = green.value(2) + 2*green.value(5) + green.value(8) - (green.value(0) + 2*green.value(3) + green.value(6));
	By_tmp = blue.value(2) + 2*blue.value(5) + blue.value(8) - (blue.value(0) + 2*blue.value(3) + blue.value(6));			
	Ry_abs = qAbs(Ry_tmp);
	Gy_abs = qAbs(Gy_tmp);
	By_abs = qAbs(By_tmp);
	Ry = ( Ry_abs > 255) ? 255 : ( (Ry_abs < 0) ? 0 : Ry_abs ) ;
	Gy = ( Gy_abs > 255) ? 255 : ( (Gy_abs < 0) ? 0 : Gy_abs ) ;
	By = ( By_abs > 255) ? 255 : ( (By_abs < 0) ? 0 : By_abs ) ;
	
	Rxy = ( Rx+Ry > 255) ? 255 : (Rx+Ry);
	Gxy = ( Gx+Gy > 255) ? 255 : (Gx+Gy);
	Bxy = ( Bx+By > 255) ? 255 : (Bx+By);
	color_result = QColor( Rxy, Gxy, Bxy, 255);
	return color_result.rgb() ;
}

////////////////////////////////////////////////////
bool WorkArea::_Laplacian()
{
	QImage imageSrc,imageDet;
	imageSrc = imageDet = image;
	int imageDet_w = imageWidth();
	int imageDet_h = imageHeight();
	
	// Laplacian operateur 3 X 3
	int laplacianOp_w = 3;
	int laplacianOp_h = 3;
	int laplacianOp_middle_w = 1;
	int laplacianOp_middle_h = 1;

	QVector<QRgb> laplacianOp_area(laplacianOp_w * laplacianOp_h);
	QRgb tmp_rgb;
	QColor color_tmp,color_tmpSrc;
	int count_tmp = 0, tmp;
	
	for(int i = laplacianOp_middle_h; i < imageDet_h - laplacianOp_h + laplacianOp_middle_h + 1; i++)
		for(int j = laplacianOp_middle_w; j < imageDet_w - laplacianOp_w + laplacianOp_middle_w + 1; j++)
		{
			//get 3 X 3 pixels
			for(int k = 0 ; k < laplacianOp_h ; k++)
				for(int l = 0; l < laplacianOp_w; l++ )
				{
					tmp_rgb = imageSrc.pixel( j - laplacianOp_middle_w + l , i - laplacianOp_middle_h + k  ); 
					laplacianOp_area.replace(count_tmp, tmp_rgb);
					count_tmp ++;
				}
			count_tmp = 0;	
			
			if( imageDet.format() > QImage::Format_Indexed8 )
			{
				tmp_rgb = add2RGB( imageSrc.pixel(j,i), laplacian_result(laplacianOp_area), 0.1);
				imageDet.setPixel(j, i, tmp_rgb );
			}
			else
			// if format = Format_Indexed8 
			{
				color_tmp = QColor(laplacian_result(laplacianOp_area));
				color_tmpSrc = QColor( imageSrc.pixel(j,i));
				tmp = color_tmp.red()/10 + color_tmpSrc.red();
				if(tmp > 255) tmp = 255;
				if(tmp < 0) tmp=0;
				imageDet.setPixel( j, i, (uint)tmp );
			}
		}
	//
	if( imageDet.isNull() || imageDet == imageSrc )
		return false;	
	image = imageDet;
    modified = true;
	update();
	return true;
}
QRgb WorkArea::add2RGB(QRgb rgb1, QRgb rgb2, float tc)
{
	QColor C1 = QColor(rgb1);
	QColor C2 = QColor(rgb2);
	
	int red1 = C1.red();
	int red2 = C2.red();
	red2 = (int) (red2 * tc) ;
	int green1 = C1.green();
	int green2 = C2.green();
	green2 =(int) (green2 * tc);
	int blue1 = C1.blue();
	int blue2 = C2.blue();
	blue2 = (int) (blue2 * tc);
	
	int tmp_red = red1 + red2;
	if (tmp_red>255) tmp_red = 255;
	if (tmp_red<0) tmp_red = 0;
	
	int tmp_green = green1 + green2;
	if (tmp_green>255) tmp_green = 255;
	if (tmp_green<0) tmp_green = 0;
	
	int tmp_blue = blue1 + blue2;
	if (tmp_blue>255) tmp_blue = 255;
	if (tmp_blue<0) tmp_blue = 0;
	
	QColor tmp = QColor(tmp_red,tmp_green,tmp_blue);
	return tmp.rgb();
}
/*
//marice 
//[ 0   -1    0 ]    [-1   -1    -1]
//[-1    4   -1]    [-1    8    -1]
//[0    -1     0]    [-1    -1   -1]
QRgb WorkArea::laplacian_result(QVector<QRgb> laplacianOp_area)
{
	QRgb z1 = laplacianOp_area.value(0);
	QRgb z2 = laplacianOp_area.value(1);
	QRgb z3 = laplacianOp_area.value(2);
	QRgb z4 = laplacianOp_area.value(3);
	QRgb z5 = laplacianOp_area.value(4);
	QRgb z6 = laplacianOp_area.value(5);
	QRgb z7 = laplacianOp_area.value(6);
	QRgb z8 = laplacianOp_area.value(7);
	QRgb z9 = laplacianOp_area.value(8);
	QRgb z2468 = z2 + z4 + z6 + z8;
	QRgb z12346789 =  z1+z2+z3+z4+z6+z7+z8+z9;
	QRgb Gx_abs = (4*z5 > z2468) ? (4*z5 - z2468) : (z2468 - 4*z5); 
	QRgb Gy_abs = (8*z5 > z12346789) ? (8*z5 - z12346789) : (z12346789 - 8*z5);
	QRgb val_laplacian = Gx_abs + Gy_abs;
	return val_laplacian;
}
*/
//marice 
//[ 0   -1    0 ]    [-1   -1    -1]
//[-1    4   -1]    [-1    8   -1]
//[0    -1     0]    [-1    -1   -1]
QRgb WorkArea::laplacian_result(QVector<QRgb> laplacianOp_area)
{
	QColor color, color_result;
	int Rx, Gx, Bx, Rx_tmp, Gx_tmp, Bx_tmp, Rx_abs, Gx_abs, Bx_abs;
	int Ry, Gy, By, Ry_tmp, Gy_tmp, By_tmp, Ry_abs, Gy_abs, By_abs;
	int Rxy, Gxy, Bxy;
	QVector<int> red(0), green(0), blue(0);
	for(int k = 0; k < laplacianOp_area.size(); k++)
	{
		color = QColor(laplacianOp_area.value(k));
		red.append( color.red() );
		green.append( color.green() );
		blue.append( color.blue() );
	}
	
	Rx_tmp = 4*red.value(4) - red.value(1) - red.value(3) - red.value(5) - red.value(7) ;
	Gx_tmp = 4*green.value(4) - green.value(1) - green.value(3) - green.value(5) - green.value(7) ;
	Bx_tmp = 4*blue.value(4) - blue.value(1) - blue.value(3) - blue.value(5) - blue.value(7) ;	
	Rx_abs = qAbs(Rx_tmp);
	Gx_abs = qAbs(Gx_tmp);
	Bx_abs = qAbs(Bx_tmp);
	Rx = ( Rx_abs > 255) ? 255 : ( (Rx_abs < 0) ? 0 : Rx_abs ) ;
	Gx = ( Gx_abs > 255) ? 255 : ( (Gx_abs < 0) ? 0 : Gx_abs ) ;
	Bx = ( Bx_abs > 255) ? 255 : ( (Bx_abs < 0) ? 0 : Bx_abs ) ;
	
	Ry_tmp = 8*red.value(4) - red.value(0) - red.value(1) - red.value(2) - red.value(3) - red.value(5) 
					- red.value(6) - red.value(7) - red.value(8) ;
	Gy_tmp = 8*green.value(4) - green.value(0) - green.value(1) - green.value(2) - green.value(3) 
				- green.value(5) - green.value(6) - green.value(7) - green.value(8);
	By_tmp = 8*blue.value(4) - blue.value(0) - blue.value(1) - blue.value(2) - blue.value(3)
					- blue.value(5) - blue.value(6) - blue.value(7) - blue.value(8);	
	Ry_abs = qAbs(Ry_tmp);
	Gy_abs = qAbs(Gy_tmp);
	By_abs = qAbs(By_tmp);
	Ry = ( Ry_abs > 255) ? 255 : ( (Ry_abs < 0) ? 0 : Ry_abs ) ;
	Gy = ( Gy_abs > 255) ? 255 : ( (Gy_abs < 0) ? 0 : Gy_abs ) ;
	By = ( By_abs > 255) ? 255 : ( (By_abs < 0) ? 0 : By_abs ) ;

	Rxy = ( Rx+Ry > 255) ? 255 : (Rx+Ry);
	Gxy = ( Gx+Gy > 255) ? 255 : (Gx+Gy);
	Bxy = ( Bx+By > 255) ? 255 : (Bx+By);
	color_result = QColor( Rxy, Gxy, Bxy, 255);
	return color_result.rgb() ;
	
}

////////////////////////////////////////////////////
bool WorkArea::gradient_sobel_ED()
{
	QImage imageSrc,imageDet;
	imageSrc = imageDet = image;
	int imageDet_w = imageWidth();
	int imageDet_h = imageHeight();
	
	// sobel operateur 3 X 3
	int sobelOp_w = 3;
	int sobelOp_h = 3;
	int sobelOp_middle_w = 1;
	int sobelOp_middle_h = 1;

	QVector<QRgb> sobelOp_area(sobelOp_w * sobelOp_h);
	QRgb tmp_rgb;
	QColor color_tmp ;
	int count_tmp = 0 ;
	
	for(int i = sobelOp_middle_h; i < imageDet_h - sobelOp_h + sobelOp_middle_h + 1; i++)
		for(int j = sobelOp_middle_w; j < imageDet_w - sobelOp_w + sobelOp_middle_w + 1; j++)
		{
			//get 3 X 3 pixels
			for(int k = 0 ; k < sobelOp_h ; k++)
				for(int l = 0; l < sobelOp_w; l++ )
				{
					tmp_rgb = imageSrc.pixel( j - sobelOp_middle_w + l , i - sobelOp_middle_h + k  ); 
					sobelOp_area.replace(count_tmp, tmp_rgb);
					count_tmp ++;
				}
			count_tmp = 0;	
			
			if( imageDet.format() > QImage::Format_Indexed8 )
			{
				imageDet.setPixel(j, i, sobel_result(sobelOp_area) );
			}
			else
			// if format = Format_Indexed8 
			{
 				color_tmp = QColor(sobel_result(sobelOp_area));
				imageDet.setPixel( j, i, color_tmp.red() );
			}
		}
		
	if( imageDet.isNull() || imageDet == imageSrc )
		return false;	
	image = imageDet;
    modified = true;
	update();
	return true;
}
////////////////////////////////////////////////////
bool WorkArea::laplacian_ED()
{
	QImage imageSrc,imageDet;
	imageSrc = imageDet = image;
	int imageDet_w = imageWidth();
	int imageDet_h = imageHeight();
	
	// Laplacian operateur 3 X 3
	int laplacianOp_w = 3;
	int laplacianOp_h = 3;
	int laplacianOp_middle_w = 1;
	int laplacianOp_middle_h = 1;

	QVector<QRgb> laplacianOp_area(laplacianOp_w * laplacianOp_h);
	QRgb tmp_rgb;
	QColor color_tmp;
	int count_tmp = 0;
	
	for(int i = laplacianOp_middle_h; i < imageDet_h - laplacianOp_h + laplacianOp_middle_h + 1; i++)
		for(int j = laplacianOp_middle_w; j < imageDet_w - laplacianOp_w + laplacianOp_middle_w + 1; j++)
		{
			//get 3 X 3 pixels
			for(int k = 0 ; k < laplacianOp_h ; k++)
				for(int l = 0; l < laplacianOp_w; l++ )
				{
					tmp_rgb = imageSrc.pixel( j - laplacianOp_middle_w + l , i - laplacianOp_middle_h + k  ); 
					laplacianOp_area.replace(count_tmp, tmp_rgb);
					count_tmp ++;
				}
			count_tmp = 0;	
			
			if( imageDet.format() > QImage::Format_Indexed8 )
			{
				imageDet.setPixel(j, i, laplacian_result(laplacianOp_area) );
			}
			else
			// if format = Format_Indexed8 
			{
				color_tmp = QColor(laplacian_result(laplacianOp_area));
				imageDet.setPixel( j, i, color_tmp.red());
			}
		}
	
	if( imageDet.isNull() || imageDet == imageSrc )
		return false;	
	image = imageDet;
    modified = true;
	update();
	return true;
}
////////////////////////////////////////////////
bool WorkArea::selectionImage()
{
	selection = true;

	image_tmp = image;
	if(selection)
	{
		modified = true;
		return true;	
	}
	else
		return false;
}
void WorkArea::mousePressEvent(QMouseEvent *event)
{
	if( event->button() == Qt::LeftButton && selection)
	{
		beginPoint = event->pos();
	}
}
void WorkArea::mouseMoveEvent(QMouseEvent *event)
{
	if( (event->buttons() & Qt::LeftButton) && selection )
		//drawRectangle(event->pos())
		;
	
}
void WorkArea::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton && selection)
	{
		endPoint = event->pos();
		drawRectangle(event->pos());
		selection = false;
	}
}
void WorkArea::drawRectangle(QPoint point)
{
	QPainter painter(&image);
	QRect rectangle(beginPoint, point);
	painter.drawRect(rectangle);
	lastPoint = point;
	update();
}
////////////////////////////////////////////////////
bool WorkArea::CCNIImage()
{
	if(beginPoint.isNull() || endPoint.isNull())
		return false;
	QRect rectangle(beginPoint, endPoint);
	QImage newImage = image.copy(rectangle);

	if(image != newImage && !newImage.isNull() )
	{
		image = newImage;
		modified = true;
		update();
		return true;
	}
	else
		return false;
}
////////////////////////////////////////////////////
bool WorkArea::pixelImage(int pic_x,int pic_y)
{
	QColor *c=new QColor(image.pixel(pic_x,pic_y));

	QWidget* mywidget1=new QWidget();
	mywidget1->setGeometry(100,100,150,100);
	
	QLabel *l_red =new QLabel ("red :");
	QLabel *aff_red=new QLabel();
	aff_red->setNum(c->red());
	QLabel *l_green =new QLabel ("green :");
	QLabel *aff_green=new QLabel();
	aff_green->setNum(c->green());
	QLabel *l_blue =new QLabel ("blue :");
	QLabel *aff_blue=new QLabel();
	aff_blue->setNum(c->blue());
	
	QGridLayout *layout_p= new QGridLayout(mywidget1);
	layout_p->addWidget(l_red,0,0);
	layout_p->addWidget(aff_red,0,1);
	layout_p->addWidget(l_green,1,0);
	layout_p->addWidget(aff_green,1,1);
	layout_p->addWidget(l_blue,2,0);
	layout_p->addWidget(aff_blue,2,1);
	
	mywidget1->setLayout(layout_p);
	mywidget1->show();
	return true;
}

bool WorkArea::histogramImage()
{
	int tmp1 , tmp2 , tmp3;
	QColor c;
		
	for (int k=0;k<=255;k++)
	{	
		red_image[k]=0;
		green_image[k]=0;
		blue_image[k]=0;
		gray_image[k]=0;
	}

	if (image.format() > QImage::Format_Indexed8 )
	{
		for (int i=0;i<image.width();i++)
			for (int j=0;j<image.height();j++)
			{
				c = QColor(image.pixel(i,j));
				tmp1=c.red();
				tmp2=c.green();
				tmp3=c.blue();
				red_image[tmp1]=red_image[tmp1]+1;
				green_image[tmp2]=green_image[tmp2]+1;
				blue_image[tmp3]=blue_image[tmp3]+1;
			}   
	} 
	else 
	if(image.format() == QImage::Format_Indexed8)
	{	
		for (int i=0;i<image.width();i++)
			for (int j=0;j<image.height();j++)
			{
				c=QColor(image.pixel(i,j));
				tmp1=c.red();
				gray_image[tmp1]=gray_image[tmp1]+1;
			}   
				
	}
	else
		return false; 
		
	return true;
}


bool WorkArea::egaliser_histogram()
{
	if (!histogramImage())
		return false;
	if(image.format() == QImage::Format_Indexed8)
	{
		QImage Im_eq(imageWidth(), imageHeight(), QImage::Format_Indexed8 );
		
		QVector<QRgb> color_table(0);
		QRgb val_rgb;
		for(int k = 0; k < 256; ++k)
		{
			val_rgb = qRgb(k,k,k);
			color_table.append(val_rgb);
		}
		Im_eq.setColorTable(color_table);
		
		for(int i=0;i<=255;i++)
		{
			hist_p[i]=((float)gray_image[i]/(float)(image.width() * image.height()));
		}
		float somme=0.0;
		for(int i=0;i<=255;i++)
		{
			somme+=hist_p[i];
			hist_c[i]=somme;
		}
		int niveau_init=0;
		QColor c;
		for(int i=0;i<imageWidth();i++)	
			for (int j=0;j<imageHeight();j++)
			{
				c = QColor(image.pixel(i,j));
				niveau_init=c.red();
				Im_eq.setPixel(i,j,(int)(255*hist_c[niveau_init]));
			}
		image=Im_eq;
		modified = true;
	

	}
	else if (image.format() > QImage::Format_Indexed8)
	{
		QImage Im_eq(image) ;
		for(int i=0;i<=255;i++)
		{
		hist_p[i]=((float)(((float)red_image[i]+(float)blue_image[i]+(float)green_image[i])/3)/(float)(image.width() * image.height()));
		}
		float somme=0.0;
		for(int i=0;i<=255;i++)
		{
			somme+=hist_p[i];
			hist_c[i]=somme;
		}
		//int niveau_init=0;
		QColor c;
		for(int i=0;i<imageWidth();i++)	
			for (int j=0;j<imageHeight();j++)
			{
				c = QColor(image.pixel(i,j));
				Im_eq.setPixel(i,j,qRgb((int)(255*hist_c[c.red()]),(int)(255*hist_c[c.green()]),(int)(255*hist_c[c.blue()])));
			}
		image=Im_eq;
		modified =true;
	
	}
	update();	
	return true;
}

