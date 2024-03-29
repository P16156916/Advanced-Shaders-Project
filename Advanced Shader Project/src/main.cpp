

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Camera.h>
#include "vbocube.h"
#include "Debugger.h"
#include <Model.h>
#include "stb_image.h"
#include <winuser.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool m_bGeo = false;
bool m_bNormal = false;
bool m_bReflection = false;
bool m_bRefraction = false;
bool m_bInstance = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
int l_iRefresh = GLFW_REFRESH_RATE;
int l_iCount = 0;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Message l_mFPS;
Message l_mHighestFPS;
Message l_mLowestFPS;

float l_fFPS;
float l_fHighestFPS = 0;
float l_fLowestFPS = 100000;

void setUpFPS()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	l_fFPS = 1.f / deltaTime;
	if (l_iCount % (l_iRefresh / 2) == 0) //Set FPS display rate to be twice a second.
	{
		l_mFPS.message = "FPS: " + to_string(l_fFPS);
		l_mFPS.yPosition = 10.f;
		l_mFPS.xPosition = 5.f;
	}
	if (l_fFPS < l_fLowestFPS)
		l_fLowestFPS = l_fFPS;
	if (l_fFPS > l_fHighestFPS)
		l_fHighestFPS = l_fFPS;

	l_mLowestFPS.message = "Lowest FPS: " + to_string(l_fLowestFPS);
	l_mLowestFPS.yPosition = 10.f;
	l_mLowestFPS.xPosition = 250.f;

	l_mHighestFPS.message = "Highest FPS: " + to_string(l_fHighestFPS);
	l_mHighestFPS.yPosition = 10.f;
	l_mHighestFPS.xPosition = 550.f;
}

glm::mat4* setUpInstances()
{
	unsigned int amount = 25;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	int l_iCounter = 0;

	for (int iY = 0; iY < 5; iY++)
	{
		for (int iX = 0; iX < 5; iX++)
		{
			glm::mat4 model = glm::mat4(1.f);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			glm::vec3 l_vPos = glm::vec3(-20.f + (iX * 10.f), 0.f, -20.f + (iY * 10.f));
			model = glm::translate(model, l_vPos);
			modelMatrices[l_iCounter] = model;
			l_iCounter++;
		}
	}

	return modelMatrices;
}

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "P16156916 - IMAT3907 Advanced Shaders", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	glfwSwapInterval(0);
	// build and compile shaders
	// -------------------------
	Shader ourShader("resources/shaders/model.vert", "resources/shaders/model.frag");
	Shader ourShaderNormal("resources/shaders/modelNormalMap.vert", "resources/shaders/modelNormalMap.frag");
	Shader ourShaderGeo("resources/shaders/modelGeo.vert", "resources/shaders/modelGeo.frag", "resources/shaders/modelGeo.geo");
	Shader ourShaderReflect("resources/shaders/reflection.vert", "resources/shaders/reflection.frag");
	Shader ourShaderRefract("resources/shaders/refraction.vert", "resources/shaders/refraction.frag");
	Shader ourShaderInstance("resources/shaders/modelInstance.vert", "resources/shaders/modelInstance.frag");
	Shader ourShaderSky("resources/shaders/sky.vert", "resources/shaders/sky.frag");

	ourShaderNormal.use();
	ourShaderNormal.setInt("diffuseMap", 0);
	ourShaderNormal.setInt("normalMap", 1);

	glm::vec3 lightPos(0.0f, 10.f, 0.0f);

	// load models
	// -----------

	Model l_mNano("resources/models/Nanosuit/nanosuit.obj", false);
	Model l_mFloor("resources/models/Floor/Floor.obj", false);

	VBOCube l_vbCube = VBOCube(50.f, ourShaderSky.getID());
	Debugger l_dScreenText;

	unsigned int amount = 25;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	modelMatrices = setUpInstances();

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
	l_mNano.setUpInstancing();

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		setUpFPS();

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		if (m_bGeo) //use geometry shader
		{
			ourShaderGeo.use();
			ourShaderGeo.setMatrix4("projection", projection);
			ourShaderGeo.setMatrix4("view", view);
			// render the geometry lines
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			ourShaderGeo.setMatrix4("model", model);
			l_mNano.Draw(ourShaderGeo);
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
			ourShaderNormal.setMatrix4("model", model);
			l_mFloor.Draw(ourShaderGeo);
		}

		if (m_bNormal)
		{
			ourShaderNormal.use();
			ourShaderNormal.setMatrix4("projection", projection);
			ourShaderNormal.setMatrix4("view", view);
			// render normal-mapped quad
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			//model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			ourShaderNormal.setMatrix4("model", model);
			ourShaderNormal.setVec3("viewPos", camera.getPosition());
			ourShaderNormal.setVec3("lightPos", lightPos);
			l_mNano.Draw(ourShaderNormal);
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
			ourShaderNormal.setMatrix4("model", model);
			l_mFloor.Draw(ourShaderNormal);
		}
		else if (m_bReflection)
		{
			ourShaderReflect.use();
			ourShaderReflect.setMatrix4("P", projection);
			ourShaderReflect.setMatrix4("V", view);
			// render normal-mapped quad
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			//model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			ourShaderReflect.setMatrix4("M", model);
			ourShaderReflect.setVec3("cameraPos", camera.getPosition());
			l_mNano.Draw(ourShaderReflect);
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
			ourShaderReflect.setMatrix4("M", model);
			l_mFloor.Draw(ourShaderReflect);
		}
		else if (m_bRefraction)
		{
			ourShaderRefract.use();
			ourShaderRefract.setMatrix4("P", projection);
			ourShaderRefract.setMatrix4("V", view);
			// render normal-mapped quad
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			//model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			ourShaderRefract.setMatrix4("M", model);
			ourShaderRefract.setVec3("cameraPos", camera.getPosition());
			l_mNano.Draw(ourShaderRefract);
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
			ourShaderRefract.setMatrix4("M", model);
			l_mFloor.Draw(ourShaderRefract);
		}
		else if (m_bInstance)
		{
			// view/projection transformations
			ourShaderInstance.use();
			ourShaderInstance.setMatrix4("projection", projection);
			ourShaderInstance.setMatrix4("view", view);
			// render the loaded model
			l_mNano.Draw(ourShaderInstance, amount);

			ourShader.use();
			ourShader.setMatrix4("projection", projection);
			ourShader.setMatrix4("view", view);
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
			ourShader.setMatrix4("model", model);

			l_mFloor.Draw(ourShader);
		}
		else
		{
			// view/projection transformations
			ourShader.use();
			ourShader.setMatrix4("projection", projection);
			ourShader.setMatrix4("view", view);
			// render the loaded model
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
			ourShader.setMatrix4("model", model);
			l_mNano.Draw(ourShader);
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
			ourShader.setMatrix4("model", model);
			l_mFloor.Draw(ourShader);
		}

		//render skybox
		ourShaderSky.use();
		ourShaderSky.setMatrix4("P", projection);
		ourShaderSky.setMatrix4("VM", glm::mat4(glm::mat3(view)));
		ourShaderSky.setInt("cubeMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, l_vbCube.getID());
		l_vbCube.render();

		l_dScreenText.drawMessage(l_mFPS, l_mFPS.xPosition);
		l_dScreenText.drawMessage(l_mLowestFPS, l_mLowestFPS.xPosition);
		l_dScreenText.drawMessage(l_mHighestFPS, l_mHighestFPS.xPosition);
		if (l_iCount > 1000)
			l_iCount = 0;
		else l_iCount++;

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

bool returnOpposite(bool p_bParam)
{
	if (p_bParam)
		return false;
	else return true;
}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) //used so when toggling it doesn't flicker on and off.
{
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		m_bGeo = returnOpposite(m_bGeo);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		m_bNormal = returnOpposite(m_bNormal);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		m_bReflection = returnOpposite(m_bReflection);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		m_bRefraction = returnOpposite(m_bRefraction);
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		m_bInstance = returnOpposite(m_bInstance);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

