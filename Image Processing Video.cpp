/*HW4_Level1: �ɪO�H������*/
//#include "stdafx.h" //�p�G�M�׼��Y�ɸ̸̦�stdafx.h�A��l�{���ɦ�stdafx.cpp�A�ݥ[�J�o�@��C�p�G�O�űM�סA�h���Υ[�o�@��C
#include "opencv2/opencv.hpp" //���Jopencv�禡�w
#include <iostream> //���Jc++�禡�w

using namespace cv; //�ŧi opencv �禡�w���R�W�Ŷ�
using namespace std; //�ŧi C++�禡�w���R�W�Ŷ�

#define COINS 20 //�ɪO�H���������`�q�A�����O��ơA�ƭȶV�C�V�}��
//RNG rng(1234); //�ھ�1234�ؤl�ȡA���ͯS�w���Ǥ��H����
RNG rng(clock()); //�ھ�clock()�����H���ơA�C�����泣���P

/*�D�{��*/
int main(void)
{
	/*�ܼ�(�v���e��)�ŧi*/
	Mat im1, im2, im3, im4;
	Mat im1_gray, im2_gray, im3_gray, im4_gray;

	im1 = imread("data/c01.png", 1); //���J1���ɪO�v��
	im2 = imread("data/c05.png", 1); //���J5���ɪO�v��
	im3 = imread("data/c10.png", 1); //���J10���ɪO�v��
	im4 = imread("data/c50.png", 1); //���J50���ɪO�v��
	cvtColor(im1, im1_gray, COLOR_BGR2GRAY); //1���ɪO�ഫ�ܦǶ��榡
	cvtColor(im2, im2_gray, COLOR_BGR2GRAY); //5���ɪO�ഫ�ܦǶ��榡
	cvtColor(im3, im3_gray, COLOR_BGR2GRAY); //10���ɪO�ഫ�ܦǶ��榡
	cvtColor(im4, im4_gray, COLOR_BGR2GRAY); //50���ɪO�ഫ�ܦǶ��榡
	threshold(im1_gray, im1_gray, 10, 255, THRESH_BINARY);  //1���ɪO�G�Ȥ�
	threshold(im2_gray, im2_gray, 10, 255, THRESH_BINARY);  //5���ɪO�G�Ȥ�
	threshold(im3_gray, im3_gray, 10, 255, THRESH_BINARY);  //10���ɪO�G�Ȥ�
	threshold(im4_gray, im4_gray, 10, 255, THRESH_BINARY);  //50���ɪO�G�Ȥ�

	VideoCapture cap("data/moving.mp4"); //Ū���I���v��
	if (!cap.isOpened()) return 0;

	Size S(int(cap.get(3)), int(cap.get(4)));		//Ū�����T�e/�� cap.get:3(�����e��) cap.get:4(��������) 
	int frame_count = cap.get(CAP_PROP_FRAME_COUNT); //Ū�����T�e���`��

	VideoWriter put("data/drop.mp4", VideoWriter::fourcc('X', '2', '6', '4'), 60, S); //�إ߻ɪO�H�����������T����

	if (!put.isOpened())
	{
		cout << "File could not be created for writing." << endl;
		return -1;
	}
	namedWindow("Video"); //�إߵ��T����

	int start[COINS][3]; //�C�ӻɪO�������_�l�e���s���B�����B������m

	for (int i = 0; i < COINS; i++)
	{
		start[i][0] = rng.uniform(0, frame_count - 100);	//�ɪOi���_�l�e���s��(�H��)
		start[i][1] = rng.uniform(1, 5);					//�ɪOi������(�H��)
		start[i][2] = rng.uniform(0, S.width - 150);		//�ɪOi��������V��m(�H��)
	}

	//��for�j��Ū���C�@�e��
	for (int n = 0; n < frame_count; n++)
	{
		Mat im_bg;
		cap >> im_bg; //Ū���I���v��
		if (im_bg.empty()) //�p�G�v�����s�b
		{
			printf(" --(!) No captured im -- Break!");  //��ܿ��~�T��
			break;
		}

		//�N��i �ӻɪO�����v���[�J�I���v�����j��
		for (int i = 0; i < COINS; i++)
		{
			int y = 5 * (n - start[i][0]); //�ɪOi�x�ήت�y�y��(�x�ήؤW�t��������V�y��)
			if (y > 0 && y < S.height - 150) //�p�G�ɪOi�x�ή�(������V)�b�e���d�򤺡A����U�C�ʧ@
			{
				switch (start[i][1]) //�ھڻɪOi�������A��ܻɪO�v��
				{
				case 1: //1���w��
				{
					//�ھ�start[i][2]�Py����m�A�إ� roi1 �x�ή� 
					Rect roi1(start[i][2], y, im1.size().width, im1.size().height);
					//�N im1 �ƻs�� im_bg ���x�ήءA�H im1_gray ���B�n
					im1.copyTo(im_bg(roi1),im1_gray);
					break; //���} switch
				}
				case 2: //5���w��
				{
					//�ھ�start[i][2]�Py����m�A�إ� roi2 �x�ή�(Rect type)
					Rect roi2(start[i][2], y, im2.size().width, im2.size().height);
					//�N im2 �ƻs�� im_bg ���x�ήءA�H im2_gray ���B�n
					im2.copyTo(im_bg(roi2), im2_gray);
					break; //���} switch
				}
				case 3: //10���w��
				{
					//�ھ�start[i][2]�Py����m�A�إ� roi3 �x�ή�(Rect type)
					Rect roi3(start[i][2], y, im3.size().width, im3.size().height);
					//�N im3 �ƻs�� im_bg ���x�ήءA�H im3_gray ���B�n
					im3.copyTo(im_bg(roi3), im3_gray);
					break; //���} switch
				}
				case 4: //50���w��
				{
					//�ھ�start[i][2]�Py����m�A�إ� roi4 �x�ή�(Rect type)
					Rect roi4(start[i][2], y, im4.size().width, im4.size().height);
					//�N im4 �ƻs�� im_bg ���x�ήءA�H im4_gray ���B�n
					im4.copyTo(im_bg(roi4), im4_gray);
					break; //���} switch 
				}
				}
			}
		}

		imshow("Video", im_bg); //����|�W�ɪO�᪺�I���v��
		waitKey(10); //����10�@��
		put << im_bg; //�ץX�v����n��X�����T��
	}
	cout << "please check: data/drop.mp4" << endl; //��ܿ�X���T�����|/�ɦW

	return 0;
}