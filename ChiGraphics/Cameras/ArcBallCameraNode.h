#pragma once

#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {
class CameraComponent;

class ArcBallCameraNode : public SceneNode {
 public:
  ArcBallCameraNode(float fov = 45.f,
                    float aspect = 0.75f,
                    float distance = 2.f);
  void Update(double delta_time) override;
  
  void Calibrate();

  bool bCanUpdate;
 private:
  void UpdateViewport();
  void ArcBallRotation(glm::dvec2 pos);
  void PlaneTranslation(glm::dvec2 pos);
  void DistanceZoom(glm::dvec2 pos);
  void DistanceZoom(float delta);

  float fov_;
  float distance_;
  glm::vec3 start_position_;
  glm::quat start_rotation_;
  float start_distance_;
  glm::dvec2 mouse_start_click_;
};
}  

