//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------
#include "iostream"
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <stb/stb_image.h>
//#include "../Geotiff.cpp"
#include"../Texture.h"
#include"../shaderClass.h"
#include"../VAO.h"
#include"../VBO.h"
#include "../EBO.h"
#include <tiffio.h>
#include <stdarg.h>
#include "../Camera.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

using namespace std;

const unsigned int width = 800;
const unsigned int height = 800;



struct C
{
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
};


// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-2.5f, -2.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
	-2.5f,  2.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
	 2.5f,  2.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
	 2.5f, -2.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
};

// Vertices coordinates
GLfloat vertices2[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-7.5f, -2.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
	-7.5f,  2.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
	-2.5f,  2.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
	-2.5f, -2.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
};

// Indices for vertices order
GLuint indices[] =
{
	0, 2, 1, // Upper triangle
	0, 3, 2 // Lower triangle
};


/*Using Sam Leffler's libtiff library */
TIFFRGBAImage img;
vector<uint32*> raster;


size_t npixels;
int imgwidth, imgheight, imgcomponents;

int hasABGR = 0;
char* imgfilename = NULL;

int readTiffImage(const char* name, GLuint* textureID) {
	TIFF* tif;
	char emsg[1024] = {0};


	glBindTexture(GL_TEXTURE_2D, *textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	
	tif = TIFFOpen(name, "r");
	if (tif == NULL) { // if the tif file didnt open exit and print error
		fprintf(stderr, "tif == NULL\n");
		exit(1);
	}
	if (TIFFRGBAImageBegin(&img, tif, 0, emsg)) {
		npixels = img.width * img.height;
		raster.push_back((uint32*)_TIFFmalloc(npixels * sizeof(uint32)));
		if (raster.back() != NULL) {
			if (TIFFRGBAImageGet(&img, raster.back(), img.width, img.height) == 0) { // catches errors with getting the image data
				TIFFError(name, emsg);
				exit(1);
			}
		}

		TIFFRGBAImageEnd(&img);
		fprintf(stderr, "Read image %s (%d x %d)\n", name, img.width, img.height);
	}
	else {
		TIFFError(name, emsg);
		exit(1);
	}
	imgwidth = img.width;
	imgheight = img.height;

	// code based upon
	// http://www.opengl.org/developers/code/mjktips/libtiff/showtiff.c

/* If cannot directly display ABGR format, we need to reverse the component
   ordering in each pixel. :-( 
	if (!hasABGR) {
		int i;

		for (i = 0; i < npixels; i++) {
			register unsigned char* cp = (unsigned char*)&raster[i];
			int t;

			t = cp[3];
			cp[3] = cp[0];
			cp[0] = t;
			t = cp[2];
			cp[2] = cp[1];
			cp[1] = t;
		}
	}
	*/
	

}



int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "my geotiff viewer", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);



	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("../default.vert", "../default.frag");


	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	
	GLuint textures[2];
	glGenTextures(2, textures);
	glActiveTexture(GL_TEXTURE0);

	string path = "D:/opengl-gdal/data/satellite/res2.tif";
	readTiffImage(path.c_str(), &(textures[0]));
	

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		imgwidth, imgheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, raster.back());

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	Shader shade2rProgram("../default2.vert", "../default2.frag");


	// code under here was used for a 2nd image to show 2 tifs at the same time
	// Generates Vertex Array Object and binds it
	//VAO VAO2;
	//VAO2.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	//VBO VBO2(vertices2, sizeof(vertices2));
	// Generates Element Buffer Object and links it to indices
	//EBO EBO2(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	//VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	//VAO2.LinkAttrib(VBO2, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//VAO2.LinkAttrib(VBO2, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	//VAO2.Unbind();
	//VBO2.Unbind();
	//EBO2.Unbind();

	/*
	int i;
	// create object of Geotiff class
	Geotiff tiff((const char*)"D:\\opengl-gdal\\data\\satellite\\res.tif");
	int nXBlockSize, nYBlockSize;
	C* pixel;
	// dump out array (band) dimensions of Geotiff data  
	int* dims = tiff.GetDimensions();
	cout << dims[0] << " " << dims[1] << " " << dims[2] << endl;

	std::vector<GLbyte> colours;
	cout << endl;
	
	float** redband = tiff.GetRasterBand(1);
	float** greenband = tiff.GetRasterBand(2);
	float** blueband = tiff.GetRasterBand(3);
	float** alphaband = tiff.GetRasterBand(4);

	std::vector<GLfloat> testvec;
	for (int i = 0; i < dims[0]; i++)
	{
		for (int j = 0; j < dims[1]; j++)
		{
			pixel = new C;
			pixel->r = redband[i][j];
			pixel->g = greenband[i][j];
			pixel->b = blueband[i][j];
			pixel->a = alphaband[i][j];
			testvec.push_back(redband[i][j]);
			colours.push_back(redband[i][j]);
			colours.push_back(greenband[i][j]);
			colours.push_back(blueband[i][j]);
			colours.push_back(alphaband[i][j]);
			//cout << colours[dims[0] * dims[1]]->r << "  " << colours[dims[0] * dims[1]]->g << "  " << colours[dims[0] * dims[1]]->b<< endl;
		}
	}
	*/
	

	
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();
	glEnable(GL_DEPTH_TEST);
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 5.0f));

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		// Handles camera inputs
		camera.Inputs(window);

		// min near = 0.0000001f
		camera.Matrix(45.0f, 0.00001f, 100000000000.0f, shaderProgram, "camMatrix");
	
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// Bind the VAO so OpenGL knows to use it
		//VAO2.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	//VAO2.Delete();
	//VBO2.Delete();
	//EBO2.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}