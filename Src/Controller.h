//
//  Controller.h
//  OpenGL_Practice
//
//  Created by Tomoya Fujii on 2017/10/23.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#ifndef Controller_h
#define Controller_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
#include <vector>

#define JOYSTICK_LEFT 0
#define JOYSTICK_RIGHT 1

//PS4�R���g���[���[�}�N��
#define PS4_SQUARE_BUTTON   0
#define PS4_X_BUTTON        1
#define PS4_CIRCLE_BUTTON   2
#define PS4_TRIANGLE_BUTTON 3
#define PS4_L1_BUTTON       4
#define PS4_R1_BUTTON       5
#define PS4_L2_BUTTON       6
#define PS4_R2_BUTTON       7
#define PS4_SHARE_BUTTON    8
#define PS4_OPTIONS_BUTTON  9
#define PS4_L3_BUTTON       10
#define PS4_R3_BUTTON       11
#define PS4_HOME_BUTTON     12
#define PS4_TOUCH_PAD       13
#define PS4_DPAD_UP         14
#define PS4_DPAD_RIGHT      14
#define PS4_DPAD_BOTTOM     14
#define PS4_DPAD_LEFT       14


class ControllerManager;
class ControllerData {
public:

	friend ControllerManager;

	struct Joystick {
		float axisX;
		float axisY;
	};

	Joystick getLeftJoystick() { return joystickes[JOYSTICK_LEFT]; }
	Joystick getRigthJoystick() { return joystickes[JOYSTICK_RIGHT]; }

	bool getPressed(int buttonNum);
	ControllerData(int Id);
	void clear();
private:
	int joystickId;

	std::vector<Joystick>   joystickes;
	std::vector<bool>       buttons;
};

class ControllerManager {
public:
	/**
	* �C���X�^���X�擾
	*/
	static ControllerManager& getInstance();

	/**
	* �g�p�\�ȃW���C�X�e�B�b�NID���擾
	*/
	int getEnableId();

	/**
	* �g�p����߂鎞�Ăяo��
	*
	* @param Id    �g�p���Ă����W���C�X�e�B�b�N��ID
	*/
	void stopUse(int Id);
	/**
	* �f�[�^�̃A�h���X�擾
	*/
	ControllerData* getJoystick(int Id);

	/**
	* �f�[�^�X�V�֐�
	*/
	void update();

	//�W���C�X�e�B�b�N�ڑ�����ڑ����R�[���o�b�N
	void joystickCallback(int joy, int event);

private:
	ControllerManager();
	~ControllerManager();
	ControllerManager(const ControllerManager&) = delete;
	ControllerManager& operator=(const ControllerManager&) = delete;

	void addJoystick(int joy);
	void removeJoystick(int joy);
	/**
	* GLFW����f�[�^�擾
	*/
	void acquisition(int Id);

	std::map<int, ControllerData*> controllerData;
	std::vector<bool> enableId;
};

#endif /* Controller_h */

