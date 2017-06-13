#include "sys.h"
#include "sub_method.h"

//�r�����䂪�K�v�ȕϐ��������֐�

//mbed�Ƃ̒ʐM��ؒf����t���O
//���C���X���b�h�ł��������ߔr��������s��
bool mbed_end_flag = false;
int mode = TEST_MODE;
std::mutex mbed_mtx;
std::mutex mode_mtx;
std::mutex calib_mtx;

//�f�[�^����������̂�h�����ߔr������Ńt���O����舵��
//�I���t���O�𗧂��グ��ϐ�
void change_flag() {
	mbed_mtx.lock();
	mbed_end_flag = TRUE;
	mbed_mtx.unlock();
}
//�I���t���O�̒l��Ԃ��֐�
bool check_flag() {
	bool check;
	mbed_mtx.lock();
	check = mbed_end_flag;
	mbed_mtx.unlock();
	return check;
}
void mode_releace() {
	mode_mtx.lock();
	mode = RELEASE_MODE;
	mode_mtx.unlock();
}
int check_mode() {
	int n;
	mode_mtx.lock();
	n = mode;
	mode_mtx.unlock();
	return n;
}

void change_calib() {

}

bool calib_motor(int ch) {
	bool m;
	if (ch == 0) {
		calib_mtx.lock();
		calib_m = !calib_m;
		calib_mtx.unlock();
	}
	else {
		calib_mtx.lock();
		m = calib_m;
		calib_mtx.unlock();
	}
	return m;
}