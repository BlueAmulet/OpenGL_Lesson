//
//  Shader.h
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/12.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#ifndef Shader_h
#define Shader_h

#include <GL/glew.h>
#include <string>
#include <memory>

namespace Shader {

	class Program;
	typedef std::shared_ptr<Program> ProgramPtr; /// �v���O�����I�u�W�F�N�g�|�C���^�^

												 /**
												 * �V�F�[�_�v���O�����N���X
												 */
	class Program {
	public:
		/**
		* �V�F�[�_�v���O���������֐�
		*
		* @param   vsFilename  �o�[�e�b�N�X�V�F�[�_�t�@�C����
		* @param   fsFilename  �t���O�����g�V�F�[�_�t�@�C����
		*
		* @return �V�F�[�_�v���O����
		*/
		static ProgramPtr Create(const char* vsFilename, const char* fsFilename);

		/**
		* �V�F�[�_�v���O���������֐�
		*
		* @param   vsFilename  �o�[�e�b�N�X�V�F�[�_�t�@�C����
		* @param   gsFilename  �W�I���g���V�F�[�_�t�@�C����
		* @param   fsFilename  �t���O�����g�V�F�[�_�t�@�C����
		* @param   input_type  �W�I���g���V�F�[�_�̓��̓^�C�v
		* @param   output_type �W�I���g���V�F�[�_����̏o�̓^�C�v
		* @param   vertex_out  �W�I���g���V�F�[�_�̍ő�o�͒��_��
		* @return �V�F�[�_�v���O����
		*/
		static ProgramPtr Create(const char* vsFilename, const char* gsFilename, const char* fsFilename, GLint input_type, GLint output_type, GLint vertex_out = -1);

		bool UniformBlockBinding(const char* blockName, GLuint bindingPoint);
		void UseProgram();
		void BindTexture(GLenum unit, GLenum type, GLuint texture);

	private:
		Program() = default;
		~Program();
		Program(const Program&) = delete;
		Program& operator=(const Program&) = delete;

	private:
		GLuint program = 0;         ///�v���O�����I�u�W�F�N�g
		GLint samplerLocation = -1; ///�T���v���[�̈ʒu
		int samplerCount = 0;       ///�T���v���[�̐�
		std::string name;           ///�v���O������
	};

	GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename);
	GLuint CreateProgramFromFile(const char* vsFilename, const char* gsFilename, const char* fsFilename, GLint input_type, GLint output_type, GLint vertex_out);

}

#endif /* Shader_h */

