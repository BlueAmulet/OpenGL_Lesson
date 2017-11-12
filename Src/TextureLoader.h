//
//  TextureLoader.h
//  OpenGL_Practice
//
//  Created by Tomoya Fujii on 2017/10/20.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#ifndef TextureLoader_h
#define TextureLoader_h

#include <GL/glew.h>
#include <vector>
#include <map>
#include <sys/stat.h>

enum class TEXTURE_TYPE {
	BMP,
	PNG,

	NONE,
};

struct TextureData {
	//�摜���
	TEXTURE_TYPE type = TEXTURE_TYPE::NONE;
	//�f�[�^�`��
	GLenum iformat;
	//�ǂݍ��ݕ��@
	GLenum format;

	//�摜��
	int width;
	//�摜����
	int height;
	//�摜�f�[�^�ւ̃A�h���X
	std::vector<uint8_t>data;
};

class Loader;

class TextureLoader {
public:
	static TextureLoader& getInstance();

	void loadFile(const char* filename, TextureData& textureData);

private:
	TextureLoader();
	~TextureLoader();
	//�摜��ރ`�F�b�N
	TEXTURE_TYPE checkType(const std::vector<uint8_t>& buf, const struct stat& st);
	std::map<TEXTURE_TYPE, Loader*> loaderes;

};

class Loader {
public:
	virtual ~Loader() {}
public:
	virtual void loadFile(const std::vector<uint8_t>& buf, TextureData& textureData) = 0;
};

class BMPLoader : public Loader {
public:
	void loadFile(const std::vector<uint8_t>& buf, TextureData& textureData) override;
};

class PNGLoader : public Loader {
public:
	void loadFile(const std::vector<uint8_t>& buf, TextureData& textureData) override;

private:
	struct HeaderInfo {
		uint32_t width;
		uint32_t height;
		uint32_t bitDepth;
		uint32_t colorType;
		uint32_t compressionMethod;
		uint32_t filteMethod;
		uint32_t interlaceMethod;
	};

	HeaderInfo getHeaderInfo(const uint8_t* data);
	/**
	* IDATA�f�[�^�ǂݍ��݊֐�
	*
	* @param data      �ǂݍ��݌��f�[�^�ւ̃A�h���X
	* @param offset    �ǂݍ��݊J�n�ʒu
	* @param length    �ǂݍ��ޒ���
	* @param buf       �ǂݍ��ݐ�z��
	*/
	void readIDATData(const uint8_t* data, int offset, size_t length, std::vector<uint8_t>& buf);
	//1�s�N�Z���̃o�C�g���擾
	size_t getPixelBytes(const HeaderInfo& info);

	/**
	* Peath�擾
	*
	* @param l     ��
	* @param t     ��
	* @param tl    ����
	*/
	float paethPredictor(float l, float t, float tl);
};


#endif /* TextureLoader_h */
