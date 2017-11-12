/**
* @file Uniform.h
*/
#ifndef UNIFORM_H_INCLUDED
#define UNIFORM_H_INCLUDED

#include <glm/glm.hpp>
namespace Uniform {
	/**
	*���_�V�F�[�^�̃p�����[�^�^
	*/
	struct VertexData {
		glm::mat4 matMVP;
		glm::mat4 matModel;
		glm::mat3x4 matNormal;
		glm::vec4 color;
	};


	/**
	* ���C�g�f�[�^(�_����)
	*/
	struct PointLight {
		glm::vec4 position;     ///���W(���[���h���W)
		glm::vec4 color;        ///���邳
	};

	const int maxLightCount = 4;    ///���C�g�̐�

									/**
									* ���C�e�B���O�p�����[�^
									*/
	struct LightData {
		glm::vec4 ambinetColor;     ///����
		PointLight light[maxLightCount];  ///���C�g�̃��X�g
	};

	/**
	* �|�X�g�G�t�F�N�g�f�[�^
	*/
	struct PostEffectData {
		glm::mat4x4 matColor;   ///�F�ϊ��s��
	};
};

#endif //UNIFORM_H_INCLUDED
