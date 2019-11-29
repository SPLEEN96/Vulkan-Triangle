#pragma once
#include "Rendering/RenderBackend.h"

namespace Firefly {
namespace Rendering {
class RenderModule {
  public:
    void Load(Window window) {
        if (!is_loaded) {
            backend.Init(window);
            is_loaded = true;
        }
    }

    void OnUpdate() { backend.OnUpdate(); }

  private:
    RenderBackend backend;
    bool          is_loaded = false;
};
} // namespace Rendering
} // namespace Firefly