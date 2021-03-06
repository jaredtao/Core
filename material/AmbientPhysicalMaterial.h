#pragma once

#include "../util/WeakPointer.h"
#include "StandardPhysicalMaterial.h"
#include "../common/Constants.h"

namespace Core {

    // forward declarations
    class Engine;
    class Texture;

    class AmbientPhysicalMaterial : public StandardPhysicalMaterial {
        friend class Engine;

    public:
         virtual Bool build() override;
         
    protected:
        AmbientPhysicalMaterial(WeakPointer<Graphics> graphics);
    
    };
}
