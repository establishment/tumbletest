#pragma once

#include <string>
#include <iostream>

namespace tumbletest {

    std::string RGBColor(int a, int b, int c) {
        int index = a * 6 * 6 + b * 6 + c + 16; 
        auto str = std::to_string(index);
        return "\033[38;5;" + str + "m";
    }

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

    std::string Colored(int a, int b, int c, const std::string& text) {
        return RGBColor(a, b, c) + text + Color::default_color;
    }   
 
    std::string Gradient(const std::string txt) {
        std::string finish = "";
        static int l[6] = {1, 5, 1, 5, 1, 5};
        static int a = l[0] + rand() % (l[1] - l[0]), b = l[2] + rand() % (l[3] - l[2]), c = l[4] + rand() % (l[5] - l[4]), d = rand() % 6;
        static int da[6] = {0, 0, 0, 0, +1, -1};
        static int db[6] = {0, 0, +1, -1, 0, 0};
        static int dc[6] = {+1, -1, 0, 0, 0, 0};
        for (int i = 0; i < (int)txt.size(); i += 1) {
            int na, nb, nc;
            while (1) {
                na = a + da[d];
                nb = b + db[d];
                nc = c + dc[d];
                if (l[0] <= na and na < l[1] and l[2] <= nb and nb < l[3] and l[4] <= nc and nc < l[5]) {
                    break;
                } else {
                    d = rand() % 6;
                }
            }
            a = na; b = nb; c = nc;
            std::string f(1, txt[i]);
            if (txt[i] < 0) {
                i += 1;
                f += std::string(1, txt[i]);
            }
            finish += Colored(a, b, c, f);
//            finish += std::string(1, ch);
        }
        return finish;
    }

    std::string GetBloatware() {
        static int next = 0;
        std::string bloatware[2] = {
            "¸,ø¤º°°º¤ø,¸,ø¤º°°º¤ø,",
            "°º¤ø,¸¸,ø¤º°º¤ø,¸¸,ø¤°"
        };

        return Gradient(bloatware[(next++)&1]);
    }

} // namespace tumbletest
