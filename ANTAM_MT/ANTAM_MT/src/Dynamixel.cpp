#include <dynamixel.h>
#include "sys.h"
#include "sub_method.h"
#pragma comment(lib,"dynamixel.lib")

bool calib_m = false;

void init_Device() {
	if (dxl_initialize(PORT_NUM, BORD_NUM) == 0) {
		//ポートが見つからなかった場合の処理
		std::cout << "Not Conection Dynamixel" << std::endl;
	}
	else {
		//ポートが見つかったらプログラムを実行する
		std::cout << "Dynamixel Conection OK" << std::endl;
	}
}
void motor_task() {
	int x, y, st;
	double t;
	int result = dxl_get_result();
	if (result != COMM_RXSUCCESS)
	{
		std::cout << "Dynamixel 再接続" << std::endl;
		init_Device();
	}
	if (!calib_motor(1)) {
		calc_speed(&x, &y, &st);
		move_rx28(x, y, st);
	}
	else {
		//printf("calib////////////////////////////////////////////////////////////////////////");
		move_rx28(0, 0, 0);
	}
	t = cv::getTickCount();
}
//モータへ指令値を送る関数
void move_rx28(int speed_x, int speed_y, int st) {
	//重心の位置によってモータの動作を決定
	switch (st)
	{
	case 1:
		dxl_write_word(0, 32, speed_y);
		dxl_write_word(1, 32, speed_y + 1024);
		dxl_write_word(2, 32, speed_x + 1024);
		dxl_write_word(3, 32, speed_x);
		break;
	case 2:
		dxl_write_word(0, 32, speed_y);
		dxl_write_word(1, 32, speed_y + 1024);
		dxl_write_word(2, 32, speed_x);
		dxl_write_word(3, 32, speed_x + 1024);
		break;
	case 3:
		dxl_write_word(0, 32, speed_y + 1024);
		dxl_write_word(1, 32, speed_y);
		dxl_write_word(2, 32, speed_x);
		dxl_write_word(3, 32, speed_x + 1024);
		break;
	case 4:
		dxl_write_word(0, 32, speed_y + 1024);
		dxl_write_word(1, 32, speed_y);
		dxl_write_word(2, 32, speed_x + 1024);
		dxl_write_word(3, 32, speed_x);

		break;
	default:
		for (int i = 0; i<4; i++)
			dxl_write_word(i, 32, 0);
		break;
	}
	//std::cout << "st:" << st << " x:" << speed_y << " y:" << speed_x << std::endl;
}
//速度を計算する関数
void calc_speed(int *x, int *y, int *st) {
	int abs_x, abs_y;
	int error_x, error_y;
	//画面中心からのずれを求める
	error_x = pos_x - CAM_W / 2;
	error_y = pos_y - CAM_H / 2;
	//std::cout << "st:" << check_st() << "error_x:" << error_x << "error_y" << error_y << std::endl;
	abs_x = abs(error_x);
	abs_y = abs(error_y);

	//ずれにKP_GAINを掛けて速度を決定
	*x = KP_GAIN * abs_x;
	*y = KP_GAIN * abs_y;
	//モータの速度の限界以上なら限界速度に設定
	if (*x > 1023)
		*x = 1023;
	if (*y > 1023)
		*y = 1023;
	if (pos_x >= CAM_W / 2 && pos_y >= CAM_H / 2) {
		*st = 1;
	}
	else if (pos_x < CAM_W / 2 && pos_y > CAM_H / 2) {
		*st = 2;
	}
	else if (pos_x <= CAM_W / 2 && pos_y <= CAM_H / 2) {
		*st = 3;
	}
	else if (pos_x > CAM_W / 2 && pos_y < CAM_H / 2) {
		*st = 4;
	}
	else {
		*st = 0;
	}
}
void end_device_rx28() {
	for (int i = 0; i < 4; i++) {
		dxl_write_word(i, 32, 0);
	}
	dxl_terminate();
}