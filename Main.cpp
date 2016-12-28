#include "Log.h"
#include "SyringeDebugger.h"
#include "Support.h"

#include <string>

#include <commctrl.h>

int Run(char* const lpCmdLine) {
	constexpr auto const VersionString = "Syringe 0.7.0.6";

	InitCommonControls();

	Log::Open("syringe.log");

	Log::WriteLine(VersionString);
	Log::WriteLine("===============");
	Log::WriteLine();
	Log::WriteLine("WinMain: lpCmdLine = \"%s\"", lpCmdLine);

	auto failure = "Could not load executable.";
	auto exit_code = ERROR_ERRORS_ENCOUNTERED;

	try
	{
		auto const command = get_command_line(lpCmdLine);

		if(!command.flags.empty()) {
			// artificial limitation
			throw invalid_command_arguments{};
		}

		Log::WriteLine(
			"WinMain: Trying to load executable file \"%.*s\"...",
			printable(command.executable));
		Log::WriteLine();

		SyringeDebugger Debugger{ command.executable };
		failure = "Could not run executable.";

		Log::WriteLine("WinMain: SyringeDebugger::FindDLLs();");
		Log::WriteLine();
		Debugger.FindDLLs();

		Log::WriteLine(
			"WinMain: SyringeDebugger::Run(\"%.*s\");",
			printable(command.arguments));
		Log::WriteLine();

		Debugger.Run(command.arguments);
		Log::WriteLine("WinMain: SyringeDebugger::Run finished.");
		Log::WriteLine("WinMain: Exiting on success.");
		return ERROR_SUCCESS;
	}
	catch(lasterror const& e)
	{
		Log::WriteLine("WinMain: %s (%d)", e.message.c_str(), e.error);

		auto const msg = std::string(failure) + "\n\n" + e.message;
		MessageBoxA(nullptr, msg.c_str(), VersionString, MB_OK | MB_ICONERROR);

		exit_code = e.error;
	}
	catch(invalid_command_arguments const& e)
	{
		MessageBoxA(
			nullptr, "Syringe cannot be run just like that.\n\n"
			"Usage:\nSyringe.exe \"<exe name>\" <arguments>",
			VersionString, MB_OK | MB_ICONINFORMATION);

		Log::WriteLine(
			"WinMain: No or invalid command line arguments given, exiting...");

		exit_code = ERROR_INVALID_PARAMETER;
	}

	Log::WriteLine("WinMain: Exiting on failure.");
	return exit_code;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdShow);

	return Run(lpCmdLine);
}
