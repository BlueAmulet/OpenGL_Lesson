/**
* @file Uniform.h
*/
#ifndef UNIFORM_H_INCLUDED
#define UNIFORM_H_INCLUDED

#include <glm/glm.hpp>
namespace Uniform {
	/**
	*頂点シェータのパラメータ型
	*/
	struct VertexData {
		glm::mat4 matMVP;
		glm::mat4 matModel;
		glm::mat3x4 matNormal;
		glm::vec4 color;
	};


	/**
	* ライトデータ(点光源)
	*/
	struct PointLight {
		glm::vec4 position;     ///座標(ワールド座標)
		glm::vec4 color;        ///明るさ
	};

	const int maxLightCount = 4;    ///ライトの数

									/**
									* ライティングパラメータ
									*/
	struct LightData {
		glm::vec4 ambinetColor;     ///環境光
		PointLight light[maxLightCount];  ///ライトのリスト
	};

	/**
	* ポストエフェクトデータ
	*/
	struct PostEffectData {
		glm::mat4x4 matColor;   ///色変換行列
	};
};

#endif //UNIFORM_H_INCLUDED
