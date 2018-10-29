#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include <learnopengl/filesystem.h>

#include <iostream>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Initial camera position
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool firstMouse = true;

// Initial cursor position
float lastX = SCR_WIDTH / 2.0f; 
float lastY = SCR_HEIGHT / 2.0f;

// Frame timers
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos(0.0f, -0.0f, 0.5f);
float lightradius = 5.0f;
bool fulllight = false;

// Jump height
bool jumping = false;
float jumpFrame = 0.0f;

// Door animation
float animFrame = 0.0f;
bool doorOpen = false;
bool doorOpening = false;
bool doorClosing = false;

// Projection type
bool ortho = false;

// Key statuses
bool pheld = false;
bool kheld = false;
bool lheld = false;
bool oheld = false;
bool fheld = false;
bool zoominheld = false;
bool zoomoutheld = false;

// Lantern state
bool holdingLantern = false;

int main()
{
   	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader lightingShader("maplighting.vs", "flatlighting.fs");
	Shader lampShader("lamp.vs", "lamp.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	float faceTexture[36 * 8];	
	
	int i;	
	for (i = 0; i < 36 * 8; i++)
	{
		faceTexture[i] = vertices[i];
	}

	for (i = 7; i < 48; i = i + 8)
	{
		if (faceTexture[i] == 0.0f)
			faceTexture[i] = 0.5f;
	}

	for (i = 55; i < 36 * 8; i = i + 8)
	{
		if (faceTexture[i] == 0.0f)
			faceTexture[i] = 0.5f;
		if (faceTexture[i] == 1.0f)
			faceTexture[i] = 0.0f;
	}

	// ================= CUBE ===============
	// Configure the cube's VAO & VBO
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	// ================= LIGHT ==================
	// Configure the light's VAO & VBO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ================== FACE ===================
	// Configure the VAO & VBO for a cube with a forward-facing texture
	unsigned int faceVAO, faceVBO;
	glGenVertexArrays(1, &faceVAO);
	glGenBuffers(1, &faceVBO);
	glBindVertexArray(faceVAO);

	glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(faceTexture), faceTexture, GL_STATIC_DRAW);
	
	glBindVertexArray(faceVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);

	// ==================== LOADING TEXTURES =======================
	unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/container2.png").c_str());
	unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/container2_specular.png").c_str());
	unsigned int marble = loadTexture(FileSystem::getPath("resources/textures/marble2.jpg").c_str());
	unsigned int nothing = loadTexture(FileSystem::getPath("resources/textures/nothing.png").c_str());
	unsigned int booface = loadTexture(FileSystem::getPath("resources/textures/booface.jpg").c_str());
	unsigned int white = loadTexture(FileSystem::getPath("resources/textures/white.png").c_str());
	unsigned int bricks = loadTexture(FileSystem::getPath("resources/textures/brickwall.jpg").c_str());
	unsigned int painting = loadTexture(FileSystem::getPath("resources/textures/ghost_clouds.jpg").c_str());
	unsigned int wood = loadTexture(FileSystem::getPath("resources/textures/wood2.jpg").c_str());
	unsigned int door = loadTexture(FileSystem::getPath("resources/textures/door2.jpg").c_str());
	unsigned int corridor = loadTexture(FileSystem::getPath("resources/textures/corridor2.png").c_str());

	// Get time at start of render loop
	float startFrame = glfwGetTime();

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		// activate lighting shader
		lightingShader.use();
		
		// set light and camera positions
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", camera.Position);

		// set light source radius
		lightingShader.setFloat("light.falloff", lightradius);

		// set lighting brightness
		if (!fulllight)
		{
			lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
			lightingShader.setVec3("light.diffuse", 0.7f, 0.7f, 0.7f);
			lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		}
		else
		{
			lightingShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("light.diffuse", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("light.specular", 0.0f, 0.0f, 0.0f);
		}

		glm::mat4 projection, view;
		
		// view/projection transformations
		if (!ortho)
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		else
			projection = glm::ortho( -2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);

		if (!ortho)
			view = camera.GetViewMatrix();
		else
			view = glm::lookAt(
				glm::vec3(0.0f, 1.0f, 2.9f),
				glm::vec3(0,1,0),
				glm::vec3(0,1,0)
			);

		// process jump
		if (jumping)
		{
			glm::vec3 jumpHeight = glm::vec3(0.0f, glm::sin(3 * (jumpFrame - currentFrame)), 0.0f);

			if (3 * (jumpFrame - currentFrame) <= -M_PI)
			{
				jumpHeight.y = 0.0f;
				jumping = false;
			}

			view = glm::translate(view, jumpHeight);
		}

		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// ========== GHOST ===========
		// initialise ghost transformation
		float ghostBob = 0.2f * glm::sin(currentFrame * 4);
		glm::mat4 ghostTransform = glm::rotate(glm::mat4(), startFrame - currentFrame, glm::vec3(0.0f, 1.0f, 0.0f));
		ghostTransform = glm::translate(ghostTransform, glm::vec3(2.0f, 1.2f + ghostBob, 0.0f));
		ghostTransform = glm::scale(ghostTransform, glm::vec3(0.7f, 0.7f, 0.7f));
		
		glm::mat4 model = ghostTransform;

		// set material properties
		lightingShader.setVec3("material.ambient", 0.3f, 0.3f, 0.3f);
		lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.setFloat("material.shininess", 32.0f);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, booface);

		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, nothing);

		// render the spooky ghost
		glBindVertexArray(faceVAO);
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// render the spooky ghost's spooky arms
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, white);

		model = ghostTransform;
		model = glm::scale(model, glm::vec3(1.6f, 0.3f, 0.3f));

		lightingShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// render the spooky ghost's spooky tail
		model = ghostTransform;
		model = glm::translate(model, glm::vec3(0.0f, -0.35f, -0.151f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 1.3f));

		lightingShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// ============ FLOOR ==============
		// set material properties
		lightingShader.setVec3("material.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.setFloat("material.shininess", 32.0f);

		// Draw the floor
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, marble);

		/*glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, nothing);*/

		model = glm::mat4();

		glm::mat4 floorTransforms[] = {
			glm::translate(model, glm::vec3(1.5f, -1.0f, 1.5f)),
			glm::translate(model, glm::vec3(1.5f, -1.0f, -1.5f)),
			glm::translate(model, glm::vec3(-1.5f, -1.0f, 1.5f)),
			glm::translate(model, glm::vec3(-1.5f, -1.0f, -1.5f))
		};

		lightingShader.setVec3("material.specular", glm::vec3(0.7f,0.7f,0.7f));
		lightingShader.setFloat("material.shininess", 100.0f);

		for (i = 0; i < 4; i++)
		{
			model = floorTransforms[i];
			model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Draw the ceiling
		for (i = 0; i < 4; i++)
		{
			model = floorTransforms[i];
			model = glm::translate(model, glm::vec3(0.0f, 4.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Create walls
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bricks);

		model = glm::mat4();

		glm::mat4 wallTransforms[] = {
			glm::translate(model, glm::vec3(1.5f, 1.0f, -3.0f)),
			glm::translate(model, glm::vec3(-1.5f, 1.0f, -3.0f)),
			glm::translate(model, glm::vec3(1.5f, 1.0f, 3.0f)),
			glm::translate(model, glm::vec3(-1.5f, 1.0f, 3.0f)),
			glm::translate(model, glm::vec3(3.0f, 1.0f, -1.5f)),
			glm::translate(model, glm::vec3(-3.0f, 1.0f, -1.5f)),
			glm::translate(model, glm::vec3(3.0f, 1.0f, 1.5f)),
			glm::translate(model, glm::vec3(-3.0f, 1.0f, 1.5f))		
		};

		lightingShader.setVec3("material.specular", glm::vec3(0.1f, 0.1f, 0.1f));
		lightingShader.setFloat("material.shininess", 20.0f);

		for (i = 0; i < 8; i++)
		{
			model = wallTransforms[i];
			
			if (i >= 4)
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 0.01f));

			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Ghost clouds painting
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, painting);

		model = glm::mat4();

		model = glm::translate(model, glm::vec3(0.0f, 1.2f, -3.0f));
		model = glm::scale(model, glm::vec3(3.0f, 1.65f, 0.02f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Painting Frame
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wood);

		glm::vec3 frameTransforms[] = {
			glm::vec3(0.0f, -0.825f, 0.0f),
			glm::vec3(0.0f, 0.825f, 0.0f),
			glm::vec3(1.55f, 0.0f, 0.0f),
			glm::vec3(-1.55f, 0.0f, 0.0f)
		};

		for (i = 0; i < 4; i++)
		{
			model = glm::mat4();

			model = glm::translate(model, frameTransforms[i]);
			model = glm::translate(model, glm::vec3(0.0f, 1.2f, -3.0f));

			if (i <= 1)
				model = glm::scale(model, glm::vec3(3.2f, 0.1, 0.04f));
			else
				model = glm::scale(model, glm::vec3(0.1f, 1.65f, 0.04f));

			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Render table
		model = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.15f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.05f, 1.5f));
		
		lightingShader.setMat4("model", model);
		lightingShader.setVec3("material.specular", glm::vec3(0.3f, 0.3f, 0.3f));
		lightingShader.setFloat("material.shininess", 40.0f);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Table legs
		glm::vec3 legTransforms[] = {
			glm::vec3(1.45f, -0.3f, 0.7f),
			glm::vec3(1.45f, -0.3f, -0.7f),
			glm::vec3(-1.45f, -0.3f, 0.7f),
			glm::vec3(-1.45f, -0.3f, -0.7f)
		};

		for (i = 0; i < 4; i++)
		{
			model = glm::translate(glm::mat4(), legTransforms[i]);
			model = glm::scale(model, glm::vec3(0.05f, 0.35f, 0.05f));
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Render door
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, door);
		
		float doorAngle;

		if (doorOpening)
		{
			doorAngle = 1.5f * (currentFrame - animFrame);
			if (doorAngle > glm::radians(120.0f))
			{
				doorOpening = false;
				doorOpen = true;
				doorAngle = glm::radians(120.0f);
			}
		}	
		else if (doorClosing)
		{
			doorAngle = glm::radians(120.0f) - 1.5 * (currentFrame - animFrame);
			if (doorAngle < 0.0f)
			{
				doorClosing = false;
				doorOpen = false;
				doorAngle = 0.0f;
			}
		}
		else if (doorOpen)
			doorAngle = glm::radians(120.0f);
		else
			doorAngle = 0.0f;

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.3f));
		model = glm::rotate(model, doorAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.3f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = scale(model, glm::vec3(0.6f, 1.2f, 0.08f));

		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// render corridor behind door
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, corridor);
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = scale(model, glm::vec3(0.6f, 1.2f, 0.02f));

		lightingShader.setMat4("model", model);
		lightingShader.setVec3("material.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		lightingShader.setFloat("material.shininess", 0.0f);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// set lantern position
		if (holdingLantern)
		{
			lightPos = camera.Position;
			lightPos = lightPos + camera.Front * 0.2f + camera.Right * 0.2f - camera.Up * 0.2f;
		}

		// render the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2));
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	
	// Jump with the space key
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		jumping = true;
		jumpFrame = glfwGetTime();
	}

	// Change the perspective with the p key
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pheld)
	{
		ortho = !ortho;
		pheld = true;
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && pheld)
	{
		pheld = false;
	}
	
	// Change light radius with k and l
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lheld)
	{
		lightradius++;
		lheld = true;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && lheld)
	{
		lheld = false;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !kheld)
	{
		if (lightradius >= 1.0)
			lightradius--;
		kheld = true;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE && kheld)
	{
		kheld = false;
	}

	// Toggle lighting mode with o
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !oheld)
	{
		fulllight = !fulllight;
		oheld = true;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE && oheld)
	{
		oheld = false;
	}

	// Zoom in and out with 3 and 1
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !zoomoutheld)
	{
		camera.decreaseZoom();
		zoomoutheld = true;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE && zoomoutheld)
	{
		zoomoutheld = false;
	}
	
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !zoominheld)
	{
		camera.increaseZoom();
		zoominheld = true;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE && zoominheld)
	{
		zoominheld = false;
	}

	// Pick up the lantern with f
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fheld)
	{
		if (glm::length(lightPos - camera.Position) < 2.0f)
		{
			holdingLantern = !holdingLantern;
			if (!holdingLantern)
				lightPos = glm::vec3(lightPos.x, -0.4f, lightPos.z);
		}
		fheld = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && fheld)
	{
		fheld = false;
	}

	// Open the door with r
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !doorOpening && !doorClosing)
	{
		if (glm::length(camera.Position - glm::vec3(3.0f, 0.0f, 0.0f)) < 2.0f)
		{
			animFrame = glfwGetTime();
		
			if (doorOpen)
				doorClosing = true;
			else
				doorOpening = true;
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// Function for loading a 2D texture
unsigned int loadTexture(char const *path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
