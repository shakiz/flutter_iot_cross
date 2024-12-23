#include <flutter/dart_project.h>
#include <flutter/flutter_view_controller.h>
#include <windows.h>

#include "flutter_window.h"
#include "utils.h"
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <string>

int APIENTRY wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev,
                      _In_ wchar_t *command_line, _In_ int show_command) {
  // Attach to console when present (e.g., 'flutter run') or create a
  // new console when running with a debugger.
  if (!::AttachConsole(ATTACH_PARENT_PROCESS) && ::IsDebuggerPresent()) {
    CreateAndAttachConsole();
  }

  // Initialize COM, so that it is available for use in the library and/or
  // plugins.
  ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  flutter::DartProject project(L"data");

  std::vector<std::string> command_line_arguments =
      GetCommandLineArguments();

  project.set_dart_entrypoint_arguments(std::move(command_line_arguments));

  FlutterWindow window(project);
  Win32Window::Point origin(10, 10);
  Win32Window::Size size(1280, 720);
  if (!window.Create(L"flutter_iot_cross", origin, size)) {
    return EXIT_FAILURE;
  }
  window.SetQuitOnClose(true);

  ::MSG msg;
  while (::GetMessage(&msg, nullptr, 0, 0)) {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }

  ::CoUninitialize();
  return EXIT_SUCCESS;
}

// Load the DLL and execute the function
std::string ExecuteDllFunction(const std::string& function_name) {
    HINSTANCE hDLL = LoadLibrary(TEXT("YourLibrary.dll"));
    if (hDLL == nullptr) {
        return "Failed to load DLL";
    }

    typedef int (*FunctionType)();
    FunctionType func = (FunctionType)GetProcAddress(hDLL, function_name.c_str());
    if (func == nullptr) {
        FreeLibrary(hDLL);
        return "Failed to find function";
    }

    int result = func();
    FreeLibrary(hDLL);
    return "Function executed successfully with result: " + std::to_string(result);
}

// Handle Flutter method calls
void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue>& call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
    if (call.method_name().compare("executeDllFunction") == 0) {
        const auto* args = std::get_if<flutter::EncodableMap>(call.arguments());
        auto function_name = std::get<std::string>(args->at(flutter::EncodableValue("functionName")));

        std::string output = ExecuteDllFunction(function_name);
        result->Success(flutter::EncodableValue(output));
    } else {
        result->NotImplemented();
    }
}
