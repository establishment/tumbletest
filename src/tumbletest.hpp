#pragma once

#include <string>

void RunTumbletest() {
    tumbletest::test_archive.Run();
}

void SetTestsLocation(const std::string &location) {
    tumbletest::test_archive.SetTestsLocation(location);
}
