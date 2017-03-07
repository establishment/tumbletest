#pragma once

#include "good_defines.hpp"

#include <ostream>
#include <string>
#include <vector>

namespace tumbletest {

/*
 * Unifies the concept of a path.
 * Everything should be absolute so there's no need to worry about that.
 */
class Path {
  public:
    MakeCopyAndMove(Path)

        virtual ~Path() = default;

    Path(const std::string& location) : absolute_path(GetAbsolutePath(location)) {
    }

    Path(const char* location) : absolute_path(GetAbsolutePath(std::string(location))) {
    }

    // string shits
    std::string to_string() const {
        return absolute_path;
    }

    operator std::string() const {
        return absolute_path;
    }

    std::string Extension() const;

    std::string ExtensionLess() const;

    std::string File() const;

    std::string Folder() const;

    std::string md5() const;

  private:
    std::string absolute_path;

    std::string GetAbsolutePath(const std::string& input_path) const;

    static std::string GetBasePath();

  public:
    const static std::string default_path;
};

/*
 * Random utilities when working with the os.
 * It can be changed when moving to other OSes
 */
class OS {
  public:
    std::string RunBashCommand(const char* cmd);

    std::string RunBashCommand(const std::string cmd);

    std::string RunBashCommand(std::vector<std::string> cmds);

    void CreateArchive(std::vector<Path> files, Path archive);

    bool ValidFile(Path file);

    std::string ReadFile(Path file);

    void WriteFile(Path file, const std::string& content);

    Path TmpFile();

    PermanentSingleton(OS)
};

/*
 * Responsible for storing auxiliar data for the process.
 */
class TumbletestCache {
  public:
    void ClearTmp();

    PermanentSingleton(TumbletestCache);
};

}  // namespace tumbletest

#include "os_implementation.hpp"  // include implementation
