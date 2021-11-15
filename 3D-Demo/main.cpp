#include "pch.h"
#include "Window.h"
#include "Scene.h"

int main()
{	
	Window window(800, 600);
	Scene scene;

	while (true)
	{
		window.Clear(0.2f, 0.3f, 0.4f);
		window.Bind();
		scene.Draw();
		window.Update();
	}

	return 0;
}
