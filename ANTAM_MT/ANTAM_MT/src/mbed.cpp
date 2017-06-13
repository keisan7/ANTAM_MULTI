#include "sys.h"
#include "sub_method.h"

HANDLE mbed;
char *buf;//��M���������𕶎���Ƃ��Ď�M����ϐ�
int hip = 0;//buf�ɏ������񂾕�����
bool rec = false;
std::ofstream mouse;//�}�E�X�ړ��ʂ��t�@�C���ɏ����o���ϐ�
DWORD start_time = 0;//�f�[�^�擾�J�n�����i�X���b�h�J�n�����j
					 //DWORD t1;
DWORD t1, t2;

//�V���A���|�[�g�̏����֐�
void serial_setup() {
	bool check = false;
	//�V���A���|�[�g��ڑ�
	mbed = CreateFile(_T(COM_NUM), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (mbed == INVALID_HANDLE_VALUE) {
		std::cout << "mbed PORT COULD NOT OPEN" << std::endl;
	}
	else {
		std::cout << "mbed PORT OPEN" << std::endl;
	}
	//�u�b�t�@�[�̏���
	check = SetupComm(mbed, 1024, 1024);
	if (!check) {
		std::cout << "mbed COULD NOT SET UP BUFFER" << std::endl;
		CloseHandle(mbed);
	}
	else {
		std::cout << "mbed SET UP OK" << std::endl;
	}
	//�u�b�t�@�̏�����
	check = PurgeComm(mbed, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!check) {
		std::cout << "mbed COULD NOT BUFFER CLER" << std::endl;
		CloseHandle(mbed);
	}
	else {
		std::cout << "mbed BUFFER OK" << std::endl;
	}
	//�V���A���ʐM�̃X�e�[�^�X��ݒ�
	DCB dcb;
	GetCommState(mbed, &dcb);
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = 57600;
	dcb.fBinary = TRUE;
	dcb.ByteSize = 8;
	dcb.fParity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	//�ݒ�̓K�p
	check = SetCommState(mbed, &dcb);
	if (!check) {
		std::cout << "mbe SetCommState FAILED" << std::endl;
		CloseHandle(mbed);
	}
	else {
		std::cout << "mbed SetCommOK" << std::endl;
	}
}
//�ړ��ʂ̎擾
/*
void recive_value(int *x, int *y, int *move_x, int *move_y) {
//�f�[�^�̎�M
int length = 0;
char c;
DWORD errors;//�G���[�����i�[����ϐ�
COMSTAT comStat;//��M�o�b�t�@�̃o�C�g�����i�[����ϐ�
ClearCommError(mbed, &errors, &comStat);//erros�ɃG���[���AcomStat�Ɏ�M�o�b�t�@�����i�[
length = comStat.cbInQue; // ��M�������b�Z�[�W�����擾����
//std::cout << length << std::endl;
if (length > 0) {
for (int i = 0; i < length; i++) {
DWORD numberOfPut;//��M�������b�Z�[�W��
ReadFile(mbed, &c, 1, &numberOfPut, NULL);
if (c == ',') {
//x�������ւ̈ړ��ʂ�int�֕ϊ�
*x = atoi(buf);
memset(buf, 0, sizeof(buf));
hip = 0;
}//���s�R�[�h�Ȃ�t�@�C���֒l�̏����o��
else if (c == '\n') {
//���������ւ̈ړ��ʂ�int�֕ϊ�
*y = atoi(buf);
//�t�@�C���̏����o������
t1 = timeGetTime() - start_time;
if (check_mode() == RELEASE_MODE) {
//csv�t�@�C���ւ̏����o��
*move_x += *x;
*move_y += *y;
std::cout << "REC:" << (int64)t1  << ":" << *move_x << "," << *move_y << std::endl;
mouse << (int64)t1  << "," << *move_x << "," << *move_y << std::endl;
}
else {
//debug
std::cout << (int64)t1 << ":" << *x << "," << *y << std::endl;
}
memset(buf, 0, sizeof(buf));
hip = 0;
}//�f�t�H���g�ł�buf�ɕ������i�[
else {
buf[hip] = c;
hip++;
}
}
}
}
*/
void recive_value(int *x, int *y, int *move_x, int *move_y) {
	//�f�[�^�̎�M
	int length = 0;
	char c;
	DWORD errors;//�G���[�����i�[����ϐ�
	COMSTAT comStat;//��M�o�b�t�@�̃o�C�g�����i�[����ϐ�
	ClearCommError(mbed, &errors, &comStat);//erros�ɃG���[���AcomStat�Ɏ�M�o�b�t�@�����i�[
	length = comStat.cbInQue; // ��M�������b�Z�[�W�����擾����
	if (length > 0) {
		for (int i = 0; i < length; i++) {
			DWORD numberOfPut;//��M�������b�Z�[�W��
			ReadFile(mbed, &c, 1, &numberOfPut, NULL);
			if (c == ',') {
				//x�������ւ̈ړ��ʂ�int�֕ϊ�
				*x = atoi(buf);
				memset(buf, 0, sizeof(buf));
				hip = 0;
			}//���s�R�[�h�Ȃ�t�@�C���֒l�̏����o��
			else if (c == '\n') {
				//���������ւ̈ړ��ʂ�int�֕ϊ�
				*y = atoi(buf);
				//�t�@�C���̏����o������
				t1 = timeGetTime() - start_time;
				if (check_mode() == RELEASE_MODE) {
					//csv�t�@�C���ւ̏����o��
					*move_x += *x;
					*move_y += *y;
					std::cout << "REC:" << (int64)t1 << ":" << *move_x << "," << *move_y << std::endl;
					mouse << (int64)t1 << "," << *move_x << "," << *move_y << std::endl;
				}
				else {
					//debug
					std::cout << (int64)t1 << ":" << *x << "," << *y << std::endl;
				}
				memset(buf, 0, sizeof(buf));
				hip = 0;
			}//�f�t�H���g�ł�buf�ɕ������i�[
			else {
				buf[hip] = c;
				hip++;
			}
		}
	}
	else {
		t2 = timeGetTime() - start_time;
		if ((t2 - t1) >= 8) {
			if (check_mode() == RELEASE_MODE) {

				//csv�t�@�C���ւ̏����o��
				std::cout << "REC:" << (int64)t2 << ":" << *move_x << "," << *move_y << std::endl;
				mouse << (int64)t2 << "," << *move_x << "," << *move_y << std::endl;
			}
			else {
				std::cout << (int64)t2 << ":" << "0" << "," << "0" << std::endl;
			}
			t1 = timeGetTime() - start_time;
		}
		else {
			Sleep(1);
		}
	}
}

//mbed����̒ʐM���󂯎��֐�
void serial_task() {
	buf = new char[255];
	int x, y;
	int move_x = 0, move_y = 0;
	serial_setup();
	while (1) {
		if (check_mode() == RELEASE_MODE && !rec) {
			rec = true;
			mouse = std::ofstream(mouse_filename);
			mouse << "time[ms]" << "," << "x" << "," << "y" << std::endl;
		}
		recive_value(&x, &y, &move_x, &move_y);
		if (check_flag())
			break;
	}
	serial_exit();
}
void serial_exit() {
	bool check = false;
	check = PurgeComm(mbed, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!check) {
		std::cout << "COULD NOT CLER" << std::endl;
	}
	CloseHandle(mbed);
	std::cout << "close serial port" << std::endl;
}