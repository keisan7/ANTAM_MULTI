extern int pos_x, pos_y;
extern int mode;
extern int THRE;
extern cv::Mat circleFrame;
extern bool mbed_flag;
extern bool calib_m;
extern int start_t;
extern DWORD start_time;
extern char mouse_filename[];
extern char main_filename[];
extern char video_filename[];

//mbed�̃V���A���|�[�Ə��
extern HANDLE mbed;
//���[�^�֐�
void init_Device();
void move_rx28(int speed_x, int speed_y, int st);
void calc_speed(int *x, int *y, int *st);
void motor_task();
void end_device_rx28();

//�摜�����֐�
void img_processing_main(cv::Mat *src, cv::Mat *back, cv::Mat *dst);
void moment_task(cv::Mat *dst, cv::Moments mom);

//�V���A���ʐM�֐�
void serial_task();
void serial_setup();
void serial_exit();

//�t���O�����֐�
bool check_flag();
void change_flag();
void mode_releace();
int check_mode();
bool calib_motor(int ch);

//�t�@�C����
void set_fname();