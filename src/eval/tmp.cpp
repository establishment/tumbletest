#include <string.h> 
#include <unistd.h>

#include "tmp.hpp"
#include "../os.hpp"
#include "../utils/string.hpp"
#include "../utils/logging.hpp"

namespace tumbletest {

Path TmpFile() {
    std::string file_str = StrCat(Path::default_path, "/tumbletest/tmp/", "XXXXXX");
    char* file = strdup(file_str.c_str());
    auto descriptor = mkstemp(file);
    
    if (descriptor == -1) {
        Error("Could not create tmp file!");
    }

    close(descriptor);

    return std::string(file);
}

extern bool tmp_is_init;
bool InitTmp() {
    if (tmp_is_init) {
        return true;
    }

    os.RunBashCommand("mkdir -p " + tmp_root); 
    return true;
}
bool tmp_is_init = InitTmp();
}
