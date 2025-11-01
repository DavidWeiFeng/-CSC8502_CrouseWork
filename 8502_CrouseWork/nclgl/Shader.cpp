#include "Shader.h"      // 包含着色器类的头文件（类声明、常量定义）
#include "Mesh.h"        // 包含顶点缓冲区枚举（VERTEX_BUFFER、COLOUR_BUFFER等）
#include <iostream>      // 标准输入输出库
#include <filesystem> // C++17

using std::string;
using std::cout;
using std::ifstream;

// 用于保存所有已创建的 Shader 实例的静态成员
vector<Shader*> Shader::allShaders;

// OpenGL 对应的着色器类型常量数组
GLuint shaderTypes[SHADER_MAX] = {
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER
};

// 每种着色器类型对应的名称字符串，用于打印日志
string ShaderNames[SHADER_MAX] = {
	"Vertex",
	"Fragment",
	"Geometry",
	"Tess. Control",
	"Tess. Eval"
};
// 构造函数：根据传入的着色器文件名创建着色器对象
Shader::Shader(const string& vertex, const string& fragment, const string& geometry, const string& domain, const string& hull)	{
	shaderFiles[SHADER_VERTEX]		= vertex;
	shaderFiles[SHADER_FRAGMENT]	= fragment;
	shaderFiles[SHADER_GEOMETRY]	= geometry;
	shaderFiles[SHADER_DOMAIN]		= domain;
	shaderFiles[SHADER_HULL]		= hull;

	Reload(false);
	allShaders.emplace_back(this);
}

Shader::~Shader(void)	{
	DeleteIDs();
}

void	Shader::Reload(bool deleteOld) {
	if(deleteOld) {
		DeleteIDs();
	}

	programID		= glCreateProgram();
	// 为每种类型的着色器加载、编译
	for (int i = 0; i < SHADER_MAX; ++i) {
		if (!shaderFiles[i].empty()) {  // 若该类型文件存在
			GenerateShaderObject(i);// 创建并编译该类型着色器
		}
		else {
			objectIDs[i]	= 0; // 没有该类型文件，设为空
			shaderValid[i]	= 0;
		}
	}
	SetDefaultAttributes(); // 绑定默认的顶点属性位置（position、normal等）
	LinkProgram();
	PrintLinkLog(programID);
}

bool	Shader::LoadShaderFile(const string& filename, string &into)	{
	ifstream	file(SHADERDIR + filename);
	string		textLine;
	char buffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buffer); // 获取当前工作目录
	std::string currentDir(buffer);

	std::string fullPath = SHADERDIR + filename;
	std::string absolutePath = currentDir + "\\" + fullPath; // 拼接成绝对路径（简单拼接）

	std::cout << "Current directory: " << currentDir << std::endl;
	std::cout << "Trying to load shader from: " << absolutePath << std::endl;
	cout << "Loading shader text from " << filename << "\n\n";

	if(!file.is_open()){
		cout << "ERROR ERROR ERROR ERROR: File does not exist!\n";
		return false;
	}
	int lineNum = 1; 
	while(!file.eof()){
		getline(file,textLine);
		textLine += "\n";
		into += textLine;
		++lineNum;
	}
	cout << "\nLoaded shader text!\n\n";
	return true;
}
//-----------------------------------------------------------
// 编译指定类型的着色器（顶点、片段等）
//-----------------------------------------------------------
void	Shader::GenerateShaderObject(unsigned int i)	{
	cout << "Compiling Shader...\n";

	string shaderText;
	if(!LoadShaderFile(shaderFiles[i],shaderText)) {
		cout << "Loading failed!\n";
		shaderValid[i] = false;
		return;
	}

	objectIDs[i] = glCreateShader(shaderTypes[i]);  // 创建对应类型的着色器对象

	const char* chars = shaderText.c_str(); // 获取源码字符串指针
	int textLength		= (int)shaderText.length(); // 获取源码长度
	glShaderSource(objectIDs[i], 1, &chars, &textLength); // 传入着色器源码
	glCompileShader(objectIDs[i]); // 编译着色器

	glGetShaderiv(objectIDs[i], GL_COMPILE_STATUS, &shaderValid[i]);

	if (!shaderValid[i]) {
		cout << "Compiling failed!\n";
		PrintCompileLog(objectIDs[i]);
	}
	else {
		cout << "Compiling success!\n\n";
	}

	glObjectLabel(GL_SHADER, objectIDs[i], -1, shaderFiles[i].c_str());
	// 将着色器附加到当前的程序对象上
	glAttachShader(programID, objectIDs[i]);
}

// 链接着色器程序（在所有着色器都附加后调用）
void Shader::LinkProgram()	{
	glLinkProgram(programID); // 链接程序
	glGetProgramiv(programID, GL_LINK_STATUS, &programValid); // 获取链接状态
}

//-----------------------------------------------------------
// 为常见的顶点属性绑定默认位置索引
//-----------------------------------------------------------
void Shader::SetDefaultAttributes() {
	glBindAttribLocation(programID, VERTEX_BUFFER, "position");      // 顶点位置
	glBindAttribLocation(programID, COLOUR_BUFFER, "colour");        // 顶点颜色
	glBindAttribLocation(programID, NORMAL_BUFFER, "normal");        // 顶点法线
	glBindAttribLocation(programID, TANGENT_BUFFER, "tangent");       // 顶点切线
	glBindAttribLocation(programID, TEXTURE_BUFFER, "texCoord");      // 纹理坐标

	glBindAttribLocation(programID, WEIGHTVALUE_BUFFER, "jointWeights"); // 骨骼权重
	glBindAttribLocation(programID, WEIGHTINDEX_BUFFER, "jointIndices"); // 骨骼索引
}


void	Shader::DeleteIDs() {
	if (!programID) {
		return;
	}
	for (int i = 0; i < SHADER_MAX; ++i) {
		if (objectIDs[i]) {
			glDetachShader(programID, objectIDs[i]);
			glDeleteShader(objectIDs[i]);
		}
	}
	glDeleteProgram(programID);
	programID = 0;
}

void	Shader::PrintCompileLog(GLuint object) {
	int logLength = 0;
	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength) {
		char* tempData = new char[logLength];
		glGetShaderInfoLog(object, logLength, NULL, tempData);
		std::cout << "Compile Log:\n" << tempData << std::endl;
		delete[] tempData;
	}
}

void	Shader::PrintLinkLog(GLuint program) {
	int logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength) {
		char* tempData = new char[logLength];
		glGetProgramInfoLog(program, logLength, NULL, tempData);
		std::cout << "Link Log:\n" << tempData << std::endl;
		delete[] tempData;
	}
}

void Shader::ReloadAllShaders() {
	for (auto& i : allShaders) {
		i->Reload();
	}
}