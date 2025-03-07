#include <cctype>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/screen/color.hpp>
#include <functional>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include "ollama.hpp"
#include "config.h"
#include "json.hpp"
#include "storage.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>

#define VERSION "0.1.0"

using json = nlohmann::json;

void printArgs(int argc, char** argv)
{
	std::cout << "Got " << argc << " args" << std::endl;
	for (int i = 0; i < argc ; i++)
	{
		std::cout << argv[i] << std::endl;
	}
}

void printModels()
{
	auto m = ollama::list_models();
	for (auto& model : m)
	{
		std::cout << model << std::endl;
	}

}

void printVersion()
{
	std::cout << "lm version " << VERSION << std::endl;
}

struct arg
{
	std::string argName;
	std::function<void()> callback;
	std::string description;
};

std::vector<arg> args;

void printHelp()
{
	std::cout << "lm - a command line interface for llms" << std::endl;
	std::cout << "Usage: lm [options]" << std::endl;
	std::cout << std::endl;
	std::cout << "Options:" << std::endl;
	for (auto& arg : args)
	{
		std::cout << "  --" << arg.argName << ", " << arg.description << std::endl;
	}
}

bool matchArg(const std::string& cliArg, const arg& Arg)
{
	if (cliArg == "--" + Arg.argName)
	{
		return true;
	}

	if (cliArg == std::string("-") + Arg.argName.at(0))
	{
		return true;
	}

	return false;
}


std::string wipMessage = "";
bool chatActive = false;

std::mutex wipMessageMutex;
std::mutex chatActiveMutex;

int tokenCount = 0;

ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::FullscreenPrimaryScreen();

void on_receive_response(const ollama::response& response)
{
	//std::cout << response << std::flush;
	tokenCount++;
	wipMessageMutex.lock();
	wipMessage += response;
	wipMessageMutex.unlock();

	if (response.as_json()["done"]==true)
	{
		chatActiveMutex.lock();
		chatActive = false;
		chatActiveMutex.unlock();
	};
	screen.PostEvent(ftxui::Event::Custom);
}

void runChat()
{
    using namespace ftxui;

    std::string input_str;
    bool enterPressed = false;
    ollama::messages msgs;

    ollama::options options;
    options["seed"] = 1;




    Component input_prompt = Input(&input_str, "Input:");

    input_prompt |= CatchEvent([&](Event event) {
        enterPressed = (Event::Character('\n') == event);
        if(enterPressed)
        {
            if(input_str == "/exit")
            {
                screen.Exit();
            } else if(input_str != "") {
                ollama::message msg("user", input_str);
                msgs.push_back(msg);

            	wipMessageMutex.lock();
            	wipMessage = "";
            	wipMessageMutex.unlock();

            	chatActiveMutex.lock();
            	chatActive = true;
            	chatActiveMutex.unlock();

				std::function<void(const ollama::response&)> response_callback = on_receive_response;
                try
                {
					std::thread new_thread( [response_callback, msgs, options]{
						chat(getConfig(ConfigKey::MODEL_NAME), msgs, response_callback, options);
					});
                	new_thread.detach();

                }
                catch (const std::exception& e)
                {
                	std::cout << "Error: " << e.what() << std::endl;
                }
            }
        }
        return enterPressed;
    });

    auto renderer = Renderer(input_prompt, [&] {

        chatActiveMutex.lock();
		bool generating = chatActive;
		chatActiveMutex.unlock();

    	if (!generating)
    	{
			wipMessageMutex.lock();
			if (wipMessage != "")
			{
				msgs.push_back(ollama::message("assistant", wipMessage));
				wipMessage = "";
			}
			wipMessageMutex.unlock();
    	}

        Elements e = {};
        for(int i = 0; i<msgs.size(); i++)
        {
            auto msg = msgs[i].as_json_string();
            auto j = json::parse(msg);
            std::string role = j["role"];
            role[0] = toupper(role[0]);
            std::string msg_txt = j["content"];

            e.push_back(
                window(
                    text(" "+role+": "),
                    paragraph(msg_txt)
                ) | color(role == "User" ? Color::Red: Color::Blue)
            );
        }


		if (!generating)
		{
	        e.push_back({
	            window(text("User:"), hbox(text(" User : "), input_prompt->Render()))
	        });
		}
    	else
		{
			std::string tmpMsg;
			wipMessageMutex.lock();
			tmpMsg = wipMessage;
			wipMessageMutex.unlock();
			e.push_back(
				window(
					text(" Assistant: "),
					paragraph(tmpMsg)
				) | color(Color::Blue)
			);
		}

        return frame(vbox(e));

     });

    screen.Loop(renderer);
}


int main(int argc, char** argv)
{
	args.push_back({
		"help",
		printHelp,
		"Print this help message"
	});
	args.push_back({
		"models",
		printModels,
		"List available models"
	});
	args.push_back({
		"version",
		printVersion,
		"Print version"
	});

	loadConfig();
	auto db = initDatabase();

	if (argc < 2)
	{
		runChat();
	}
	else
	{
		for (auto& arg : args)
		{
			if(matchArg(std::string(argv[1]), arg))
			{
				arg.callback();
				return 0;
			}
		}

		std::cout << "generating response" << std::endl;
		std::string prompt;
		for (int i = 2; i < argc; i++)
		{
			prompt += argv[i];
			if (i < argc - 1)
			{
				prompt += " ";
			}
		}

		std::function<void(const ollama::response&)> response_callback = on_receive_response;
		ollama::generate(getConfig(ConfigKey::MODEL_NAME), prompt, on_receive_response);

		wipMessageMutex.lock();
		std::string tmpChatMsg = wipMessage;
		wipMessageMutex.unlock();

		Request req = {
			.id = std::nullopt,
		    .msg = prompt,
			.answer = tmpChatMsg,
			.model = getConfig(ConfigKey::MODEL_NAME),
		};
		addRequest(db, req);
	}

	return 0;
}
