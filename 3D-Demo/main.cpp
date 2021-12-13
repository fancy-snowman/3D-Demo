#include "pch.h"
#include "Resource/Window.h"
#include "Resource/Resource.h"
#include "Scene/Scene.h"

int main()
{	
	Resource::Window window(800, 600);
	Scene scene;

	//auto materials = Resource::Manager::LoadMaterial("models/mandalorian.mtl");

	while (window.Process())
	{
		window.Clear(0.2f, 0.3f, 0.4f);
		window.Bind();
		scene.Update(0.05f);
		scene.Draw();
		window.Present();
	}

	return 0;
}
