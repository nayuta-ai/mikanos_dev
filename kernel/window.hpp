/**

@file window.hpp
Provides a Window class that represents a display area.
*/
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "frame_buffer.hpp"
#include "graphics.hpp"

enum class WindowRegion {
  kTitleBar,
  kCloseButton,
  kBorder,
  kOther,
};

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
  virtual ~Window() = default;
  Window(const Window& rhs) = delete;
  Window& operator=(const Window& rhs) = delete;

  /** @brief Draw the display area of this window in the given FrameBuffer.
   *
   * @param dst where to draw to
   * @param pos The window position relative to the upper left of dst
   * @param area The area to be drawn with respect to the upper left of dst
   */
  void DrawTo(FrameBuffer& dst, Vector2D<int> pos, const Rectangle<int>& area);
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
  /** @brief Returns the height of the drawing area in pixels. */
  int Height() const;
  /** @brief Returns the size of the drawing area in pixels. */
  Vector2D<int> Size() const;

  /** @brief Moves a rectangular area within the flat drawing area of this
   * window.
   *
   * @param src_pos The origin of the rectangular area to be moved.
   * @param src_size The size of the rectangular area to be moved.
   * @param dst_pos The destination of the origin of the rectangular area.
   */
  void Move(Vector2D<int> dst_pos, const Rectangle<int>& src);

  virtual void Activate() {}
  virtual void Deactivate() {}
  virtual WindowRegion GetWindowRegion(Vector2D<int> pos);

 private:
  int width_, height_;
  std::vector<std::vector<PixelColor>> data_{};
  WindowWriter writer_{*this};
  std::optional<PixelColor> transparent_color_{std::nullopt};

  FrameBuffer shadow_buffer_{};
};

class ToplevelWindow : public Window {
 public:
  static constexpr Vector2D<int> kTopLeftMargin{4, 24};
  static constexpr Vector2D<int> kBottomRightMargin{4, 4};
  static constexpr int kMarginX = kTopLeftMargin.x + kBottomRightMargin.x;
  static constexpr int kMarginY = kTopLeftMargin.y + kBottomRightMargin.y;

  class InnerAreaWriter : public PixelWriter {
   public:
    InnerAreaWriter(ToplevelWindow& window) : window_{window} {}
    virtual void Write(Vector2D<int> pos, const PixelColor& c) override {
      window_.Write(pos + kTopLeftMargin, c);
    }
    virtual int Width() const override {
      return window_.Width() - kTopLeftMargin.x - kBottomRightMargin.x;
    }
    virtual int Height() const override {
      return window_.Height() - kTopLeftMargin.y - kBottomRightMargin.y;
    }

   private:
    ToplevelWindow& window_;
  };

  ToplevelWindow(int width, int height, PixelFormat shadow_format,
                 const std::string& title);

  virtual void Activate() override;
  virtual void Deactivate() override;
  virtual WindowRegion GetWindowRegion(Vector2D<int> pos) override;

  InnerAreaWriter* InnerWriter() { return &inner_writer_; }
  Vector2D<int> InnerSize() const;

 private:
  std::string title_;
  InnerAreaWriter inner_writer_{*this};
};

void DrawWindow(PixelWriter& writer, const char* title);
void DrawTextbox(PixelWriter& writer, Vector2D<int> pos, Vector2D<int> size);
void DrawTerminal(PixelWriter& writer, Vector2D<int> pos, Vector2D<int> size);
void DrawWindowTitle(PixelWriter& writer, const char* title, bool active);
