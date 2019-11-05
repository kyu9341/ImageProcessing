#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>
#include<stdlib.h>
#include<math.h>
#include <stdio.h>

//#define unsigned char uchar
typedef unsigned char uchar;

int histogram[256], cdfOfHisto[256], histogramEqual[256];

unsigned char** uc_alloc(int size_x, int size_y) {
	unsigned char** m;
	int i;

	if ((m = (unsigned char**)calloc(size_y, sizeof(unsigned char*))) == NULL) {

		printf("uc_alloc error 1\7\n");
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if ((m[i] = (unsigned char*)calloc(size_x, sizeof(unsigned char))) == NULL) {
			printf("uc_alloc error 2\7\n");
			exit(0);

		}

	return m;

}

void read_ucmartrix(int size_x, int size_y, unsigned char** ucmatrix, char* filename) {
	int i;
	FILE* f;

	if ((fopen_s(&f, filename, "rb")) != NULL) {
		printf("%s File open Error!\n", filename);
		exit(0);

	}

	for (i = 0; i < size_y; i++)
		if (fread(ucmatrix[i], sizeof(unsigned char), size_x, f) != size_x) {
			printf("data read error \n");
			exit(0);
		}

	fclose(f);
}

void write_ucmatrix(int size_x, int size_y, unsigned char** ucmatrix, char* filename) {
	int i;
	FILE* f;


	if ((fopen_s(&f, filename, "wb")) != NULL) {
		printf("%s File open Error!\n", filename);
		exit(0);

	}

	for (i = 0; i < size_y; i++)
		if (fread(ucmatrix[i], sizeof(unsigned char), size_x, f) != size_x) {
			printf("data read error \n");
			exit(0);
		}

	fclose(f);
}

void get_hist1(uchar** img, int X_Size, int Y_Size)
{
	int i, j, tmp;
	double tmp1;
	int t, tp, range, hrange;
	CvSize histoSize, cdfSize;
	IplImage* imgHisto, * cdfImgHisto;

	histoSize.width = 256;
	histoSize.height = 256;

	cdfSize.width = 256;
	cdfSize.height = 256;

	imgHisto = cvCreateImage(histoSize, 8, 1);
	cdfImgHisto = cvCreateImage(cdfSize, 8, 1);

	// �ʱ�ȭ
	for(i = 0; i < histoSize.height; i++)
		for (j = 0; j < histoSize.width; j++)
		{
			((uchar*)(imgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;
		}
	for(i = 0; i < cdfSize.height; i++)
		for (j = 0; j < cdfSize.width; j++)
		{
			((uchar*)(cdfImgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;
		}

	tp = X_Size * Y_Size;

	for (i = 0; i < 256; i++)
		histogram[i] = 0; // ������ 0 ~ 255������ ȭ�Ұ� ������ ���� �ʱ�ȭ

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; j++)
			histogram[img[i][j]]++; // �̹����� ������ ������ ȭ�Ұ��� ������ŭ ����


	// Find the maximum histogram value
	tmp1 = 0;
	for (i = 0; i < 256; ++i)
	{
		tmp1 = tmp1 > histogram[i] ? tmp1 : histogram[i]; // �ִ밪�� tmp1�� ���� (�̹����� ���� ���� ������ ��)
	}

	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (histogram[i] / tmp1);

		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
		//- cvLine(CvArr* img, CvPoint pt1, CvPoint pt2, CvScalar color, int thickness = 1, int lineType = 8, int shift = 0;
		/*
		Parameters
	- img: ���� �׸� ���� �̹���.
	- pt1: ���� �� �� �� �� ���� ��.
	- pt2: ���� �� �� �� �� ������ ���� ��.
	- color: ���� ����.
	- thickness: ���� �β�
	- lineType:  ���� ����
		*/

	}

	cvShowImage("Histo Equal ", imgHisto); // cvNamedWindow() �Լ��� ���� ������ �����쿡 ������ ����ϱ� ���� ���Ǵ� �Լ�

	cdfOfHisto[0] = histogram[0];
	for (i = 1; i < 256; i++)
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i]; // �� ���� ���� �� ������ ���� ����



	// Draw the CDF of Histogram
	tmp1 = (double)cdfOfHisto[255];
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);

		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0); // ������׷� �׸���
	}
	cvShowImage("CDF of Histogram ", cdfImgHisto); // ������׷� ���


	range = cdfOfHisto[255] - cdfOfHisto[0]; // cdf�� ���� ����
	histogramEqual[0] = 0;
	for (i = 0; i < 256; ++i) // ** 
	{
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range); // ceil() : �Ҽ����� �ø� ó��
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t; // clipping
	}

	// �޸� ����
	cvReleaseImage(&imgHisto);
	cvReleaseImage(&cdfImgHisto);


	for(i = 0; i < Y_Size; ++i)
		for (j = 0; j < X_Size; ++j)
			img[i][j] = histogramEqual[img[i][j]]; //


}

void get_Match(uchar** img, int X_Size, int Y_Size, int histogram[256], int histogramSpec[256], int histogramMatch[256])
{
	int i, j, tmp, matchz = 0;
	float diff;
	printf("Start HistoGram Specification \n");
	for (i = 0; i < 256; i++)
	{
		histogramMatch[i] = 0;
		for (j = 0; j < 256; j++)
		{
			if ((i - histogramSpec[j]) > 0)
			{
				histogramMatch[i] = j;
			}
		}
	}
/*
// Inverse Processing of expected Histogram
	for (i = 0; i < 256; i++)
	{
		diff = abs(i - histogramSpec[0]);
		matchz = 0;
		for (j = 0; j < 256; j++)
		{
			if (abs(i - histogramSpec[j]) < diff)
			{
				diff = abs(i - histogramSpec[j]);
				matchz = j;
			}
		}
		histogramMatch[i] = (uchar)matchz;
	}

*/

	for (i = 0; i < Y_Size; ++i)
		for(j = 0; j < X_Size; ++j)
			img[i][j] = histogramMatch[img[i][j]];
}

void get_hist(uchar** img, int X_Size, int Y_Size)
{
	int i, j, tmp;
	double tmp1;
	int t, tp, range, hrange;
	CvSize histoSize, cdfSize;
	IplImage* imgHisto, * cdfImgHisto;

	histoSize.width = 256;
	histoSize.height = 256;

	cdfSize.width = 256;
	cdfSize.height = 256;

	imgHisto = cvCreateImage(histoSize, 8, 1);
	cdfImgHisto = cvCreateImage(cdfSize, 8, 1);

	for (i = 0; i < histoSize.height; i++)
		for (j = 0; j < histoSize.width; j++)
		{
			((uchar*)(imgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;
		}
	for (i = 0; i < cdfSize.height; i++)
		for (j = 0; j < cdfSize.width; j++)
		{
			((uchar*)(cdfImgHisto->imageData + cdfImgHisto->widthStep * i))[j] = 0;
		}

	tp = X_Size * Y_Size;

	for (i = 0; i < 256; i++)
		histogram[i] = 0;

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; j++)
			histogram[img[i][j]]++;

	tmp1 = 0;
	for (i = 0; i < 256; ++i)
	{
		tmp1 = tmp1 > histogram[i] ? tmp1 : histogram[i];
	}

	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (histogram[i] / tmp1);

		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}

	cvShowImage("Histo Line ", imgHisto);

	cdfOfHisto[0] = histogram[0];
	for (i = 1; i < 256; i++)
	{
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i];
	}
	// Draw the CDF of Histogram
	tmp1 = (double)cdfOfHisto[255];
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);

		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
	}
	cvShowImage("Original CDF of Histogram ", cdfImgHisto);

	range = cdfOfHisto[255] - cdfOfHisto[0];
	histogramEqual[0] = 0;
	for (i = 0; i < 256; ++i)
	{
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
	}

	cvReleaseImage(&imgHisto);
	for (i = 0; i < Y_Size; ++i)
		for (j = 0; j < X_Size; ++j)
			img[i][j] = histogramEqual[img[i][j]];
}

int main(int argc, char* argv[]) {

	int i, j;
	IplImage* cvImg;
	CvSize imgSize;
	unsigned char** img;

	if (argc != 4) {
		printf("exe imgdata xsie ysize \n");
		exit(0);
	}

	imgSize.width = atoi(argv[2]);
	imgSize.height = atoi(argv[3]);
	img = uc_alloc(imgSize.width, imgSize.height);
	read_ucmartrix(imgSize.width, imgSize.height, img, argv[1]);

	cvImg = cvCreateImage(imgSize, 8, 1);
	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{
			((unsigned char*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];
		}

	cvNamedWindow(argv[1], 1); // ������ ����ϱ� ���� �����츦 ����� ���� ���Ǵ� �Լ�
	cvShowImage(argv[1], cvImg); // cvNamedWindow() �Լ��� ���� ������ �����쿡 ������ ����ϱ� ���� ���Ǵ� �Լ�

	get_hist(img, imgSize.width, imgSize.height);
	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{
			((unsigned char*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];
		}
	cvShowImage("HistoGram Equalizer...", cvImg);
	get_hist1(img, imgSize.width, imgSize.height);

	cvWaitKey(0); // Ű����κ��� Ű �Է��� ��ٸ��� ���� �Լ�, �� ������ ��µǴ� �ð��� ����

	cvDestroyWindow("image"); // ������ â�� �����ϴ� �Լ�
	cvReleaseImage(&cvImg); // �޸𸮿� �ε�� ������ �޸𸮷κ��� �����ϱ� ���� ����ϴ� �Լ�

	getchar();
	getchar();
	return 0;
}