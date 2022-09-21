#include "process_interaction.h"
#include "utils.h"
#include <iostream>

using namespace std;

void reader_worker(HANDLE output, const function<void(const string&)>& handler) {
    static constexpr int BufferSize = 10;
    string current_line;

    DWORD bytes_read;
    CHAR buffer[BufferSize];
    for(;;) {
        auto success = ReadFile(output, buffer, BufferSize, &bytes_read, nullptr);
        if (bytes_read == 0) continue;
        if (!success) break;
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\r') continue;
            if (buffer[i] == '\n') {
                handler(current_line);
                current_line.clear();
            } else {
                current_line.push_back(buffer[i]);
            }
        }
    }
}

process_interaction::process_interaction(const string& exe_path,
                                         const function<void(const string&)>& output_handler) 
                                         : output_handler(output_handler) {
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_information;
    SECURITY_ATTRIBUTES security_attributes;

    ZeroMemory(&process_information, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&security_attributes, sizeof(SECURITY_ATTRIBUTES));
    security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    security_attributes.bInheritHandle = true;
    security_attributes.lpSecurityDescriptor = nullptr;

    HANDLE stdout_read = nullptr;
    HANDLE stdout_write = nullptr;
    HANDLE stdin_read = nullptr;
    HANDLE stdin_write = nullptr;
  
    HandleError(CreatePipe(&stdout_read, &stdout_write, &security_attributes, 0))
    HandleError(SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0))
    HandleError(CreatePipe(&stdin_read, &stdin_write, &security_attributes, 0))
    HandleError(SetHandleInformation(stdin_write, HANDLE_FLAG_INHERIT, 0))

    ZeroMemory(&startup_info, sizeof(STARTUPINFO));
    startup_info.cb = sizeof(STARTUPINFO);
    startup_info.hStdError = stdout_write;
    startup_info.hStdOutput = stdout_write;
    startup_info.hStdInput = stdin_read;
    startup_info.dwFlags |= STARTF_USESTDHANDLES;

    HandleError(CreateProcess(
            nullptr,
            const_cast<LPSTR>(exe_path.c_str()),
            nullptr,
            nullptr,
            true,
            0,
            nullptr,
            nullptr,
            &startup_info,
            &process_information))

    CloseHandle(process_information.hProcess);
    CloseHandle(process_information.hThread);
    CloseHandle(stdout_write);
    CloseHandle(stdin_read);

    process_input = stdin_write;
    process_output = stdout_read;
}

void process_interaction::start_reading() {
    reader_worker(this->process_output, this->output_handler);
}

void process_interaction::write(const string& text) {
//    cout << ">> " << text;
    DWORD dwWritten;
    WriteFile(process_input, text.c_str(), text.length(), &dwWritten, nullptr);
    Assert(dwWritten == text.length())
}
