#include <iostream>
#include <ostream>
#include <string>
#include "ollama.hpp"
#include "cfgpath.h"
#include "json.hpp"

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

std::string getConfigFilePath()
{
	char cfgdir[512];
	get_user_config_file(cfgdir, sizeof(cfgdir), "myapp");
	if (cfgdir[0] == 0) {
		printf("Unable to find home directory.\n");
		return std::string();
	}
	return std::string(cfgdir);
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

std::string tmpChatMsg;

int tokenCount = 0;

void on_receive_response(const ollama::response& response)
{
	std::cout << response << std::flush;
	tokenCount++;
	tmpChatMsg += response;

	if (response.as_json()["done"]==true)
	{
		std::cout << std::endl;
		std::cout << "Used " << tokenCount << " tokens" << std::endl;
	};
}

void runChat()
{
    std::cout << "Starting Chat (/exit to leave)" << std::endl;
    std::string currentMessage;
    ollama::messages msgs;
    std::function<void(const ollama::response&)> response_callback = on_receive_response;

    ollama::options options;
    options["seed"] = 1;

    while(true) {
        std::cout << "User:";
        std::getline(std::cin, currentMessage);

        if(currentMessage == "/exit") {
            break;
        }

        ollama::message msg("user", currentMessage);
        msgs.push_back(msg);
        tmpChatMsg = "";
        std::cout << "AI: ";
        ollama::chat("qwen2.5-coder:1.5b", msgs, on_receive_response, options);
        ollama::message response("assistant", currentMessage);
        msgs.push_back(response);
    }

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
		ollama::generate("qwen2.5-coder:1.5b", prompt, on_receive_response);
	}

	return 0;
}
