#include <format>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <thread>

#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

Element getDoc(float val = 0.f)
{
	return vbox({
		hbox({
			border(gauge(val)),
			vcenter(text(std::format("{:3.2f}%", val*100.f)) | size(WIDTH, EQUAL, 10)),
		})
	});
}

int main(void) {
    auto screen = ScreenInteractive::FixedSize(50, 5);

	std::atomic val = 0.f;
	std::atomic<bool> running = false;

	std::thread* timerThread = nullptr;

	auto renderer = Renderer([&val, &timerThread] {
	  return vbox({
	  	getDoc(val),
	  	separator(),
	  	hbox({
	  		text("Thread Running: "),
	  		text(timerThread != nullptr ? "Yes" : "No") | color(timerThread == nullptr ? Color::Red : Color::Green),
	  	})
	  });
	});

	auto component = CatchEvent(renderer, [&](Event event) {
		if (event == Event::Character('r'))
		{
			if (timerThread == nullptr)
			{
				running = true;
				timerThread = new std::thread([&val, &running, &screen] {
					while (running)
					{
						val += 0.1f;
						if (val > 1.f)
						{
							val = 0.f;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						screen.PostEvent(Event::Custom);
					}
				});
				return true;
			}
		}

		if (event == Event::Character('s'))
		{
			if (timerThread != nullptr)
			{
				running = false;
				timerThread->join();
				delete timerThread;
				timerThread = nullptr;
				return true;
			}
		}

		if (event == Event::Character('q')) {
			running = false;
			if (timerThread != nullptr)
			{
				timerThread->join();
				delete timerThread;
				timerThread = nullptr;
			}
			screen.ExitLoopClosure()();
			return true;
		}
		if (event == Event::ArrowLeft) {
			val = 0.f;
			return true;
		}
		if (event == Event::ArrowRight) {
			val = 1.f;
			return true;
		}
		if (event == Event::Character('1')) {
			val = 0.1f;
			return true;
		}
		if (event == Event::Character('2')) {
			val = 0.2f;
			return true;
		}
		if (event == Event::Character('3')) {
			val = 0.3f;
			return true;
		}
		if (event == Event::Character('4')) {
			val = 0.4f;
			return true;
		}
		if (event == Event::Character('5')) {
			val = 0.5f;
			return true;
		}
		if (event == Event::Character('6')) {
			val = 0.6f;
			return true;
		}
		if (event == Event::Character('7')) {
			val = 0.7f;
			return true;
		}
		if (event == Event::Character('8')) {
			val = 0.8f;
			return true;
		}
		if (event == Event::Character('9')) {
			val = 0.9f;
			return true;
		}

		return false;
	});

	screen.Loop(component);


    return EXIT_SUCCESS;
}