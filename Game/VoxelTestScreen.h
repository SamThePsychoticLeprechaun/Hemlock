#pragma once

#include <app/IScreen.h>
#include <camera/BasicFPSCamera.h>
#include <graphics/GLSLProgram.h>
#include <ui/InputDispatcher.h>
#include <ui/InputManager.h>
#include <procedural/Noise.hpp>
#include <voxel/ChunkGrid.h>

#include "Constants.hpp"

class VoxelTestScreen : public happ::IScreen {
public:
    VoxelTestScreen(hui::InputManager* inputManager) :
        m_inputManager(inputManager)
    { /* EMPTY */ }

    void init(const char* name) override;
    void dispose() override;

    void update(TimeData time) override;
    void draw(TimeData time) override;

    bool handleMouseMovement(h::Sender sender, hui::MouseMoveEvent event);
    bool handleMouseCapture(h::Sender sender, hui::KeyboardButtonEvent event);
private:
    hcam::BasicFPSCamera  m_camera;
    hg::GLSLProgram       m_shader;
    hui::InputManager*    m_inputManager;
    
    GLuint m_texture1;
    GLuint m_texture2;

    hvox::ChunkGrid m_chunkGrid;

    union {
        GLuint m_quadVAOs[6];
        struct {
            GLuint m_frontVAO;
            GLuint m_backVAO;
            GLuint m_leftVAO;
            GLuint m_rightVAO;
            GLuint m_bottomVAO;
            GLuint m_topVAO;
        };
    };

    glm::f64vec3 m_chunkLoc = glm::f64vec3(0, 0, 0);
};
