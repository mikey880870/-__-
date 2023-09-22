//HW4_Level2: ���� drop.mp4 ���T���A�������ɪO�A�íp����`���B�C(���G�S�����체���ɤ���)
//1.�]�w�Ѽ�
//2.�������T�����H�y�P�H��
//3.��������H���ɡA�ﱼ�����ɪO�� HoughCircles ����
//4.ø�s�H�y/�H����
//5.ø�s�ɪO��ήءA�H�ΤH�����ߪ��p�g��
//6.�Ϊ���ϲέp�C�Ӥ����϶��A���g�������ߪ��̰����B(�ھڨ�b�|)
//7.��X�t�������ءN�p�g���B�������B�����T
//8.�ھڪ���ϡA�έp�ɪO���`���B

#include "opencv2/opencv.hpp"	//���Jopencv�禡�w
#include <iostream>				//���Jc++�禡�w
//#include "stdafx.h"			//�p�G�M�׼��Y�ɸ̸̦�stdafx.h�A��l�{���ɦ�stdafx.cpp�A�ݥ[�J�o�@��C�p�G�O�űM�סA�h���Υ[�o�@��C

using namespace cv;				//�ŧi opencv �禡�w���R�W�Ŷ�
using namespace std;			//�ŧi C++�禡�w���R�W�Ŷ�

/* �ѼƳ]�w(�����ܼ�) */
Mat frame;						//���T�v���e��(Mat class)
int show_rect = 1;				//�O�_��ܤH�y/�H���ت��Ѽ�(int type)
int frame_count = 0;			//���T�e���s���p�ƾ�(int type)
bool draw_lines = true;         //�O�_�n�e�y/�����~��
int draw_type = 1;				//�n�e�x���٬O��� (1 : �x�� 2 : ���) 
int coins_sum = 0;              //�`���B
int hist_sum = 0;
int coins_sum_helper = 0;       //���B�Ȧs�ܼ�

//�����H���������T����(VideoCapture class)
VideoCapture cap("data/drop.mp4");

//���T�v���e,��(Size type)
Size S(int(cap.get(3)), int(cap.get(4)));		//Ū�����T�e/�� cap.get:3(�����e��) cap.get:4(��������) 

//��X�������G�����T����(VideoWriter class)
VideoWriter put("data/output.mp4", VideoWriter::fourcc('X', '2', '6', '4'), 60, S);

//�H�y���p��������(CascadeClassifier class)
CascadeClassifier faceCascade;
//�H�����p��������(CascadeClassifier class)
CascadeClassifier eyes_cascade;

vector<Rect> faces; //�H�y�x�ήئV�q(Rect type)
vector<Rect> eyes; //�H���x�ήئV�q(Rect type)
vector<Vec3f> circles; //�ɪO��ΦV�q(Vec3f type)
float binW = 10;	 //����ϨC�Ӥ����϶���M�v���e�ת�����(���:����)

/* �l�{���쫬�ŧi */
vector<Rect> detectFace(Mat);			//�H�y����
vector<Rect> detectEye(Mat);			//�H������
vector<Vec3f> detectCircle(Mat);		//�ɪO��ΰ���
Mat boxFace(Mat, vector<Rect>);			//�H�y��ø�s
Mat drawCircle(Mat, vector<Vec3f>);		//�H����ø�s
Mat drawEye(Mat, Rect, vector<Rect>);	//�ɪO���ø�s

/* �D�{�� */
int main(int, char** argv) {


	Mat frame_gray;           //�ŧi�Ƕ��v���ܼ�
	Mat face_roi;
	/* ���p���������l��(���Jxml�Ѽ�) */
	if (!faceCascade.load("data/haarcascade_frontalface_alt.xml")) { printf("--(!)Error loading face cascade\n"); return -1; };
	if (!eyes_cascade.load("data/haarcascade_eye_tree_eyeglasses.xml")) { printf("--(!)Error loading eyes cascade\n"); return -1; };
	if (!put.isOpened()) { return -1; };

	/* �έp���B������� hist ��l��*/
	//Mat class�A�s�x�}�A�e=���T�e/binW�A��=1�ACV_32FC1�榡

	Mat hist = Mat::zeros(1, int(cap.get(3)) / binW, CV_32FC1);
	Mat histImage;
	int histSize = 64;

	bool c = false;    // �P�_ histImage ���L�w�q�L

	cout << "�� r �����O�_����y/����" << endl;
	cout << "�� Esc ���}" << endl;
	cout << "�� q �����y/���~�اΪ� " << endl;
	cout << "******************************" << endl;

	//�p�G���T����i�HŪ���ɡA����while�j�餤�����e
	while (cap.isOpened()){
		cap.read(frame); //Ū���e��
		if (frame.empty()) break; //�p�G�e��O�Ū��A���Xwhile�j��
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY); //���l�v����Ƕ�

		// �ŧi histImage
		if (!c) {
			histImage = Mat::zeros(1, int(cap.get(3)) / binW, CV_32FC1);
			c = true;
		}

		/* �����H�y */
		//�l�{�������H�y�A��X�H�y��
		faces = detectFace(frame_gray);    
		//�p�G���H�y�ءA����U�C�ʧ@
		if(!faces.empty())
		{
			 //�p�G��ܤH�y/�H���ت��ѼƫD0�A����U�Cø�Ϥl�{��
			{
				//�l�{�������H��(�`�N�G�Ȱ���faces[0]�϶�)
				face_roi = frame_gray(faces[0]);
				eyes = detectEye(face_roi);
				//�l�{��ø�s�H���ءB�H�y��
				if (draw_lines) {
					boxFace(frame, faces);
					drawEye(frame, faces[0], eyes);
				}
			}

			/* �������체�� �B ��U�e��s�����H5�l0��(�i�g��not(frame_count %5))... */
			if(!eyes.empty() && frame_count % 5 == 0)
			{ 
				//�l�{�������e���������
				circles = detectCircle(frame_gray);
				/* ���������(�w��)��... */
				if (circles.size() > 0) {
					 //�l�{��ø�s��ή�
					drawCircle(frame, circles);
					// for �j��A�ﰻ���쪺��ΡA�@�@ø�s�ӦۤH�����p�g�A��ܿ������B�A�æs�Jhist�����
					for(int i = 0;i < circles.size();i++)
					{						
						//��i�Ӷ�ζ�ߪ�x�y��(int type)
						int center_x = cvRound(circles[i][0]);
						
						//��i�Ӷ�ζ�ߪ�y�y��(int type)
						int center_y = cvRound(circles[i][1]);
						//��i�Ӷ�Ϊ��b�|(int type)
						int radius = cvRound(circles[i][2]);                         

						//�p�Ⲵ�������߮y��(Point type)
						for (size_t i = 0; i < faces.size(); i++) {
							for (size_t j = 0; j < eyes.size(); j++) {
								Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
								line(frame, eye_center, Point(center_x, center_y), Scalar(0, 0, 255), 2);
							}
						}

						/* �Q�εw���b�|�P�_���B */
						//�b�����W��A��putText()��ܪ��B
						if(circles[i][2] <= 47)
						{ 
							//�p�G���<=47�A���B=1
							putText(frame, "1", Point(center_x, center_y - radius), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255), 2);
							coins_sum_helper = 1;
						}
						else if(circles[i][2] > 47 && circles[i][2] <= 60)
						{ 
							//�p�G���>47 && <=60�A���B=5
							putText(frame, "5", Point(center_x, center_y - radius), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255), 2);
							coins_sum_helper = 5;

						}
						else if(circles[i][2] > 60 && circles[i][2] <= 67)
						{ 
							//�p�G���>60 && <=67�A���B=10
							putText(frame, "10", Point(center_x, center_y - radius), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255), 2);
							coins_sum_helper = 10;
						}
						else
						{ 
							//�_�h�A���B=50
							putText(frame, "50", Point(center_x, center_y - radius), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255), 2);
							coins_sum_helper = 50;
						}

						//�ھڶ��x�y�п��hist���϶��A�p�G�w�����B����Ӱ϶����̰����B�A�h��s�����B�C�i��|�Ψ�at<float>, cvRound, MAX()

						int x_tohistx = center_x / binW; // ����Ϧ� 64 ��A�� frame ���e�׬� 640 �A�G�N��� x �y�а� binW �ι��� 64 �椤���۹��m

						// �p�⪽���
						calcHist(&frame_gray,1,0,Mat(),hist,1,&histSize,0);

						// �]�w����ϼƭ� 
						for (int i = 0; i < histSize; i++){
							if (i == x_tohistx) {
								hist.at<float>(x_tohistx,0) = coins_sum_helper;           // ����������ϳ]�w�����������B
							}
							else {
								hist.at<float>(i, 0) = 0;                                  // ��l���]�� 0
							}
						}

						// ����ϼƭ��`�M
						int hist_sum = int(sum(hist).val[0]);
						// �`���B�`�M
						coins_sum += hist_sum;

						// ø�s����� bar�A�C�⬰ coins_sum_helper (���������B)
						rectangle(histImage, Point(x_tohistx, histImage.rows), 
							Point(x_tohistx , histImage.rows - cvRound(hist.at<float>(x_tohistx))), 
							Scalar::all(coins_sum_helper), -1, 8, 0);
					}
				}
			}
		}

		/* ��ܼv�� */

		//�b�e���k�U���A��putText()��ܻɪO�`���B�C���G���B���r��i�� to_string(int(sum(hist).val[0]))
		putText(frame, to_string(coins_sum), Point(550, 450), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 255), 2);
		//��ܻɪO�����e��
		imshow("frame", frame);			
		//�N�e���s�J�ݿ�X�����T����
		put << frame;					

		/* ��ܪ���� */

		//�Nhist�Aresize��j�C������V��j8���A������V��j40��
		Mat r_img;    // �s�� resize �L�� histImage 
		resize(histImage, r_img, Size(), 8, 40);
		//hist �v�������ƭȬO[0,1,5,10,50]�A���e���ݥX���t�t�O�A�]���N�ƭȶ}�ڸ� sqrt(src,dst)�C
		sqrt(r_img, r_img);
		//���Nhist�v���ƭȰ��H5�A�A��ܸӼv���C���P���B���ɪO�A�N�e�{���P�����t�A���B�V���V�G�C
		r_img = r_img / 5;
		//��� hist ���G
		imshow("histogram", r_img);

		/* ��L�B�z */
		int c = waitKey(10); 

		//�� Esc ���} while �j��
		if (c == 27) { break; }

		//�� 'r' �����H/���ت���ܪ��A
		if (c == 114) { 
			if (draw_lines == true) { draw_lines = false; }
			else { draw_lines = true; }
		}

		//�� 'q' �����H/���ت�ø�s���A(�x��/���)
		if (c == 113) {
			if (draw_type == 1) {
				draw_type = 2;
			}
			else {
				draw_type = 1;
			}
		}
		//�e��s���[1
	}
	
	//�b command window ����`���B(��int(sum(hist).val[0]))
	printf("sum = %d\n", coins_sum);
	destroyAllWindows(); //�����Ҧ�����
	
	return 0;
}

// �����H�y����m (x, y, w, h)
vector<Rect> detectFace(Mat frame_gray) {
	vector<Rect> faces;
	 //�H�y�h�ثׯ��p�����A�Ѽ�1.5, 4, 0, Size(80, 80)
	faceCascade.detectMultiScale(frame_gray,faces,1.5,4,0 | CASCADE_SCALE_IMAGE, Size(80, 80));
	return faces;
}

// ������������m (x, y, w, h)
vector<Rect> detectEye(Mat faceROI) {
	vector<Rect> eyes;
	 //�H���h�ثׯ��p�����A�Ѽ�1.1, 2, 0, Size(20, 20)
	eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(20, 20));
	return eyes;
}	

// �����ꪺ��m (x, y, r)
vector<Vec3f> detectCircle(Mat frame_gray) {
	vector<Vec3f> circles;
	//���C���T
	GaussianBlur(frame_gray, frame_gray, Size(9, 9), 2, 2);
	//�N�Ҷ갻��
	HoughCircles(frame_gray, circles, HOUGH_GRADIENT, 1.3, 120, 150, 100, 0, 0);
	return circles;
}

// �e�X�H�y����m
Mat boxFace(Mat frame, vector<Rect> faces) {
	if (faces.size() > 0) {
		for (size_t i = 0; i < faces.size(); i++) {
			int x = faces[i].x;
			int y = faces[i].y;
			int w = faces[i].width;
			int h = faces[i].height;
			// draw_type == 1 �e�x�ΡA draw_type == 2 �e���
			if (draw_type == 1) { 
				rectangle(frame, Point(x, y), Point(x + w, y + h), Scalar(255, 255, 0), 1, LINE_AA);
			}
			else {
				int face_xcenter = x + w / 2;
				int face_ycenter = y + h / 2;
				ellipse(frame,Point(face_xcenter, face_ycenter),Size((w/2),(h/2)),0,0,360, Scalar(255, 255, 0),1, LINE_AA,0);
			}
		}
	}
	return frame;
}

// �e�X�ꪺ��m
Mat drawCircle(Mat frame, vector<Vec3f> circles){
	{ //��for�j��ø�s circles ���C�@�Ӷ��
		for (size_t i = 0; i < circles.size(); i++)
		{
			//�i�λݭncvRound()�|�ˤ��J�����
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));   // [0] �� circles ���ߪ� x �y�СA [1]�� y �y��
			int radius = cvRound(circles[i][2]);                            // [2] �� �b�|

			//��circle�A����A�u��=2
			circle(frame, center, radius, Scalar(0, 0, 255), 2, 8, 0);
		}
	}
	return frame;
}

// �e�X��������m
Mat drawEye(Mat frame, Rect face, vector<Rect> eyes) {
	{
		//��for�j��ø�s eyes ����
		for (size_t i = 0; i < faces.size(); i++) {
			for (size_t j = 0; j < eyes.size(); j++) {
				//�ݭn�Nface��x,y�y�Х[�Weyes��x,y�y��
				Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
				int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);

				// draw_type == 1 �e�x�ΡA draw_type == 2 �e���
				if (draw_type == 1) {
					rectangle(frame, Point(eye_center.x - radius, eye_center.y + radius), Point(eye_center.x + radius, eye_center.y - radius), Scalar(255, 255, 0), 1, LINE_AA);
				}
				else {
					circle(frame, eye_center, radius, Scalar(255, 255, 0), 1, 8, 0);
				}
			}
		}
	}
	return frame;
}
	


