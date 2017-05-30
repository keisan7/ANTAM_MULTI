#include <dynamixel.h>
#include "sys.h"
#include "sub_method.h"
#pragma comment(lib,"dynamixel.lib")
void init_Device() {
	if (dxl_initialize(PORT_NUM, BORD_NUM) == 0) {
		//�|�[�g��������Ȃ������ꍇ�̏���
		std::cout << "Not Conection Dynamixel" << std::endl;
	}
	else {
		//�|�[�g������������v���O���������s����
		std::cout << "Dynamixel Conection OK" << std::endl;
	}
}
void motor_task() {
	int x, y, st;
	double t;
	int result = dxl_get_result();
	if (result != COMM_RXSUCCESS)
	{
		std::cout << "Dynamixel �Đڑ�" << std::endl;
		init_Device();
	}
	calc_speed(&x, &y, &st);
	t = cv::getTickCount();
	move_rx28(x, y, st);
}
//���[�^�֎w�ߒl�𑗂�֐�
void move_rx28(int speed_x, int speed_y, int st) {
	//�d�S�̈ʒu�ɂ���ă��[�^�̓��������
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
//���x���v�Z����֐�
void calc_speed(int *x, int *y, int *st) {
	int abs_x, abs_y;
	int error_x, error_y;
	//��ʒ��S����̂�������߂�
	error_x = pos_x - CAM_W / 2;
	error_y = pos_y - CAM_H / 2;
	//std::cout << "st:" << check_st() << "error_x:" << error_x << "error_y" << error_y << std::endl;
	abs_x = abs(error_x);
	abs_y = abs(error_y);

	//�����KP_GAIN���|���đ��x������
	*x = KP_GAIN * abs_x;
	*y = KP_GAIN * abs_y;
	//���[�^�̑��x�̌��E�ȏ�Ȃ���E���x�ɐݒ�
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