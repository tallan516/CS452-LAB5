#include "timShader.h"

#include "SDL2/SDL.h"

using namespace std;

GLuint abuffer;
GLuint buffer[2];
GLuint ebuffer;
GLuint tbuffer;

GLuint program;

GLuint texture;

GLint uniform_texture;
GLint texcoord;

GLfloat pit,yaw,scalar=1;
glm::vec3 cubeTran;

GLfloat pyramid_texcoords[] = {	-5.0f,0.0f,-5.0f,
						5.0f,0.0f,-5.0f,
						5.0f,0.0f,5.0f,
						-5.0f,0.0f,5.0f,
						0.0f,10.0f,0.0f	
						};

GLfloat vertices[] = {	-5.0f,0.0f,-5.0f,		//Bottom, Back, Left
				5.0f,0.0f,-5.0f,		//Bottom, Back, Right
				5.0f,0.0f,5.0f,		//Bottom, Front, Right
				-5.0f,0.0f,5.0f,		//Bottom, Front, Left
				0.0f,10.0f,0.0f	};	//Top, Center


				//R, B, G, A (transparency)
GLfloat colors[] = {	0.0f,1.0f,1.0f,1.0f,	//Cyan
				0.0f,1.0f,1.0f,1.0f,	//Cyan
				0.0f,1.0f,1.0f,1.0f,	//Cyan
				0.0f,1.0f,1.0f,1.0f,	//Cyan
				0.0f,1.0f,1.0f,1.0f  };	//Cyan	

GLubyte elems[] = {	4,3,0,
    	          		4,2,3,
    	          		4,1,2,
    	          		4,0,1,
    	          		0,1,2,
    				2,3,0
    	          		};

//Declare functions
void init();
void display(SDL_Window* window);
void input(SDL_Window* window);


int main(int argc, char **argv)
{
	//Main method stuff
	SDL_Window *window;	//SDL window
	if(SDL_Init(SDL_INIT_VIDEO) < 0)	//Tries to intitiate SDL
	{
		cerr << "Error, cannot initialize SDL." << endl;
		SDL_Quit();		//Close out of SDL
		exit(0);		//End program
	}
	
	//Create window
	window = SDL_CreateWindow("CS452-LAB5", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500, SDL_WINDOW_OPENGL);
	
	//Check window creation
	if(window == NULL)
	{
		cerr << "Error, cannot create window." << endl;
	}
	
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);		//Creates opengl context associated with the window
	glewInit();		//Initializes glew
	
	init();	//Calls function to initialize the shaders and set up buffers
	
	//Keep looping through to make sure
	while(true)
	{
		input(window);		//Check keyboard input
		display(window);		//Render
	}
	
	//Close out of SDL stuff
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}

void display(SDL_Window* window)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	//Clears the frame buffer
	
	glm::mat4 trans;	//Matrix for transformations
	
	trans = glm::translate(trans, cubeTran);	//translate the cube
	trans = glm::rotate(trans, pit, glm::vec3(1, 0, 0));	//rotate the cube around the x axis
	trans = glm::rotate(trans, yaw, glm::vec3(0, 1, 0));	//rotate the cube arround the y axis
	trans = glm::scale(trans, glm::vec3(scalar));	//scaling the cube
    
	GLint tempLoc = glGetUniformLocation(program, "modelMatrix");	//Matrix that handles the transformations
	glUniformMatrix4fv(tempLoc, 1 ,GL_FALSE,&trans[0][0]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	uniform_texture = glGetUniformLocation(program, "texture");
	glUniform1i(uniform_texture, 0);
	
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, NULL);
	glFlush();				//Makes sure all data is rendered as soon as possible
	SDL_GL_SwapWindow(window);	//Updates the window
}

void input(SDL_Window* window)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))		//Handling the keyboard
	{	
		if(event.type == SDL_QUIT)
		{
			exit(0);
		}
		else if(event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_ESCAPE: exit(0);
				case SDLK_w: cubeTran.y+=2; break;
				case SDLK_s: cubeTran.y-=2; break;
				case SDLK_a: cubeTran.x-=2; break;
				case SDLK_d: cubeTran.x+=2; break;
				case SDLK_e: scalar+=.1f; break;
				case SDLK_q: scalar-=.1f; break;
				case SDLK_i: pit+=2; break;
				case SDLK_k: pit-=2; break;
				case SDLK_j: yaw+=2; break;
				case SDLK_l: yaw-=2; break;
			}
		}
	}
}

void init()
{
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_TEXTURE_2D);	//Enable 2D Textures
	program = glCreateProgram();	//Creates program
	initShaders(program);		//Calls the initialize shader function in the header file
	
	glGenVertexArrays(1, &abuffer);
	glBindVertexArray(abuffer);
	
	glGenBuffers(2, buffer);
	
	//Similar to the checkerboard pattern in lecture on 2-25
	GLubyte image[64][64][3];
	for(int i = 0; i < 64; i++ )
	{
		for(int j = 0; j < 64; j++ )
		{
			if(i%2 != j%2)
			{
				GLubyte c = 255;
				image[i][j][0] = c;
				image[i][j][1] = c;
				image[i][j][2] = c;
			}
		}
	}
		
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 3, 3, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	
	//Sets up pointers and stuff
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glGenBuffers(1, &ebuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);
	
	glGenBuffers(1, &tbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_texcoords), pyramid_texcoords, GL_STATIC_DRAW);
	
	//Enables vertex arrays to draw stuff
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);	
	
	texcoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(texcoord);
	glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
}



