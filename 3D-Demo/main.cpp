#include "pch.h"
#include "Resource/Window.h"
#include "Resource/Resource.h"
#include "Scene/Scene.h"

#include <chrono>

int main()
{
	Resource::Window window(800, 600);
	
	Scene scene;
	scene.Setup();

	using Clock = std::chrono::high_resolution_clock;
	Clock::duration elapsed = std::chrono::seconds(0);
	Clock::time_point then = Clock::now();
	Clock::time_point now = then;

	const Clock::duration TARGET_DELTA = std::chrono::microseconds(16667); // 16.667 ms -> 60 fps

	while (window.Process())
	{
		now = Clock::now();
		elapsed += now - then;
		then = now;

		if (elapsed >= TARGET_DELTA || true)
		{
			elapsed -= TARGET_DELTA;

			window.Bind();
			window.Clear(0.2f, 0.3f, 0.4f);

			float delta = TARGET_DELTA.count() / 1000000000.f;

			scene.Update(delta);
			scene.Draw();
			window.Present();
		}
	}

	return 0;
}
