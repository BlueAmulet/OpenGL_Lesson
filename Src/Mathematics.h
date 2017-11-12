//
//  Mathematics.h
//  Cocos2dxPractice
//
//  Created by Tomoya Fujii on 2017/06/25.
//
//

#ifndef Mathematics_h
#define Mathematics_h
#include <math.h>

//========================================================
// 2�����x�N�g���N���X
//========================================================

class Vector2 {
public:
	//�R���X�g���N�^
	Vector2() {}
	Vector2(float x, float y) : m_x(x), m_y(y) {}

public:
	float m_x;
	float m_y;

	/**
	* @desc    �ݒ�p�֐�
	* @param   x ���W
	* @param   y ���W
	*/
	void set(float x, float y) {
		this->m_x = x;
		this->m_y = y;
	}

	void set(Vector2 vec) {
		this->m_x = vec.m_x;
		this->m_y = vec.m_y;
	}

	/**
	* @desc    �x�N�g���̑傫�������߂�
	* @return  �x�N�g���̑傫��
	*/
	float norm() {
		return sqrtf(this->m_x * this->m_x + this->m_y * this->m_y);
	}

	/**
	* @desc    �Q�_�Ԃ̋��������߂�
	* @param   dest    �ړI�̍��W
	* @return  �ړI�܂ł̋���
	*/
	float distance(Vector2* dest) {
		float distX = dest->m_x - this->m_x;
		float distY = dest->m_y - this->m_y;
		return sqrtf(distX * distX + distY * distY);
	}

	/**
	* @desc    ����
	* @param   vec �ΏۂƂȂ������̃x�N�g��
	* @return  ���ς̒l
	*/
	float dot(Vector2* vec) {
		return this->m_x * vec->m_y + this->m_y * vec->m_x;
	}

	/**
	* @desc    �O��
	* @param   vec �ΏۂƂȂ������̃x�N�g��
	* @return  �O�ς̒l
	*/
	float cross(Vector2* vec) {
		return this->m_x * vec->m_y - this->m_y * vec->m_x;
	}

	/**
	* @desc    �P�ʃx�N�g���擾
	*/
	Vector2 getUnitVector() {
		float norm = this->norm();
		return Vector2(this->m_x / norm, this->m_y / norm);
	}

	/////////////////////////////////////////////
	//Vector2�p ���Z�q�̃I�[�o�[���[�h
	/////////////////////////////////////////////

	// + ���Z�q�̃I�[�o�[���[�h
	Vector2 operator +(Vector2 vec)
	{
		return Vector2(this->m_x + vec.m_x, this->m_y + vec.m_y);
	}

	// - ���Z�q�̃I�[�o�[���[�h
	Vector2 operator -(Vector2 vec)
	{
		return Vector2(this->m_x - vec.m_x, this->m_y - vec.m_y);
	}

	// * ���Z�q�̃I�[�o�[���[�h
	Vector2 operator *(float param)
	{
		return Vector2(this->m_x * param, this->m_y * param);
	}

	// / ���Z�q�̃I�[�o�[���[�h
	Vector2 operator /(float param)
	{
		return Vector2(this->m_x / param, this->m_y / param);
	}

	// = ���Z�q�̃I�[�o�[���[�h
	Vector2 operator =(Vector2 vec)
	{
		return Vector2(this->m_x = vec.m_x, this->m_y = vec.m_y);
	}

	// += ���Z�q�̃I�[�o�[���[�h
	Vector2 operator +=(Vector2 vec)
	{
		return Vector2(this->m_x += vec.m_x, this->m_y += vec.m_y);
	}

	// -= ���Z�q�̃I�[�o�[���[�h
	Vector2 operator -=(Vector2 vec)
	{
		return Vector2(this->m_x -= vec.m_x, this->m_y -= vec.m_y);
	}

	// *= ���Z�q�̃I�[�o�[���[�h
	Vector2 operator *=(float param)
	{
		return Vector2(this->m_x *= param, this->m_y *= param);
	}

	// /= ���Z�q�̃I�[�o�[���[�h
	Vector2 operator /=(float param)
	{
		return Vector2(this->m_x /= param, this->m_y /= param);
	}

	//== ���Z�q�̃I�[�o�[���[�h
	bool operator ==(Vector2 vec) {
		return this->m_x == vec.m_x && this->m_y == vec.m_y;
	}

	//!= ���Z�q�̃I�[�o�[���[�h
	bool operator != (Vector2 vec) {
		return !(this->m_x == vec.m_x && this->m_y == vec.m_y);
	}
	/////////////////////////////////////////////
	//���Z�q�̃I�[�o�[���[�h�I���
	/////////////////////////////////////////////

};

using Point = Vector2;

class CLine {
public:
	//�R���X�g���N�^
	CLine() : m_setStartPoint(false), m_setEndPoint(false), m_startPoint(Point(0, 0)), m_endPoint(Point(0, 0)), m_vector(Vector2(0, 0)) {}
	CLine(Point startPoint, Point endPoint) : m_startPoint(startPoint), m_endPoint(endPoint) {
		this->m_vector = endPoint - startPoint;
	}
private:
	//�n�_�ݒ�ς݃t���O
	bool m_setStartPoint = false;
	//�I�_�ݒ�ς݃t���O
	bool m_setEndPoint = false;

public:
	//�n�_
	Point m_startPoint;
	//�I�_
	Point m_endPoint;
	//�x�N�g��
	Vector2 m_vector;

public:
	//�n�_�ݒ�
	void setStartPoint(Point point) {
		this->m_startPoint = point;
		m_setStartPoint = true;
		//�I�_���ݒ肳��Ă�����
		if (m_setEndPoint) {
			this->m_vector = this->m_endPoint - this->m_startPoint;
		}
	}
	//�I�_�ݒ�
	void setEndPoint(Point point) {
		this->m_endPoint = point;
		this->m_setEndPoint = true;
		//�n�_���ݒ肳��Ă�����
		if (m_setStartPoint) {
			this->m_vector = this->m_endPoint - this->m_startPoint;
		}
	}
	//�x�N�g���ݒ�
	void setVector(Vector2 vec) {
		this->m_vector = vec;
		if (this->m_setStartPoint == true) {
			this->m_endPoint = this->m_startPoint + this->m_vector;
		}
	}
	//�n�_�I�_�ݒ�
	void set(Point startPoint, Point endPoint) {
		this->m_startPoint = startPoint;
		this->m_endPoint = endPoint;
		this->m_vector = endPoint - startPoint;
	}
};
#endif /* Mathematics_h */