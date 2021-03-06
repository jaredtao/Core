#include "TextureAttr.h"

namespace Core {
    
    TextureAttributes::TextureAttributes() {
        UseAlpha = false;
        WrapMode = TextureWrap::Repeat;
        FilterMode = TextureFilter::Point;
        MipLevels = 0;
        IsDepthTexture = false;
        Format = TextureFormat::RGBA8;
    }

    TextureAttributes::~TextureAttributes() {

    }
    
}