#pragma once

#include "ChiGraphics/SceneNode.h"

namespace CHISTUDIO {
class CameraComponent;

/** Camera node used for rendering the scene viewport. 
 *  Right click and drag to rotate the camera, middle click and
 *  drag to translate. Mouse wheel to zoom.
 * 
 *  TODO: Redo this class to be more usable, including rotating around an up-vector,
 *  allowing the center focus point to be set, and use orthographic rendering.
 */
class ArcBallCameraNode : public SceneNode {
 public:
  ArcBallCameraNode(float fov = 45.f,
                    float aspect = 0.75f,
                    float distance = 2.f);
  void Update(double delta_time) override;
  
  /** Set start position and rotation */
  void Calibrate();

  bool bCanUpdate;

 private:
  /** Check inputs for necessary updates, then update the camera view matrix. */
  void UpdateViewport();

  /** Calculate rotation for new mouse position. */
  void ArcBallRotation(glm::dvec2 pos);

  /** Calculate translation for new mouse position. */
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

