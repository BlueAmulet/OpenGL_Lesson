/**
* @file GameEngine.cpp
*/
#include "GameEngine.h"
#include "GLFWEW.h"
#include "Audio.h"
#include "Constants.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

///���_�f�[�^�^
struct Vertex {
	glm::vec3 position;     ///���W
	glm::vec4 color;        ///�F
	glm::vec2 texCoord;     ///�e�N�X�`�����W
	glm::vec3 normal;       ///�@��
};

float length = sqrt(2) * 0.5;
///���_�f�[�^
const Vertex vertices[] = {
	{ { -0.5f, -0.3f,  0.5f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.3f, -0.3f,  0.5f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.3f,  0.5f,  0.5f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.5f,  0.5f,  0.5f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },

	{ { -0.3f,  0.3f,  0.1f } ,{ 0.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.3f, -0.5f,  0.1f } ,{ 0.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.5f, -0.5f,  0.1f } ,{ 0.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.5f, -0.5f,  0.1f } ,{ 1.0f, 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 0.5f,  0.3f,  0.1f } ,{ 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.3f,  0.3f,  0.1f } ,{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f } },

	{ { -1.0f, -1.0f, 0.0f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 1.0f, -1.0f, 0.0f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { 1.0f,  1.0f, 0.0f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
	{ { -1.0f,  1.0f, 0.0f } ,{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } },
};

///�C���f�b�N�X�f�[�^
const GLuint indices[] = {
	0, 1, 2, 2, 3, 0,

	4, 5, 6, 7, 8, 9,

	10, 11, 12, 12, 13, 10,

};




/**
* �����`��f�[�^
*/
struct RenderingPart {
	GLsizei size;       ///�`�悷��C���f�b�N�X��
	GLvoid* offset;     ///�`��J�n�C���f�b�N�X�̃o�C�g�I�t�Z�b�g
};

/**
* RenderingPart ���쐬����
*
* @param   size    �`�悷��C���f�b�N�X��
* @param   offset  �`��J�n�C���f�b�N�X���̃I�t�Z�b�g�i�C���f�b�N�X�P�ʁj
*
* @return  �쐬���������`��f�[�^
*/
RenderingPart MakeRenderingPart(GLsizei size, GLsizei offset) {
	return { size, reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)) };
}

/**
* �����`��f�[�^���X�g
*/
static const RenderingPart renderingParts[] = {
	MakeRenderingPart(12, 0),
	MakeRenderingPart(6, 12),
};

/**
*  Vertex Buffer Onject ���쐬����
*
* @param   size    ���_�f�[�^�̃T�C�Y
* @param   data    ���_�f�[�^�ւ̃|�C���^
*
* @return  �쐬���� VBO
*/
GLuint CreateVBO(GLsizeiptr size, const GLvoid* data) {
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo;
}


///�C���f�b�N�X�f�[�^���g�����Ƃł���Ȃ��f�[�^�𕡐����Ȃ��Ă���
/**
* Index Buffer Object���쐬����
*
* @param   size    �C���f�b�N�X�f�[�^�̃T�C�Y
* @param   data    �C���f�b�N�X�f�[�^�ւ̃|�C���^
*
* @return  �쐬����IBO
*/
GLuint CreateIBO(GLsizeiptr size, const GLvoid* data) {
	GLuint ibo = 0;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;
}

/**
*  ���_�A�g���r���[�g��ݒ肷��
*
* @param   index   ���_�A�g���r���[�g�̃C���f�b�N�X
* @param   cls     ���_�f�[�^�^��
* @param   mbr     ���_�A�g���r���[�g�ɐݒ肷�� cls �̃����o�ϐ���
*/
#define SetVertexAttribPointer( index, cls, mbr ) SetVertexAttribPointerI ( \
index, \
sizeof( cls::mbr ) / sizeof( float ), \
sizeof(cls),\
reinterpret_cast<GLvoid*>(offsetof(cls,mbr)))

void SetVertexAttribPointerI(GLuint index, GLint size, GLsizei stride, const GLvoid* pointer) {
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}

/**
*  Vertex Array Object ���쐬����
*
* @param   vbo VAO �Ɋ֘A�Â����� VBO
* @param   ibo VAO �Ɋ֘A�Â����� IBO
*
* @return  �쐬���� VAO
*/
GLuint CreateVAO(GLuint vbo, GLuint ibo) {
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2, Vertex, texCoord);
	SetVertexAttribPointer(3, Vertex, normal);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	return vao;
}


/**
* �Q�[���G���W���̃C���X�^���X���擾����
*
* @return �Q�[���G���W���̃C���X�^���X
*/
GameEngine & GameEngine::Instance(){
	static GameEngine instance;
	return instance;
}

/**
* �Q�[���G���W��������������
*
* @param w	�E�B���h�E�̕`��͈͂̕��i�s�N�Z���j
* @param h	�E�B���h�E�̕`��͈͂̍����i�s�N�Z���j
* @param title	�E�B���h�E�^�C�g���iUTF-8��0�I�[������j
*
* @retval true	����������
* @retval false ���������s
*
* Run�֐����Ăяo���O�ɁA��x�����Ăяo���Ă����K�v������B
* ��x�������ɐ�������ƁA�Ȍ�̌Ăяo���ł͉���������true��Ԃ�
*/
bool GameEngine::Init(int w, int h, const char * title)
{
	if (isInitialized) {
		return true;
	}
	if (!GLFWEW::Window::Instance().Init(w, h, title)) {
		return false;
	}

	//<-----------------�����ɏ�����������ǉ�����----------------->
	vbo = CreateVBO(sizeof(vertices), vertices);
	ibo = CreateIBO(sizeof(indices), indices);
	vao = CreateVAO(vbo, ibo);
	uboLight = UniformBuffer::Create(sizeof(Uniform::LightData), 1, "LightData");
	uboPostEffect = UniformBuffer::Create(sizeof(Uniform::PostEffectData), 2, "PostEffectData");
	offscreen = OffscreenBuffer::Create(960, 600);
	if (!vbo || !ibo || !vao || !uboLight || !uboPostEffect || !offscreen) {
		std::cerr << "ERROR : GameEngine�̏������Ɏ��s" << std::endl;
		return false;
	}

	static const char* const shaderNameList[][3] = {
		{"Tutorial", "Res/Tutorial.vert", "Res/Tutorial.frag"},
		{"ColorFilter", "Res/ColorFilter.vert", "Res/ColorFilter.frag"},
		{"NonLighting", "Res/NonLighting.vert", "Res/NonLighting.frag"},
	};
	shaderMap.reserve(sizeof(shaderNameList) / sizeof(shaderNameList[0]));
	for (auto& e : shaderNameList) {
		Shader::ProgramPtr program = Shader::Program::Create(e[1], e[2]);
		if (!program) {
			continue;
		}
		shaderMap.insert(std::make_pair(std::string(e[0]), program));
	}
	shaderMap["Tutorial"]->UniformBlockBinding("VertexData", 0);
	shaderMap["Tutorial"]->UniformBlockBinding("LightData", 1);
	shaderMap["ColorFilter"]->UniformBlockBinding("PostEffectData", 2);
	shaderMap["NonLighting"]->UniformBlockBinding("VertexData", 0);

	meshBuffer = Mesh::Buffer::Create(10 * 1024, 30 * 1024);
	if (!meshBuffer) {
		std::cerr << "ERROR : GameEngine�̏������Ɏ��s" << std::endl;
		return false;
	}

	entityBuffer = Entity::Buffer::Create(1024, sizeof(Uniform::VertexData), 0, "VertexData");
	if (!entityBuffer) {
		std::cerr << "ERROR : GameEngine�̏������Ɏ��s" << std::endl;
		return false;
	}

	rand.seed(std::random_device()());
	fontRenderer.Init(1024, glm::vec2(800, 600));
	isInitialized = true;

	return true;
}

/**
* �Q�[�������s����
*/
void GameEngine::Run(){
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	double prevTime = glfwGetTime();

	while (!window.ShouldClose()) {
		const double curTime = glfwGetTime();
		const double delta = curTime - prevTime;
		prevTime = curTime;
		window.UpdateGamePad();
		Update(glm::min(0.25,delta));
		Render();
		window.SwapBuffers();
	}
}

/**
* ��ԍX�V�֐���ݒ肷��
*
* @param func �ݒ肷��X�V�֐�
*/
void GameEngine::UpdateFunc(const UpdateFuncType & func){
	updateFunc = func;
}

/**
* ��ԍX�V�֐����擾����
*
* @return �ݒ肳��Ă���X�V�֐�
*/
const GameEngine::UpdateFuncType& GameEngine::UpdateFunc() const{
	return updateFunc;
}

/**
* �e�N�X�`����ǂݍ���
*
* @param filename	�e�N�X�`���t�@�C����
*
* @retval true	�ǂݍ��ݐ���
* @retval false �ǂݍ��ݎ��s
*/
bool GameEngine::LoadTextureFromFile(const char * filename){
	const auto itr = textureBuffer.find(filename);
	if (itr != textureBuffer.end()) {
		return true;
	}
	TexturePtr texture = Texture::LoadFromFile(filename);
	if (!texture) {
		return false;
	}
	textureBuffer.insert(std::make_pair(std::string(filename), texture));
	return true;
}

/**
* ���b�V����ǂݍ���
*
* @param filename	���b�V���t�@�C����
*
* @retval true	�ǂݍ��ݐ���
* @retval fales �ǂݍ��ݎ��s
*/
bool GameEngine::LoadMeshFromFile(const char * filename)
{
	return meshBuffer->LoadMeshFromFile(filename);
}

/**
* �G���e�B�e�B��ǉ�����
*
* @param groupId	�G���e�B�e�B�̃O���[�vID
* @param pos		�G���e�B�e�B�̏ꏊ
* @param meshName	�G���e�B�e�B�̕\���Ɏg�p���郁�b�V����
* @param texName	�G���e�B�e�B�̕\���Ɏg�p����e�N�X�`���t�@�C����
* @param normalName	�G���e�B�e�B�̕\���Ɏg�p����m�[�}���}�b�v�e�N�X�`����
* @param func		�G���e�B�e�B�̏�Ԃ��X�V����֐��i�܂��͊֐��I�u�W�F�N�g�j
* @param shader		�G���e�B�e�B�̕\���Ɏg���V�F�[�_��
*
* @return	�ǉ������G���e�B�e�B�ւ̃|�C���^
*			����ȏ�G���e�B�e�B��ǉ��ł��Ȃ��ꍇ��nullptr���Ԃ����
*			��]��g�嗦�͂��̃|�C���^�o�R�Őݒ肷��
*			�Ȃ��A���̃|�C���^���A�v���P�[�V�������ŕێ�����K�v�͂Ȃ�
*/
Entity::Entity * GameEngine::AddEntity(int groupId, const glm::vec3 & pos, const char * meshName, const char * texName, const char * normalName, Entity::Entity::UpdateFuncType func, const char * shader){
	decltype(shaderMap)::const_iterator itr = shaderMap.end();
	if (shader) {
		itr = shaderMap.find(shader);
	}
	if (itr == shaderMap.end()) {
		itr = shaderMap.find("Tutorial");
		if (itr == shaderMap.end()) {
			return nullptr;
		}
	}
	const Mesh::MeshPtr& mesh = meshBuffer->GetMesh(meshName);
	TexturePtr tex[2];
	tex[0] = GetTexture(texName);
	if (normalName) {
		tex[1] = GetTexture(normalName);
	}
	else {
		tex[1] = GetTexture(NORMALMAP_FILE_DUMMY);
	}
	return entityBuffer->AddEntity(groupId, pos, mesh, tex, itr->second, func);
}

/**
* �G���e�B�e�B��ǉ�����
*
* @param groupId	�G���e�B�e�B�̃O���[�vID
* @param pos		�G���e�B�e�B�̏ꏊ
* @param meshName	�G���e�B�e�B�̕\���Ɏg�p���郁�b�V����
* @param texName	�G���e�B�e�B�̕\���Ɏg�p����e�N�X�`���t�@�C����
* @param func		�G���e�B�e�B�̏�Ԃ��X�V����֐��i�܂��͊֐��I�u�W�F�N�g�j
* @param shader		�G���e�B�e�B�̕\���Ɏg���V�F�[�_��
*
* @return	�ǉ������G���e�B�e�B�ւ̃|�C���^
*			����ȏ�G���e�B�e�B��ǉ��ł��Ȃ��ꍇ��nullptr���Ԃ����
*			��]��g�嗦�͂��̃|�C���^�o�R�Őݒ肷��
*			�Ȃ��A���̃|�C���^���A�v���P�[�V�������ŕێ�����K�v�͂Ȃ�
*/
Entity::Entity * GameEngine::AddEntity(int groupId, const glm::vec3 & pos, const char * meshName, const char * texName,
	Entity::Entity::UpdateFuncType func, const char* shader){
	return AddEntity(groupId, pos, meshName, texName, nullptr, func, shader);
}

/**
* �G���e�B�e�B���폜����
*
* @param �폜����G���e�B�e�B�̃|�C���^
*/
void GameEngine::RemoveEntity(Entity::Entity* e){
	entityBuffer->RemoveEntity(e);
}

/**
* ���C�g��ݒ肷��
*
* @param index	�ݒ肷�郉�C�g�̃C���f�b�N�X
* @param light	���C�g�f�[�^
*/
void GameEngine::Light(int index, const Uniform::PointLight & light){
	if (index < 0 || index >= Uniform::maxLightCount) {
		std::cerr << "WARNING : '" << index << "'�͕s���ȃ��C�g�C���f�b�N�X�ł�" << std::endl;
		return;
	}
	lightData.light[index] = light;
}

/**
* ���C�g���擾����
*
* @param index	�擾���郉�C�g�̃C���f�b�N�X
*
* @return ���C�g�f�[�^
*/
const Uniform::PointLight & GameEngine::Light(int index) const{
	if (index < 0 || index >= Uniform::maxLightCount) {
		std::cerr << "WARNING : '" << index << "'�͕s���ȃ��C�g�C���f�b�N�X�ł�" << std::endl;
		static const Uniform::PointLight dummy;
		return dummy;
	}
	return lightData.light[index];

}

/**
* ������ݒ肷��
*
* @param color	�����̖��邳
*/
void GameEngine::AmbientLight(const glm::vec4 & color){
	lightData.ambinetColor = color;
}

/**
* �������擾����
*
* @return �����̖��邳
*/
const glm::vec4 & GameEngine::AmbientLight() const{
	return lightData.ambinetColor;
}

/**
* ���_�̈ʒu�Ǝp����ݒ肷��
*
* @param cam �ݒ肷��J�����f�[�^
*/
void GameEngine::Camera(const CameraData & cam){
	camera = cam;
}

/**
* ���_�̈ʒu�Ǝp�����擾����
*
* @return �J�����f�[�^
*/
const GameEngine::CameraData& GameEngine::Camera() const{
	return camera;
}

/**
* �����I�u�W�F�N�g���擾����
*
* @return �����I�u�W�F�N�g
*/
std::mt19937& GameEngine::Rand(){
	return rand;
}

const GamePad & GameEngine::GetGamePad() const{
	return GLFWEW::Window::Instance().GetGamePad();
}

/**
* @copydoc Audio::Initialize
*/
bool GameEngine::InitAudio(const char * acfPath, const char * acbPath, const char * awbPath, const char * dspBusName){
	return Audio::Initialize(acfPath, acbPath, awbPath, dspBusName);
}

/**
* @copydoc Audio::Play
*/
void GameEngine::PlayAudio(int playerId, int cueId){
	Audio::Play(playerId, cueId);
}

/**
* @copydoc Audio::Stop
*/
void GameEngine::StopAudio(int playerId){
	Audio::Stop(playerId);
}

/**
* �Փˉ����n���h����ݒ肷��
*
* @param gid0		�ՓˑΏۂ̃O���[�vID
* @param gid1		�ՓˑΏۂ̃O���[�vID
* @param handler	�Փˉ����n���h��
*
* �Փ˂��������Փˉ����n���h�����Ăяo�����Ƃ��A��菬�����O���[�vID�����G���e�B�e�B�����ɓn�����
* �����Ŏw�肵���O���[�vID�̏����Ƃ͖��֌W�ł��邱�Ƃɒ���
* ex)
*	CollisionHandler(10, 1, Func)
*   �Ƃ����R�[�h�Ńn���h����o�^�����Ƃ���ƏՓ˂����������
*	Func(�O���[�vID = 1�̃G���e�B�e�B�A�O���[�vID=10�̃G���e�B�e�B)
*	�̂悤�ɌĂяo�����
*/
void GameEngine::CollisionHandler(int gid0, int gid1, Entity::CollisionHandlerType handler){
	entityBuffer->CollisionHandler(gid0, gid1, handler);
}

/**
* �Փˉ����n���h�����擾����
*
* @param gid0	�ՓˑΏۂ̃O���[�vID
* @param gid1	�ՓˑΏۂ̃O���[�vID
*
* @return �Փˉ����n���h��
*/
const Entity::CollisionHandlerType& GameEngine::CollisionHandler(int gid0, int gid1) const{
	return entityBuffer->CollisionHandler(gid0, gid1);
}

/**
* �S�Ă̏Փˉ����n���h�����폜����
*/
void GameEngine::ClearCollisionHandlerList(){
	entityBuffer->ClearCollisionHandlerList();
}

/**
* �f�X�g���N�^
*/
GameEngine::~GameEngine(){
	updateFunc = nullptr;
	Audio::Destroy();
	if (vao) {
		glDeleteVertexArrays(1, &vao);
	}
	if (ibo) {
		glDeleteBuffers(1, &ibo);
	}
	if (vbo) {
		glDeleteBuffers(1, &vbo);
	}
}

/**
* �Q�[���̏�Ԃ��X�V����
*
* @param delta	�O��̍X�V����̌o�ߎ���
*/
void GameEngine::Update(double delta){
	fontRenderer.MapBuffer();
	if (updateFunc) {
		updateFunc(delta);
	}
	//<---------------�����ɍX�V������ǉ�����------------------->
	const glm::mat4x4 matProj =
		glm::perspective(glm::radians(45.0f), 960.0f / 600.0f, 0.1f, 200.0f);
	const glm::mat4x4 matView = glm::lookAt(camera.position, camera.target, camera.up);
	entityBuffer->Update(delta, matView, matProj);
	fontRenderer.UnmapBuffer();
}

/**
* �Q�[���̏�Ԃ�`�悷��
*/
void GameEngine::Render() const{
	//<-------------�����ɕ`�揈����ǉ�����--------------------->
	glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFrameBuffer());
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, 960, 600);
	glScissor(0, 0, 960, 600);
	glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	uboLight->BufferSubData(&lightData);
	entityBuffer->Draw(meshBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glBindVertexArray(vao);
	const Shader::ProgramPtr& progColorFilter = shaderMap.find("ColorFilter")->second;
	progColorFilter->UseProgram();
	Uniform::PostEffectData postEffect;
	uboPostEffect->BufferSubData(&postEffect);
	progColorFilter->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexture());
	glDrawElements(GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);
	fontRenderer.Draw();
}

