#include <iostream>

#include "glew.h"

int main()
{
	glewExperimental = GL_TRUE;
	glewInit();

	std::cout << "Hello World!" << std::endl;
	getchar();
}
