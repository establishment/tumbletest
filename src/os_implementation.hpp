#pragma once

#include "os.hpp"  // header include

#include "logging.hpp"

#include <unistd.h>
#include <cstring>

#include <fstream>
#include <string>
#include <vector>

namespace tumbletest {

const std::string Path::default_path = Path::GetBasePath();

OS& os = OS::GetSingleton();                                          // singleton for OS
TumbletestCache& tumbletest_cache = TumbletestCache::GetSingleton();  // singleton for cache

/*
 * Random operators for path to work well with std::string
 */
std::string operator+(const std::string& lhs, const Path& rhs) {
    return lhs + rhs.to_string();
}

std::string operator+(const Path& lhs, const std::string& rhs) {
    return lhs.to_string() + rhs;
}

std::ostream& operator<<(std::ostream& out, const Path& rhs) {
    out << rhs.to_string();
    return out;
}

/*
 * Run path for the binary
 */
std::string Path::GetBasePath() {
    std::string path = os.RunBashCommand("pwd");
    path.pop_back();
    return path;
}

/*
 * Absolute path for a given path (if it's only "file") the path is considered to be
 * relative to the binary spawn not generator's code path
 */
std::string Path::GetAbsolutePath(const std::string& current_path) const {
    if (current_path.size() == 0) {
        Error("Path is empty.");
    }

    if (current_path[0] == '/' or current_path[0] == '~') {
        return current_path;
    } else {
        return default_path + "/" + current_path;
    }
}

/*
 * TODO(@velea) check if the path is a file or not
 */
std::string Path::File() const {
    auto position = absolute_path.find_last_of("/");
    return absolute_path.substr(position + 1, absolute_path.size() - position);
}

std::string Path::Folder() const {
    auto position = absolute_path.find_last_of("/");
    return absolute_path.substr(0, position);
}

std::string Path::Extension() const {
    std::string file = File();
    auto position = file.find(".");
    return file.substr(position + 1, file.size() - position);
}

std::string Path::ExtensionLess() const {
    auto file = File();
    auto position = file.find(".");
    return file.substr(0, position);
}

/*
 * md5 checksum of a file. Only the checksum.
 */
std::string Path::md5() const {
    std::string result = "";
#ifdef __APPLE__
    auto md5 = os.RunBashCommand("md5 " + absolute_path);
    auto position = md5.find_last_of("=");
    result = md5.substr(position + 2, md5.size() - position - 3);
#elif __linux__
    auto md5sum = os.RunBashCommand("md5sum " + absolute_path);
    result = md5sum.substr(0, 32);
#else
#error "gtfo windows"
#endif

    return result;
}

OS::OS() {
}

std::string OS::RunBashCommand(const char* cmd) {
    //    Info("Runbashcommand: ", cmd);
    FILE* pipe(popen(cmd, "r"));
    if (!pipe)
        return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

std::string OS::RunBashCommand(const std::string cmd) {
    return RunBashCommand(cmd.c_str());
}

std::string OS::RunBashCommand(std::vector<std::string> cmds) {
    std::string cmd = "";
    for (std::string& itr : cmds) {
        cmd += itr;
        cmd += ';';
    }
    return RunBashCommand(cmd);
}

void OS::CreateArchive(std::vector<Path> files, Path archive) {
    if (files.empty()) {
        return;
    }

    std::string zip_command = "";
    if (archive.Extension() == ".zip") {
        zip_command = "zip -j " + archive + ".zip";
    } else {
        zip_command = "zip -j " + archive;
    }

    std::string files_to_zip = "";
    for (auto file_path : files) {
        files_to_zip += " " + file_path;
    }

    RunBashCommand(zip_command + files_to_zip);
    RunBashCommand("rm " + files_to_zip);
}

bool OS::ValidFile(Path file) {
    std::string answer = RunBashCommand("( test -e " + file + " ) && echo \"yes\" || echo \"no\"");
    return answer == "yes\n";
}

std::string OS::ReadFile(Path file) {
    if (not ValidFile(file)) {
        Error("Invalid file.(", file, ")");
    }

    std::ifstream fin(file, std::ios::in | std::ios::binary);
    std::string file_information;

    if (fin) {
        fin.seekg(0, fin.end);
        file_information.resize(fin.tellg());

        fin.seekg(0, fin.beg);

        fin.read(&file_information[0], file_information.size());
        fin.close();
    } else {
        Error("Error while reading.");
    }

    return file_information;
}

void OS::WriteFile(Path file, const std::string& content) {
    std::ofstream fout(file);
    fout << content;
    fout.close();
    return;
}

Path OS::TmpFile() {
    std::string file_str = StrCat(Path::default_path, "/tumbletest/tmp/", "XXXXXX");
    char* file = strdup(file_str.c_str());
    auto descriptor = mkstemp(file);

    if (descriptor == -1) {
        Error("Could not create file!");
    }

    close(descriptor);

    return std::string(file);
}

TumbletestCache::TumbletestCache() {
    os.RunBashCommand("mkdir -p " + Path::default_path + "/tumbletest");
    os.RunBashCommand("mkdir -p " + Path::default_path + "/tumbletest/tmp");
    os.RunBashCommand("mkdir -p " + Path::default_path + "/tumbletest/src");
}

void TumbletestCache::ClearTmp() {
    os.RunBashCommand("rm " + Path::default_path + "/tumbletest/tmp/*");
}

}  // namespace tumbletest

/*
 * std::to_string for Path
 */
namespace std {
string to_string(const tumbletest::Path& path) {
    return path.to_string();
}
}  // namespace std
