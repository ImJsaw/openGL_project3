#pragma once

#include <vgl.h>
#include "DotNetUtilities.h"
#include "Mesh/GUA_OM.h"
#include "Mesh/DP.h"
#include <LoadShaders.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
using namespace glm;
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
using namespace std;

#define FACE_SIZE 5000
int OBJ_NUM = 13;
int objptr = 0;
Tri_Mesh *mesh;
Tri_Mesh *patch; // selected patch

GLint SCR_WIDTH = 817;
GLint SCR_HEIGHT = 541;

GLuint quadVAO;
GLuint quadVBO;
unsigned int framebuffer; 
unsigned int textureColorbuffer;
unsigned int programFrame;
unsigned int programUV;
unsigned int programImg;
glm::vec4 pixel;
int facesid[FACE_SIZE];
vector<int> facesid2;

float quadVertices1[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};

bool isLoad = false;
bool wholeModel = false;
bool line = true;
std::vector<double> vertices[13];

std::vector<double> meshUV[13];
std::vector<double> verticesPatch; // patch的點，給vbo用
std::vector<double> patchUV; // patch的uv座標，給vbo用
std::vector<double> selectedVertices;

unsigned int checkerBoardImg[3]; // image on model
//unsigned int imgptr[13] = { 1, 0, 2, 1, 0, 2, 1, 0, 2, 0, 1, 2, 0 };
unsigned int imgptr[4][13] = { 0, 1, 2, 0, 2, 0, 1, 1, 2, 0, 2, 0, 1, 
							   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
							   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

unsigned int img = 0; // one patch
unsigned int design = 0;
unsigned int checkerBoardImg2[3]; // show the picture
double rotateAngle = 0.0f;

float eyeAngleX = 0.0;
float eyeAngleY = 0.0;
float translateX = 0.0;
float translateY = 0.0;
float eyedistanceuv = 2.0;

float eyedistance = 2.0;
#define DOR(angle) (angle*3.1415/180);
int prevMouseX,prevMouseY;

mat4 MVPuv;
mat4 ProjectionUV;
mat4 ViewMatrixUV;

GLuint VBO;
GLuint meshVBO;
GLuint VAO;
GLuint VBOuv;
GLuint VAOuv;
GLuint VBOi;
GLuint VAOi;
int face[13];
int facePatch;

GLuint program;

GLuint mvpID;
GLuint ColorID;

mat4 Projection;
mat4 ViewMatrix;
mat4 Model;
mat4 MVP;

ShaderInfo meshShaders[] = {
	{ GL_VERTEX_SHADER, "meshShader.vp" },//vertex shader
	{ GL_FRAGMENT_SHADER, "meshShader.fp" },//fragment shader
	{ GL_NONE, NULL } };

typedef struct _TextureData
{
	_TextureData() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char* data;
} TextureData;

TextureData Load_png(const char* path, bool mirroredY = true)
{
	TextureData texture;
	int n;
	stbi_uc *data = stbi_load(path, &texture.width, &texture.height, &n, 4);
	if (data != NULL)
	{
		texture.data = new unsigned char[texture.width * texture.height * 4 * sizeof(unsigned char)];
		memcpy(texture.data, data, texture.width * texture.height * 4 * sizeof(unsigned char));
		// vertical-mirror image data
		if (mirroredY)
		{
			for (size_t i = 0; i < texture.width; i++)
			{
				for (size_t j = 0; j < texture.height / 2; j++)
				{
					for (size_t k = 0; k < 4; k++) {
						std::swap(texture.data[(j * texture.width + i) * 4 + k], texture.data[((texture.height - j - 1) * texture.width + i) * 4 + k]);
					}
				}
			}
		}
		stbi_image_free(data);
		printf("texture load complete at path : %s\n", path);
	}
	else {
		printf("texture load un-complete at path : %s\n", path);
	}
	return texture;
}

bool read_mouse(int x, int y, point &p, mat4 mv, mat4 proj) {

	GLdouble M[16], P[16]; GLint V[4];

	//glGetDoublev(GL_MODELVIEW_MATRIX, M);
	//glGetDoublev(GL_PROJECTION_MATRIX, P);
	glGetIntegerv(GL_VIEWPORT, V);

	/*
	ViewMatrix = lookAt(
		glm::vec3(eyedistance*cos(horizonAngle)*cos(verticleAngle), eyedistance*sin(verticleAngle), eyedistance*sin(horizonAngle)*cos(verticleAngle)),
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	*/
	

	mat4 Model = translate(translateX, translateY, 0.0f);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			M[i] = Model[i][j];
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			P[i] = proj[i][j];
		}
	}

	static const float dx[] =
	{ 0, 1,-1,-1, 1, 3,-3, 0, 0, 6,-6,-6, 6, 25,-25,  0,  0 };
	static const float dy[] =
	{ 0, 1, 1,-1,-1, 0, 0, 3,-3, 6, 6,-6,-6,  0,  0, 25,-25 };
	const float scale = 0.01f;
	const int displacements = sizeof(dx) / sizeof(float);

	int xmin = V[0], xmax = V[0] + V[2] - 1, ymin = V[1], ymax = V[1] + V[3] - 1;

	for (int i = 0; i < displacements; i++) {
		int xx = std::min(std::max(x + int(dx[i] * scale*V[2]), xmin), xmax);
		int yy = std::min(std::max(y + int(dy[i] * scale*V[3]), ymin), ymax);

		float d;
		glReadPixels(xx, yy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);

		static float maxd = 0.0f;
		if (!maxd) {
			glScissor(xx, yy, 1, 1);
			glEnable(GL_SCISSOR_TEST);
			glClearDepth(1);
			glClear(GL_DEPTH_BUFFER_BIT);
			glReadPixels(xx, yy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &maxd);
			if (maxd) {
				glClearDepth(d / maxd);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			glDisable(GL_SCISSOR_TEST);
			glClearDepth(1);
			if (!maxd)
				return false;
		}

		d /= maxd;
		if (d > 0.0001f && d < 0.9999f) {
			GLdouble X, Y, Z;
			gluUnProject(xx, yy, d, M, P, V, &X, &Y, &Z);
			p = point((float)X, (float)Y, (float)Z);
			return true;
		}
	}
	return false;
}

namespace OpenMesh_EX {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MyForm 的摘要
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form{
		public:
			MyForm(void){
				//constructer
				InitializeComponent();
				std::cout << "construct" << std::endl;
				pixel.r = 0.0f;
				pixel.g = 0.0f;
				pixel.b = 0.0f;
				pixel.a = 0.0f;
				for (int i = 0; i < FACE_SIZE; i++) facesid[i] = -1;
				Projection = glm::perspective(100.0f, 4.0f / 3.0f, 0.001f, 100.0f);
			}

		protected:
			/// <summary>
			/// 清除任何使用中的資源。
			/// </summary>
			~MyForm(){
				if (components){
					delete components;
				}
			}

		private: System::Windows::Forms::MenuStrip^  menuStrip1;
		private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
		private: System::Windows::Forms::ToolStripMenuItem^  loadModelToolStripMenuItem;
		private: System::Windows::Forms::OpenFileDialog^  openModelDialog;
		private: System::Windows::Forms::SaveFileDialog^  saveModelDialog;
		private: System::Windows::Forms::ToolStripMenuItem^  saveModelToolStripMenuItem;
		private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
		private: HKOGLPanel::HKOGLPanelControl^  hkoglPanelControl1;
		private: HKOGLPanel::HKOGLPanelControl^  hkoglPanelControl2;
		private: System::Windows::Forms::Button^  button1;
		private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel2;
	private: HKOGLPanel::HKOGLPanelControl^  hkoglPanelControl3;
	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::Button^  button6;
	protected:

		private:
			/// <summary>
			/// 設計工具所需的變數。
			/// </summary>
			System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
			/// <summary>
			/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改
			/// 這個方法的內容。
			/// </summary>
			void InitializeComponent(void)
			{
				HKOGLPanel::HKCOGLPanelCameraSetting^  hkcoglPanelCameraSetting1 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
				HKOGLPanel::HKCOGLPanelPixelFormat^  hkcoglPanelPixelFormat1 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
				HKOGLPanel::HKCOGLPanelCameraSetting^  hkcoglPanelCameraSetting2 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
				HKOGLPanel::HKCOGLPanelPixelFormat^  hkcoglPanelPixelFormat2 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
				HKOGLPanel::HKCOGLPanelCameraSetting^  hkcoglPanelCameraSetting3 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
				HKOGLPanel::HKCOGLPanelPixelFormat^  hkcoglPanelPixelFormat3 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
				this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
				this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->loadModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->saveModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->openModelDialog = (gcnew System::Windows::Forms::OpenFileDialog());
				this->saveModelDialog = (gcnew System::Windows::Forms::SaveFileDialog());
				this->hkoglPanelControl1 = (gcnew HKOGLPanel::HKOGLPanelControl());
				this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
				this->hkoglPanelControl2 = (gcnew HKOGLPanel::HKOGLPanelControl());
				this->button1 = (gcnew System::Windows::Forms::Button());
				this->button2 = (gcnew System::Windows::Forms::Button());
				this->textBox1 = (gcnew System::Windows::Forms::TextBox());
				this->button3 = (gcnew System::Windows::Forms::Button());
				this->button4 = (gcnew System::Windows::Forms::Button());
				this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
				this->hkoglPanelControl3 = (gcnew HKOGLPanel::HKOGLPanelControl());
				this->button5 = (gcnew System::Windows::Forms::Button());
				this->button6 = (gcnew System::Windows::Forms::Button());
				this->menuStrip1->SuspendLayout();
				this->tableLayoutPanel1->SuspendLayout();
				this->tableLayoutPanel2->SuspendLayout();
				this->SuspendLayout();
				// 
				// menuStrip1
				// 
				this->menuStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
				this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
				this->menuStrip1->Location = System::Drawing::Point(0, 0);
				this->menuStrip1->Name = L"menuStrip1";
				this->menuStrip1->Padding = System::Windows::Forms::Padding(8, 2, 0, 2);
				this->menuStrip1->Size = System::Drawing::Size(1286, 27);
				this->menuStrip1->TabIndex = 1;
				this->menuStrip1->Text = L"menuStrip1";
				this->menuStrip1->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &MyForm::menuStrip1_ItemClicked);
				// 
				// fileToolStripMenuItem
				// 
				this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
					this->loadModelToolStripMenuItem,
						this->saveModelToolStripMenuItem
				});
				this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
				this->fileToolStripMenuItem->Size = System::Drawing::Size(45, 23);
				this->fileToolStripMenuItem->Text = L"File";
				// 
				// loadModelToolStripMenuItem
				// 
				this->loadModelToolStripMenuItem->Name = L"loadModelToolStripMenuItem";
				this->loadModelToolStripMenuItem->Size = System::Drawing::Size(168, 26);
				this->loadModelToolStripMenuItem->Text = L"Load Model";
				this->loadModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::loadModelToolStripMenuItem_Click);
				// 
				// saveModelToolStripMenuItem
				// 
				this->saveModelToolStripMenuItem->Name = L"saveModelToolStripMenuItem";
				this->saveModelToolStripMenuItem->Size = System::Drawing::Size(168, 26);
				this->saveModelToolStripMenuItem->Text = L"Save Model";
				this->saveModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::saveModelToolStripMenuItem_Click);
				// 
				// openModelDialog
				// 
				this->openModelDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::openModelDialog_FileOk);
				// 
				// saveModelDialog
				// 
				this->saveModelDialog->DefaultExt = L"obj";
				this->saveModelDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::saveModelDialog_FileOk);
				// 
				// hkoglPanelControl1
				// 
				hkcoglPanelCameraSetting1->Far = 1000;
				hkcoglPanelCameraSetting1->Fov = 45;
				hkcoglPanelCameraSetting1->Near = -1000;
				hkcoglPanelCameraSetting1->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
				this->hkoglPanelControl1->Camera_Setting = hkcoglPanelCameraSetting1;
				this->hkoglPanelControl1->Dock = System::Windows::Forms::DockStyle::Fill;
				this->hkoglPanelControl1->Location = System::Drawing::Point(4, 4);
				this->hkoglPanelControl1->Margin = System::Windows::Forms::Padding(4);
				this->hkoglPanelControl1->Name = L"hkoglPanelControl1";
				hkcoglPanelPixelFormat1->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
				hkcoglPanelPixelFormat1->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
				hkcoglPanelPixelFormat1->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
				this->hkoglPanelControl1->Pixel_Format = hkcoglPanelPixelFormat1;
				this->hkoglPanelControl1->Size = System::Drawing::Size(691, 608);
				this->hkoglPanelControl1->TabIndex = 2;
				this->hkoglPanelControl1->Load += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl1_Load);
				this->hkoglPanelControl1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::hkoglPanelControl1_Paint);
				this->hkoglPanelControl1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseDown);
				this->hkoglPanelControl1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseMove);
				this->hkoglPanelControl1->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseWheel);
				// 
				// tableLayoutPanel1
				// 
				this->tableLayoutPanel1->ColumnCount = 1;
				this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					61.05442F)));
				this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					38.94558F)));
				this->tableLayoutPanel1->Controls->Add(this->hkoglPanelControl1, 0, 0);
				this->tableLayoutPanel1->Location = System::Drawing::Point(0, 27);
				this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
				this->tableLayoutPanel1->RowCount = 1;
				this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
				this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
				this->tableLayoutPanel1->Size = System::Drawing::Size(699, 616);
				this->tableLayoutPanel1->TabIndex = 3;
				this->tableLayoutPanel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::tableLayoutPanel1_Paint);
				// 
				// hkoglPanelControl2
				// 
				hkcoglPanelCameraSetting2->Far = 1000;
				hkcoglPanelCameraSetting2->Fov = 45;
				hkcoglPanelCameraSetting2->Near = -1000;
				hkcoglPanelCameraSetting2->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
				this->hkoglPanelControl2->Camera_Setting = hkcoglPanelCameraSetting2;
				this->hkoglPanelControl2->Dock = System::Windows::Forms::DockStyle::Fill;
				this->hkoglPanelControl2->Location = System::Drawing::Point(4, 4);
				this->hkoglPanelControl2->Margin = System::Windows::Forms::Padding(4);
				this->hkoglPanelControl2->Name = L"hkoglPanelControl2";
				hkcoglPanelPixelFormat2->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
				hkcoglPanelPixelFormat2->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
				hkcoglPanelPixelFormat2->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
				this->hkoglPanelControl2->Pixel_Format = hkcoglPanelPixelFormat2;
				this->hkoglPanelControl2->Size = System::Drawing::Size(475, 322);
				this->hkoglPanelControl2->TabIndex = 4;
				this->hkoglPanelControl2->Load += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl2_Load);
				this->hkoglPanelControl2->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::hkoglPanelControl2_Paint);
				this->hkoglPanelControl2->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl2_MouseWheel);
				// 
				// button1
				// 
				this->button1->Location = System::Drawing::Point(705, 35);
				this->button1->Name = L"button1";
				this->button1->Size = System::Drawing::Size(92, 47);
				this->button1->TabIndex = 4;
				this->button1->Text = L"loadPatch";
				this->button1->UseVisualStyleBackColor = true;
				this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
				// 
				// button2
				// 
				this->button2->Location = System::Drawing::Point(705, 88);
				this->button2->Name = L"button2";
				this->button2->Size = System::Drawing::Size(92, 46);
				this->button2->TabIndex = 5;
				this->button2->Text = L"line";
				this->button2->UseVisualStyleBackColor = true;
				this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
				// 
				// textBox1
				// 
				this->textBox1->Location = System::Drawing::Point(705, 155);
				this->textBox1->Name = L"textBox1";
				this->textBox1->Size = System::Drawing::Size(92, 25);
				this->textBox1->TabIndex = 6;
				this->textBox1->TextChanged += gcnew System::EventHandler(this, &MyForm::textBox1_TextChanged);
				// 
				// button3
				// 
				this->button3->Location = System::Drawing::Point(705, 196);
				this->button3->Name = L"button3";
				this->button3->Size = System::Drawing::Size(92, 38);
				this->button3->TabIndex = 7;
				this->button3->Text = L"uvAngleUp";
				this->button3->UseVisualStyleBackColor = true;
				this->button3->Click += gcnew System::EventHandler(this, &MyForm::button3_Click);
				// 
				// button4
				// 
				this->button4->Location = System::Drawing::Point(705, 240);
				this->button4->Name = L"button4";
				this->button4->Size = System::Drawing::Size(92, 40);
				this->button4->TabIndex = 8;
				this->button4->Text = L"uvAngleDown";
				this->button4->UseVisualStyleBackColor = true;
				this->button4->Click += gcnew System::EventHandler(this, &MyForm::button4_Click);
				// 
				// tableLayoutPanel2
				// 
				this->tableLayoutPanel2->ColumnCount = 1;
				this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					50)));
				this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
					50)));
				this->tableLayoutPanel2->Controls->Add(this->hkoglPanelControl3, 0, 1);
				this->tableLayoutPanel2->Controls->Add(this->hkoglPanelControl2, 0, 0);
				this->tableLayoutPanel2->Location = System::Drawing::Point(803, 27);
				this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
				this->tableLayoutPanel2->RowCount = 2;
				this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 53.59477F)));
				this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 46.40523F)));
				this->tableLayoutPanel2->Size = System::Drawing::Size(483, 616);
				this->tableLayoutPanel2->TabIndex = 9;
				// 
				// hkoglPanelControl3
				// 
				hkcoglPanelCameraSetting3->Far = 1000;
				hkcoglPanelCameraSetting3->Fov = 45;
				hkcoglPanelCameraSetting3->Near = -1000;
				hkcoglPanelCameraSetting3->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
				this->hkoglPanelControl3->Camera_Setting = hkcoglPanelCameraSetting3;
				this->hkoglPanelControl3->Dock = System::Windows::Forms::DockStyle::Fill;
				this->hkoglPanelControl3->Location = System::Drawing::Point(4, 334);
				this->hkoglPanelControl3->Margin = System::Windows::Forms::Padding(4);
				this->hkoglPanelControl3->Name = L"hkoglPanelControl3";
				hkcoglPanelPixelFormat3->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
				hkcoglPanelPixelFormat3->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
				hkcoglPanelPixelFormat3->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
				this->hkoglPanelControl3->Pixel_Format = hkcoglPanelPixelFormat3;
				this->hkoglPanelControl3->Size = System::Drawing::Size(475, 278);
				this->hkoglPanelControl3->TabIndex = 5;
				this->hkoglPanelControl3->Load += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl3_Load);
				this->hkoglPanelControl3->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::hkoglPanelControl3_Paint);
				// 
				// button5
				// 
				this->button5->Location = System::Drawing::Point(705, 286);
				this->button5->Name = L"button5";
				this->button5->Size = System::Drawing::Size(92, 38);
				this->button5->TabIndex = 10;
				this->button5->Text = L"changeImg";
				this->button5->UseVisualStyleBackColor = true;
				this->button5->Click += gcnew System::EventHandler(this, &MyForm::button5_Click);
				// 
				// button6
				// 
				this->button6->Location = System::Drawing::Point(705, 330);
				this->button6->Name = L"button6";
				this->button6->Size = System::Drawing::Size(92, 40);
				this->button6->TabIndex = 11;
				this->button6->Text = L"changeDesign";
				this->button6->UseVisualStyleBackColor = true;
				this->button6->Click += gcnew System::EventHandler(this, &MyForm::button6_Click);
				// 
				// MyForm
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(1286, 645);
				this->Controls->Add(this->button6);
				this->Controls->Add(this->button5);
				this->Controls->Add(this->tableLayoutPanel2);
				this->Controls->Add(this->button4);
				this->Controls->Add(this->button3);
				this->Controls->Add(this->textBox1);
				this->Controls->Add(this->button2);
				this->Controls->Add(this->button1);
				this->Controls->Add(this->tableLayoutPanel1);
				this->Controls->Add(this->menuStrip1);
				this->MainMenuStrip = this->menuStrip1;
				this->Margin = System::Windows::Forms::Padding(4);
				this->Name = L"MyForm";
				this->Text = L"OpenMesh_EX";
				this->menuStrip1->ResumeLayout(false);
				this->menuStrip1->PerformLayout();
				this->tableLayoutPanel1->ResumeLayout(false);
				this->tableLayoutPanel2->ResumeLayout(false);
				this->ResumeLayout(false);
				this->PerformLayout();

			}
#pragma endregion

			//init
		private: System::Void hkoglPanelControl1_Load(System::Object^  sender, System::EventArgs^  e) {

			SCR_WIDTH = this->hkoglPanelControl1->Size.Width;
			SCR_HEIGHT = this->hkoglPanelControl1->Size.Height;



			glewExperimental = GL_TRUE; //置於glewInit()之前
			if (glewInit()) {
				std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
				exit(EXIT_FAILURE);
			}
			else std::cout << "initialize GLEW success" << std::endl;//c error

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			/*
			*/
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			//VAO
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);

			glGenBuffers(1, &VBO);
			glGenBuffers(1, &meshVBO);

			program = LoadShaders(meshShaders);//讀取shader

			glUseProgram(program);//uniform參數數值前必須先use shader
			mvpID = glGetUniformLocation(program, "MVP");

			ColorID = glGetUniformLocation(program, "Color");


			ViewMatrix = glm::lookAt(
				glm::vec3(0, 5, 5), // Camera is at (0,10,25), in World Space
				glm::vec3(0, 0, 0), // and looks at the origin
				glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
			);


			for (int i = 0; i < 3; i++) {
				std::string ProjectName;
				if(i == 0) ProjectName = "circle2.jpg";
				else if (i == 1) ProjectName = "circle3.jpg";
				else if (i == 2) ProjectName = "circle4.jpg";

				TextureData tdata = Load_png((ProjectName).c_str(), true);

				//Generate empty texture
				glGenTextures(1, &checkerBoardImg[i]);


				glBindTexture(GL_TEXTURE_2D, checkerBoardImg[i]);


				//Do texture setting
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glBindTexture(GL_TEXTURE_2D, 0);
				//-----------------------------------------------------------
				glUniform1i(glGetUniformLocation(program, "sprite"), 0);
			}
			


			glClearColor(0.0, 0.0, 0.0, 1);//black screen

			//use frameBuffer to store face id
			ShaderInfo shaderframe[] = {
			{ GL_VERTEX_SHADER, "framebuffer.vp" },//vertex shader
			{ GL_FRAGMENT_SHADER, "framebuffer.fp" },//fragment shader
			{ GL_NONE, NULL } };
			programFrame = LoadShaders(shaderframe);
			glUseProgram(programFrame);//uniform參數數值前必須先use shader

			// screen quad VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);

			glUseProgram(programFrame);
			glUniform1i(glGetUniformLocation(programFrame, "screenTexture"), 0);

			// framebuffer configuration
			// -------------------------
			glGenFramebuffers(1, &framebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			// create a color attachment texture

			glGenTextures(1, &textureColorbuffer);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
			//GL_RGBA32F for store value > 1
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, hkoglPanelControl1->Width, hkoglPanelControl1->Height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);


			GLenum dr[2] = { GL_COLOR_ATTACHMENT0 ,GL_DEPTH_ATTACHMENT };
			glDrawBuffers(2, dr);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				cout << "Framebuffer is not complete!" << endl;
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
				cout << "Framebuffer is not complete attach!" << endl;

			//bind to normal
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}
				 //display
		private: System::Void hkoglPanelControl1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
			//std::cout << "refresh" << std::endl;
			SCR_WIDTH = this->hkoglPanelControl1->Size.Width;
			SCR_HEIGHT = this->hkoglPanelControl1->Size.Height;

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
			glClearDepth(1.0);

			glClearColor(0.0, 0.0, 0.0, 1.0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < objptr; i++) {
				if (mesh != NULL) {
					//std::cout << "refresh mesh not null" << std::endl;
				
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					glBufferData(GL_ARRAY_BUFFER, (vertices[i]).size() * sizeof(double), &(vertices[i][0]), GL_STATIC_DRAW);

				
					glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
					glBufferData(GL_ARRAY_BUFFER, (meshUV[i]).size() * sizeof(double), &(meshUV[i][0]), GL_STATIC_DRAW);
				}
				glEnable(GL_DEPTH_TEST);

				glBindVertexArray(VAO);
				glUseProgram(program);//uniform參數數值前必須先use shader

				float horizonAngle = DOR(eyeAngleX);
				float verticleAngle = DOR(eyeAngleY);
				ViewMatrix = lookAt(
					glm::vec3(eyedistance*cos(horizonAngle)*cos(verticleAngle), eyedistance*sin(verticleAngle), eyedistance*sin(horizonAngle)*cos(verticleAngle)),
					glm::vec3(0, 0, 0), // and looks at the origin
					glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
				);

				mat4 Model = translate(translateX, translateY, 0.0f);

				MVP = Model * Projection * ViewMatrix;//translate via screen viewport, so model last
				//MVP = Projection * ViewMatrix * Model;

				glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				// 1rst attribute buffer : vertices
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0,				//location
					3,				//vec3
					GL_DOUBLE,			//type
					GL_FALSE,			//not normalized

					0,		//strip
					0);//buffer offset
				glBindBuffer(GL_ARRAY_BUFFER, meshVBO); // this attribute comes from a different vertex buffer
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 0, (void*)0); // 從instanceVBO傳入的
				glBindBuffer(GL_ARRAY_BUFFER, 0);


				if (isLoad) {//draw faceID to frameBuffer
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glm::vec3 color = glm::vec3(-1.0, 0.0, 0.0);
					glUniform3fv(ColorID, 1, &color[0]);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, checkerBoardImg[imgptr[design][i]]);
					glDrawArrays(GL_TRIANGLES, 0, face[i] * 3);
				}
				glClearColor(1.0, 1.0, 1.0, 1.0);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				if (isLoad) {//draw to screen
					//line
					if (line) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						glm::vec3 color = glm::vec3(0.0, 0.0, 0.0);
						glUniform3fv(ColorID, 1, &color[0]);
						glDrawArrays(GL_TRIANGLES, 0, face[i] * 3);
					}
					//face
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glm::vec3 color = glm::vec3(1.0, 0.85, 0.5);
					glUniform3fv(ColorID, 1, &color[0]);
					glDrawArrays(GL_TRIANGLES, 0, face[i] * 3);
				
				}
			}
			
			

			//----------------------------
			//畫出所選到的面(紅色)
			//---------------------------
			//glDeleteVertexArrays(1, &VAO);
			//glDeleteBuffers(1, &VBO);
			if (!wholeModel) { // 直接選片段的
				if (patch != NULL) {
					//std::cout << "refresh mesh not null" << std::endl;
					//mesh->Render_SolidWireframe();
					//glGenBuffers(1, &VBO);
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					//std::cout << verticesPatch[0] << std::endl;
					//std::cout << verticesPatch.size() << std::endl;
					glBufferData(GL_ARRAY_BUFFER, verticesPatch.size() * sizeof(double), &verticesPatch[0], GL_STATIC_DRAW);

					//glGenBuffers(1, &meshVBO);
					glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
					glBufferData(GL_ARRAY_BUFFER, patchUV.size() * sizeof(double), &patchUV[0], GL_STATIC_DRAW);

					//glBufferData(GL_ARRAY_BUFFER, verticesPatch.size() * sizeof(double) + patchUV.size() * sizeof(double), &verticesPatch[0], GL_STATIC_DRAW);
					//glBufferSubData(GL_ARRAY_BUFFER, verticesPatch.size() * sizeof(double), patchUV.size() * sizeof(double), &patchUV[0]);
					
					//printf("change the VBO to patch...\n");

					//debug1，把VAO重訂的部分拉上來
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0,				//location
						3,				//vec3
						GL_DOUBLE,			//type
						GL_FALSE,			//not normalized
						0,				//strip
						0);//buffer offset
					glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
					glEnableVertexAttribArray(1);
					/*glVertexAttribPointer(1,				//location
						2,				//vec3
						GL_DOUBLE,			//type
						GL_FALSE,			//not normalized
						0,				//strip
						(void *)(verticesPatch.size() * sizeof(double)));//buffer offset*/
					glVertexAttribPointer(1,				//location
						2,				//vec3
						GL_DOUBLE,			//type
						GL_FALSE,			//not normalized
						0,				//strip
						(void *)(0));
					//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				}

				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				//glBindVertexArray(VAO);
				glUseProgram(program);//uniform參數數值前必須先use shader
				glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);

				if (facesid2.size() != 0) {
					//printf("draw red patch...\n");
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glm::vec3 color = glm::vec3(1.0, 0.0, 0.5);
					glUniform3fv(ColorID, 1, &color[0]);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, checkerBoardImg[img]);
					glDrawArrays(GL_TRIANGLES, 0, facePatch * 3);
					//glDrawArrays(GL_TRIANGLES, 0, facePatch * 3);
					//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					//color = glm::vec3(0.0, 0.0, 0.0);
					//glUniform3fv(ColorID, 1, &color[0]);
					//glDrawArrays(GL_TRIANGLES, 0, face * 3);

				}

				if (selectedVertices.size() != 0) {
					//printf("tuck data into point shader...\n");
					//glGenBuffers(1, &VBO);
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					//std::cout << verticesPatch[0] << std::endl;
					//std::cout << verticesPatch.size() << std::endl;
					glBufferData(GL_ARRAY_BUFFER, selectedVertices.size() * sizeof(double), &selectedVertices[0], GL_STATIC_DRAW);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0,				//location
						3,				//vec3
						GL_DOUBLE,			//type
						GL_FALSE,			//not normalized
						0,				//strip
						0);//buffer offset
					glDisable(GL_DEPTH_TEST);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glm::vec3 color = glm::vec3(1.0, 0.0, 1.0);
					glUniform3fv(ColorID, 1, &color[0]);
					glPointSize(10.0);
					glDrawArrays(GL_POINTS, 0, 1);
				}
			}
			else { // 整塊拿去解
				if (patchUV.size() != 0) {
					//glGenBuffers(1, &VBO);
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					//std::cout << verticesPatch[0] << std::endl;
					//std::cout << verticesPatch.size() << std::endl;
					glBufferData(GL_ARRAY_BUFFER, vertices[objptr - 1].size() * sizeof(double), &vertices[objptr - 1][0], GL_STATIC_DRAW);

					//glGenBuffers(1, &meshVBO);
					glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
					glBufferData(GL_ARRAY_BUFFER, patchUV.size() * sizeof(double), &patchUV[0], GL_STATIC_DRAW);

					//debug1，把VAO重訂的部分拉上來
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0,				//location
						3,				//vec3
						GL_DOUBLE,			//type
						GL_FALSE,			//not normalized
						0,				//strip
						0);//buffer offset
					glBindBuffer(GL_ARRAY_BUFFER, meshVBO); // this attribute comes from a different vertex buffer
					glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 0, 0); // 從instanceVBO傳入的
					glEnableVertexAttribArray(1);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					glEnable(GL_DEPTH_TEST);
					glDepthFunc(GL_LEQUAL);
					glUseProgram(program);//uniform參數數值前必須先use shader
					glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);
					glBindBuffer(GL_ARRAY_BUFFER, VBO);

					printf("draw whole patch...\n");
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glm::vec3 color = glm::vec3(1.0, 0.0, 0.5);
					glUniform3fv(ColorID, 1, &color[0]);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, checkerBoardImg[img]);
					glDrawArrays(GL_TRIANGLES, 0, face[objptr - 1] * 3);
				}
			}
		}

		//mouseClick
		private: System::Void hkoglPanelControl1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e){
			if (e->Button == System::Windows::Forms::MouseButtons::Left){
				//leftClick	
				//record mouse position for drag event
				prevMouseX = e->X;
				prevMouseY = e->Y;
				//cout << "click : " << e->X << "," << e->Y << endl;
				std::vector<double> mousePosition;
				double mouseOnScreenX = (double)e->X * 2 / (double)SCR_WIDTH - 1;
				double mouseOnScreenY = (double)(SCR_HEIGHT - e->Y) * 2 / (double)SCR_HEIGHT - 1;
				mousePosition.push_back(mouseOnScreenX);
				mousePosition.push_back(mouseOnScreenY);
				//cout << "click SCR : " << mouseOnScreenX << "," << mouseOnScreenY << endl;
				//read face
				glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glReadPixels(e->X, hkoglPanelControl1->Height - e->Y, 1, 1, GL_RGBA, GL_FLOAT, &pixel);
				cout << "face id : " << pixel.r << endl;

				selectedVertices.clear();
				mesh->findNearestVert(*mesh, mousePosition, pixel.r - 1, selectedVertices, MVP , eyedistance);

				//printf("mouse x = %d mouse y = %d\n", e->X, hkoglPanelControl1->Height - e->Y);
				if (isLoad) {
					//detect same face already stored
					for (int i = 0; i < facesid2.size(); i++) {
						if (facesid2[i] == int(pixel.r) - 1) break;
						if (pixel.r != 0 && ( facesid2[i] > int(pixel.r) - 1 || i == facesid2.size()-1 ) ) { 
							//when id > curID mean no repeat 'cause vector sorted
							facesid2.push_back(int(pixel.r) - 1);
							break;
						}
					}
					//first value
					if (pixel.r != 0 && facesid2.size() == 0) facesid2.push_back(int(pixel.r) - 1);
					std::sort(facesid2.begin(), facesid2.end());

					cout << "selected faceID: ";
					for (int i = 0; i < facesid2.size(); i++) cout << facesid2[i] << " ";
					cout << endl;
					//cout << endl << "selected face count : " << facesid2.size() << endl;
				}

				//----------------------------------
				//load face into vector
				//----------------------------------
				if (facesid2.size() != 0) {
					//del old mesh on screen
					if (patch != NULL) delete patch;
					patch = new Tri_Mesh;
					//clear vertices and face to null
					verticesPatch.clear();
					patchUV.clear();
					//verticesPatch.resize(0);
					//std::cout << "verticesPatch.resize(0)" << verticesPatch.size() << std::endl;
					facePatch = 0;
					// ReadFile(filename, patch); // change form here
					//patch->loadToBufferPatch(verticesPatch, facePatch, facesid, facesptr);
					mesh->loadToBufferPatch(verticesPatch, facePatch, facesid2, *patch);
					patch->getUV(patchUV, *patch, rotateAngle);
					//std::cout << "facePatch" << facePatch << std::endl;
					//std::cout << "verticesPatch.size()" << verticesPatch.size() << std::endl;
				}

				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

			}
			if (e->Button == System::Windows::Forms::MouseButtons::Middle) {
				//record mouse position for drag event
				prevMouseX = e->X;
				prevMouseY = e->Y;
			}
			hkoglPanelControl1->Invalidate();
			hkoglPanelControl2->Invalidate();
		}

		//mouseDrag
		private: System::Void hkoglPanelControl1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e){
			if (e->Button == System::Windows::Forms::MouseButtons::Left){
				//std::cout << "left" << std::endl;
				//eyeAngleX += (e->X - prevMouseX)*0.05;
				//eyeAngleY += (e->Y - prevMouseY)*0.05;
				eyeAngleX += (e->X - prevMouseX)*0.2;
				eyeAngleY += (e->Y - prevMouseY)*0.2;
				//record mouse position for drag event
				prevMouseX = e->X;
				prevMouseY = e->Y;
			}
			if (e->Button == System::Windows::Forms::MouseButtons::Middle){
				//std::cout << "middle" << std::endl;
				translateX += (e->X - prevMouseX)*0.002;
				translateY -= (e->Y - prevMouseY)*0.002;
				//record mouse position for drag event
				prevMouseX = e->X;
				prevMouseY = e->Y;
			}
			hkoglPanelControl1->Invalidate();
		}

		//mouseWheel
		private: System::Void hkoglPanelControl1_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e){
			if (e->Delta < 0) eyedistance += 0.1;
			else{
				eyedistance -= 0.1;
				//if (eyedistance < 0.4) eyedistance = 0.4;
				//std::cout << "wheel up, distance : "  << eyedistance << std::endl;
			}
			hkoglPanelControl1->Invalidate();
		}

		//click "openModel"
		private: System::Void loadModelToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e){
			openModelDialog->Filter = "Model(*.obj)|*obj";
			openModelDialog->Multiselect = false;
			openModelDialog->ShowDialog();
		}

		//check openModel dialog
		private: System::Void openModelDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e){
			std::string filename;
			MarshalString(openModelDialog->FileName, filename);
			//del old mesh on screen
			if (mesh == NULL) {
				mesh = new Tri_Mesh;
				for (int i = 0 ; i <  OBJ_NUM ; i++) {
					vertices[i].clear();
					face[i] = 0;
					meshUV[i].clear();
				}
			}
			else if (mesh != NULL) delete mesh;
			mesh = new Tri_Mesh;

			if (ReadFile(filename, mesh)) std::cout << filename << std::endl;
			isLoad = true;

			mesh->loadToBuffer(*mesh, vertices[objptr], face[objptr], meshUV[objptr]);
			std::cout << "meshUV.size() : " << meshUV[objptr].size() << "vertices.size()" << vertices[objptr].size() << endl;
			std::cout << "face" << face[objptr] << std::endl;
			objptr++;
			hkoglPanelControl1->Invalidate();
			hkoglPanelControl2->Invalidate();
		}

		//saveObj menu open
		private: System::Void saveModelToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e){
			saveModelDialog->Filter = "Model(*.obj)|*obj";
			saveModelDialog->ShowDialog();
		}

		//check saveObj in dialog 
		private: System::Void saveModelDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e){
			std::string filename;
			MarshalString(saveModelDialog->FileName, filename);
			if (SaveFile(filename, mesh)) std::cout << filename << std::endl;
		}
		private: System::Void hkoglPanelControl2_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {

			glClearColor(0.5, 0.5, 0.5, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			std::cout << "un-paint uv ， facesid2.size() = " << facesid2.size() << endl;
			std::cout << "facePatch = " << facePatch << endl;
			std::cout << "patchUV.size() = " << patchUV.size() << endl;
			/*for (int i = 0; i < patchUV.size(); i+=2) {
				std::cout << "s = " << patchUV[i] << " " << "t = " << patchUV[i + 1] << endl;
			}*/
			if (isLoad) { // 把有uv的模型load進來時
				std::cout << "Drawing the uv texcoord(from obj with uv)..." << endl;
				//glGenBuffers(1, &VBOuv);
				glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
				glBufferData(GL_ARRAY_BUFFER, meshUV[objptr-1].size() * sizeof(double), &meshUV[objptr-1][0], GL_STATIC_DRAW);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(test), test, GL_STATIC_DRAW);

				glBindVertexArray(VAOuv);
				glUseProgram(programUV);

				float horizonAngle = DOR(eyeAngleX);
				float verticleAngle = DOR(eyeAngleY);
				ViewMatrixUV = lookAt(
					glm::vec3(eyedistanceuv*cos(horizonAngle)*cos(verticleAngle), eyedistanceuv*sin(verticleAngle), eyedistanceuv*sin(horizonAngle)*cos(verticleAngle)),
					glm::vec3(0, 0, 0), // and looks at the origin
					glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
				);

				mat4 Modeluv = glm::translate(translateX, translateY, 0.0f);
				//MVPuv = Modeluv * ProjectionUV * ViewMatrixUV;
				MVPuv = ProjectionUV * ViewMatrixUV * Modeluv;
				glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVPuv[0][0]);

				glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
				glEnableVertexAttribArray(0);
				//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(double), (void*)0);
				glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);

				glUseProgram(programUV);

				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				//glDrawArrays(GL_TRIANGLES, 0, facePatch * 3);
				glDrawArrays(GL_TRIANGLES, 0, face[objptr-1] * 3);
			}
			if (wholeModel) { // 拿整塊模型去解時
				std::cout << "Drawing the uv texcoord(wholeModel)..." << endl;
				//glGenBuffers(1, &VBOuv);
				glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
				glBufferData(GL_ARRAY_BUFFER, patchUV.size() * sizeof(double), &patchUV[0], GL_STATIC_DRAW);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(test), test, GL_STATIC_DRAW);

				glBindVertexArray(VAOuv);
				glUseProgram(programUV);

				float horizonAngle = DOR(eyeAngleX);
				float verticleAngle = DOR(eyeAngleY);
				ViewMatrixUV = lookAt(
					glm::vec3(eyedistanceuv*cos(horizonAngle)*cos(verticleAngle), eyedistanceuv*sin(verticleAngle), eyedistanceuv*sin(horizonAngle)*cos(verticleAngle)),
					glm::vec3(0, 0, 0), // and looks at the origin
					glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
				);

				mat4 Modeluv = glm::translate(translateX, translateY, 0.0f);
				//MVPuv = Modeluv * ProjectionUV * ViewMatrixUV;
				MVPuv = ProjectionUV * ViewMatrixUV * Modeluv;
				glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVPuv[0][0]);

				glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
				glEnableVertexAttribArray(0);
				//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(double), (void*)0);
				glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);

				glUseProgram(programUV);

				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				//glDrawArrays(GL_TRIANGLES, 0, facePatch * 3);
				glDrawArrays(GL_TRIANGLES, 0, face[objptr-1] * 3);
			}
			if (facesid2.size() != 0) { // 慢慢選點選面時
				std::cout << "Drawing the uv texcoord(selectface)..." << endl;
				//glGenBuffers(1, &VBOuv);
				glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
				glBufferData(GL_ARRAY_BUFFER, patchUV.size() * sizeof(double), &patchUV[0], GL_STATIC_DRAW);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(test), test, GL_STATIC_DRAW);

				glBindVertexArray(VAOuv);
				glUseProgram(programUV);

				float horizonAngle = DOR(eyeAngleX);
				float verticleAngle = DOR(eyeAngleY);
				ViewMatrixUV = lookAt(
					glm::vec3(eyedistanceuv*cos(horizonAngle)*cos(verticleAngle), eyedistanceuv*sin(verticleAngle), eyedistanceuv*sin(horizonAngle)*cos(verticleAngle)),
					glm::vec3(0, 0, 0), // and looks at the origin
					glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
				);

				mat4 Modeluv = glm::translate(translateX, translateY, 0.0f);
				//MVPuv = Modeluv * ProjectionUV * ViewMatrixUV;
				MVPuv = ProjectionUV * ViewMatrixUV * Modeluv;
				glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVPuv[0][0]);

				glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
				glEnableVertexAttribArray(0);
				//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(double), (void*)0);
				glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);

				glUseProgram(programUV);

				//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				//glDrawArrays(GL_TRIANGLES, 0, facePatch * 3);
				glDrawArrays(GL_TRIANGLES, 0, facePatch * 3);
			}


			//hkoglPanelControl2->Invalidate();
		}
		private: System::Void hkoglPanelControl2_Load(System::Object^  sender, System::EventArgs^  e) {

			glewExperimental = GL_TRUE; //置於glewInit()之前
			if (glewInit()) {
				std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
				exit(EXIT_FAILURE);
			}
			else std::cout << "initialize GLEW success" << std::endl;//c error

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glDisable(GL_CULL_FACE);

			ProjectionUV = glm::perspective(80.0f, 4.0f / 3.0f, 0.1f, 1000.0f);

			glGenVertexArrays(1, &VAOuv);
			glBindVertexArray(VAOuv);

			glGenBuffers(1, &VBOuv);

			ShaderInfo UVShader[] = {
			{ GL_VERTEX_SHADER, "uv.vp" },//vertex shader
			{ GL_FRAGMENT_SHADER, "uv.fp" },//fragment shader
			{ GL_NONE, NULL } };
			programUV = LoadShaders(UVShader);//讀取shader

			glUseProgram(programUV);//uniform參數數值前必須先use shader


			mvpID = glGetUniformLocation(programUV, "MVP");
			//glGenVertexArrays(1, &VAOuv);
			//glGenBuffers(1, &VBOuv);

			//glBindVertexArray(VAOuv);

			//glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
			//glBufferData(GL_ARRAY_BUFFER, patchUV.size() * sizeof(double), &patchUV[0], GL_STATIC_DRAW);

			// position attribute
			//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			//glEnableVertexAttribArray(0);

			//glUseProgram(programUV);
		}
		private: System::Void hkoglPanelControl2_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			if (e->Delta < 0) {
				eyedistanceuv += 0.1;
				std::cout << "wheel down, distance : " << eyedistanceuv << std::endl;
			}
			else {
				eyedistanceuv -= 0.1;
				if (eyedistanceuv < 0.4) eyedistanceuv = 0.4;
				std::cout << "wheel up, distance : " << eyedistanceuv << std::endl;
			}
			hkoglPanelControl1->Invalidate();
		}
	private: System::Void tableLayoutPanel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {}
	private: System::Void menuStrip1_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {}
	// button1 is for 解整塊patch用
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		//----------------------------------
			//將該model的所有面直接解uv座標
			//----------------------------------
		if (isLoad) {

			patchUV.clear();

			mesh->getUV(patchUV, *mesh, rotateAngle);

			wholeModel = true;
		}

		hkoglPanelControl1->Invalidate();
		hkoglPanelControl2->Invalidate();
	}
	private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		System::String^ degree = textBox1->Text;
		//rotateAngle = int::Parse(degree);
		printf("enter textbox1...\n");
		printf("rotateAngle = %d\n", rotateAngle);
	}
	// button3 and button 4 is for 旋轉uv座標
	private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
		if (isLoad) {

			patchUV.clear();
			rotateAngle += 90;
			mesh->getUV(patchUV, *mesh, rotateAngle);

			wholeModel = true;
		}

		hkoglPanelControl1->Invalidate();
		hkoglPanelControl2->Invalidate();
		
	}
	private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {
		if (isLoad) {

			patchUV.clear();
			rotateAngle -= 90;
			mesh->getUV(patchUV, *mesh, rotateAngle);

			wholeModel = true;
		}

		hkoglPanelControl1->Invalidate();
		hkoglPanelControl2->Invalidate();
	}
	// button2 is for 模型上的線on/off
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
		if (line) line = false;
		else line = true;
	}
	private: System::Void hkoglPanelControl3_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);




		glUseProgram(programImg);
		glBindVertexArray(VAOi);

		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, checkerBoardImg2[img]);

		glUseProgram(programImg);
		glBindVertexArray(VAOi);
		printf("print the image...\n");
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	private: System::Void hkoglPanelControl3_Load(System::Object^  sender, System::EventArgs^  e) {

		glewExperimental = GL_TRUE; //置於glewInit()之前
		if (glewInit()) {
			std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
			exit(EXIT_FAILURE);
		}
		else std::cout << "initialize GLEW success" << std::endl;//c error

		ShaderInfo imgShader[] = {
		{ GL_VERTEX_SHADER, "img.vp" },//vertex shader
		{ GL_FRAGMENT_SHADER, "img.fp" },//fragment shader
		{ GL_NONE, NULL } };
		programImg = LoadShaders(imgShader);//讀取shader

		glUseProgram(programImg);//uniform參數數值前必須先use shader

		glGenVertexArrays(1, &VAOi);
		glGenBuffers(1, &VBOi);
		glBindVertexArray(VAOi);

		glBindBuffer(GL_ARRAY_BUFFER, VBOi);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices1), quadVertices1, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture coord attribute(for image1)
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glUseProgram(programImg);

		//Generate empty texture

		for (int i = 0; i < 3; i++) {

			//----------------------------------------------------
			std::string ProjectName;
			if (i == 0) ProjectName = "circle2.jpg";
			else if (i == 1) ProjectName = "circle3.jpg";
			else if (i == 2) ProjectName = "circle4.jpg";
			TextureData tdata = Load_png((ProjectName).c_str());

			glGenTextures(1, &checkerBoardImg2[i]);

			glBindTexture(GL_TEXTURE_2D, checkerBoardImg2[i]);

			//Do texture setting
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			//-----------------------------------------------------------
			glUniform1i(glGetUniformLocation(programImg, "sprite"), 0);

		}
	}
	private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {
		if (img != 2) img++;
		else img = 0;
		hkoglPanelControl1->Invalidate();
		hkoglPanelControl3->Invalidate();

	}
	private: System::Void button6_Click(System::Object^  sender, System::EventArgs^  e) {
		if (design != 3) design++;
		else design = 0;
		hkoglPanelControl1->Invalidate();
		hkoglPanelControl3->Invalidate();
	}
};
}
