/*
����ó��

mod
0 : nagative 
1 : mosaic / block
2 : gamma correction / select (1 :gamma correction / gamma or 2 : average-> average)
3 : bit plane (��Ʈ ��� ����) / position
4 : bit plane �ռ� ���� / direction(1: ������Ʈ���� �ռ�, 2 : ������Ʈ ���� �ռ�) , position
5 : circle �κ� ���� ������ũ / diameter, block
6 : circle �κ� ������ũ - �߽� ���� ( 1 : ����, 2 : �簢��)/ diameter(������), block(������ũ ���) / x, y �߽� ��ǥ
7 :  ����þ� ���� �̿� AddNoise Img > �������� / count ���� Ƚ��
8 :
9 :


*/
#define _USE_MATH_DEFINES


#include<stdlib.h>
#include <stdio.h>
#include<iostream>
#include<math.h>
#include<time.h>
#include<string.h>
#include<assert.h>

typedef unsigned char uchar;

//void Bubble_sort(uchar* Sort, uchar* median_value, int Mode, int filterSize);

int Col, Row; // �̹��� ������ 
double **gaussMask, **aveMask;
int block_size = 3;

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

int** int_alloc(int size_x, int size_y) { // int������ �Ҵ�
	int** m;
	int i;

	if ((m = (int**)calloc(size_y, sizeof(int*))) == NULL) {

		printf("int_alloc error 1\7\n");
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if ((m[i] = (int*)calloc(size_x, sizeof(int))) == NULL) {
			printf("int_alloc error 2\7\n");
			exit(0);

		}

	return m;

}

double** d_alloc(double size_x, double size_y)
{
	double** m;
	int i;
	
	if ((m = (double**)calloc(size_y, sizeof(double*))) == NULL) {
		printf("double_alloc error 1\7\n");
		exit(0);
	}
	
	for(i = 0; i < size_y; i++)
		if ((m[i] = (double*)calloc(size_x, sizeof(double))) == NULL) {
			printf("double_alloc error 2\7\n");
			exit(0);

		}

	return m;
}

int** i_alloc(int size_x, int size_y)
{
	int** m;
	int i;

	if ((m = (int**)calloc(size_y, sizeof(int*))) == NULL) {
		printf("int_alloc error 1\7\n");
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if ((m[i] = (int*)calloc(size_x, sizeof(int))) == NULL) {
			printf("int_alloc error 2\7\n");
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
			printf("data read error read\n");
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
		if (fwrite(ucmatrix[i], sizeof(unsigned char), size_x, f) != size_x) {
			printf("data read error write\n");
			exit(0);
		}

	fclose(f);
}
void read_dmartrix(int size_x, int size_y, double** dmatrix, char* filename) {
	int i;
	FILE* f;

	if ((fopen_s(&f, filename, "rb")) != NULL) {
		printf("%s double File open Error!\n", filename);
		exit(0);

	}

	for (i = 0; i < size_y; i++)
		if (fread(dmatrix[i], sizeof(double), size_x, f) != size_x) {
			printf("double data read error read\n");
			exit(0);
		}

	fclose(f);
}

void write_dmatrix(int size_x, int size_y, double** dmatrix, char* filename) {
	int i;
	FILE* f;


	if ((fopen_s(&f, filename, "wb")) != NULL) {
		printf("%s double File open Error!\n", filename);
		exit(0);

	}

	for (i = 0; i < size_y; i++)
		if (fwrite(dmatrix[i], sizeof(double), size_x, f) != size_x) {
			printf("double data read error write\n");
			exit(0);
		}

	fclose(f);
}

void Negative(uchar** img, uchar** Result, int Row, int Col) // Nagative ȿ�� �ֱ� (����)
{
	int i, j;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
			Result[i][j] = 255 - img[i][j];

}

void mosaic(uchar** img, uchar** out, int Row, int Col, int Block) // �̹��� ������ũ ó��
{
	int i, j, x, y, tmp, count;
	for (i = 0; i < Row; i += Block) // �Է� ���� ��� ũ�⸸ŭ �����ϸ� �ݺ�
	{
		for (j = 0; j < Col; j += Block)
		{
			tmp = 0;
			count = 0;
			for (y = 0; y < Block; y++) // ��� ũ�⸸ŭ �ݺ�
			{
				for (x = 0; x < Block; x++) {
					tmp += img[i + y][j + x]; // uchar�������� 0~255 ���ۿ� ó�� ���ϹǷ� ������ ������ �� �ȼ����� ����
					count++; // ��� �����ߴ��� ����
				}
			}
			tmp /= count; // ��� ���� ����� ����
			for (y = 0; y < Block; y++)
			{
				for (x = 0; x < Block; x++) {
					out[i + y][j + x] = tmp; // �ش� ����� �� ��ġ�� ��� ��հ��� ����
				}

			}
		}
	}
}

double average(uchar** img, int Row, int Col) // �̹����� ��ü ��հ��� ���ϴ� �Լ�
{
	double sum = 0, avg;
	int i, j;

	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			sum += img[i][j];
		}
	}
	avg = sum / ((double)Row * Col);
	printf("Average of Image %1f \n", avg);
	return avg;

}

double mAverage(uchar** img, int Row, int Col) // Ȯ�������� ��� ���ϴ� �Լ�
{

	double msum = 0, mavg;
	int i, j;


		for (i = 0; i < Row; i++) {
			for (j = 0; j < Col; j++) {

				msum += img[i][j] / ((double)Row * Col);

			}
		}
	

//	mavg = msum/img[i][j] * ((double)Row * Col);
	printf("mAverage of Image %1f \n", msum);

	return msum;
}

void PowImg(uchar** img, uchar** Result, int Row, int Col, double gamma) // gamma correction image
{
	// ������ ��ü���� ��⸦ ���� (gamma ���� ���� - gamma�� Ŭ���� �����)
	int i, j;
	double tmp;

	for(i=0; i< Row; i++)
		for (j = 0; j < Col; j++) {
			tmp = pow(img[i][j] / 255., 1 / gamma); // pow(a, b) -> a^b ��ȯ
			// ������Ģ ��ȯ ���� : 0 ~ 1 -> �����ϸ� �ʿ� : s = [(r/255)^(1/r)] * 255 

			if (tmp * 255 > 255) tmp = 1; // 
			else if (tmp * 255 < 0) tmp = 0;

			Result[i][j] = tmp * 255;
		}
}



void AdaptiveBinary2(uchar** img, uchar** out, int Row, int Col)
{
	int i, j;

	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (img[i][j] > 50 && img[i][j] < 120) out[i][j] = img[i][j];
			else out[i][j] = 0;
		}
	}
}

void AdaptiveBinary1(uchar** img, uchar** out, int Row, int Col)
{
	int i, j;

	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (img[i][j] > 50 && img[i][j] < 120) out[i][j] = 200;
			else out[i][j] = img[i][j];
		}
	}
}


void AdaptiveBinary0(uchar** img, uchar** out, int Row, int Col)
{
	int i, j;

	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (img[i][j] > 50 && img[i][j] < 120) out[i][j] = 200;
			else out[i][j] = 0;
		}
	}
}



void makeBinary(uchar** img, uchar** out, int Row, int Col, double avg) // �̹��� 2��ȭ 
{
	int i, j;
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (img[i][j] > avg) out[i][j] = 255; // Change value for book with avg-30
			else out[i][j] = 0; // ��պ��� ū ���� 255, ���� ���� 0 ����

		}
	}
}



void BitSlicing(uchar** img, uchar** Result, int Row, int Col, int position)
{
	int i, j;
	uchar mask = 0x01;
	mask <<= position;

	for(i=0;i<Row;i++)
		for (j = 0; j < Col; j++) {
			if ((mask & img[i][j]) == pow(2, position)) //pow(2, position) = mask
			{
				Result[i][j] = 255;
			}
			else
			{
				Result[i][j] = 0;
			}
		}
}

void MaskOr(uchar** in1Img, uchar** in2Img, uchar** outImg, int Row, int Col) //90p
{
	int i, j;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++) {
			outImg[i][j] = in1Img[i][j] | in2Img[i][j];
		}
}
void MaskAnd(uchar** in1Img, uchar** in2Img, uchar** outImg, int Row, int Col)
{
	int i, j;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++) {
			outImg[i][j] = in1Img[i][j] & in2Img[i][j];
		}
}

void MaskAdd(uchar** in1Img, uchar** in2Img, uchar** outImg, int Row, int Col) // custom , mask�� �������� �̹����� ����
{
	int i, j, t;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++) {
//			t = in1Img[i][j] + in2Img[i][j];
//			outImg[i][j] = (t < 0) ? 0 : (t > 255) ? 255 : t;
			outImg[i][j] = in1Img[i][j] + in2Img[i][j];

		}
}

void BitMasking(uchar** in1Img, uchar** outImg, int Row, int Col, int position) // �� position�� �´� mask���� ���� �̹����� ����
{
	int i, j;
	uchar mask = 0x01; 
	mask <<= position;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++) {
			outImg[i][j] = mask & in1Img[i][j];
		}

}
void ImageCombine(uchar** img, uchar** tmpimg, uchar** outimg, int Row, int Col, int position, int direction) // 7���� position������ ��Ʈ�̹����� �ռ�
{
	int i, j, k;
	uchar mask = 0x01;

	if (direction == 1) // 7���� position���� �ռ�
	{

		for (k = 7; k > 7 - position; k--) // 7���� �Է¹��� ������� �ݺ�
		{
			mask <<= k;
			for (i = 0; i < Row; i++) // BitSlicing
				for (j = 0; j < Col; j++) {
					if ((mask & img[i][j]) == mask) //pow(2, position) = mask
					{
						tmpimg[i][j] = 255;
					}
					else
					{
						tmpimg[i][j] = 0;
					}

				}
			for (i = 0; i < Row; i++)
				for (j = 0; j < Col; j++) {
					outimg[i][j] += tmpimg[i][j] & mask; 
				}
			mask = 0x01;
		}
		average(outimg, Row, Col);

	}
/*	 ��Ʈ�����̵̽� �̹����� �ش� mask�� and���� �ϴ� ����
	 �ش� ��Ʈ���� 255�� ��ȯ�ϰ� �������� 0���� ��ȯ�߱� ������
	 �� 255 : 0b1111111 & 0b1000000 => 0b1000000
						& 0b0100000 => 0b0100000
						�� ���� ��ȯ �� ��� ������� �������� ���� ����.
*/
	if (direction == 2) // 1���� position���� �ռ�
	{

		for (k = 1; k <= position; k++) // 1���� �Է¹��� ������� �ݺ�
		{
			mask <<= k;
			for (i = 0; i < Row; i++) // BitSlicing
				for (j = 0; j < Col; j++) {
					if ((mask & img[i][j]) == mask) //pow(2, position) = mask
					{
						tmpimg[i][j] = 255;
					}
					else
					{
						tmpimg[i][j] = 0;
					}

				}
			for (i = 0; i < Row; i++) // BitMasking, MaskAdd 
				for (j = 0; j < Col; j++) {
					outimg[i][j] += tmpimg[i][j] & mask; // ��Ʈ�����̵̽� tmpimg�� �ش� ��Ʈ�� ����ũ ������ ����ŷ �� ����
				}
			mask = 0x01; // �ʱ�ȭ
		}
		average(outimg, Row, Col);

	}

}

void Square(uchar** Result, int Row, int Col, double diameter)
{
	int i, j;
	double tmp;
	diameter = diameter / 2; // �簢���� �Ѻ��� ���̸� �Է¹޾����Ƿ� 1/2 ����

	for(i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			if (i >= (Row / 2) - diameter && i <= (Row / 2) + diameter && j >= (Col / 2) - diameter && j <= (Col / 2) + diameter)
					Result[i][j] = 255;
			else Result[i][j] = 0;
		}
	
}

void Circle(uchar** Result, int Row, int Col, double diameter) // ���ϴ� �������� ũ��� �� ����
{
	int i, j;
	double tmp, xSquare, ySquare;

	for(i=0;i<Row;i++)
		for (j = 0; j < Col; j++)
		{
			ySquare = (abs(Row / 2 - i)) * (abs(Row / 2 - i)); // (Row/2, Col/2) �� �߽��� ��ǥ
			xSquare = (abs(Col / 2 - j)) * (abs(Col / 2 - j)); 

			tmp = sqrt(ySquare + xSquare); // tmp�� ���� ��ġ�� �߽ɰ��� �Ÿ� - ��Ÿ��� ���� x^2 + y^2 = z^2
			//sqrt() : �������� ���ϴ� �Լ�
			if (tmp < diameter) Result[i][j] = 255; // 
			else Result[i][j] = 0;
		}
}

void myCircle(uchar** Result, int Row, int Col, int x, int y, double diameter)
{
	int i, j;
	double tmp, xSquare, ySquare;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			ySquare = (abs(y - i)) * (abs(y - i));
			xSquare = (abs(x - j)) * (abs(x - j));

			tmp = sqrt(ySquare + xSquare);

			if (tmp < diameter) Result[i][j] = 255;
			else Result[i][j] = 0;
		}
}

void CircleMosaic(uchar** img, uchar** outimg, int Row, int Col, int x, int y, double diameter, int block)
{
	int i, j, a, b;
	double tmp, xSquare, ySquare, avg = 0;
	int mtmp, count;
	
	mosaic(img, outimg, Row, Col, block); // outimg�� ������ũ�� �̹��� ����

	for(i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			ySquare = (y - j) * (y - j);
			xSquare = (x - i) * (x - i);

			tmp = sqrt(ySquare + xSquare); // ��Ÿ��� ���� -> �߽ɰ��� �Ÿ� ã��

			if (tmp < diameter) outimg[i][j]; // ���� �ȼ��� ��ġ�� �Է¹��� ���������� ������ ������ũ�� �̹����� ����
			else outimg[i][j] = img[i][j]; // �׷��� ������ �����̹����� ����
		}

}

void SquareMosaic(uchar** img, uchar** outimg, int Row, int Col, int x, int y, int diameter, int block)
{
	int i, j;
	diameter = diameter / 2; // �簢���� �� ���� ���̸� �Է¹޾����Ƿ� 1/2 ����

	mosaic(img, outimg, Row, Col, block);
	
	for(i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			if (i >= x - diameter && i <= x + diameter && j >= y - diameter && j <= y + diameter) // �簢�� ���� ����
				outimg[i][j]; // �簢�� ���δ� ������ũ �̹��� ����
			else outimg[i][j] = img[i][j]; // �簢�� �ܺθ� ���� ����
		}
	
}

double uniform()
{
	return((double)(rand() & RAND_MAX) / RAND_MAX - 0.5);
}

double gaussian() // ����þ� ���� ����
{
	static int ready = 0;
	static double gstore;
	double v1, v2, r, fac, gaus;
	double uniform();

	if (ready == 0)
	{
		do {
			v1 = 2. * uniform();
			v2 = 2. * uniform();
			r = v1 * v1 + v2 * v2;
		} while (r > 1.0);

		fac = sqrt(-2. * log(r) / r);
		gstore = v1 * fac;
		gaus = v2 * fac;
		ready = 1;
		
	}
	else
	{
		ready = 0;
		gaus = gstore;
	}

	return(gaus);
}

void AddNoise(uchar** img, uchar** outimg, int Row, int Col) // ����þ� ������ ���� ���� ����
{
	int i, j;
	int temp;
	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++) {

			outimg[i][j] = img[i][j] + gaussian() * 50;
		}

}

void RmNoise(uchar** img, int** tempimg, uchar** outimg, int Row, int Col, int count) // ���������� ���� ���� ����
{
	int i, j;
	int k;
	int temp;
	for (k = 0; k < count; k++) { // count : �������� ���� Ƚ��
		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++) {
				tempimg[i][j] += (img[i][j] + gaussian() * 50); 
				// unsigned char �� 0~255 �� ������ ���ϱ� ������
				// int������ �����Ͽ� ������Ŵ
//				printf("%lf\n", gaussian() * 50);
			}
	}

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++) {
			temp = tempimg[i][j] / count; // ���� ���� ��ճ��� temp�� ����
//			printf("temp : %d\n", temp);

			if (temp < 0) // clipping
			{ 
				temp = 0;
				outimg[i][j] = temp;
			}
			else if (temp > 255)
			{
				temp = 255;
				outimg[i][j] = temp;
			}
			else 
			{
				outimg[i][j] = temp;
			}
//			printf("outimg[i][j] : %d\n", outimg[i][j]);
		}
	
}

void get_Equalization(uchar** img, int Row, int Col, int histogramEqual[256]) // Histogram Equalization
{
	int i, j, range, t;
	int histogram[256];
	int cdfOfHisto[256];

	for (i = 0; i < 256; i++) // �ʱ�ȭ
		histogram[i] = 0;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
			histogram[img[i][j]]++; // �̹����� ������ ������ ���� ������ŭ ������Ŵ

	cdfOfHisto[0] = histogram[0];
	for (i = 1; i < 256; i++)
	{
		cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i]; // �� ���� ���� �� ������ ���� ����
		printf("cdf[%d] = %d\t", i, cdfOfHisto[i]);
	}

	range = cdfOfHisto[255] - cdfOfHisto[0];
	histogramEqual[0] = 0;
	for (i = 0; i < 256; ++i)
	{
		t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
		histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
	}


	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
			img[i][j] = histogramEqual[img[i][j]];
}

void get_Match(uchar** img, uchar** outimg, int X_Size, int Y_Size, int histogram[256], int histogramSpec[256], int histogramMatch[256]) // Histogram Specification
{// cdf���� ����ȯ�� ������ų �� ���������Լ��� ���� ���� ���Ͽ� ���� ���� ������ �ִ� ��ġ�� 
//  ���� ���� �������� �ʴ� ��� �� ������ ���� ����� ���� ��ġ�� ������׷� ���� ����
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
		printf("histogramMatch[%d] = %d\n", i, histogramMatch[i]);

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
		for (j = 0; j < X_Size; ++j)
			outimg[i][j] = histogramMatch[img[i][j]];
}


void contrastStreching(uchar** img, uchar** outimg, int X_Size, int Y_Size)
{
	int i, j, histogram[256];
	int min = 255, max = 0;
	uchar LUT[256];
	double scaleFactor, tmp;
	for (i = 0; i < 256; i++)
		histogram[i] = 0;

	for(i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; j++)
		{
			histogram[img[i][j]]++;
		}
	for(i = 0; i < 256; i++)
		if (histogram[i])
		{
			min = i;
			break;
		}
	for(i = 255; i >= 0; i--)
		if (histogram[i])
		{
			max = i;
			break;
		}

	printf(" Low Threshold is %d High Threshold is %d\n", min, max);
	for (i = 0; i < min; i++)
		LUT[i] = 0;
	for (i = 255; i >= 0; i--)
		LUT[i] = 255;

	scaleFactor = 255.0 / (double)(max - min);

	for (i = min; i < max; i++)
	{
		tmp = (i - min) * scaleFactor;

		if (tmp < 0) tmp = 0;
		if (tmp > 255) tmp = 255;
		LUT[i] = (uchar)tmp;
	}

	for (i = 0; i < Y_Size; i++)
		for (j = 0; j < X_Size; j++)
			outimg[i][j] = LUT[img[i][j]];

}

void convolution(double** h, int F_length, int size_x, int size_y, uchar** image1, uchar** image2)
{
	//  ������� ����� ����ũ�� �̹��� �� �����Ǵ� ������ ���Ͽ� ��� ���Ͽ� ���ϸ�, ������� ��� ������ ���� ��ġ�� ���
	int i, j, x, y;
	int margin, indexX, indexY;
	double sum, coeff;

	margin = (int)(F_length / 2);

	for (i = 0; i < size_y; i++)
	{
		for (j = 0; j < size_x; j++)
		{
			sum = 0;
			for (y = 0; y < F_length; y++)
			{
				indexY = i - margin + y;
				if (indexY < 0) indexY = -indexY;
				else if (indexY >= size_y) indexY = (2 * size_y - indexY - 1);

				for (x = 0; x < F_length; x++)
				{
					indexX = j - margin + x;
					if (indexX < 0) indexX = -indexX;
					else if (indexX >= size_x) indexX = (2 * size_x - indexX - 1); // �ܰ� ó�� (��Ī ���(�ø�Ʈ��)??)

					sum += h[y][x] * (double)image1[indexY][indexX]; // ����ũ�� �� ���� �̹����� �ȼ� ���� ���Ͽ� ��� �����ش�.
				}
			}
//			sum += 128; // �簢 ȿ���� ���� ���� �߰� -> �̺κи� �ٸ�

			// clipping
			if (sum < 0) sum = 0.;
			else if (sum > 255) sum = 255;
			image2[i][j] = (uchar)sum;
		}
	}
}

void convolution0(double** h, int F_length, int size_x, int size_y, uchar** image1, uchar** image2)
{
	//  ������� ����� ����ũ�� �̹��� �� �����Ǵ� ������ ���Ͽ� ��� ���Ͽ� ���ϸ�, ������� ��� ������ ���� ��ġ�� ���
	int i, j, x, y;
	int margin, indexX, indexY;
	double sum, coeff;

	margin = (int)(F_length / 2);

	for (i = 0; i < size_y; i++)
	{
		for (j = 0; j < size_x; j++)
		{
			sum = 0;
			for (y = 0; y < F_length; y++)
			{
				indexY = i - margin + y;
				if (indexY < 0) indexY = 0;
				else if (indexY >= size_y) indexY = 0;

				for (x = 0; x < F_length; x++)
				{
					indexX = j - margin + x;
					if (indexX < 0) indexX =0;
					else if (indexX >= size_x) indexX = 0; // �ܰ� ó�� (0����)

					sum += h[y][x] * (double)image1[indexY][indexX]; // ����ũ�� �� ���� �̹����� �ȼ� ���� ���Ͽ� ��� �����ش�.
				}
			}
			//			sum += 128; // �簢 ȿ���� ���� ���� �߰� -> �̺κи� �ٸ�

						// clipping
			if (sum < 0) sum = 0.;
			else if (sum > 255) sum = 255;
			image2[i][j] = (uchar)sum;
		}
	}
}
void Bubble_sort(uchar* Sort, uchar* median_value, int Mode, int filterSize)
{
	int i, x;
	uchar temp, swap;
	for (x = 0; x < filterSize * filterSize; x++)
	{
		temp = Sort[x];
		for (i = x; i < filterSize * filterSize - 1; i++)
		{
			if (temp >= Sort[i + 1])
			{
				swap = temp;
				temp = Sort[i + 1];
				Sort[i + 1] = swap;
			}
		}
		Sort[x] = temp;
	}
	if (Mode == -1)
		* median_value = (uchar)Sort[0]; // median filter�� �߾Ӱ��� ���ͳ��� �ּڰ����� ����
	else if (Mode == 0)
		* median_value = (uchar)Sort[filterSize * filterSize / 2]; // median filter�� �߾Ӱ��� ���ͳ��� �߰������� ����
	else if (Mode == 1)
		* median_value = (uchar)Sort[filterSize * filterSize - 1]; // median filter�� �߾Ӱ��� ���ͳ��� �ִ밪���� ����

}
void Bubble_sort1(uchar* Sort, uchar* median_value, int Mode, int filterSize)
{
	int i, x;
	uchar temp, swap;
	for (x = 0; x < filterSize ; x++)
	{
		temp = Sort[x];
		for (i = x; i < filterSize - 1; i++)
		{
			if (temp >= Sort[i + 1])
			{
				swap = temp;
				temp = Sort[i + 1];
				Sort[i + 1] = swap;
			}
		}
		Sort[x] = temp;
	}
	if (Mode == -1)
		* median_value = (uchar)Sort[0]; // median filter�� �߾Ӱ��� ���ͳ��� �ּڰ����� ����
	else if (Mode == 0)
		* median_value = (uchar)Sort[filterSize / 2]; // median filter�� �߾Ӱ��� ���ͳ��� �߰������� ����
	else if (Mode == 1)
		* median_value = (uchar)Sort[filterSize - 1]; // median filter�� �߾Ӱ��� ���ͳ��� �ִ밪���� ����

}
void median(uchar** inImg, uchar** outImg, int ROW, int COL, int Mode, int filterSize) // Median Filtering
{
	int i, j, x, y, z, count = 0;
	uchar median_value; // ������ �߾Ӱ�
	uchar* Sort; 
	Sort = (uchar*)malloc(filterSize * filterSize * sizeof(uchar)); // ������ ����ũ���� ������ ������ ������ �迭 �����Ҵ�


	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
			outImg[i][j] = inImg[i][j];

	for (i = 0; i < ROW - filterSize; i++)
		for (j = 0; j < COL - filterSize; j++) 
		{
			for (x = 0; x < filterSize; x++)
				for (y = 0; y < filterSize; y++)
					Sort[filterSize * x + y] = inImg[i + x][j + y];
			Bubble_sort(Sort, &median_value, Mode, filterSize);
			outImg[i + 1][j + 1] = median_value;
		}
	free(Sort);
}

void median1(uchar** inImg, uchar** outImg, int ROW, int COL, int Mode, int filterSize) // Median Filtering -> 1���� ���ͷ� ��ȯ
{
	int i, j, x, y, z, count = 0;
	uchar median_value; // ������ �߾Ӱ�
	uchar* Sort;
	Sort = (uchar*)malloc(filterSize * sizeof(uchar)); // ������ ����ũ���� ������ ������ ������ �迭 �����Ҵ�


	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
			outImg[i][j] = inImg[i][j];

	for (i = 0; i < ROW; i++) {

		for (j = 0; j < COL; j++)
		{

			for (x = 0; x < filterSize ; x++)
				Sort[x] = inImg[i][j + x - (filterSize/2)];

			Bubble_sort1(Sort, &median_value, Mode, filterSize);
			outImg[i][j+1] = median_value;
		}
	}
	free(Sort);
}


void convolutionD(double** h, int F_length, int size_x, int size_y, uchar** image1, double** image2) // convolution double������ ��ȯ
{
	int i, j, x, y;
	int margin, indexX, indexY;
	double sum, coeff;

	margin = (int)(F_length / 2);

	for (i = 0; i < size_y; i++)
	{
		for (j = 0; j < size_x; j++)
		{
			sum = 0;
			for (y = 0; y < F_length; y++)
			{
				indexY = i - margin + y;
				if (indexY < 0) indexY = -indexY;
				else if (indexY >= size_y) indexY = (2 * size_y - indexY - 1);

				for (x = 0; x < F_length; x++)
				{
					indexX = j - margin + x;
					if (indexX < 0) indexX = -indexX;
					else if (indexX >= size_x) indexX = (2 * size_x - indexX - 1);

					sum += h[y][x] * (double)image1[indexY][indexX];
				}
			}
//			sum += 128;

			// clipping
			image2[i][j] = sum;
		}
	}
}

void convolutionEmbossing(double** h, int F_length, int size_x, int size_y, uchar** image1, uchar** image2) // Embossing Filtering
{
	int i, j, x, y;
	int margin, indexX, indexY;
	double sum, coeff;

	margin = (int)(F_length / 2);

	for (i = 0; i < size_y; i++)
	{
		for (j = 0; j < size_x; j++)
		{
			sum = 0;
			for (y = 0; y < F_length; y++)
			{
				indexY = i - margin + y;
				if (indexY < 0) indexY = -indexY;
				else if (indexY >= size_y) indexY = (2 * size_y - indexY - 1);

				for (x = 0; x < F_length; x++)
				{
					indexX = j - margin + x;
					if (indexX < 0) indexX = -indexX;
					else if (indexX >= size_x) indexX = (2 * size_x - indexX - 1);
					
					sum += h[y][x] * (double)image1[indexY][indexX];
				}
			}
			sum += 128; // �簢 ȿ���� ���� ���� �߰� -> �̺κи� �ٸ�

			// clipping
			if (sum < 0) sum = 0.;
			else if (sum > 255) sum = 255;
			image2[i][j] = (uchar)sum;
		}
	}
}

int mask_size = 3;

void make_Mask(int mask_size, double** Mask, int checkMask, int apply) // Sharpening Filtering 0~9
{
	int i, j;
	// �� Mask ����
	// sobel : �Һ� �����ڴ� 1�� �̺��� ����� ���� -> ������ ���⸦ ���ϴ� �����ڿ� ������ ���⸦ ���ϴ� �����ڷ� �̷����
	// ��ü������ �β��� ��輱 ������ �����ش�.
	double sobelYMask[3][3] = { 1, 0, -1, 2, 0, -2, 1, 0, -1 };
//	double sobelYMask[3][3] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
	double sobelXMask[3][3] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
//	double sobelXMask[3][3] = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };

	// prewitt : sobel�����ڴ� ���� ���� �����ϱ� ���� 2�� ����ġ�� ����� �ݸ� prewitt�� ��� �����ϰ� 1�� ����ġ�� �̿��� ���͸� ����
	// ��輱 ������ ���� �� �� ����
	double PrewittYMask[3][3] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	double PrewittXMask[3][3] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };

	// Roberts : ���� �缱�� ������ �缱�� ��輱 �������� ȹ���Ͽ� ����� ������ 
	// 1�� �̺� �������� ��輱�� ���� ���ϰ� ��Ÿ��
	double RobertsYMask[3][3] = { 0, 0, -1, 0, 1, 0, 0, 0, 0 }; 
	double RobertsXMask[3][3] = { -1, 0, 0, 0, 1, 0, 0, 0, 0 };
	// -----------���� 1�� �̺�-------------

	// 2�� �̺� ���
	double Laplace1Mask[3][3] = { 0, -1, 0, -1, 4, -1, 0, -1, 0 };
	double Laplace2Mask[3][3] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };

	//������ �����ϰ� ����� ȿ���� ��� ���ؼ��� �ȼ����� ������ ������ �����Ǿ�� �Ѵ�.
	// �׷��� �ϱ� ���ؼ� ����ũ �迭�� �׸���� ���� 1�� �ǵ��� ���ö�þ� ����ũ�� �߾Ӱ��� 1 ������Ų��.
	double Laplace3Mask[3][3] = { 0, -1, 0, -1, 5, -1, 0, -1, 0 };
	double Laplace4Mask[3][3] = { -1, -1, -1, -1, 9, -1, -1, -1, -1 };
	// ���� ���� ���ؼ��� Laplace �����ڸ� ������ ���͸��� �ǹ̰� ����

	// ���õ� Mask ���� ���� ����
	switch (checkMask)
	{
	case 0:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = sobelYMask[i][j];
		break;
	case 1:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = sobelXMask[i][j];
		break;
	case 2:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = PrewittYMask[i][j];
		break;
	case 3:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = PrewittXMask[i][j];
		break;
	case 4:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = RobertsYMask[i][j];
		break;
	case 5:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = RobertsXMask[i][j];
		break;
	case 6:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = Laplace1Mask[i][j];
		break;
	case 7:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = Laplace2Mask[i][j];
		break;
	case 8:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = Laplace3Mask[i][j];
		break;
	case 9:
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = Laplace4Mask[i][j];
		break;
	case 10: // sobleX + sobleY
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = sobelYMask[i][j] + sobelXMask[i][j];
		break;
	case 11: // prewittX + prewittY
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = PrewittYMask[i][j] + PrewittXMask[i][j];
		break;
	case 12: // robertsX + robertsY
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				Mask[i][j] = RobertsYMask[i][j] + RobertsXMask[i][j];
		break;

	default:
		printf("Mask Number is wrong \n");
		exit(1);
	}
	if (apply == 1) // apply���� 1�ΰ�� ���� ���� ����
	{
		Mask[1][1] ++; // �߾Ӱ��� 1�����־� �� ���� 0�� ������ �� ���� 1�θ���� �� ���� ����
	}
	


}

void LogImg(double** dimg, uchar** Result)
{
	int i, j;
	double max = -10000000000000000000000.0,
		min = 9999999999999999999999.0;
	double tmp, total;

	for (i = 0; i < Row; i++) 
		for (j = 0; j < Col; j++)
		{
			//tmp = log(1 + dimg[i][j]);
			tmp = log(dimg[i][j]);
			if (max < tmp) max = tmp;
			if (min > tmp) min = tmp;
		}
	total = max - min;
	printf("max = %lf  min = %lf", max, min);

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			tmp = ((dimg[i][j] - min) / total);
			tmp *= 255;
			//printf("tmp = %lf\n", tmp);

			if (tmp > 255) tmp = 255;
			else if (tmp < 0) tmp = 0;

			Result[i][j] = tmp;
			//printf("result[%d][%d] = %lf\n", i, j, tmp);
		}

}

int rearrange(double* X, int N)
{
	int i, j, * power_of_2, * pos, stage, num_of_stages = 0;
	double temp;

	for (i = N; i > 1; i >>= 1, num_of_stages++);
	if ((power_of_2 = (int*)malloc(sizeof(int) * num_of_stages)) == NULL)
		return -1;
	if ((pos = (int*)malloc(sizeof(int) * N)) == NULL)
		return -1;


	power_of_2[0] = 1;
	for (stage = 1; stage < num_of_stages; stage++)
		power_of_2[stage] = power_of_2[stage - 1] << 1;



	for (i = 1; i < N - 1; i++)
		pos[i] = 0;
	for (i = 1; i < N - 1; i++)
	{
		if (!pos[i])
		{
			for (j = 0; j < num_of_stages; j++)
			{
				if (i & power_of_2[j])
					pos[i] += power_of_2[num_of_stages - 1 - j];
			}
			temp = X[i];
			X[i] = X[pos[i]];
			X[pos[i]] = temp;
			pos[pos[i]] = 1;
		}
	}
	free(power_of_2);
	free(pos);
	return 0;
}

void fft(double* X_re, double* X_im, int N)
{
	double X_temp_re, X_temp_im;
	double phase;
	int num_of_stages = 0, num_of_elements, num_of_sections, size_of_butterfly;
	int i, j, stage, m1, m2;

//	printf("num_of_stages : %d\n", num_of_stages);

	for (i = N; i > 1; i >>= 1, num_of_stages++);

	num_of_elements = N;
	num_of_sections = 1;
	size_of_butterfly = N >> 1;

	for (stage = 0; stage < num_of_stages; stage++)
	{
		m1 = 0;
		m2 = size_of_butterfly;
		for (i = 0; i < num_of_sections; i++)
		{
			for (j = 0; j < size_of_butterfly; j++, m1++, m2++)
			{
				X_temp_re = X_re[m1] - X_re[m2];
				X_temp_im = X_im[m1] - X_im[m2];
				X_re[m1] = X_re[m1] + X_re[m2];
				X_im[m1] = X_im[m1] + X_im[m2];
				phase = -2.0 * M_PI * j / num_of_elements;
				X_re[m2] = X_temp_re * cos(phase) - X_temp_im * sin(phase);
				X_im[m2] = X_temp_re * sin(phase) + X_temp_im * cos(phase); // <<<<
			}
			m1 += size_of_butterfly;
			m2 += size_of_butterfly;
		}
//		printf("--num_of_stages : %d, num_of_elements : %d, num_of_sections : %d, size_of_butterfly : %d \n", num_of_stages, num_of_elements, num_of_sections, size_of_butterfly);


		num_of_elements >>= 1;
		num_of_sections <<= 1;
		size_of_butterfly >>= 1;
	}
//	printf("num_of_stages : %d, num_of_elements : %d, num_of_sections : %d, size_of_butterfly : %d \n", num_of_stages, num_of_elements, num_of_sections, size_of_butterfly);

	rearrange(X_re, N);
	rearrange(X_im, N);
}
int fft_2d(double** X_re, double** X_im, int N, int Mode) // ��� Ǫ���� ��ȯ
{
	int i, j;
	double* temp_re, *temp_im;

	if ((temp_re = (double *)malloc(sizeof(double) * N)) == NULL)
		return -1;
	if ((temp_im = (double *)malloc(sizeof(double) * N)) == NULL)
		return -1;

	if (Mode == 0)
	{
		// Processing for ROW
		for (i = 0; i < N; i++)
			fft(X_re[i], X_im[i], N);

		// Processing for COLUMN
		for (j = 0; j < N; j++)
		{
			for (i = 0; i < N; i++)
			{
				temp_re[i] = X_re[i][j];
				temp_im[i] = X_im[i][j];
			}
			fft(temp_re, temp_im, N);
			for (i = 0; i < N; i++)
			{
				X_re[i][j] = temp_re[i] / N;
				X_im[i][j] = temp_im[i] / N;
			}
		}

	}
	else if (Mode == 1)
	{
		// Processing for ROW
		for (i = 0; i < N; i++)
			fft(X_re[i], X_im[i], N);

		// Processing for COLUMN
		for (j = 0; j < N; j++)
		{
			for (i = 0; i < N; i++)
			{
				temp_re[i] = X_re[i][j];
				temp_im[i] = X_im[i][j];
			}
			fft(temp_re, temp_im, N);
			for (i = 0; i < N; i++)
			{
				X_re[i][j] = temp_re[i] / N;
				X_im[i][j] = temp_im[i] / N;
			}
		}
	}
	else if (Mode == 2)
	{
		for (i = 0; i < N; i++)
		{
			for (j = 0; j < N; j++)
			{
				temp_re[j] = X_re[i][j] * pow(-1, j);
				temp_im[j] = X_im[i][j] * pow(-1, j);
			}

			fft(temp_re, temp_im, N);
			for (j = 0; j < N; j++)
			{
				X_re[i][j] = temp_re[j];
				X_im[i][j] = temp_im[j];
			}
		}
		for (j = 0; j < N; j++)
		{
			for (i = 0; i < N; i++)
			{
				temp_re[i] = X_re[i][j] * pow(-1, i);
				temp_im[i] = X_im[i][j] * pow(-1, i);
			}


			fft(temp_re, temp_im, N);
			for (i = 0; i < N; i++)
			{
				X_re[i][j] = temp_re[i] / N;
				X_im[i][j] = temp_im[i] / N;
			}
		}
	}

	free(temp_re);
	free(temp_im);
	return 0;
}

/* integer DCT & IDCT */

static int dct_buffer[8][8] = {
	{ 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096},
	{ 5681, 4816, 3218, 1130, -1130, -3218, -4816, -5681 },
	{ 5352, 2217, -2217, -5352, -5352, -2217, 2217, 5352 },
	{ 4816, -1130, -5681, -3218, 3218, 5681, 1130, -4816 },
	{ 4096, -4096, -4096, 4096, 4096, -4096, -4096, 4096 },
	{ 3218, -5681, 1130, 4816, -4816, -1130, 5681, -3218 },
	{ 2217, -5352, 5352, -2217, -2217, 5352, -5352, 2217 },
	{ 1130, -3218, 4816, -5681, 5681, -4816, 3218, -1130 }, };

// Forward DCT original
void Fdct(int** PEL, int** Coeff)
{
	int i, j, k;
	long dd;
	int t[8][8];

	for(i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
		{
			dd = 0;
			for (k = 0; k < 8; k++)
				dd += (long)PEL[i][k] * dct_buffer[j][k];
			t[i][j] = ((dd + 2048) >> 12);
		}

	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
		{
			dd = 0;
			for (k = 0; k < 8; k++)
				dd += (long)t[i][k] * dct_buffer[j][k];
			Coeff[i][j] = ((dd + 16384) >> 15);
		}

}

// Forward DCT row col
void mFdct(int** PEL, int** Coeff, int Row, int Col)
{
	int i, j, k;
	long dd;
	int t[8][8];
	int x, y;
	

	for (y = 0; y < Row; y += 8)
		for (x = 0; x < Col; x += 8)
		{
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
				{
					dd = 0;
					for (k = 0; k < 8; k++)
						dd += (long)PEL[i + y][k + x] * dct_buffer[j][k];
					t[i][j] = ((dd + 2048) >> 12);
				}

			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
				{
					dd = 0;
					for (k = 0; k < 8; k++)
						dd += (long)t[i][k] * dct_buffer[j][k];
					Coeff[i + y][j + x] = ((dd + 16384) >> 15);
				}
		}
	

}

// Inverse DCT  original
void idct(int** Coeff, int** PEL)
{
	int i, j, k;
	long dd;
	int t[8][8];
		
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
		{
			dd = 0;
			for (k = 0; k < 8; k++)
				dd += (long)Coeff[k][i] * dct_buffer[k][j];
			t[i][j] = ((dd + 2048) / 4096);
		}



	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
		{
			dd = 0;
			for (k = 0; k < 8; k++)
				dd += (long)t[k][i] * dct_buffer[k][j];
			PEL[i][j] = ((dd + 16384) / 32768);
		}
		

}

// Inverse DCT row col
void mIdct(int** Coeff, int** PEL, int Row, int Col)
{
	int i, j, k;
	long dd;
	int t[8][8];
	int x, y;

	for (y = 0; y < Row; y += 8)
		for (x = 0; x < Col; x += 8)
		{
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
				{
					dd = 0;
					for (k = 0; k < 8; k++)
						dd += (long)Coeff[k+y][i+x] * dct_buffer[k][j];
					t[i][j] = ((dd + 2048) / 4096);
				}
		
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
				{
					dd = 0;
					for (k = 0; k < 8; k++)
						dd += (long)t[k][i] * dct_buffer[k][j];
					PEL[i+y][j+x] = ((dd + 16384) / 32768);
				}
		}

}

// Antonini Filter
double A97L1[9] = {
	0.026749,
	-0.016864,
	-0.078223,
	0.266864,
	0.602949,
	0.266864,
	-0.078223,
	-0.016864,
	0.026749 };

int A97L1_l = 9;
double A97H1[11] = {
	0.0,
	0.0,
	-0.045636,
	0.028772,
	0.295636,
	-0.557543,
	0.295636,
	0.028772,
	-0.045636,
	0.0,
	0.0, };

int A97H1_l = 11;
double A97L2[11] = {
	0.0,
	0.0,
	-0.045636,
	-0.028772,
	0.295636,
	0.557543,
	0.295636,
	-0.028772,
	0.045636,
	0.0,
	0.0, };

int A97L2_l = 11;
// Antonini Filter
double A97H2[9] = {
	-0.026749,
	-0.016864,
	0.078223,
	0.266864,
	-0.602949,
	0.266864,
	0.078223,
	-0.016864,
	-0.026749 };
int A97H2_l = 9;

void row_analysis(double* h, int F_length, int size_x, int size_y, double** image1, double** image2)
{
	int i, j, k, margin, index, size_x2;
	double sum, coeff;
	double** temp;

	size_x2 = size_x / 2;
	temp = d_alloc(size_x, size_y);

	for (i = 0, coeff = 0.; i < F_length; i++) coeff += h[i];
	printf("coeff = %lf\n", coeff);

	margin = (int)(F_length / 2);
	for(i = 0; i < size_y; i++)
		for (j = 0; j < size_x; j++)
		{
			for (k = 0, sum = 0.; k < F_length; k++)
			{
				index = j - margin + k;
				if (index < 0) index = -index;
				else if (index >= size_x) index = (2 * size_x - index - 2);
				sum += h[k] * image1[i][index];
			}
			if (coeff > 1.) sum /= coeff;
			/*	if (sum < 0) sum = 0.;
				else if (sum > 255) sum = 255.;
			*/
			temp[i][j] = sum;
		}

	for (i = 0; i < size_y; i++)
		for (j = 0, k = 0; j < size_x; j+=2, k++)
			image2[i][k] = temp[i][j];

	//free(temp); // d_free(size_x, size_y, temp);
}

void column_analysis(double* h, int F_length, int size_x, int size_y, double** image1, double** image2)
{
	int i, j, k, margin, index, size_x2, size_y2;
	double sum, coeff;
	double** temp;

	size_x2 = size_x / 2;
	size_y2 = size_y / 2;

	temp = d_alloc(size_x2, size_y);

	for (i = 0, coeff = 0.; i < F_length; i++) coeff += h[i];
	printf("coeff = %lf\n", coeff);

	margin = (int)(F_length / 2);
	for (i = 0; i < size_x2; i++)
		for (j = 0; j < size_y; j++)
		{
			for (k = 0, sum = 0.; k < F_length; k++)
			{
				index = j - margin + k;
				if (index < 0) index = -index;
				else if (index >= size_y) index = (2 * size_y - index - 2);
				sum += h[k] * image1[i][index];
			}
			if (coeff > 1.) sum /= coeff;
			/*	if (sum < 0) sum = 0.;
				else if (sum > 255) sum = 255.;
			*/
			temp[i][j] = sum;
		}

	for (i = 0; i < size_x2; i++)
		for (j = 0, k = 0; j < size_y; j+=2, k++)
			image2[k][i] = temp[j][i];

	//free(temp); // d_free(size_x2, size_y, temp);
}

void conv_uc_to_d(int Row, int Col, uchar** ucimage, double** dimage)
{
	int i, j;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			dimage[i][j] = (double)ucimage[i][j];
		}
}
void conv_d_to_uc(int Row, int Col, double** dimage, uchar** ucimage)
{
	int i, j;

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			if (dimage[i][j] > 255) ucimage[i][j] = 255;
			else if (dimage[i][j] < 0) ucimage[i][j] = 0;
			else ucimage[i][j] = (uchar)dimage[i][j];
		}
}
void psnruc(uchar** image1, uchar** image2, int Row, int Col, double* MSE, double* PSNR)
{
	int i, j;
	double temp, diff_sum = 0.;

	for(i = 0; i < Row; i ++)
		for (j = 0; j < Col; j++)
		{
			temp = (double)(image1[i][j] - image2[i][j]);
			diff_sum += temp * temp;
		}
	*MSE = diff_sum / ((double)Row * (double)Col);
	if (*MSE)
		* PSNR = (10. * log10((255. * 255.) / *MSE));

	printf("PSNR = %10.4lf\n", *PSNR);

}

int main(int argc, char* argv[]) {

	int i, j;
	int avg;
	double msum;
	uchar** img, **outimg, **targetImg;
	uchar** img1, ** img2, ** outimg1, ** outimg2, ** cirimg, ** mosaimg;
	uchar** tmpimg;
	int** tempimg; // ������ �̹���
	int block; // ������ũ ���
	int diameter; // circle ������
	double gamma; // r (��� ����)
	int position; // �����̽� �� ��Ʈ
	int x, y; // x, y ��ġ��
	int mod; // ��� ����
	int histogramSpec[256];
	int histogram[256];
	int histogramMatch[256];
	int cdfOfHisto[256];
	int flag; // average and gaussian filtering

	srand(time(NULL)); // �����Լ� ���


	printf("	0 : nagative\n");
	printf("	1 : mosaic / block\n");
	printf("	2 : gamma correction / select(1 :gamma correction / gamma or 2 : average->average)\n");
	printf("	3 : bit plane(��Ʈ ��� ����) / position\n");
	printf("	4 : bit plane �ռ� ���� / direction(1: ������Ʈ���� �ռ�, 2 : ������Ʈ ���� �ռ�), position\n");
	printf("	5 : circle �κ� ���� ������ũ / diameter, block\n");
	printf("	6 :circle �κ� ������ũ - �߽� ���� ( 1 : ����, 2 : �簢��) / diameter(������), block(������ũ ���) / x, y �߽� ��ǥ\n");
	printf("	7 : ����þ� ���� �̿� AddNoise Img > �������� / count ���� Ƚ��\n");
	printf("	8 : ��Ʈ��Ʈ ��Ʈ��Ī(Contrast Stretching)\n");
	printf("	9 : Histogram Specification\n ");
	printf("	10 : Average and Gaussian Filtering \n ");
	printf("	11 : Median Filtering \n ");
	printf("	12 : Sharpening Filtering \n ");
	printf("	13 : Embossing Filtering \n ");
	printf("	14 : test Q1 (�߰���� 1�� ����) \n");
	printf("	15 : test Q2 (�߰���� 2�� ����) \n");
	printf("	16 : Log Power \n");
	printf("	17 : Frequency Domain ���ļ� ���� ���͸� \n");
	printf("	18 : fourier transform Ǫ���� ��ȯ  \n");
	printf("	19 : �̻��ڻ��κ�ȯ or ����ȯ \n");
	printf("	20 : Wavelet Transform \n");


	scanf_s("%d", &mod);

	Row = (int)atoi(argv[2]);
	Col = (int)atoi(argv[3]);

	img = uc_alloc(Row, Col);
	outimg = uc_alloc(Row, Col);

	read_ucmartrix(Row, Col, img, argv[1]);

	if (mod == 0) // nagative
	{
		Negative(img, outimg, Row, Col);
	}

	if (mod == 1) // mosaic
	{
		printf("block : ");
		scanf_s("%d", &block); // block : ������ũ ��� ũ��

		mosaic(img, outimg, Row, Col, block);
	}

	if (mod == 2) // gamma correction
	{
		int select; // ������ �Լ� ����
		printf("(gamma ���� Ȯ�� : 1, ���ϴ� ��հ� �̹��� ���ϱ� : 2)  ");
		printf("select : ");
		scanf_s("%d", &select);

		msum = average(img, Row, Col);

		if (select == 1) // gamma collection ���� ���� Ȯ��
		{
			printf("gamma : ");
			scanf_s("%lf", &gamma); // gamma : ��⸦ �����ϴ� r��
			PowImg(img, outimg, Row, Col, gamma);
			msum = average(outimg, Row, Col);
		}


		if (select == 2) // ���ϴ� ��հ��� ���� gamma collection ���� ���� ���
		{
			int hopeAvg;
			printf("���ϴ� ��հ� �Է� : ");
			scanf_s("%d", &hopeAvg);
			// ���ϴ� ��հ� ���ϱ�
			if (msum < hopeAvg) { // ���� �̹����� ��հ��� ���ϴ� ��հ� ���� ���� ���
				for (gamma = 1; gamma < 4; gamma += 0.005) { // ���귮�� ���̱� ���� gamma = 1(���� �̹���) ���� ����
					PowImg(img, outimg, Row, Col, gamma);
					msum = average(outimg, Row, Col);
					printf("gamma = %f\n", gamma);

					if (msum >= hopeAvg)
						break;

				}
			}
			else // ���� �̹����� ��հ��� ���ϴ� ��հ� �̻��� ���
			{
				for (gamma = 1; gamma > 0; gamma -= 0.005) { // ���귮�� ���̱� ���� gamma = 1(���� �̹���) ���� ����
					PowImg(img, outimg, Row, Col, gamma);
					msum = average(outimg, Row, Col);
					printf("gamma = %f\n", gamma);

					if (msum < hopeAvg)
						break;

				}
			}

		}


	}

	if (mod == 3) // bit plane (��Ʈ ��� ����)
	{
		printf("position : ");
		scanf_s("%d", &position); // position : BitPlane

		BitSlicing(img, outimg, Row, Col, position);
		/*
		ǥ�� ������ ������ 8bit�� �����Ǿ� 0~255 ���� ����
		������ 8���� bit plane���� �����Ǿ� ������ �ֻ��� ��Ʈ�� 1�� ��� 128���� ū ���� ������ �ǰ� 
		�ٷ� ������Ʈ�� 1�ΰ��� 64���� ū ���� ������ ����. �ش� ��Ʈ ���� 1 �̸� 255, 0�̸� 0���� ǥ��
		-> ����ȭ
		*/
	}

	if (mod == 4) // bit plane �ռ� ����
	{
		int direction; // 1: ������Ʈ���� �ռ�, 2 : ������Ʈ ���� �ռ�
		printf("(1 -> 7 ~ position / 2 -> 0 ~ position ) direction, position : ");
		scanf_s("%d %d", &direction, &position); // position : BitPlane

		tmpimg = uc_alloc(Row, Col);

		ImageCombine(img, tmpimg, outimg, Row, Col, position, direction); 

	}

	if (mod == 5) // circle �κ� ���� ������ũ
	{
		printf("diameter, block : ");
		scanf_s("%d %d", &diameter, &block); // ���� ���� (circle img)

		outimg1 = uc_alloc(Row, Col);
		outimg2 = uc_alloc(Row, Col);
		img1 = uc_alloc(Row, Col);
		img2 = uc_alloc(Row, Col);
		cirimg = uc_alloc(Row, Col);
		mosaimg = uc_alloc(Row, Col);

		mosaic(img, mosaimg, Row, Col, block); // �⺻ ������ũ �̹��� ����
		Circle(cirimg, Row, Col, diameter); // �⺻ Circle �̹��� ����

		MaskAnd(mosaimg, cirimg, outimg1, Row, Col); // ������ũ ��Ŭ �κи� ����
		MaskOr(img, cirimg, outimg2, Row, Col); // �⺻ �̹����� Or�������� ������ũ �� �κ� ����
		MaskAdd(outimg1, outimg2, outimg, Row, Col);

	}
	if (mod == 6) // circle �κ� ������ũ - �߽� ���� ( 1 : ����, 2 : �簢��)
	{
		int select;
		printf("(���� : 1, �簢�� : 2)  select : ");
		scanf_s("%d", &select);
		if (select == 1)
		{
			printf("diameter, block : ");
			scanf_s("%d %d", &diameter, &block); // ���� ������ (circle img)
			printf("x, y : ");
			scanf_s("%d %d", &x, &y); // �߽� ��ǥ

			CircleMosaic(img, outimg, Row, Col, x, y, diameter, block);
		}
		if (select == 2)
		{

			printf("diameter, block : ");
			scanf_s("%d %d", &diameter, &block); // �簢���� �Ѻ��� ���� (Square img)
			printf("x, y : ");
			scanf_s("%d %d", &x, &y); // �߽� ��ǥ

			SquareMosaic(img, outimg, Row, Col, x, y, diameter, block);

		}
	}

	if (mod == 7) // ����þ� ���� �̿� AddNoise Img > ��������
	{
		int count; // �������� ���� Ƚ��
		printf("count : ");
		scanf_s("%d", &count);

		tempimg = int_alloc(Row, Col); // int������ �޸� �Ҵ�(�������� ��� ����)

//		AddNoise(img, outimg, Row, Col);
		RmNoise(img, tempimg,outimg, Row, Col, count); // count��ŭ ������ ���Ͽ� ���� ����
	}
	if (mod == 8) // ��Ʈ��Ʈ ��Ʈ��Ī(Contrast Stretching)
	{
		contrastStreching(img, outimg, Row, Col);
	}

	if (mod == 9)
	{
		int range, t;
		int histogramEqual[256];
		int tarHistogramEqual[256] = { 0 };
		int tarRow, tarCol;

		char* targetImgName = (char*)malloc(20 * sizeof(char));



		printf("Specification Image : ");
		scanf_s("%s", targetImgName, 20*sizeof(char)); // �ֽŹ��� ���ڿ� �Է�
//		fgets(imgSpecName, sizeof(imgSpecName), stdin);
		printf("%s\n", targetImgName);
		printf("target image size : ");
		scanf_s("%d %d", &tarRow, &tarCol);

		targetImg = uc_alloc(tarRow, tarCol);

		read_ucmartrix(tarRow, tarCol, targetImg, targetImgName);

		get_Equalization(img, Row, Col, histogramEqual);
		get_Equalization(targetImg, tarRow, tarCol, tarHistogramEqual);

		get_Match(img, outimg, Row, Col, histogram, tarHistogramEqual, histogramMatch);
/*
*/
		free(targetImgName);
	}
	
	if (mod == 10) // Average nad Gaussian Filtering
	{
		printf("flag = 0 -> Gaussian Filtering\n");
		printf("flag = 1 -> Average Filtering\n");
		printf("flag : "); // 0 : Gaussian , 1 : Average
		scanf_s("%d", &flag);

		gaussMask = d_alloc(block_size, block_size);
		aveMask = d_alloc(block_size, block_size);
		
		//gaussian mask ����
		gaussMask[0][0] = 1 / 16.;
		gaussMask[0][1] = 2 / 16.;
		gaussMask[0][2] = 1 / 16.;
		gaussMask[1][0] = 2 / 16.;
		gaussMask[1][1] = 4 / 16.;
		gaussMask[1][2] = 2 / 16.;
		gaussMask[2][0] = 1 / 16.;
		gaussMask[2][1] = 2 / 16.;
		gaussMask[2][2] = 1 / 16.;

		// average mask ����
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				aveMask[i][j] = 1 / 9.;

		printf("Start Simple Filtering \n");

		if (flag == 0)
			convolution(gaussMask, block_size, Col, Row, img, outimg);
		else if (flag == 1)
			convolution(aveMask, block_size, Col, Row, img, outimg);
		else
		{
			printf("flag must be 0 or 1 \n");
			exit(1);
		}

		printf("Finished Simple Filtering \n");


	}

	
	if (mod == 11) // Median Filtering
	{
		int filterSize;
		int mode;

		printf("Filter Size(ex:3, 5) : "); // ���ͻ����� �Է� 3*3 5*5
		scanf_s("%d", &filterSize);
		printf("mode = -1 -> min \n");
		printf("mode = 0 -> average \n");
		printf("mode = 1 -> max \n");
		printf("Mode : "); // ��� ����
		scanf_s("%d", &mode);

		median(img, outimg, Row, Col, mode, filterSize);
	}

	if (mod == 12) // Sharpening Filtering 
	{
		int checkMask;
		double** Mask;
		int apply; // ���� ���� �����ų�� ���� ���� 0 : ����X, 1 : ����
		

//		outimg = d_alloc(Row, Col);
		Mask = d_alloc(mask_size, mask_size);

		printf("Sharpening Filtering \n");
		printf("	0 : sobelYMask \n");
		printf("	1 : sobelXMask \n");
		printf("	2 : PrewittYMask \n");
		printf("	3 : PrewittXMask \n");
		printf("	4 : RobertsYMask \n");
		printf("	5 : RobertsXMask \n");
		printf("	6 : Laplace1Mask \n");
		printf("	7 : Laplace2Mask \n");
		printf("	8 : Laplace3Mask \n");
		printf("	9 : Laplace4Mask \n ");
		printf("	10 : sobelYMask + sobelXMask \n ");
		printf("	11 : PrewittYMask + PrewittXMask \n ");
		printf("	12 : RobertsYMask + RobertsXMask \n ");

		printf("Check Mask : ");
		scanf_s("%d", &checkMask);
		
		printf("apply(�������� ���� ���� [0-����X 1-����O] : ");
		scanf_s("%d", &apply);

		make_Mask(mask_size, Mask, checkMask, apply);
		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				printf("mask[%d][%d] = %lf\n", i, j, Mask[i][j]);

		convolution(Mask, mask_size, Row, Col, img, outimg);

	}

	if (mod == 13) // Embossing Filtering
	{
		double** embosMask;
		int embos_size = 3; // filter size : 3*3
		embosMask = d_alloc(embos_size, embos_size);

		embosMask[0][0] = -1;
		embosMask[2][2] = 1;


		for (i = 0; i < mask_size; i++)
			for (j = 0; j < mask_size; j++)
				printf("mask[%d][%d] = %lf\n", i, j, embosMask[i][j]);

		convolutionEmbossing(embosMask, embos_size, Col, Row, img, outimg);
	}

	if (mod == 14)
	{
		int i, j, range, t;
		int histogram[256], histogramEqual[256];
		int cdfOfHisto[256];
		int center = 0;
		int min = 255, max = 0;
		uchar LUT[256];
		double scaleFactor, tmp;



		for (i = 0; i < 256; i++) // �ʱ�ȭ
			histogram[i] = 0;

		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
				histogram[img[i][j]]++; // �̹����� ������ ������ ���� ������ŭ ������Ŵ

		cdfOfHisto[0] = histogram[0];
		for (i = 1; i < 256; i++)
		{
			cdfOfHisto[i] = cdfOfHisto[i - 1] + histogram[i]; // �� ���� ���� �� ������ ���� ����
			printf("cdf[%d] = %d\t", i, cdfOfHisto[i]);
		}

		range = cdfOfHisto[255] - cdfOfHisto[0];
		histogramEqual[0] = 0;
		for (i = 0; i < 256; ++i)
		{
			t = (int)ceil(((cdfOfHisto[i] - cdfOfHisto[0]) * 255.0) / range);
			histogramEqual[i] = (t < 0) ? 0 : (t > 255) ? 255 : t;
			printf("histogramEqual[%d] = %d\n", i, histogramEqual[i]);

		}

			for (i = 0; i < 256; i++)
			{
				printf("%d", i);
				if (histogramEqual[i] > 128 )
				{
					center = i;
					break;
				}
			}
					

//		center = (histogram[(max + min) / 2])*255.0 / range;
//		center = (histogram[])*255.0 / range;

		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
				if (img[i][j] < center)
					outimg[i][j] = 0;
				else
					outimg[i][j] = 255;

		printf("center = %d\n", center);
		average(img, Row, Col);


	}
	if (mod == 15)
	{
		int filterSize;
		int mode;

		printf("Filter Size(ex:3, 5) : "); // ���ͻ����� �Է� 3*3 5*5
		scanf_s("%d", &filterSize);
		printf("mode = -1 -> min \n");
		printf("mode = 0 -> average \n");
		printf("mode = 1 -> max \n");
		printf("Mode : "); // ��� ����
		scanf_s("%d", &mode);

		median1(img, outimg, Row, Col, mode, filterSize);
	}

	if (mod == 16)
	{
		double** dimg;
		dimg = d_alloc(Row, Col);

		uchar** circle;
		circle = uc_alloc(Row, Col);


		printf("diameter : ");
		scanf_s("%d", &diameter); // ������

		Circle(circle, Row, Col, diameter);
		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
				dimg[i][j] = (double)img[i][j];

		LogImg(dimg, outimg);

	}

	if (mod == 17) // ���ļ� ���� ���͸�
	{
		int i, j, Mode = 2, select, filter;
		double max = -10E30, min = 10E30;
		uchar** out2img;
		FILE* fp1;
		double** fourier_img, ** imaginary_img, ** Butterworth, ** Gaussian;
		int cutoff;

		printf("1 : Ideal filter \n");
		printf("2 : Butterworth filter \n");
		printf("3 : Gaussian filter \n");
		printf("select : ");
		scanf_s("%d", &select);

		printf("1 : Low Pass Filter \n");
		printf("2 : High Pass Filter \n");
		printf("filter : ");
		scanf_s("%d", &filter);
		printf("Cutoff(D0) : ");
		scanf_s("%d", &cutoff);

		Butterworth = d_alloc(Row, Col);
		out2img = uc_alloc(Row, Col);
		fourier_img = d_alloc(Row, Col);
		imaginary_img = d_alloc(Row, Col);
		Gaussian = d_alloc(Row, Col);

		for(i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
			{
				fourier_img[i][j] = (double)img[i][j];
				imaginary_img[i][j] = 0.;
			}
		fft_2d(fourier_img, imaginary_img, Row, Mode);

		if (select == 1) // Ideal Filter
		{

			// Filtering Process
			for (i = 0; i < Row; i++)
				for (j = 0; j < Col; j++)
				{
					double dimeter;

					dimeter = sqrt((Row / 2. - i) * (Row / 2. - i) + (Col / 2. - j) * (Col / 2. - j));
					if (filter == 1) // low pass filter : ������ ���и� ���
					{
						if (fabs(dimeter) > cutoff) // fabs : double���� ���밪 ��ȯ , cutoff 
						{
							fourier_img[i][j] = 0;
							imaginary_img[i][j] = 0;
						}
						else
							imaginary_img[i][j] = -imaginary_img[i][j];
					}
					else if (filter == 2) // high pass filter : ������ ���и� ���
					{
						if (fabs(dimeter) < cutoff) // fabs : double���� ���밪 ��ȯ , cutoff 
						{
							fourier_img[i][j] = 0;
							imaginary_img[i][j] = 0;
						}
						else
							imaginary_img[i][j] = -imaginary_img[i][j];

					}
					
				}
			// End of Ideal Filtering
			fft_2d(fourier_img, imaginary_img, Row, Mode - 1);
			// Shift Process
			for (i = 0; i < Row; i++)
				for (j = 0; j < Col; j++)
				{
					fourier_img[i][j] = fourier_img[i][j] * pow(-1, i + j);
				}

		}
		if (select == 2) // Butterworth Filter
		{

			// Filtering Process
			// For Butterworth Filtering
			int n = 2;
//			int D0 = 50;
			
			double diameter;

			for (i = 0; i < Row; i++)
				for (j = 0; j < Col; j++)
				{
					diameter = sqrt((Row / 2. - i) * (Row / 2. - i) + (Col / 2. - j) * (Col / 2. - j));

					if (filter == 1)
					{
						// For Lowpass Filter
						Butterworth[i][j] = 1 / (1 + pow(diameter / cutoff, 2 * n)); // cutoff(D0)
					}
					else if (filter == 2)
					{
						// For HighPass Filter
						Butterworth[i][j] = 1 - 1/ (1 + pow(diameter / cutoff, 2 * n)); // cutoff(D0)
					}

					fourier_img[i][j] *= Butterworth[i][j];
					imaginary_img[i][j] *= -Butterworth[i][j]; // Effect for flip image
				}

			fft_2d(fourier_img, imaginary_img, Row, Mode - 1);

			for (i = 0; i < Row; i++)
				for (j = 0; j < Col; j++)
					fourier_img[i][j] = fourier_img[i][j] * pow(-1, i + j);

		}
		if (select == 3)
		{
			for (i = 0; i < Row; i++)
				for (j = 0; j < Col; j++)
				{
					diameter = sqrt((Row / 2. - i) * (Row / 2. - i) + (Col / 2. - j) * (Col / 2. - j));
					if (filter == 1)
						Gaussian[i][j] = exp(-(diameter * diameter) / (2 * cutoff * cutoff));
					else if(filter == 2)
						Gaussian[i][j] = 1 - exp(-(diameter * diameter) / (2 * cutoff * cutoff));

					fourier_img[i][j] *= Gaussian[i][j];
					imaginary_img[i][j] *= -Gaussian[i][j]; // Effect for flip
				}
			fft_2d(fourier_img, imaginary_img, Row, Mode - 1); // Original FFT

			if (filter == 2)
			{
				for (i = 0; i < Row; i++)
					for (j = 0; j < Col; j++)
						fourier_img[i][j] *= pow(-1, i + j);
			}
			
		}

		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
			{
				if (max < fourier_img[i][j]) max = fourier_img[i][j];
				if (min > fourier_img[i][j]) min = fourier_img[i][j];
			}
		printf("clipping before -- Fourier Max Min Value = %lf  %lf\n", max, min);

		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
			{
				if (fourier_img[i][j] > 255) fourier_img[i][j] = 255;
				else if (fourier_img[i][j] < 0) fourier_img[i][j] = 0;

				outimg[i][j] = (uchar)fourier_img[i][j];

			}
		
	}
	if (mod == 18)
	{
		double** fourierimg, ** imimg;
		int temp;
		fourierimg = d_alloc(Row, Col);
		imimg = d_alloc(Row, Col);

		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
			{
				fourierimg[i][j] = (double)img[i][j];
				imimg[i][j] = 0.;
			}
		fft_2d(fourierimg, imimg, Row, 2); // N : N��ŭ�� ó��(���ٿ�)
		// LogImg(fourierimg, outimg);
		conv_d_to_uc(Row, Col, fourierimg, outimg);
		/*
		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
			{
				temp = fourierimg[i][j];
				if (temp > 255) outimg[i][j] = 255;
				else if (temp < 0) outimg[i][j] = 0;
				else outimg[i][j] = temp;

			}
		*/
	}

	if (mod == 19) // Discrete Cosine Transform(DCT)
	{
		int** iimg, ** ioutimg;
		int mode;

		printf("mode (1 : DCT, 2 : Inverse DCT) : ");
		scanf_s("%d", &mode);

		iimg = i_alloc(Row, Col);
		ioutimg = i_alloc(Row, Col);

		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
				iimg[i][j] = (int)img[i][j];

		if (mode == 1)
			mFdct(iimg, ioutimg, Row, Col);
		else if(mode == 2)
			mIdct(iimg, ioutimg, Row, Col);

		for (i = 0; i < Row; i++)
			for (j = 0; j < Col; j++)
			{
				if (ioutimg[i][j] > 255) outimg[i][j] = 255;
				else if (ioutimg[i][j] < 0) outimg[i][j] = 0;
				else outimg[i][j] = (uchar)ioutimg[i][j];
			}
		
	}
	if (mod == 20) // ���̺� ��ȯ
	{
		int i, j;
		double **dimage1, **dimage2, **dimage3;

		dimage1 = d_alloc(Row, Col);
		dimage2 = d_alloc(Row/2, Col);
		dimage3 = d_alloc(Row/2, Col/2);

		conv_uc_to_d(Row, Col, img, dimage1);
		row_analysis(A97L1, A97H1_l, Row, Col, dimage1, dimage2);
		column_analysis(A97L1, A97L1_l, Row, Col, dimage2, dimage3);

		conv_d_to_uc(Row, Col, dimage3, outimg);

	}

	write_ucmatrix(Row, Col, outimg, argv[4]);

	return 0;
}
