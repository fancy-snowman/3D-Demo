#include "pch.h"
#include "Resource/Window.h"
#include "Resource/Resource.h"
#include "Scene/Scene.h"

#include <chrono>

int main()
{	
	Scene scene;
	scene.Setup();

	using Clock = std::chrono::high_resolution_clock;
	Clock::duration elapsed = std::chrono::seconds(0);
	Clock::time_point then = Clock::now();
	Clock::time_point now = then;

	const Clock::duration TARGET_DELTA = std::chrono::microseconds(16667); // 16.667 ms -> 60 fps

	while (true) // temp
	{
		now = Clock::now();
		elapsed += now - then;
		then = now;

		if (elapsed >= TARGET_DELTA || true)
		{
			elapsed -= TARGET_DELTA;

			float delta = TARGET_DELTA.count() / 1000000000.f;

			scene.Update(delta);
			scene.Draw();
		}
	}

	return 0;
}
