/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries for separate legal notices                          |
|******************************************************************************|
| This demo uses the Assimp library to load a mesh from a file, and supports   |
| many formats. The library is VERY big and complex. It's much easier to write |
| a simple Wavefront .obj loader. I have code for this in other demos. However,|
| Assimp will load animated meshes, which will we need to use later, so this   |
| demo is a starting point before doing skinning animation                     |
\******************************************************************************/
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "gl_utils.h"
#include "tools.h"
#include "malla.h"
#include "image.h"

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow* g_window = NULL;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main(){
	restart_gl_log ();
	start_gl ();
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glFrontFace (GL_CCW); // set counter-clock-wise vertex order to mean the front
	glClearColor (0.2, 0.2, 0.2, 1.0); // grey background to help spot mistakes
	glViewport (0, 0, g_gl_width, g_gl_height);

	glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(g_window, mouse_callback);
	glfwSetScrollCallback(g_window, scroll_callback);
	// tell GLFW to capture our mouse
	glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/*-------------------------------CREATE SHADERS-------------------------------*/
	GLuint shader_programme = create_programme_from_files (
		VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE
	);

	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	int view_mat_location = glGetUniformLocation (shader_programme, "view");
	glUseProgram (shader_programme);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
	int proj_mat_location = glGetUniformLocation (shader_programme, "proj");
	glUseProgram (shader_programme);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);


    // creacion objetos
    malla *sun = new malla((char *) "mallas/untitled.obj", shader_programme);
    malla *venus = new malla((char *) "mallas/untitled.obj", shader_programme);
    malla *earth = new malla((char *) "mallas/untitled.obj", shader_programme);
    malla *moon = new malla((char *) "mallas/untitled.obj", shader_programme);
    malla *mars = new malla((char *) "mallas/untitled.obj", shader_programme);
    malla *jupyter = new malla((char *) "mallas/untitled.obj", shader_programme);
    malla *saturn = new malla((char *) "mallas/untitled.obj", shader_programme);

    malla *halo_venus = new malla((char *) "mallas/halo.obj", shader_programme);
    malla *halo_earth = new malla((char *) "mallas/halo.obj", shader_programme);
    malla *halo_mars = new malla((char *) "mallas/halo.obj", shader_programme);
    malla *halo_jupyter = new malla((char *) "mallas/halo.obj", shader_programme);
    malla *halo_saturn = new malla((char *) "mallas/halo.obj", shader_programme);


    // localizacion espacial
    sun -> setpos(glm::vec3(0.0f, 0.0f, -6.0f));
    venus -> setpos(glm::vec3(3.f, 3.f, -2.f));
    earth -> setpos(glm::vec3(4.f, 4.f, -2.f));
    moon -> setpos(earth->getpos() + glm::vec3(2.f, 3.f, 8.f));
    mars-> setpos(glm::vec3(7.f, 7.f, -2.f));
    jupyter-> setpos(glm::vec3(14.f, 14.f, -2.f));
    saturn-> setpos(glm::vec3(20.f, 20.f, -2.f));

    halo_venus -> setpos(glm::vec3(0.0f, 0.0f, -6.0f));
    halo_earth -> setpos(glm::vec3(0.0f, 0.0f, -8.0f));
    halo_mars -> setpos(glm::vec3(0.0f, 0.0f, -14.0f));
    halo_jupyter -> setpos(glm::vec3(0.0f, 0.0f, -28.0f));
    halo_saturn -> setpos(glm::vec3(0.0f, 0.0f, -40.0f));
    


/*
        venus->setpos(sun->getpos() + glm::vec3(8*sin(mitick), 0.0f, -8*cos(mitick)));
        earth->setpos(sun->getpos() + glm::vec3(12*sin(0.8 * mitick), 0.0f, -12*cos(0.8 * mitick)));
        moon->setpos(earth->getpos() + glm::vec3(2*sin(mitick*3), 0.0f, 1.5*cos(-3*mitick)));
        mars->setpos(sun->getpos() + glm::vec3(20*sin(0.5 * mitick), 0.0f, -20*cos(0.5 * mitick)));
        jupyter->setpos(sun->getpos() + glm::vec3(40*sin(0.2 * mitick), 0.0f, -40*cos(0.2 * mitick)));
        saturn->setpos(sun->getpos() + glm::vec3(50*sin(0.1 * mitick), 0.0f, -50*cos(0.1 * mitick)));
        halo->setpos(sun->getpos());
*/

    // Dimension
    sun->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    venus->scale = glm::vec3(0.3f, 0.3f, 0.3f);
    earth->scale = glm::vec3(0.3f, 0.3f, 0.3f);
    moon->scale = glm::vec3(0.03f, 0.03f, 0.03f);
    mars->scale = glm::vec3(0.2f, 0.2f, 0.2f);
    jupyter->scale = glm::vec3(0.7f, 0.7f, 0.7f);
    saturn->scale = glm::vec3(0.65f, 0.65f, 0.65f);

    halo_venus->scale =  glm::vec3(8.f, 1.f, 8.f);
    halo_earth->scale =  glm::vec3(12.f, 1.f, 12.f);
    halo_mars->scale =  glm::vec3(20.f, 1.f, 20.f);
    halo_jupyter->scale =  glm::vec3(40.f, 1.f, 40.f);
    halo_saturn->scale =  glm::vec3(50.f, 1.f, 50.f);

    earth->vel_rotacion = 0.5f;
     venus->vel_rotacion = 0.5f;
       moon->vel_rotacion = 0.5f;
        jupyter->vel_rotacion = 0.5f;
         saturn->vel_rotacion = 0.5f;
          mars->vel_rotacion = 0.5f;

    float texCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //--------------textura tierra----------------------------------------

    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures/earth.jpg", &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glVertexAttribPointer(glGetUniformLocation(shader_programme, "texture_coord"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindTexture(GL_TEXTURE_2D, texture);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    //----------textura sol--------------

    int width2, height2, nrChannels2;
    unsigned char *data2 = stbi_load("textures/sun.jpg", &width2, &height2, &nrChannels2, 0);

    unsigned int texture2;
    glGenTextures(1, &texture2);

    glBindTexture(GL_TEXTURE_2D, texture2); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data2);

    //----------textura venus--------------

    int width3, height3, nrChannels3;
    unsigned char *data3 = stbi_load("textures/venus.jpg", &width3, &height3, &nrChannels3, 0);

    unsigned int texture3;
    glGenTextures(1, &texture3);

    glBindTexture(GL_TEXTURE_2D, texture3); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data3);

     //----------textura moon--------------

    int width4, height4, nrChannels4;
    unsigned char *data4 = stbi_load("textures/moon.jpg", &width4, &height4, &nrChannels4, 0);

    unsigned int texture4;
    glGenTextures(1, &texture4);

    glBindTexture(GL_TEXTURE_2D, texture4); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width4, height4, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data4);

    //----------textura mars--------------

    int width5, height5, nrChannels5;
    unsigned char *data5 = stbi_load("textures/mars.jpg", &width5, &height5, &nrChannels5, 0);

    unsigned int texture5;
    glGenTextures(1, &texture5);

    glBindTexture(GL_TEXTURE_2D, texture5); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width5, height5, 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data5);

    //----------textura saturn--------------

    int width6, height6, nrChannels6;
    unsigned char *data6 = stbi_load("textures/saturn.jpg", &width6, &height6, &nrChannels6, 0);

    unsigned int texture6;
    glGenTextures(1, &texture6);

    glBindTexture(GL_TEXTURE_2D, texture6); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width6, height6, 0, GL_RGB, GL_UNSIGNED_BYTE, data6);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data6);

     //----------textura jupyter--------------

    int width7, height7, nrChannels7;
    unsigned char *data7 = stbi_load("textures/jupyter.jpg", &width7, &height7, &nrChannels7, 0);

    unsigned int texture7;
    glGenTextures(1, &texture7);

    glBindTexture(GL_TEXTURE_2D, texture7); 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width7, height7, 0, GL_RGB, GL_UNSIGNED_BYTE, data7);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data7);

    //glVertexAttribPointer(glGetUniformLocation(shader_programme, "texture_coord"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(2);

    //glBindTexture(GL_TEXTURE_2D, texture);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //controla las rotaciones
    float mitick = 0.005f; 

	// render loop
	// -----------
	while (!glfwWindowShouldClose(g_window)){
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----

		processInput(g_window);

		// render
		// ------
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //graficos
        glBindTexture(GL_TEXTURE_2D, texture2);
        sun->drawtriangles();
        glBindTexture(GL_TEXTURE_2D, texture3);
        venus->drawtriangles();
        glBindTexture(GL_TEXTURE_2D, texture);
        earth->drawtriangles();
        glBindTexture(GL_TEXTURE_2D, texture4);
        moon->drawtriangles();
        glBindTexture(GL_TEXTURE_2D, texture5);
        mars->drawtriangles();
        glBindTexture(GL_TEXTURE_2D, texture7);
        jupyter->drawtriangles();
        glBindTexture(GL_TEXTURE_2D, texture6);
        saturn->drawtriangles();

        halo_venus->drawlines();
        halo_earth->drawlines();
        halo_mars->drawlines();
        halo_jupyter->drawlines();
        halo_saturn->drawlines();
        
        //logica
        venus->setpos(sun->getpos() + glm::vec3(8*sin(mitick), 0.0f, -8*cos(mitick)));
        earth->setpos(sun->getpos() + glm::vec3(12*sin(0.8 * mitick), 0.0f, -12*cos(0.8 * mitick)));
        moon->setpos(earth->getpos() + glm::vec3(2*sin(3*mitick), 0.0f, 2*cos(-3*mitick)));
        mars->setpos(sun->getpos() + glm::vec3(20*sin(0.5 * mitick), 0.0f, -20*cos(0.5 * mitick)));
        jupyter->setpos(sun->getpos() + glm::vec3(40*sin(0.2 * mitick), 0.0f, -40*cos(0.2 * mitick)));
        saturn->setpos(sun->getpos() + glm::vec3(50*sin(0.1 * mitick), 0.0f, -50*cos(0.1 * mitick)));
        halo_venus->setpos(sun->getpos());
        halo_earth->setpos(sun->getpos());
        halo_mars->setpos(sun->getpos());
        halo_jupyter->setpos(sun->getpos());
        halo_saturn->setpos(sun->getpos());

        mitick += 0.0005f;

		// activate shader
		glUseProgram (shader_programme);

		// pass projection matrix to shader (note that in this case it could change every frame)
		projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100000.0f);
		glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);

		// camera/view transformation
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
