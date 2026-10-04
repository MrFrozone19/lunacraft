
#include <glm/gtc/matrix_transform.hpp>

#include "viewport.h"
#include "constants.h"

// Far enough to reach the corners of the loaded chunk patch at max render distance
constexpr double FAR_PLANE = (MAX_RENDER_DISTANCE + 2) * CHUNK_SIZE * 1.5;

glm::dvec2 Viewport::dimensions_;
glm::dvec2 Viewport::last_mouse_pos_;
glm::mat4 Viewport::proj_matrix_;
float Viewport::fov_ = 45.0f;

void Viewport::SetDimensions(const glm::ivec2 &dimensions)
{
    dimensions_ = dimensions;
    UpdateProjectionMatrix();
}

glm::ivec2 Viewport::GetDimensions()
{
    return dimensions_;
}

// Vertical field of view, in degrees
void Viewport::SetFov(float fov)
{
    fov_ = fov;
    UpdateProjectionMatrix();
}

float Viewport::GetFov()
{
    return fov_;
}

void Viewport::UpdateProjectionMatrix()
{
    proj_matrix_ = glm::perspective(glm::radians((double)fov_), dimensions_.x / dimensions_.y, 0.1, FAR_PLANE);
}

glm::mat4 Viewport::GetProjectionMatrix()
{
    return proj_matrix_;
}
