#include "sys.h"
#include "sub_method.h"
int pos_x = CAM_W / 2, pos_y = CAM_H / 2;
int THRE = 60;
int length = 0, radian = 0;
//�d�S�����߂�֐�
void moment_task(cv::Mat *dst, cv::Moments mom) {
	double dotnum_dbl, x_cm_dbl, y_cm_dbl;
	int dotnum;
	dotnum_dbl = mom.m00;
	dotnum = (int)dotnum_dbl;
	//�d�S�����݂���Ȃ�
	if (dotnum != 0) {
		x_cm_dbl = mom.m10 / dotnum_dbl;//�d�S��x���W
		y_cm_dbl = mom.m01 / dotnum_dbl;//�d�S�̂����W
	}
	else {
		x_cm_dbl = CAM_W / 2;
		y_cm_dbl = CAM_H / 2;
	}
	length = sqrt((CAM_W / 2 - x_cm_dbl)*(CAM_W / 2 - x_cm_dbl) + (CAM_H / 2 - y_cm_dbl)*(CAM_H / 2 - y_cm_dbl));
	radian = fabs(atan((x_cm_dbl - CAM_H / 2) / (y_cm_dbl - CAM_H / 2)));
	pos_x = x_cm_dbl;
	pos_y = y_cm_dbl;
	//cv::circle(*dst, cv::Point(pos_x, pos_y), 5, cv::Scalar(100, 150, 100), 1, 5);
	//std::cout <<"state"<<state<< "length" << target_length << "radian" << target_radian << std::endl;
}
//���C�������֐�
void img_processing_main(cv::Mat *src, cv::Mat *back, cv::Mat *dst) {
	cv::Mat gray(cv::Size(CAM_W, CAM_H), CV_8UC1), diff(cv::Size(CAM_W, CAM_H), CV_8UC1);
	//�w�i�����@�œ�l���摜�̍쐬
	cv::cvtColor(*src, gray, cv::COLOR_RGB2GRAY);
	//cv::absdiff(gray, *back, diff);
	//cv::threshold(diff, *dst, THRE, 255, cv::THRESH_BINARY);
	cv::threshold(gray, *dst, THRE, 255, cv::THRESH_BINARY_INV);
	//cv::blur(*dst, *dst, cv::Size(5, 5));
	//��l���摜�̏d�S�����߂�
	cv::Moments mom = cv::moments(*dst);
	moment_task(dst, mom);
	gray.release();
}
