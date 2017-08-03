#include <getopt.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

std::ifstream in, ok, out;

void Die(const std::string& message) {
    std::cerr << message;
    exit(2);
}

struct Properties {
    std::string name;
    std::string comment;
    std::map<std::string, std::string> props;

    void setValue(std::string key, const std::string &value) {
        props[key] = value;
    }

    ///return const char * to be able to also use it as char array dirrectly
    const char* getValue(std::string key, bool dieIfMissing = false) const {
        auto it = props.find(key);
        if (it != props.end()) {
            return it->second.c_str();
        } else {
            if (dieIfMissing) {
            }
            return "";
        }
    }

    bool hasKey(std::string key) const {
        return (props.find(key) != props.end());
    }

    bool addProperty(const std::string &str, bool bExitOnError = true) {
        size_t poz = str.find('=');
        if (poz == std::string::npos) {
            if (bExitOnError) {
                Die("Can't find =");
            } else {
                return false;
            }
        }
        std::string key = str.substr(0, poz), value = str.substr(poz + 1);
        setValue(key, value);
        return true;
    }

    void addCommandLineOptions(const int argc, char *argv[]) {
        for (int i = 1; i < argc; i++) {
            addProperty(argv[i]);
        }
    }
};

void Init(int argc, char** argv) {
    Properties cmdProperties;
    cmdProperties.addCommandLineOptions(argc, argv);
    std::string in_file, ok_file, out_file;

    if (cmdProperties.hasKey("in_file")) {
        in_file = cmdProperties.getValue("in_file");
    }
    if (cmdProperties.hasKey("ok_file")) {
        ok_file = cmdProperties.getValue("ok_file");
    }
    if (cmdProperties.hasKey("out_file")) {
        out_file = cmdProperties.getValue("out_file");
    }
 
    if (in_file == "") { Die("Provide in"); }
    if (ok_file == "") { Die("Provide ok"); }
    if (out_file == "") { Die("Provide out"); }

    in.open(in_file.c_str());
    if (!in) {
        Die("Can't open in file");
    }

    ok.open(ok_file.c_str());
    if (!ok) {
        Die("Can't open ok file");
    }

    out.open(out_file.c_str());
    if (!out) {
        Die("Can't open out file");
    }
}

enum JudgementType {
    JUDGE_OK = 0,
    JUDGE_WRONG_ANSWER,
    JUDGE_PRESENTATION_ERROR,
    JUDGE_CHECKER_ERROR,
};

void Judge(JudgementType judgementType, double score, std::string message) {
    printf("[judge]\n");
    printf("judgement=%d\n", judgementType);
    printf("score=%.9f\n", score);
    printf("message=%s\n", message.c_str());

    fprintf(stderr, "[judge]\n");
    fprintf(stderr, "judgement=%d\n", judgementType);
    fprintf(stderr, "score=%.9f\n", score);
    fprintf(stderr, "message=%s\n", message.c_str());

    std::ofstream res("result.txt");
    res << (int)score << '\n' << message << '\n';
    res.close();

    fflush(stdout); fflush(stderr);
    exit(0);
}

void WA(const std::string& message = "Wrong Answer") {
    Judge(JUDGE_WRONG_ANSWER, 0.0, message);
}

void OK(const std::string& message = "OK") {
    Judge(JUDGE_OK, 1.0, message);
}

void BasicDiff() {
    std::string a, b;
    while (ok >> a) {
        if (!(out >> b)) {
            Judge(JUDGE_WRONG_ANSWER, 0, "Wrong Answer");
        }

        if (a != b) {
            Judge(JUDGE_WRONG_ANSWER, 0, "Wrong Answer");
        }
    }

    if (out >> b) {
        Judge(JUDGE_WRONG_ANSWER, 0, "Wrong Answer");
    }
}
