/**

@file window.hpp
Provides a Window class that represents a display area.
*/
#pragma once

#include <optional>
#include <vector>

#include "frame_buffer.hpp"
#include "graphics.hpp"

/** @brief The Window class represents a graphics display area.
 * This includes not only windows with titles and menus, but also display areas
 * for the mouse cursor, etc.
 */

class Window {
 public:
  /* @brief WindowWriter provides the associated PixelWriter of a Window.
   */
  class WindowWriter : public PixelWriter {
   public:
    WindowWriter(Window& window) : window_{window} {}
    /* @brief Draws the specified color at the specified position. */
    virtual void Write(Vector2D<int> pos, const PixelColor& c) override {
      window_.Write(pos, c);
    }
    /* @brief Returns the width of the associated Window in pixels. */
    virtual int Width() const override { return window_.Width(); }
    /* @brief Returns the height of the associated Window in pixels. */
    virtual int Height() const override { return window_.Height(); }

   private:
    Window& window_;
  };
  // #@@range_end(windowwriter)

  /** @brief Creates a plain drawing area with the specified number of pixels.
   */
  Window(int width, int height, PixelFormat shadow_format);
  ~Window() = default;
  Window(const Window& rhs) = delete;
  Window& operator=(const Window& rhs) = delete;

  /** @brief Draws the display area of this window to the specified PixelWriter.
   * @param writer The destination of the drawing.
   * @param position The position to draw on the writer, relative to its
   * top-left corner.
   */
  void DrawTo(FrameBuffer& dst, Vector2D<int> position);
  /* @brief Sets the transparent color. */
  void SetTransparentColor(std::optional<PixelColor> c);
  /** @brief Gets the WindowWriter associated with this instance. */
  WindowWriter* Writer();

  /* @brief Returns the pixel at the specified position. */
  const PixelColor& At(Vector2D<int> pos) const;
  /** @brief Write the pixel at the specified position. */
  void Write(Vector2D<int> pos, PixelColor c);

  /** @brief Returns the width of the drawing area in pixels. */
  int Width() const;
  /* @brief Returns the height of the drawing area in pixels. */
  int Height() const;

  /** @brief Moves a rectangular area within the flat drawing area of this
   * window.
   *
   * @param src_pos The origin of the rectangular area to be moved.
   * @param src_size The size of the rectangular area to be moved.
   * @param dst_pos The destination of the origin of the rectangular area.
   */
  void Move(Vector2D<int> dst_pos, const Rectangle<int>& src);

 private:
  int width_, height_;
  std::vector<std::vector<PixelColor>> data_{};
  WindowWriter writer_{*this};
  std::optional<PixelColor> transparent_color_{std::nullopt};

  FrameBuffer shadow_buffer_{};
};

void DrawWindow(PixelWriter& writer, const char* title);
