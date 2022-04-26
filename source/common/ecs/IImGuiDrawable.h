#pragma once

namespace our {
    // All new classes that should be drawn in ImGui should implement this interface
    // i.e. components, entitites, etc...
    class IImGuiDrawable {
    public:
        virtual void onImmediateGui() {};
    };
} // namespace our