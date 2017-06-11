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
	int key, first = 0;
	int r = 0, g = 255, b = 0;
	cv::VideoWriter writer;
	//cv::VideoWriter writer(VIDEO_NAME, -1, FPS, cv::Size(CAM_W, CAM_H));
	std::ofstream ofs;
	cv::VideoCapture cap;
	//////�J�����̏���///////
	cap.open(CAP_NUM);
	if (!cap.isOpened())
		return(1);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAM_W);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAM_H);
	cap.set(CV_CAP_PROP_FPS, FPS);
	CvFont dfont;
	char message[64] = "";
	//�t�H���g�̏�����
	cvInitFont(&dfont, CV_FONT_HERSHEY_SIMPLEX, 1.0f, 1.0f, 0.0f, 2, CV_AA);
	cv::Mat frame,
		img(cv::Size(CAM_W, CAM_H), CV_8UC3),
		back(cv::Size(CAM_W, CAM_H), CV_8UC1),
		dst(cv::Size(CAM_W, CAM_H), CV_8UC1);
	int64 t0, t1, t2, t3, t4, t5;
	double end_t = 0;
	int64 time_log;
	//dynamixel�Ƃ̒ʐM����
	init_Device();
	cap.read(frame);
	/////�����܂ŃJ�����Ɋւ���錾////
	ofs << "time_log" << "," << "�L���v�`������" << "," << "�����o����������" << "," << "�摜��������" << ","
		<< "���[�^����J�n����I��" << "," << "1���[�v" << std::endl;
	//�w�i�摜�̏���
	//frame.copyTo(img);
	//cv::cvtColor(img, back, cv::COLOR_RGB2GRAY);
	//imshow("back", back);
	cv::namedWindow(WIN_NAME, CV_WINDOW_AUTOSIZE);
	cv::createTrackbar("thresh", WIN_NAME, &THRE, 255, NULL);
	std::thread mbed(serial_task);//mbed����̒ʐM��t�X���b�h
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
		std::thread m_th(motor_task);
		//��l���摜�̏o��
		cv::imshow(WIN_NAME, dst);
		cv::imshow("frame", frame);
		key = cv::waitKey(1);
		//s�L�[�������ꂽ��w�i�摜���X�V����
	if (key == 0x072) {
			//r�L�[�������ꂽ��^�惂�[�h
		writer = cv::VideoWriter(video_filename, CV_FOURCC('X', 'V', 'I', 'D'), FPS, cv::Size(CAM_W, CAM_H));
		//cv::VideoWriter writer(VIDEO_NAME, -1, FPS, cv::Size(CAM_W, CAM_H));
		ofs = std::ofstream(main_filename);
			start_time = timeGetTime();
			mode_releace();
		}
		else if (key == 0x074) {
			//t�L�[�������ꂽ��e�X�g���[�h
			mode_test();
			first = 0;
		}
		else if (key == 0x1b) {
			//����ȊO�̃L�[�������ꂽ�烋�[�v�𔲂���
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