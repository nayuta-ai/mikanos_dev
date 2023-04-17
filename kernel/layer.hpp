/**
 *@file layer.hpp
 *Provides overlay processing.
 */
#pragma once

#include <map>
#include <memory>
#include <vector>

#include "graphics.hpp"
#include "message.hpp"
#include "window.hpp"

/** @brief Layer represents a layer.
 * Currently, only one window can be held, but in the future,
 * it may be possible to hold multiple windows.
 */
class Layer {
 public:
  /* @brief Generates a layer with the specified ID. */
  Layer(unsigned int id = 0);
  /* @brief Returns the ID of this instance. */
  unsigned int ID() const;
  /** @brief Sets the window.
   * Existing windows will be removed from this layer.
   */
  Layer& SetWindow(const std::shared_ptr<Window>& window);
  /* @brief Returns the set window. */
  std::shared_ptr<Window> GetWindow() const;
  /** @brief Get the origin coordinates of the layer. */
  Vector2D<int> GetPosition() const;
  /** @brief true to make the layer draggable. */
  Layer& SetDraggable(bool draggable);
  /** @brief Return true if the layer is draggable. */
  bool IsDraggable() const;

  /** @brief Updates the position information of the layer to the specified
   * absolute coordinates.
   * No redraw is performed.
   */
  Layer& Move(Vector2D<int> pos);
  /* @brief Updates the position information of the layer to the specified
  relative coordinates. No redraw is performed.
  */
  Layer& MoveRelative(Vector2D<int> pos_diff);
  /** @brief Draws the contents of the currently set window to the writer. */
  void DrawTo(FrameBuffer& screen, const Rectangle<int>& area) const;

 private:
  unsigned int id_;
  Vector2D<int> pos_{};
  std::shared_ptr<Window> window_{};
  bool draggable_{false};
};

/** @brief LayerManager manages multiple layers. */
// #@@range_begin(layer_manager)
class LayerManager {
 public:
  /* @brief Set the destination to draw when drawing with methods such as
   * Draw().
   */
  void SetWriter(FrameBuffer* screen);
  /** @brief Create a new layer and return a reference to it.
   *
   * The newly created layer is held in a container inside LayerManager.
   */
  Layer& NewLayer();
  /** @brief Draw the currently displayed layers. */
  void Draw(const Rectangle<int>& area) const;
  /** @brief Redraws within the drawing area of the window set on the specified
   * layer. */
  void Draw(unsigned int id) const;
  /** @brief Redraws the specified area in the window set on the specified
   * layer. */
  void Draw(unsigned int id, Rectangle<int> area) const;

  /** @brief Update the position information of the layer to the specified
   * absolute coordinate without redrawing.
   */
  void Move(unsigned int id, Vector2D<int> new_position);
  /* @brief Update the position information of the layer to the specified
   * relative coordinate without redrawing.
   */
  void MoveRelative(unsigned int id, Vector2D<int> pos_diff);

  /** @brief Move the layer to the specified height direction.
   *
   * If a negative value is specified for new_height, the layer becomes
   * invisible, and if 0 or a positive value is specified, the layer is moved to
   * that height. If a value greater than or equal to the current number of
   * layers is specified, it becomes the top layer.
   */
  void UpDown(unsigned int id, int new_height);
  /* @brief Hide the layer. */
  void Hide(unsigned int id);

  /** @brief Find the top most visible layer with a window at the specified
   * coordinates. */
  Layer* FindLayerByPosition(Vector2D<int> pos, unsigned int exclude_id) const;
  /** @brief Return the layer with the specified ID. */
  Layer* FindLayer(unsigned int id);
  /** @brief Returns the current height of the specified layer. */
  int GetHeight(unsigned int id);

 private:
  FrameBuffer* screen_{nullptr};
  mutable FrameBuffer back_buffer_{};
  std::vector<std::unique_ptr<Layer>> layers_{};
  std::vector<Layer*> layer_stack_{};
  unsigned int latest_id_{0};
};

extern LayerManager* layer_manager;

class ActiveLayer {
 public:
  ActiveLayer(LayerManager& manager);
  void SetMouseLayer(unsigned int mouse_layer);
  void Activate(unsigned int layer_id);
  unsigned int GetActive() const { return active_layer_; }

 private:
  LayerManager& manager_;
  unsigned int active_layer_{0};
  unsigned int mouse_layer_{0};
};

extern ActiveLayer* active_layer;
extern std::map<unsigned int, uint64_t>* layer_task_map;

void InitializeLayer();
void ProcessLayerMessage(const Message& msg);

constexpr Message MakeLayerMessage(uint64_t task_id, unsigned int layer_id,
                                   LayerOperation op,
                                   const Rectangle<int>& area) {
  Message msg{Message::kLayer, task_id};
  msg.arg.layer.layer_id = layer_id;
  msg.arg.layer.op = op;
  msg.arg.layer.x = area.pos.x;
  msg.arg.layer.y = area.pos.y;
  msg.arg.layer.w = area.size.x;
  msg.arg.layer.h = area.size.y;
  return msg;
}