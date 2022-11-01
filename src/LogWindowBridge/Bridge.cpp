#include "Bridge.h"

Bridge::Bridge(int x, int y, int w, int h)
{
	LogWindowUI::LoggerUI::Initialize(x, y, w, h);
}

Bridge::~Bridge()
{
	LogWindowUI::LoggerUI::Destroy();
}

void Bridge::configureSystems(std::vector<const char *> systems)
{
	System::Collections::Generic::List<System::String ^> ^systemsManaged = gcnew System::Collections::Generic::List<System::String ^>();
	for (const char *s : systems)
	{
		systemsManaged->Add(gcnew System::String(s));
	}

	LogWindowUI::LoggerUI::Instance->ConfigureSytems(systemsManaged);
}

void Bridge::setSystemEnabled(const char *system, bool enabled)
{
	LogWindowUI::LoggerUI::Instance->SetSystemEnabled(gcnew System::String(system), enabled);
}

void Bridge::configureLevels(std::vector<std::pair<const char *, const char *>> levels)
{
	System::Collections::Generic::List<System::Tuple<System::String ^, System::String ^> ^> ^levelsManaged = gcnew System::Collections::Generic::List<System::Tuple<System::String ^, System::String ^> ^>();

	for (std::pair<const char *, const char *> l : levels)
	{
		levelsManaged->Add(gcnew System::Tuple<System::String ^, System::String ^>(gcnew System::String(l.first), gcnew System::String(l.second)));
	}

	LogWindowUI::LoggerUI::Instance->ConfigureLevels(levelsManaged, levelsManaged[0]->Item1);
}

void Bridge::log(const char *level, float timestamp, const char *system, const char *message) const
{
	LogWindowUI::LoggerUI::Instance->Add(timestamp,
		                                 gcnew System::String(level),
		                                 gcnew System::String(system),
		                                 gcnew System::String(message));
}
