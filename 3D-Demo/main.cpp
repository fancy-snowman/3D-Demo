#include "pch.h"
#include "Platform/Window.h"
#include "Scene/Scene.h"

int main()
{	
	Window window(800, 600);
	Scene scene;

	while (window.Process())
	{
		window.Clear(0.2f, 0.3f, 0.4f);
		window.Bind();
		scene.Update(0.0005f);
		scene.Draw();
		window.Present();
	}

	return 0;
}
