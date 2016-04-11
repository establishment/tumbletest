#pragma once

#include <cstdio>

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace tumbletest {

class Path : public std::string {
  public:
    Path(const Path&) = default;
    Path(Path&&) = default;
    Path& operator=(const Path&) = default;
    Path& operator=(Path&&) = default;
    virtual ~Path() = default;

    Path(std::string location) : std::string(CheckIntegrity(location)), extension(GetExtension(location)) { }
    Path(const char* location) : std::string(CheckIntegrity(std::string(location))), extension(GetExtension(location)) { }

    std::string extension;

    Path ExtensionLess() const;

  private:
    static const std::string GetBasePath();

    std::string GetExtension(std::string current_path);
    std::string CheckIntegrity(std::string current_path);

    const static std::string default_path;
};

class OS {
  public:
    std::string RunBashCommand(const char *cmd);
    std::string RunBashCommand(const std::string cmd);
    std::string RunBashCommand(std::vector<std::string> cmds);

    void CreateArchive(std::vector<Path> files, Path archive);
    bool ValidFile(Path file);

    std::string ReadFile(Path file);
    void WriteFile(Path file, const std::string &content);

    std::string TmpFile();

    static OS& GetSingleton();

  private:
    OS();
} os = OS::GetSingleton();

const std::string Path::default_path = Path::GetBasePath();

const std::string Path::GetBasePath() {
    std::string path = os.RunBashCommand("pwd");
    path.pop_back();
    path += "/";
    return path;
}

std::string Path::CheckIntegrity(std::string current_path) {
    if (current_path[0] == '/' or current_path[0] == '~') {
        return current_path;
    } else {
        return default_path + current_path;
    }
}

std::string Path::GetExtension(std::string current_path) {
    std::string extension;
    bool ok = false;
    for (int index = int(current_path.size()) - 1; index >= 0; index -= 1) {
        if (current_path[index] == '/') {
            break;
        } else if (current_path[index] == '.') {
            ok = true;
            break;
        }
        extension += current_path[index];
    }

    if (ok) {
        reverse(extension.begin(), extension.end());
    } else {
        extension = "N/A";
    }

    return extension;
}

Path Path::ExtensionLess() const {
    std::string full_path = *this;
    while (full_path.back() != '.') {
        full_path.pop_back();
    }
    full_path.pop_back();
    return Path(full_path);
}

OS::OS() { }

OS& OS::GetSingleton() {
    static OS singleton;
    return singleton;
}

std::string OS::RunBashCommand(const char *cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

std::string OS::RunBashCommand(const std::string cmd) {
    return RunBashCommand(cmd.c_str());
}

std::string OS::RunBashCommand(std::vector<std::string> cmds) {
    std::string result = "";
    std::string cmd = "";
    for (std::string &itr : cmds) {
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
    if (archive.find(".zip") == std::string::npos) {
        zip_command = "zip -j " + archive + ".zip";
    } else {
        zip_command = "zip -j " + archive;
    }

    std::string files_to_zip = "";
    for (auto file_path : files) {
        files_to_zip  += " " + file_path;
    }

    RunBashCommand(zip_command + files_to_zip);
    RunBashCommand("rm " + files_to_zip);
}

bool OS::ValidFile(Path file) {
    std::string answer = RunBashCommand("( test -e " + file + " ) && echo \"yes\" || echo \"no\"");
    return answer == "yes\n";
}

std::string OS::ReadFile(Path file) {
    std::ifstream fin(file, std::ios::in | std::ios::binary);

    fin.seekg(0, std::ios::end);
    std::string file_information;
    file_information.resize(fin.tellg());

    fin.seekg(0, std::ios::beg);
    fin.read(&file_information[0], file_information.size());
    fin.close();
    return file_information;
}

void OS::WriteFile(Path file, const std::string &content) {
    std::ofstream fout(file);
    fout << content;
    fout.close();
    return;
}

std::string OS::TmpFile() {
    char *tmpname = strdup("/tmp/tmpfileXXXXXX");
    mkstemp(tmpname);
    return std::string(tmpname);
}
}  // namespace tumbletest