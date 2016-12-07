#include "builder.hpp"

#include <new>

#include "../os.hpp"
#include "../utils/logging.hpp"
#include "tmp.hpp"
#include "jailer.hpp"

namespace tumbletest {

Path Source::compile_root = StrCat(tmp_root, "/compiled");

std::string Hash(const std::string& text) {
    static const int kNumHashes = 4;
    static const int kHashes[kNumHashes] = {(int)1e9+7, (int)1e9+9, (int)1e9+11, (int)1e9+17};
    static const int kSigma = 273;
    
    int values[kNumHashes] = {0, 0, 0, 0};
    for (int i = 0; i < 4; i += 1) {
        for (char itr : text) {
            values[i] = (1LL * values[i] * kSigma + itr) % kHashes[i];
        }
    }

    std::string result;
    for (int i = 0; i < 4; i += 1) {
        for (int j = 0; j < 6; j += 1) {
            int v = values[i] % 52;
            values[i] /= 52;
            if (v < 26) {
                result += char('a' + v);
            } else {
                result += char('A' + v - 26);
            }
        }
    }

    return result;
}

// c++
RunStats ProgrammingLanguage::CPP::Compile(const Path& source) {
    Info("Compiling", "\t", source);
    compile_jailer.CopyToSandbox(StrCat(source.Folder(), "/*"), ""); 

    auto result = compile_jailer.RunCommand(StrCat("/usr/bin/g++ -O2 -std=c++14 -Wall ", source.File(), " -o _", source.ExtensionLess()));
    if (result.resultCode != RunStats::OK) {
        Error("Failed to compile ", source.File(), "\n",
                compile_jailer.stderr(), "\n");
    }

    compile_jailer.CopySandbox(source.Folder());
    compile_jailer.ClearSandbox();
    return result;
}

std::string ProgrammingLanguage::CPP::RunCommand(const Path& source) {
    return StrCat("./_", source.ExtensionLess());
}

RunStats ProgrammingLanguage::Java::Compile(const Path& source) {
    Info("Compiling", "\t", source);
    compile_jailer.CopyToSandbox(source, source.File()); 

    auto result = compile_jailer.RunCommand(StrCat("/usr/lib/jvm/java-8-openjdk-amd64/bin/javac ", source.File()));
    if (result.resultCode != RunStats::OK) {
        Error("Failed to compile ", source.File(), "\n",
                compile_jailer.stderr(), "\n");
    }

    compile_jailer.CopySandbox(source.Folder());
    compile_jailer.ClearSandbox();
    return result;
}

std::string ProgrammingLanguage::Java::RunCommand(const Path& source) {
    return StrCat("/usr/lib/jvm/java-8-openjdk-amd64/bin/java  -Xmx4g -Xss32m -DONINE_JUDGE -DCS_ACADEMY ", source.ExtensionLess());
}

// python
RunStats ProgrammingLanguage::Python::Compile(const Path& source) {
    Info("Compiling", "\t", source);
    compile_jailer.CopyToSandbox(".*", source.Folder() + "/"); 

    auto result = compile_jailer.RunCommand(StrCat("/usr/bin/python3 -m py_compile ", source.File()));
    if (result.resultCode != RunStats::OK) {
        Error("Failed to compile ", source.File(), "\n",
                compile_jailer.stderr(), "\n");
    }

    compile_jailer.CopySandbox(source.Folder());
    compile_jailer.ClearSandbox();
    return result;   
}

std::string ProgrammingLanguage::Python::RunCommand(const Path& source) {
    return StrCat("/usr/python3 ", source.File());
}

ProgrammingLanguage::Generic* ProgrammingLanguage::GetLanguage(const Path& source) {
    if (source.Extension() == "cpp") {
        return new ProgrammingLanguage::CPP();
    } else if (source.Extension() == "java") {
        return new ProgrammingLanguage::Java();
    } else if (source.Extension() == "py") {
        return new ProgrammingLanguage::Python();
    }
    Error("No language with the extension ", source.Extension(), " found");
    return nullptr;
}

Source::Source(const Path& source) : content(os.ReadFile(source)), hash(Hash(content)), lang(ProgrammingLanguage::GetLanguage(source)), path(StrCat(compile_root, "/", hash, "/", source.File())) { }

Source::~Source() {
    delete lang;
}

Path Source::CompileFolder() {
   return StrCat(compile_root, "/", hash); 
}

RunStats Source::Compile() {
    if (os.ValidFile(StrCat(compile_root, "/", hash))) {
        RunStats stats;
        stats.resultCode = RunStats::OK;
        return stats;
    }

    os.RunBashCommand(StrCat("mkdir -p ", compile_root, "/", hash));
    os.WriteFile(path, content);

    return lang->Compile(path);
}

std::string Source::RunCommand() {
    this->Compile();
    return lang->RunCommand(path);
}

extern int builder_is_init;
bool InitBuilder() {
    if (builder_is_init) {
        return true;
    }
    
    os.RunBashCommand(StrCat("mkdir -p ", Source::compile_root));
    return true;
}
int builder_is_init = InitBuilder();
} // namespace tumbletest
