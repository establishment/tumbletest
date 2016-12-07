#pragma once
#include <string>

namespace tumbletest {
    namespace Color {
        static const std::string black =            "\033[22;30m";
        static const std::string red =              "\033[22;31m";
        static const std::string green =            "\033[22;32m";
        static const std::string brown =            "\033[22;33m";
        static const std::string blue =             "\033[22;34m";
        static const std::string magenta =          "\033[22;35m";
        static const std::string cyan =             "\033[22;36m";
        static const std::string gray =             "\033[22;37m";
        static const std::string dark_gray =        "\033[01;30m";
        static const std::string light_red =        "\033[01;31m";
        static const std::string light_green =      "\033[01;32m";
        static const std::string yellow =           "\033[01;33m";
        static const std::string light_blue =       "\033[01;34m";
        static const std::string light_magenta =    "\033[01;35m";
        static const std::string light_cyan =       "\033[01;36m";
        static const std::string white =            "\033[01;37m";

        static const std::string default_color =    "\033[0m";
    };

    std::string Colored(std::string color, const std::string& text) {
        return color + text + Color::default_color;
    }
} // namespace tumbletest
