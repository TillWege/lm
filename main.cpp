#include <iostream>
#include "ollama.hpp"
#include "cfgpath.h"
#include "json.hpp"

using json = nlohmann::json;


void printArgs(int argc, char** argv)
{
	std::cout << "Got " << argc << " args" << std::endl;
	for (int i = 0; i < argc ; i++)
	{
		std::cout << argv[i] << std::endl;
	}
}


bool matchArg(std::string cliArg, std::string Arg)
{
	return false;
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

int tokenCount = 0;

void on_receive_response(const ollama::response& response)
{
	std::cout << response << std::flush;
	tokenCount++;

	if (response.as_json()["done"]==true)
	{
		std::cout << std::endl;
		std::cout << "Used " << tokenCount << " tokens" << std::endl;
	};
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "starting chat mode" << std::endl;
	}
	else
	{
		std::string arg = std::string(argv[1]);
		if (arg.starts_with("--"))
		{
			std::cout << "got cli arg " << arg << std::endl;
		}
		else
		{
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
			ollama::generate("llama3.2:3b-instruct-Q5_K_M", prompt, on_receive_response);
		}
	}

	return 0;
}