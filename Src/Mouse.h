//
//  Mouse.h
//  OpenGL_Practice
//
//  Created by Tomoya Fujii on 2017/10/23.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#ifndef Mouse_h
#define Mouse_h

#include "Mathematics.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <functional>

class Mouse {
public:
	enum class CALLBACK_TYPE {
		CLICK = 0,
		CLICK_END = 1,
		MOVE = 2,
		SCROLL = 3,
	};

	enum class BUTTON_TYPE {
		LEFT,
		RIGHT,
		MIDDLE,
		BUTTON_1,
		BUTTON_2,
		BUTTON_3,
		BUTTON_4,
		BUTTON_5,

		NONE,
	};

	struct MouseEvent {
		BUTTON_TYPE buttonType;
		Vector2 position;
		Vector2 scroll;
	};
public:
	static Mouse& getInstance();

	double getX() { return x; }
	double getY() { return y; }

	/**
	* �R�[���o�b�N�ݒ�֐�
	*
	* @param type      �R�[���o�b�N�̎��
	* @param callback  �R�[���o�b�N�֐�
	*/
	void setCallback(CALLBACK_TYPE type, std::function<void(MouseEvent*)> callback);

	//glfw�ɓn���R�[���o�b�N
	void click(GLFWwindow* window, int button, int action, int mods);
	void move(GLFWwindow* window, double xpos, double ypos);
	void scroll(GLFWwindow* window, double xoffset, double yoffset);


private:
	Mouse();
	~Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	/**
	* �}�E�X�{�^������
	*
	* @param button     glfw����擾�����{�^�����
	*
	* @return BUTTON_TYPE �ɕύX�����l
	*/
	BUTTON_TYPE checkButton(int button);

	//�}�E�X���W
	double x;
	double y;
	//�R�[���o�b�N
	std::vector<std::function<void(MouseEvent*)>> mouseCallbacks;
};

#endif /* Mouse_h */

