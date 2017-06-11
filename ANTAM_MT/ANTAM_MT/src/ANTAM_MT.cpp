////////////////////////////////////////////////////////////////////////////////////////
/////////////////sys.hを開いてファイル名やポート番号の設定を行ってください//////////////
//プログラムを開始したら、スライドバーで閾値の設定、mbed,dynamixel,カメラが
//接続されているか確認してください
//プログラム開始時はTESTモード（録画はしていない）状態
//録画した動画の再生時間はおよそ５分に１秒ほど早めにずれます
//使用方法
//背景画像を更新：’Sキー’を押してください
//計測を開始：’Rキー’を押してください
//TESTモード（計測をいったん中止、タイマーリセット）：’Tキー’を押して下さい
//上記以外のキーを押すとプログラムが終了
/////////////////////////////////////////////////////////////////////////////////////////

#include "sys.h"
#include "sub_method.h"

int main() {
	set_fname();
	int key, first = 0;
	int r = 0, g = 255, b = 0;
	cv::VideoWriter writer;
	//cv::VideoWriter writer(VIDEO_NAME, -1, FPS, cv::Size(CAM_W, CAM_H));
	std::ofstream ofs;
	cv::VideoCapture cap;
	//////カメラの準備///////
	cap.open(CAP_NUM);
	if (!cap.isOpened())
		return(1);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAM_W);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAM_H);
	cap.set(CV_CAP_PROP_FPS, FPS);
	CvFont dfont;
	char message[64] = "";
	//フォントの初期化
	cvInitFont(&dfont, CV_FONT_HERSHEY_SIMPLEX, 1.0f, 1.0f, 0.0f, 2, CV_AA);
	cv::Mat frame,
		img(cv::Size(CAM_W, CAM_H), CV_8UC3),
		back(cv::Size(CAM_W, CAM_H), CV_8UC1),
		dst(cv::Size(CAM_W, CAM_H), CV_8UC1);
	int64 t0, t1, t2, t3, t4, t5;
	double end_t = 0;
	int64 time_log;
	//dynamixelとの通信準備
	init_Device();
	cap.read(frame);
	/////ここまでカメラに関する宣言////
	ofs << "time_log" << "," << "キャプチャ時間" << "," << "書き出し処理時間" << "," << "画像処理時間" << ","
		<< "モータ制御開始から終了" << "," << "1ループ" << std::endl;
	//背景画像の準備
	//frame.copyTo(img);
	//cv::cvtColor(img, back, cv::COLOR_RGB2GRAY);
	//imshow("back", back);
	cv::namedWindow(WIN_NAME, CV_WINDOW_AUTOSIZE);
	cv::createTrackbar("thresh", WIN_NAME, &THRE, 255, NULL);
	std::thread mbed(serial_task);//mbedからの通信受付スレッド
	int64 start_t = cv::getTickCount();
	while (1) {
		if (check_mode() == RELEASE_MODE && first == 0) {
			std::cout << "REC STRT" << std::endl;
			b = 0;
			g = 0;
			r = 255;
			start_t = cv::getTickCount();
			first = 1;
		}
		t0 = cv::getTickCount();
		time_log = (int64)((t0 - start_t) * 1000 / cv::getTickFrequency());
		//カメラから画像を取得
		if (cap.grab() == false)
			continue;
		if (cap.retrieve(frame, 0) == false)
			continue;
		t1 = cv::getTickCount();
		frame.copyTo(img);
		sprintf(message, "time: %I64d h %I64d m %I64d s   %2.2lf fps", (time_log / 1000) / 3600, ((time_log / 1000) / 60) % 60, (time_log / 1000) % 60, (double)(1000 / end_t));
		putText(frame, message, cv::Point(10, 20), cv::FONT_HERSHEY_PLAIN, 1.0, cvScalar(b, g, r), 2, CV_AA);
		t2 = cv::getTickCount();
		//現在のフレームを書き出す
		if (check_mode() == RELEASE_MODE)
			writer << frame;
		t3 = cv::getTickCount();
		//二値化画像の作成と重心の取得
		img_processing_main(&img, &back, &dst);
		t4 = cv::getTickCount();
		//画像処理が終わったら別スレッドでモータ制御開始
		std::thread m_th(motor_task);
		//二値化画像の出力
		cv::imshow(WIN_NAME, dst);
		cv::imshow("frame", frame);
		key = cv::waitKey(1);
		//sキーを押されたら背景画像を更新する
	if (key == 0x072) {
			//rキーが押されたら録画モード
		writer = cv::VideoWriter(video_filename, CV_FOURCC('X', 'V', 'I', 'D'), FPS, cv::Size(CAM_W, CAM_H));
		//cv::VideoWriter writer(VIDEO_NAME, -1, FPS, cv::Size(CAM_W, CAM_H));
		ofs = std::ofstream(main_filename);
			start_time = timeGetTime();
			mode_releace();
		}
		else if (key == 0x074) {
			//tキーが押されたらテストモード
			mode_test();
			first = 0;
		}
		else if (key == 0x1b) {
			//それ以外のキーを押されたらループを抜ける
			m_th.join();
			break;
		}
		//モータ制御スレッドの終了を待つ
		m_th.join();
		t5 = cv::getTickCount();
		if (check_mode() == RELEASE_MODE && first != 0) {
			ofs << time_log << ","
				<< (double)((t1 - t0) * 1000 / cv::getTickFrequency()) << ","
				<< (double)((t3 - t2) * 1000 / cv::getTickFrequency()) << ","
				<< (double)((t4 - t3) * 1000 / cv::getTickFrequency()) << ","
				<< (double)((t5 - t4) * 1000 / cv::getTickFrequency()) << ","
				<< end_t << std::endl;
		}
		if (timer_flag) {
			//時間を越えたら終了
			if (time_log > (REC_HOUR * 3600 + REC_MINUTE * 60) * 1000)
				break;
		}
		end_t = (double)((cv::getTickCount() - t0) * 1000 / cv::getTickFrequency());
	}
	end_device_rx28();
	change_flag();
	mbed.join();
	//終了処理
	ofs.close();
	writer.release();
	dst.release();
	back.release();
	frame.release();
	cap.release();
	img.release();
	cv::destroyAllWindows();
	std::cout << "すべのてプロセスの終了" << std::endl;
	return 0;
}