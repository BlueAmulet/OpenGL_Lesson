//
//  Constants.h
//  OpenGL_Practice
//
//  Created by Tomoya Fujii on 2017/10/23.
//  Copyright c 2017年 TomoyaFujii. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

#define SAFE_DELETE(p) if(p){ delete p; p = nullptr;}
#define ARRAY_SIZE(a)   (sizeof(a)/sizeof(a[0]))

//コントローラーを使用するときはコメントを解除
#define USE_CONTROLLER    1


#define TEXTURE_NAME		"Res/cosmo.png"
#define IMAGE_FILE_TOROID   "Res/Toroid/Toroid.bmp"
#define MODEL_FILE_TOROID   "Res/Toroid/Toroid.fbx"
#define IMAGE_FILE_BULLET   "Res/Model/tama.png"
#define MODEL_FILE_BULLET   "Res/Model/tama.fbx"
#define IMAGE_FILE_PLAYER	"Res/Player/Player.bmp"
#define MODEL_FILE_PLAYER	"Res/Player/Player.fbx"
#define MODEL_FILE_BLAST	"Res/Model/Blast.fbx"
#define FONT_FILE_UNINEUE	"Res/Font/UniNeue.fnt"

#define MODEL_FILE_SPACESPHERE	"Res/Model/SpaceSphere.fbx"
#define IMAGE_FILE_SPACESPHERE	"Res/Model/SpaceSphere.bmp"

#define AUDIO_FILE_ACF_SAMPLE	"Res/Audio/SampleSound.acf"
#define AUDIO_FILE_ACB_SAMPLE	"Res/Audio/SampleCueSheet.acb"

//透過オブジェクト
//glDepthMask(GL_FALSE);

/*
meshBuffer->GetMesh("pCylinder1")->Draw(meshBuffer);
meshBuffer->GetMesh("pPlane1")->Draw(meshBuffer);
meshBuffer->GetMesh("pPlane2")->Draw(meshBuffer);
meshBuffer->GetMesh("pPlane3")->Draw(meshBuffer);
*/
//glDepthMask(GL_TRUE);


//変換行列
/*
//セピア
postEffect.matColor[0] = glm::vec4(0.393f, 0.349f, 0.272f, 0);
postEffect.matColor[1] = glm::vec4(0.769f, 0.686f, 0.534f, 0);
postEffect.matColor[2] = glm::vec4(0.189f, 0.168f, 0.131f, 0);
postEffect.matColor[3] = glm::vec4(0, 0, 0, 1);
*/

/*
//モノトーンにしたい
glm::vec4 mono(0.299, 0.587, 0.114, 0);
postEffect.matColor[0] = mono;//glm::vec4(0.299, 0.299, 0.299, 0);
postEffect.matColor[1] = mono;//glm::vec4(0.587, 0.587, 0.587, 0);;
postEffect.matColor[2] = mono;//glm::vec4(0.114, 0.114, 0.114, 0);;
postEffect.matColor[3] = glm::vec4(0, 0, 0, 1);
transpose(postEffect.matColor);
*/

/*
//無変換
postEffect.matColor[0] = glm::vec4(1, 0, 0, 0);
postEffect.matColor[1] = glm::vec4(0, 1, 0, 0);
postEffect.matColor[2] = glm::vec4(0, 0, 1, 0);
postEffect.matColor[3] = glm::vec4(0, 0, 0, 1);
*/

/*
//ネガポジ反転
postEffect.matColor[0] = glm::vec4(-1, 0, 0, 1);
postEffect.matColor[1] = glm::vec4( 0,-1, 0, 1);
postEffect.matColor[2] = glm::vec4( 0, 0,-1, 1);
postEffect.matColor[3] = glm::vec4( 0, 0, 0, 1);
transpose(postEffect.matColor);
*/

#endif /* Constants_h */
