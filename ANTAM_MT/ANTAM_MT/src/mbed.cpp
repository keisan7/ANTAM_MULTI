#include "sys.h"
#include "sub_method.h"

HANDLE mbed;
char *buf;//受信した文字を文字列として受信する変数
int hip = 0;//bufに書き込んだ文字数
std::ofstream mouse("test");//マウス移動量をファイルに書き出す変数
DWORD start_time = 0;//データ取得開始時刻（スレッド開始時刻）

					 //シリアルポートの準備関数
void serial_setup() {
	bool check = FALSE;
	//シリアルポートを接続
	mbed = CreateFile(_T(COM_NUM),
		GENERIC_READ | GENERIC_WRITE,
		0, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (mbed == INVALID_HANDLE_VALUE) {
		std::cout << "mbed PORT COULD NOT OPEN" << std::endl;
	}
	else {
		std::cout << "mbed PORT OPEN" << std::endl;
	}
	//ブッファーの準備
	check = SetupComm(mbed, 1024, 1024);
	if (!check) {
		std::cout << "mbed COULD NOT SET UP BUFFER" << std::endl;
		CloseHandle(mbed);
	}
	else {
		std::cout << "mbed SET UP OK" << std::endl;
	}
	//ブッファの初期化
	check = PurgeComm(mbed, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!check) {
		std::cout << "mbed COULD NOT BUFFER CLER" << std::endl;
		CloseHandle(mbed);
	}
	else {
		std::cout << "mbed BUFFER OK" << std::endl;
	}
	//シリアル通信のステータスを設定
	DCB dcb;
	GetCommState(mbed, &dcb);
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = 57600;
	dcb.fBinary = TRUE;
	dcb.ByteSize = 8;
	dcb.fParity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;


	//設定の適用
	check = SetCommState(mbed, &dcb);
	if (!check) {
		std::cout << "mbe SetCommState FAILED" << std::endl;
		CloseHandle(mbed);
	}
	else {
		std::cout << "mbed SetCommOK" << std::endl;
	}
}
//移動量の取得
void recive_value(int *x, int *y, int *move_x, int *move_y) {
	//データの受信
	int length = 0;
	char c;
	DWORD t;
	DWORD errors;//エラー情報を格納する変数
	COMSTAT comStat;//受信バッファのバイト数を格納する変数
	ClearCommError(mbed, &errors, &comStat);//errosにエラー情報、comStatに受信バッファ情報を格納
	length = comStat.cbInQue; // 受信したメッセージ長を取得する
	if (length > 0) {
		for (int i = 0; i < length; i++) {
			DWORD numberOfPut;//受信したメッセージ長
			ReadFile(mbed, &c, 1, &numberOfPut, NULL);
			if (c == ',') {
				//x軸方向への移動量をintへ変換
				*x = atoi(buf);
				memset(buf, 0, sizeof(buf));
				hip = 0;
			}//改行コードならファイルへ値の書き出し
			else if (c == '\n') {
				//ｙ軸方向への移動量をintへ変換
				*y = atoi(buf);
				//ファイルの書き出し時刻
				t = timeGetTime() - start_time;
				if (check_mode() == RELEASE_MODE) {
					//csvファイルへの書き出し
					*move_x += *x;
					*move_y += *y;
					std::cout << "REC:" << (double)t / 1000 << ":" << *move_x << "," << *move_y << std::endl;
					mouse << (double)t / 1000 << "," << *move_x << "," << *move_y << std::endl;
				}
				else {
					//debug
					std::cout << (double)t / 1000 << ":" << *x << "," << *y << std::endl;
				}
				memset(buf, 0, sizeof(buf));
				hip = 0;
			}//デフォルトではbufに文字を格納
			else {
				buf[hip] = c;
				hip++;
			}
		}
	}

}
//mbedからの通信を受け取る関数
void serial_task() {
	buf = new char[255];
	int x, y;
	int move_x = 0, move_y = 0;
	mouse << "time" << "," << "x" << "," << "y" << std::endl;
	serial_setup();
	while (mbed != INVALID_HANDLE_VALUE) {
		recive_value(&x, &y, &move_x, &move_y);
		if (check_flag())
			break;
		Sleep(5);
	}
	serial_exit();
}
void serial_exit() {
	bool check = FALSE;
	check = PurgeComm(mbed, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!check) {
		std::cout << "COULD NOT CLER" << std::endl;
	}
	CloseHandle(mbed);
	std::cout << "close serial port" << std::endl;
}