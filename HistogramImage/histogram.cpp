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

	// 초기화
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
		histogram[i] = 0; // 각각의 0 ~ 255까지의 화소가 가지는 값을 초기화

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; j++)
			histogram[img[i][j]]++; // 이미지가 가지는 각각의 화소값의 개수만큼 증가


	// Find the maximum histogram value
	tmp1 = 0;
	for (i = 0; i < 256; ++i)
	{
		tmp1 = tmp1 > histogram[i] ? tmp1 : histogram[i]; // 최대값을 tmp1에 저장 (이미지가 가장 많이 가지는 값)
	}

	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (histogram[i] / tmp1);

		cvLine(imgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0);
		//- cvLine(CvArr* img, CvPoint pt1, CvPoint pt2, CvScalar color, int thickness = 1, int lineType = 8, int shift = 0;
		/*
		Parameters
	- img: 선을 그릴 원본 이미지.
	- pt1: 선의 양 끝 점 중 한쪽 점.
	- pt2: 선의 양 끝 점 중 나머지 한쪽 점.
	- color: 선의 색깔.
	- thickness: 선의 두께
	- lineType:  선의 종류
		*/

	}

	cvShowImage("Histo Equal ", imgHisto); // cvNamedWindow() 함수에 의해 지정된 윈도우에 영상을 출력하기 위해 사용되는 함수

	cdfOfHisto[0] = histogram[0];
	for (i = 1; i < 256; i++)
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i]; // 각 값에 현재 값 까지의 합을 누적



	// Draw the CDF of Histogram
	tmp1 = (double)cdfOfHisto[255];
	for (i = 0; i < 256; ++i)
	{
		tmp = (int)255 * (cdfOfHisto[i] / tmp1);

		cvLine(cdfImgHisto, cvPoint(i, 255), cvPoint(i, 255 - tmp), CV_RGB(255, 255, 255), 1, 8, 0); // 히스토그램 그리기
	}
	cvShowImage("CDF of Histogram ", cdfImgHisto); // 히스토그램 출력


	range = cdfOfHisto[255] - cdfOfHisto[0]; // cdf의 범위 저장
	histogramEqual[0] = 0;
	for (i = 0; i < 256; ++i) // ** 
	{
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range); // ceil() : 소수점을 올림 처리
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t; // clipping
	}

	// 메모리 해제
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

	cvNamedWindow(argv[1], 1); // 영상을 출력하기 위한 윈도우를 만들기 위해 사용되는 함수
	cvShowImage(argv[1], cvImg); // cvNamedWindow() 함수에 의해 지정된 윈도우에 영상을 출력하기 위해 사용되는 함수

	get_hist(img, imgSize.width, imgSize.height);
	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{
			((unsigned char*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];
		}
	cvShowImage("HistoGram Equalizer...", cvImg);
	get_hist1(img, imgSize.width, imgSize.height);

	cvWaitKey(0); // 키보드로부터 키 입력을 기다리기 위한 함수, 즉 영상이 출력되는 시간을 지정

	cvDestroyWindow("image"); // 윈도우 창을 종료하는 함수
	cvReleaseImage(&cvImg); // 메모리에 로드된 영상이 메모리로부터 해제하기 위해 사용하는 함수

	getchar();
	getchar();
	return 0;
}