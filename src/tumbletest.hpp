#pragma once

#include <string>
#include <vector>

#include "test_archive.hpp"
#include "macros.hpp"

void RunTumbletest() {
    tumbletest::test_archive.Run();
}

void SetTestsLocation(const std::string &location) {
    tumbletest::test_archive.TestsLocation(location);
}

void SetOfficialSource(std::string source) {
    tumbletest::test_archive.OfficialSource(source);
}

void TestSources(std::string source) {
    tumbletest::test_archive.TestSources(100, {tumbletest::Path(source)});
}

void TestSources(int runs, std::string source) {
    tumbletest::test_archive.TestSources(runs, {tumbletest::Path(source)});
}

void TestSources(const std::vector<std::string>& _sources) {
    std::vector<tumbletest::Path> sources = {};
    for (const std::string& itr : _sources) {
        sources.push_back(tumbletest::Path(itr));
    }

    tumbletest::test_archive.TestSources(100, sources);
}

void TestSources(int runs, const std::vector<std::string>& _sources) {
    std::vector<tumbletest::Path> sources = {};
    for (const std::string& itr : _sources) {
        sources.push_back(tumbletest::Path(itr));
    }

    tumbletest::test_archive.TestSources(runs, sources);
}

