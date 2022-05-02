#pragma once

#include "ecs/IImGuiDrawable.h"
#include "imgui.h"
#include "pipeline-state.hpp"
#include "../texture/texture2d.hpp"
#include "../texture/sampler.hpp"
#include "../shader/shader.hpp"

#include <glm/vec4.hpp>
#include <json/json.hpp>
#include <string>

namespace our {

    // This is the base class for all the materials
    // It contains the 3 essential components required by any material
    // 1- The pipeline state when drawing objects using this material
    // 2- The shader program used to draw objects using this material
    // 3- Whether this material is transparent or not
    // Materials that send uniforms to the shader should inherit from the is material and add the required uniforms
    class Material : public IImGuiDrawable {
    public:
        PipelineState pipelineState;
        ShaderProgram* shader;
        bool transparent;
        
        // This function does 2 things: setup the pipeline state and set the shader program to be used
        virtual void setup() const;
        // This function read a material from a json object
        virtual void deserialize(const nlohmann::json& data);

        virtual void onImmediateGui() {
            ImGui::LabelText("", "Base material or unimplemented");
        }
    };

    // This material adds a uniform for a tint (a color that will be sent to the shader)
    // An example where this material can be used is when the whole object has only color which defined by tint
    class TintedMaterial : public Material {
    public:
        glm::vec4 tint;
        bool colorPicker;

        void setup() const override;
        void deserialize(const nlohmann::json& data) override;
        virtual void onImmediateGui() override {
            std::string materialId = std::to_string((long long)this);
            std::string tintName = "Tint ##" + materialId;
            
            ImGui::Spacing();

            ImGui::Checkbox(("Color picker ##"+materialId).c_str(), &colorPicker);
            if(colorPicker)
                ImGui::ColorPicker4(tintName.c_str(), &tint.x);
            else
                ImGui::DragFloat4(tintName.c_str(),&tint.x, 0.01, 0, 2);

        }
    };

    // This material adds two uniforms (besides the tint from Tinted Material)
    // The uniforms are:
    // - "tex" which is a Sampler2D. "texture" and "sampler" will be bound to it.
    // - "alphaThreshold" which defined the alpha limit below which the pixel should be discarded
    // An example where this material can be used is when the object has a texture
    class TexturedMaterial : public TintedMaterial {
    public:
        Texture2D* texture;
        Sampler* sampler;
        float alphaThreshold;

        void setup() const override;
        void deserialize(const nlohmann::json& data) override;
    };

    // This function returns a new material instance based on the given type
    inline Material* createMaterialFromType(const std::string& type){
        if(type == "tinted"){
            return new TintedMaterial();
        } else if(type == "textured"){
            return new TexturedMaterial();
        } else {
            return new Material();
        }
    }

}