#pragma once

#include "../util/WeakPointer.h"
#include "RawImage.h"
#include "Texture.h"

namespace Core {

    class CubeTexture : public Texture {
    public:
        virtual ~CubeTexture();
        virtual void build(WeakPointer<StandardImage> front, WeakPointer<StandardImage> back, 
                           WeakPointer<StandardImage> top, WeakPointer<StandardImage> bottom, 
                           WeakPointer<StandardImage> left, WeakPointer<StandardImage> right) = 0;
        virtual void build(WeakPointer<HDRImage> front, WeakPointer<HDRImage> back, 
                           WeakPointer<HDRImage> top, WeakPointer<HDRImage> bottom, 
                           WeakPointer<HDRImage> left, WeakPointer<HDRImage> right) = 0;
    protected:
        CubeTexture(const TextureAttributes& attributes);
    };
}
