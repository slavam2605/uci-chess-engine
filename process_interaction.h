#ifndef CHESSUCIENGINE_PROCESS_INTERACTION_H
#define CHESSUCIENGINE_PROCESS_INTERACTION_H

#include <string>
#include <functional>
#include <windows.h>
#include <thread>

class process_interaction {
    std::function<void(const std::string&)> output_handler;
    HANDLE process_input;
    HANDLE process_output;
    
public:
    process_interaction(const std::string& exe_path, const std::function<void(const std::string&)>& output_handler);
    void write(const std::string& text);
    void start_reading();
};


#endif //CHESSUCIENGINE_PROCESS_INTERACTION_H
