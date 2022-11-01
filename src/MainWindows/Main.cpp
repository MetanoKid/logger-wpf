#include <thread>

#include <LogLibrary\Logger.h>

namespace
{
	namespace Defaults
	{
		const unsigned int RENDER_SCALE = 1;
		const int LOGGER_POSITION_X = 0;
		const int LOGGER_POSITION_Y = 0;
		const int LOGGER_WIDTH = 800;
		const int LOGGER_HEIGHT = 600;
		const char *MINIMUM_LOG_LEVEL = "INFO";
	}
}

// example of how to initialize, use and destroy the Logger
int main(int argc, char **argv)
{
	Logger::initialize(Defaults::LOGGER_POSITION_X, Defaults::LOGGER_POSITION_Y, Defaults::LOGGER_WIDTH, Defaults::LOGGER_HEIGHT);

	const LogLevel selectedLogLevel = Logger::asLogLevel(Defaults::MINIMUM_LOG_LEVEL);
	Logger::getInstance()->setMinimumLogLevel(selectedLogLevel);

	Logger::iLOGGER("Initialized logger");

	Logger::dLOGIC("Starting test");
	for(int i = 0; i < 1000; ++i)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		Logger::dGENERIC("Iteration #%i running!", i);

		if (i % 5 == 0)
		{
			Logger::dAI("Some AI logic going on at iteration #%i", i);
		}
		
		if (i % 10 == 0)
		{
			Logger::wPHYSICS("Some warning happening at iteration #%i", i);
		}

		if (i % 100 == 0)
		{
			Logger::eRENDER("Yikes! Some error happened at iteration #%i", i);
		}
	}

	Logger::iLOGGER("Waiting a bit to let you read last messages");
	std::this_thread::sleep_for(std::chrono::seconds(5));

	Logger::iLOGGER("Destroying logger");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	Logger::destroy();

	return EXIT_SUCCESS;
}