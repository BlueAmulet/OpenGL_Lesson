//
//  Texture.h
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/12.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#ifndef Texture_h
#define Texture_h

#include "TextureLoader.h"
#include <GL/glew.h>
#include <memory>
#include <sys/stat.h>


class Texture;
typedef std::shared_ptr<Texture> TexturePtr; ///�e�N�X�`���|�C���^

											 /**
											 * �e�N�X�`���N���X
											 */
class Texture {
public:
	//�e�N�X�`������
	static TexturePtr Create(int width, int height, GLenum iformat, GLenum format, const void* data);
	//�摜�t�@�C���ǂݍ���
	static TexturePtr LoadFromFile(const char*);
	//ID�擾
	GLuint Id() const { return texId; }
	//���擾
	GLsizei Width() const { return width; }
	//�����擾
	GLsizei Height() const { return height; }

	/**
	* �g��k�����̕ۊǕ��@�؂�ւ�
	* �f�t�H���g
	*/
	void defaultFilter();
	/**
	* �g��k�����̕ۊǕ��@�؂�ւ�
	* �������茩����
	*/
	void dotFilter();

private:
	Texture() = default;
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	GLuint texId = 0;
	int width = 0;
	int height = 0;
	TEXTURE_TYPE type;
};

#endif /* Texture_h */