#include "util.h"
#include <cstring>

namespace our {
    std::vector<char> stringsToImGuiComboList(std::vector<std::string> strings) {
        int buffSize = 0;
        for(auto&& e : strings) {
            buffSize += e.size() + 1; // account for null terminators
        }

        std::vector<char> list(buffSize + 1); // For one last terminating null
        for(int listPtr = 0, i = 0; listPtr < buffSize;) {
            std::memcpy(&list.data()[listPtr], strings[i].data(), strings[i].size());
            listPtr += strings[i].size();
            list[listPtr++] = '\0';
            i++;
        }
        list[buffSize] = '\0';
        return list;
    }

    btVector3 glmToBtVector3(glm::vec3 glmVec) {
        return {glmVec.x, glmVec.y, glmVec.z};
    }
} // namespace our