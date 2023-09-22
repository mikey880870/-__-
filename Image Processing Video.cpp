/*HW4_Level1: 銅板隨機掉落*/
//#include "stdafx.h" //如果專案標頭檔裡裡有stdafx.h，原始程式檔有stdafx.cpp，需加入這一行。如果是空專案，則不用加這一行。
#include "opencv2/opencv.hpp" //載入opencv函式庫
#include <iostream> //載入c++函式庫

using namespace cv; //宣告 opencv 函式庫的命名空間
using namespace std; //宣告 C++函式庫的命名空間

#define COINS 20 //銅板隨機掉落的總量，必須是整數，數值越低越稀疏
//RNG rng(1234); //根據1234種子值，產生特定順序之隨機數
RNG rng(clock()); //根據clock()產生隨機數，每次執行都不同

/*主程式*/
int main(void)
{
	/*變數(影像容器)宣告*/
	Mat im1, im2, im3, im4;
	Mat im1_gray, im2_gray, im3_gray, im4_gray;

	im1 = imread("data/c01.png", 1); //載入1元銅板影像
	im2 = imread("data/c05.png", 1); //載入5元銅板影像
	im3 = imread("data/c10.png", 1); //載入10元銅板影像
	im4 = imread("data/c50.png", 1); //載入50元銅板影像
	cvtColor(im1, im1_gray, COLOR_BGR2GRAY); //1元銅板轉換至灰階格式
	cvtColor(im2, im2_gray, COLOR_BGR2GRAY); //5元銅板轉換至灰階格式
	cvtColor(im3, im3_gray, COLOR_BGR2GRAY); //10元銅板轉換至灰階格式
	cvtColor(im4, im4_gray, COLOR_BGR2GRAY); //50元銅板轉換至灰階格式
	threshold(im1_gray, im1_gray, 10, 255, THRESH_BINARY);  //1元銅板二值化
	threshold(im2_gray, im2_gray, 10, 255, THRESH_BINARY);  //5元銅板二值化
	threshold(im3_gray, im3_gray, 10, 255, THRESH_BINARY);  //10元銅板二值化
	threshold(im4_gray, im4_gray, 10, 255, THRESH_BINARY);  //50元銅板二值化

	VideoCapture cap("data/moving.mp4"); //讀取背景影片
	if (!cap.isOpened()) return 0;

	Size S(int(cap.get(3)), int(cap.get(4)));		//讀取視訊寬/高 cap.get:3(偵的寬度) cap.get:4(偵的高度) 
	int frame_count = cap.get(CAP_PROP_FRAME_COUNT); //讀取視訊畫面總數

	VideoWriter put("data/drop.mp4", VideoWriter::fourcc('X', '2', '6', '4'), 60, S); //建立銅板隨機掉落的視訊物件

	if (!put.isOpened())
	{
		cout << "File could not be created for writing." << endl;
		return -1;
	}
	namedWindow("Video"); //建立視訊視窗

	int start[COINS][3]; //每個銅板的掉落起始畫面編號、種類、水平位置

	for (int i = 0; i < COINS; i++)
	{
		start[i][0] = rng.uniform(0, frame_count - 100);	//銅板i的起始畫面編號(隨機)
		start[i][1] = rng.uniform(1, 5);					//銅板i的種類(隨機)
		start[i][2] = rng.uniform(0, S.width - 150);		//銅板i的水平方向位置(隨機)
	}

	//用for迴圈讀取每一畫格
	for (int n = 0; n < frame_count; n++)
	{
		Mat im_bg;
		cap >> im_bg; //讀取背景影像
		if (im_bg.empty()) //如果影像不存在
		{
			printf(" --(!) No captured im -- Break!");  //顯示錯誤訊息
			break;
		}

		//將第i 個銅板掉落影像加入背景影像的迴圈
		for (int i = 0; i < COINS; i++)
		{
			int y = 5 * (n - start[i][0]); //銅板i矩形框的y座標(矩形框上緣的垂直方向座標)
			if (y > 0 && y < S.height - 150) //如果銅板i矩形框(垂直方向)在畫面範圍內，執行下列動作
			{
				switch (start[i][1]) //根據銅板i的種類，選擇銅板影像
				{
				case 1: //1元硬幣
				{
					//根據start[i][2]與y的位置，建立 roi1 矩形框 
					Rect roi1(start[i][2], y, im1.size().width, im1.size().height);
					//將 im1 複製到 im_bg 的矩形框，以 im1_gray 為遮罩
					im1.copyTo(im_bg(roi1),im1_gray);
					break; //離開 switch
				}
				case 2: //5元硬幣
				{
					//根據start[i][2]與y的位置，建立 roi2 矩形框(Rect type)
					Rect roi2(start[i][2], y, im2.size().width, im2.size().height);
					//將 im2 複製到 im_bg 的矩形框，以 im2_gray 為遮罩
					im2.copyTo(im_bg(roi2), im2_gray);
					break; //離開 switch
				}
				case 3: //10元硬幣
				{
					//根據start[i][2]與y的位置，建立 roi3 矩形框(Rect type)
					Rect roi3(start[i][2], y, im3.size().width, im3.size().height);
					//將 im3 複製到 im_bg 的矩形框，以 im3_gray 為遮罩
					im3.copyTo(im_bg(roi3), im3_gray);
					break; //離開 switch
				}
				case 4: //50元硬幣
				{
					//根據start[i][2]與y的位置，建立 roi4 矩形框(Rect type)
					Rect roi4(start[i][2], y, im4.size().width, im4.size().height);
					//將 im4 複製到 im_bg 的矩形框，以 im4_gray 為遮罩
					im4.copyTo(im_bg(roi4), im4_gray);
					break; //離開 switch 
				}
				}
			}
		}

		imshow("Video", im_bg); //顯示疊上銅板後的背景影像
		waitKey(10); //等待10毫秒
		put << im_bg; //匯出影像到要輸出的視訊檔
	}
	cout << "please check: data/drop.mp4" << endl; //顯示輸出視訊的路徑/檔名

	return 0;
}