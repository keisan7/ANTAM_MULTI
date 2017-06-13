////////////////////////////////////////////////////////////////////////////////////////
/////////////////sys.h���J���ăt�@�C������|�[�g�ԍ��̐ݒ���s���Ă�������//////////////
//�v���O�������J�n������A�X���C�h�o�[��臒l�̐ݒ�Ambed,dynamixel,�J������
//�ڑ�����Ă��邩�m�F���Ă�������
//�v���O�����J�n����TEST���[�h�i�^��͂��Ă��Ȃ��j���
//�^�悵������̍Đ����Ԃ͂��悻�T���ɂP�b�قǑ��߂ɂ���܂�
//�g�p���@
//�w�i�摜���X�V�F�fS�L�[�f�������Ă�������
//�v�����J�n�F�fR�L�[�f�������Ă�������
//TEST���[�h�i�v�����������񒆎~�A�^�C�}�[���Z�b�g�j�F�fT�L�[�f�������ĉ�����
//��L�ȊO�̃L�[�������ƃv���O�������I��
/////////////////////////////////////////////////////////////////////////////////////////

#include "sys.h"
#include "sub_method.h"

int main() {
	set_fname();
	//////�J�����̏���///////
	cv::VideoCapture cap;
	cap.open(CAP_NUM);
	if (!cap.isOpened())
		return(1);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAM_W);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAM_H);
	cap.set(CV_CAP_PROP_FPS, FPS);

	//dynamixel�Ƃ̒ʐM����
	init_Device();

	int key, first = 0;
	int r = 0, g = 255, b = 0;
	bool calib = false;

	//�~�`�؂蔲���摜
	cv::circle(circleFrame, cv::Point(CAM_W / 2, CAM_H / 2), CAM_W / 2 - CAM_W/12, 255, -1);
	cv::bitwise_not(circleFrame, circleFrame);

	cv::VideoWriter writer;
	//cv::VideoWriter writer(VIDEO_NAME, -1, FPS, cv::Size(CAM_W, CAM_H));
	std::ofstream ofs;
	CvFont dfont;
	char message[64] = "";
	//�t�H���g�̏�����
	cvInitFont(&dfont, CV_FONT_HERSHEY_SIMPLEX, 1.0f, 1.0f, 0.0f, 2, CV_AA);
	//�摜�����p�̕ϐ�
	cv::Mat frame,
		img(cv::Size(CAM_W, CAM_H), CV_8UC3),
		back(cv::Size(CAM_W, CAM_H), CV_8UC1),
		dst(cv::Size(CAM_W, CAM_H), CV_8UC1);
	//���Ԍv���ϐ�
	int64 t0, t1, t2, t3, t4, t5;
	double end_t = 0;
	int64 time_log;

	cv::namedWindow(WIN_NAME, CV_WINDOW_AUTOSIZE);
	cv::createTrackbar("thresh", WIN_NAME, &THRE, 255, NULL);
	std::thread mbed(serial_task);//mbed����̒ʐM��t�X���b�h
	
	//�����̊J�n
	cap.read(frame);
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
		//�J��������摜���擾
		if (cap.grab() == false)
			continue;
		if (cap.retrieve(frame, 0) == false)
			continue;
		t1 = cv::getTickCount();
		frame.copyTo(img);
		sprintf(message, "time: %I64d h %I64d m %I64d s   %2.2lf fps", (time_log / 1000) / 3600, ((time_log / 1000) / 60) % 60, (time_log / 1000) % 60, (double)(1000 / end_t));
		putText(frame, message, cv::Point(10, 20), cv::FONT_HERSHEY_PLAIN, 1.0, cvScalar(b, g, r), 2, CV_AA);
		t2 = cv::getTickCount();
		//���݂̃t���[���������o��
		if (check_mode() == RELEASE_MODE)
			writer << frame;
		t3 = cv::getTickCount();
		//��l���摜�̍쐬�Əd�S�̎擾
		img_processing_main(&img, &back, &dst);
		t4 = cv::getTickCount();
		//�摜�������I�������ʃX���b�h�Ń��[�^����J�n
		//calib_m��true�Ȃ烂�[�^�𓮂����Ȃ�
		std::thread m_th(motor_task);
		//��l���摜�̏o��
		cv::imshow(WIN_NAME, dst);
		//calibration
		if (calib) {
			cv::line(frame, cv::Point(0, CAM_H / 2), cv::Point(CAM_W, CAM_H / 2), cv::Scalar(0, 0, 255), 1, CV_AA);
			cv::line(frame, cv::Point(CAM_W / 2, 0), cv::Point(CAM_W / 2, CAM_H), cv::Scalar(0, 0, 255), 1, CV_AA);
			putText(frame, "Calibration Mode", cv::Point(10, 40), cv::FONT_HERSHEY_PLAIN, 1.0, cvScalar(255, 0, 0), 2, CV_AA);
			if(calib_motor(MOTOR_STATE))
				putText(frame, "Motor OFF", cv::Point(10, 60), cv::FONT_HERSHEY_PLAIN, 1.0, cvScalar(255, 0, 0), 2, CV_AA);
			else
				putText(frame, "Motor ON", cv::Point(10, 60), cv::FONT_HERSHEY_PLAIN, 1.0, cvScalar(255, 0, 0), 2, CV_AA);

		}
		//���ʏo��
		cv::imshow("frame", frame);
		key = cv::waitKey(1);
		//s�L�[�������ꂽ��w�i�摜���X�V����
		if (key == 0x072) {
			//r�L�[�������ꂽ��^�惂�[�h
			writer = cv::VideoWriter(video_filename, CV_FOURCC('X', 'V', 'I', 'D'), FPS, cv::Size(CAM_W, CAM_H));
			//cv::VideoWriter writer(VIDEO_NAME, -1, FPS, cv::Size(CAM_W, CAM_H));
			ofs = std::ofstream(main_filename);
			ofs << "time_log" << "," << "�L���v�`������" << "," << "�����o����������" << "," << "�摜��������" << ","
			<< "���[�^����J�n����I��" << "," << "1���[�v" << std::endl;
			start_time = timeGetTime();
			mode_releace();
		}
		else if (key == 0x63){
			//c�L�[�������ꂽ��L�����u���[�V�������[�h�̐؂�ւ�
				if(calib)
					//�L�����u���[�V�������[�h�I�����Ƀ��[�^�̓��삪OFF�Ȃ�ON�ɐ؂�ւ�
					if (calib_motor(MOTOR_STATE)) {
						calib_motor(MOTOR_SWICH);
					}
				calib = !calib;
		}
		else if (key == 0x6d) {
			//�L�����u���[�V�������[�h����m�L�[�������ꂽ�烂�[�^�̓���ON�EOFF�؂�ւ�
			if (calib)
				calib_motor(MOTOR_SWICH);
		}
		else if (key == 0x1b) {
			//esc�L�[�������ꂽ�烋�[�v�𔲂���
			m_th.join();
			break;
		}

		//���[�^����X���b�h�̏I����҂�
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
		//debug
		if (timer_flag) {
			//���Ԃ��z������I��
			if (time_log > (REC_HOUR * 3600 + REC_MINUTE * 60) * 1000)
				break;
		}
		end_t = (double)((cv::getTickCount() - t0) * 1000 / cv::getTickFrequency());
	}
	end_device_rx28();
	change_flag();
	mbed.join();
	//�I������
	ofs.close();
	writer.release();
	dst.release();
	back.release();
	frame.release();
	cap.release();
	img.release();
	cv::destroyAllWindows();
	std::cout << "���ׂ̂ăv���Z�X�̏I��" << std::endl;
	return 0;
}