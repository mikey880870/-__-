//HW4_Level2: 偵測 drop.mp4 視訊中，掉落的銅板，並計算其總金額。(註：沒偵測到眼睛時不算)
//1.設定參數
//2.偵測視訊中的人臉與人眼
//3.有偵測到人眼時，對掉落的銅板做 HoughCircles 偵測
//4.繪製人臉/人眼框
//5.繪製銅板圓形框，以及人眼到圓心的雷射光
//6.用直方圖統計每個水平區間，曾經偵測到圓心的最高金額(根據其半徑)
//7.輸出含有偵測框﹑雷射光、偵測金額的視訊
//8.根據直方圖，統計銅板的總金額

#include "opencv2/opencv.hpp"	//載入opencv函式庫
#include <iostream>				//載入c++函式庫
//#include "stdafx.h"			//如果專案標頭檔裡裡有stdafx.h，原始程式檔有stdafx.cpp，需加入這一行。如果是空專案，則不用加這一行。

using namespace cv;				//宣告 opencv 函式庫的命名空間
using namespace std;			//宣告 C++函式庫的命名空間

/* 參數設定(全域變數) */
Mat frame;						//視訊影像容器(Mat class)
int show_rect = 1;				//是否顯示人臉/人眼框的參數(int type)
int frame_count = 0;			//視訊畫面編號計數器(int type)
bool draw_lines = true;         //是否要畫臉/眼睛外框
int draw_type = 1;				//要畫矩形還是圓形 (1 : 矩形 2 : 圓形) 
int coins_sum = 0;              //總金額
int hist_sum = 0;
int coins_sum_helper = 0;       //金額暫存變數

//錢幣隨機掉落視訊物件(VideoCapture class)
VideoCapture cap("data/drop.mp4");

//視訊影像寬,高(Size type)
Size S(int(cap.get(3)), int(cap.get(4)));		//讀取視訊寬/高 cap.get:3(偵的寬度) cap.get:4(偵的高度) 

//輸出偵測結果的視訊物件(VideoWriter class)
VideoWriter put("data/output.mp4", VideoWriter::fourcc('X', '2', '6', '4'), 60, S);

//人臉級聯偵測物件(CascadeClassifier class)
CascadeClassifier faceCascade;
//人眼級聯偵測物件(CascadeClassifier class)
CascadeClassifier eyes_cascade;

vector<Rect> faces; //人臉矩形框向量(Rect type)
vector<Rect> eyes; //人眼矩形框向量(Rect type)
vector<Vec3f> circles; //銅板圓形向量(Vec3f type)
float binW = 10;	 //直方圖每個水平區間對映影像寬度的長度(單位:像素)

/* 子程式原型宣告 */
vector<Rect> detectFace(Mat);			//人臉偵測
vector<Rect> detectEye(Mat);			//人眼偵測
vector<Vec3f> detectCircle(Mat);		//銅板圓形偵測
Mat boxFace(Mat, vector<Rect>);			//人臉框繪製
Mat drawCircle(Mat, vector<Vec3f>);		//人眼框繪製
Mat drawEye(Mat, Rect, vector<Rect>);	//銅板圓框繪製

/* 主程式 */
int main(int, char** argv) {


	Mat frame_gray;           //宣告灰階影像變數
	Mat face_roi;
	/* 級聯偵測物件初始化(載入xml參數) */
	if (!faceCascade.load("data/haarcascade_frontalface_alt.xml")) { printf("--(!)Error loading face cascade\n"); return -1; };
	if (!eyes_cascade.load("data/haarcascade_eye_tree_eyeglasses.xml")) { printf("--(!)Error loading eyes cascade\n"); return -1; };
	if (!put.isOpened()) { return -1; };

	/* 統計金額的直方圖 hist 初始化*/
	//Mat class，零矩陣，寬=視訊寬/binW，高=1，CV_32FC1格式

	Mat hist = Mat::zeros(1, int(cap.get(3)) / binW, CV_32FC1);
	Mat histImage;
	int histSize = 64;

	bool c = false;    // 判斷 histImage 有無定義過

	cout << "按 r 切換是否顯示臉/眼框" << endl;
	cout << "按 Esc 離開" << endl;
	cout << "按 q 切換臉/眼外框形狀 " << endl;
	cout << "******************************" << endl;

	//如果視訊物件可以讀取時，執行while迴圈中的內容
	while (cap.isOpened()){
		cap.read(frame); //讀取畫格
		if (frame.empty()) break; //如果畫格是空的，跳出while迴圈
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY); //對原始影像轉灰階

		// 宣告 histImage
		if (!c) {
			histImage = Mat::zeros(1, int(cap.get(3)) / binW, CV_32FC1);
			c = true;
		}

		/* 偵測人臉 */
		//子程式偵測人臉，輸出人臉框
		faces = detectFace(frame_gray);    
		//如果有人臉框，執行下列動作
		if(!faces.empty())
		{
			 //如果顯示人臉/人眼框的參數非0，執行下列繪圖子程式
			{
				//子程式偵測人眼(注意：僅偵測faces[0]區塊)
				face_roi = frame_gray(faces[0]);
				eyes = detectEye(face_roi);
				//子程式繪製人眼框、人臉框
				if (draw_lines) {
					boxFace(frame, faces);
					drawEye(frame, faces[0], eyes);
				}
			}

			/* 當有偵測到眼睛 且 當下畫格編號除以5餘0時(可寫成not(frame_count %5))... */
			if(!eyes.empty() && frame_count % 5 == 0)
			{ 
				//子程式偵測畫面中的圓形
				circles = detectCircle(frame_gray);
				/* 當有偵測到圓(硬幣)時... */
				if (circles.size() > 0) {
					 //子程式繪製圓形框
					drawCircle(frame, circles);
					// for 迴圈，對偵測到的圓形，一一繪製來自人眼的雷射，顯示錢幣金額，並存入hist直方圖
					for(int i = 0;i < circles.size();i++)
					{						
						//第i個圓形圓心的x座標(int type)
						int center_x = cvRound(circles[i][0]);
						
						//第i個圓形圓心的y座標(int type)
						int center_y = cvRound(circles[i][1]);
						//第i個圓形的半徑(int type)
						int radius = cvRound(circles[i][2]);                         

						//計算眼睛的中心座標(Point type)
						for (size_t i = 0; i < faces.size(); i++) {
							for (size_t j = 0; j < eyes.size(); j++) {
								Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
								line(frame, eye_center, Point(center_x, center_y), Scalar(0, 0, 255), 2);
							}
						}

						/* 利用硬幣半徑判斷金額 */
						//在金幣上方，用putText()顯示金額
						if(circles[i][2] <= 47)
						{ 
							//如果圓心<=47，金額=1
							putText(frame, "1", Point(center_x, center_y - radius), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255), 2);
							coins_sum_helper = 1;
						}
						else if(circles[i][2] > 47 && circles[i][2] <= 60)
						{ 
							//如果圓心>47 && <=60，金額=5
							putText(frame, "5", Point(center_x, center_y - radius), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255), 2);
							coins_sum_helper = 5;

						}
						else if(circles[i][2] > 60 && circles[i][2] <= 67)
						{ 
							//如果圓心>60 && <=67，金額=10
							putText(frame, "10", Point(center_x, center_y - radius), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255), 2);
							coins_sum_helper = 10;
						}
						else
						{ 
							//否則，金額=50
							putText(frame, "50", Point(center_x, center_y - radius), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255), 2);
							coins_sum_helper = 50;
						}

						//根據圓心x座標選擇hist的區間，如果硬幣金額高於該區間的最高金額，則更新此金額。可能會用到at<float>, cvRound, MAX()

						int x_tohistx = center_x / binW; // 直方圖有 64 格，而 frame 的寬度為 640 ，故將圓心 x 座標除 binW 及對應 64 格中的相對位置

						// 計算直方圖
						calcHist(&frame_gray,1,0,Mat(),hist,1,&histSize,0);

						// 設定直方圖數值 
						for (int i = 0; i < histSize; i++){
							if (i == x_tohistx) {
								hist.at<float>(x_tohistx,0) = coins_sum_helper;           // 相應的直方圖設定為錢幣的金額
							}
							else {
								hist.at<float>(i, 0) = 0;                                  // 其餘的設為 0
							}
						}

						// 直方圖數值總和
						int hist_sum = int(sum(hist).val[0]);
						// 總金額總和
						coins_sum += hist_sum;

						// 繪製直方圖 bar，顏色為 coins_sum_helper (錢幣的金額)
						rectangle(histImage, Point(x_tohistx, histImage.rows), 
							Point(x_tohistx , histImage.rows - cvRound(hist.at<float>(x_tohistx))), 
							Scalar::all(coins_sum_helper), -1, 8, 0);
					}
				}
			}
		}

		/* 顯示影像 */

		//在畫面右下角，用putText()顯示銅板總金額。註：金額的字串可用 to_string(int(sum(hist).val[0]))
		putText(frame, to_string(coins_sum), Point(550, 450), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 255), 2);
		//顯示銅板偵測畫面
		imshow("frame", frame);			
		//將畫面存入待輸出的視訊物件
		put << frame;					

		/* 顯示直方圖 */

		//將hist，resize放大。水平方向放大8倍，垂直方向放大40倍
		Mat r_img;    // 存取 resize 過的 histImage 
		resize(histImage, r_img, Size(), 8, 40);
		//hist 影像中的數值是[0,1,5,10,50]，不容易看出明暗差別，因此將數值開根號 sqrt(src,dst)。
		sqrt(r_img, r_img);
		//先將hist影像數值除以5，再顯示該影像。不同金額的銅板，將呈現不同的明暗，金額越高越亮。
		r_img = r_img / 5;
		//顯示 hist 結果
		imshow("histogram", r_img);

		/* 鍵盤處理 */
		int c = waitKey(10); 

		//按 Esc 離開 while 迴圈
		if (c == 27) { break; }

		//按 'r' 切換人/眼框的顯示狀態
		if (c == 114) { 
			if (draw_lines == true) { draw_lines = false; }
			else { draw_lines = true; }
		}

		//按 'q' 切換人/眼框的繪製型態(矩形/圓形)
		if (c == 113) {
			if (draw_type == 1) {
				draw_type = 2;
			}
			else {
				draw_type = 1;
			}
		}
		//畫格編號加1
	}
	
	//在 command window 顯示總金額(用int(sum(hist).val[0]))
	printf("sum = %d\n", coins_sum);
	destroyAllWindows(); //關閉所有視窗
	
	return 0;
}

// 偵測人臉的位置 (x, y, w, h)
vector<Rect> detectFace(Mat frame_gray) {
	vector<Rect> faces;
	 //人臉多尺度級聯偵測，參數1.5, 4, 0, Size(80, 80)
	faceCascade.detectMultiScale(frame_gray,faces,1.5,4,0 | CASCADE_SCALE_IMAGE, Size(80, 80));
	return faces;
}

// 偵測眼睛的位置 (x, y, w, h)
vector<Rect> detectEye(Mat faceROI) {
	vector<Rect> eyes;
	 //人眼多尺度級聯偵測，參數1.1, 2, 0, Size(20, 20)
	eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(20, 20));
	return eyes;
}	

// 偵測圓的位置 (x, y, r)
vector<Vec3f> detectCircle(Mat frame_gray) {
	vector<Vec3f> circles;
	//降低雜訊
	GaussianBlur(frame_gray, frame_gray, Size(9, 9), 2, 2);
	//霍夫圓偵測
	HoughCircles(frame_gray, circles, HOUGH_GRADIENT, 1.3, 120, 150, 100, 0, 0);
	return circles;
}

// 畫出人臉的位置
Mat boxFace(Mat frame, vector<Rect> faces) {
	if (faces.size() > 0) {
		for (size_t i = 0; i < faces.size(); i++) {
			int x = faces[i].x;
			int y = faces[i].y;
			int w = faces[i].width;
			int h = faces[i].height;
			// draw_type == 1 畫矩形， draw_type == 2 畫圓形
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

// 畫出圓的位置
Mat drawCircle(Mat frame, vector<Vec3f> circles){
	{ //用for迴圈繪製 circles 的每一個圓形
		for (size_t i = 0; i < circles.size(); i++)
		{
			//可用需要cvRound()四捨五入成整數
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));   // [0] 為 circles 中心的 x 座標， [1]為 y 座標
			int radius = cvRound(circles[i][2]);                            // [2] 為 半徑

			//用circle，紅色，線粗=2
			circle(frame, center, radius, Scalar(0, 0, 255), 2, 8, 0);
		}
	}
	return frame;
}

// 畫出眼睛的位置
Mat drawEye(Mat frame, Rect face, vector<Rect> eyes) {
	{
		//用for迴圈繪製 eyes 的框
		for (size_t i = 0; i < faces.size(); i++) {
			for (size_t j = 0; j < eyes.size(); j++) {
				//需要將face的x,y座標加上eyes的x,y座標
				Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
				int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);

				// draw_type == 1 畫矩形， draw_type == 2 畫圓形
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
	


