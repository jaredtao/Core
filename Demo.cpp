//
// Created by Mark Kellogg on 1/18/18.
//

#include <memory>
#include "Demo.h"
#include "common/types.h"
#include "math/Math.h"
#include "math/Matrix4x4.h"
#include "math/Quaternion.h"
#include "scene/Scene.h"
#include "render/Camera.h"
#include "material/StandardAttributes.h"
#include "geometry/Mesh.h"
#include "render/MeshRenderer.h"
#include "material/BasicCubeMaterial.h"
#include "material/BasicMaterial.h"
#include "material/StandardAttributes.h"
#include "image/RawImage.h"
#include "image/CubeTexture.h"

namespace Core {

  Demo::Demo(Engine& engine): engine(engine) {
    //this->imageLoader = engine.getImageLoader();
    //this->assetLoader = engine.getAssetLoader();
  }

  void Demo::run() {

    this->engine.onUpdate([this](Engine& engine) {

      // TODO: Remove this camera rotation code, it's only for demo purposes
      static Core::Real rotationAngle = 0.0;
      std::shared_ptr<Core::Camera> camera = engine.getCamera();
      if (camera) {
        rotationAngle += 0.01;
        if (rotationAngle >= Core::Math::TwoPI) rotationAngle -= Core::Math::TwoPI;

        Core::Quaternion qA;
        qA.fromAngleAxis(rotationAngle, 0, 1, 0);
        Core::Matrix4x4 rotationMatrixA;
        qA.rotationMatrix(rotationMatrixA);

        Core::Quaternion qB;
        qB.fromAngleAxis(-0.8, 1, 0, 0);
        Core::Matrix4x4 rotationMatrixB;
        qB.rotationMatrix(rotationMatrixB);

        Core::Matrix4x4 worldMatrix;
        worldMatrix.multiply(rotationMatrixA);

        worldMatrix.translate(0, 12, 15);
        worldMatrix.multiply(rotationMatrixB);

        camera->getLocalTransform().getMatrix().copy(worldMatrix);
      }

    });

    std::shared_ptr<Core::Scene> scene = std::make_shared<Core::Scene>();
    engine.setScene(scene);

    std::shared_ptr<Core::Mesh> skyboxMesh = std::make_shared<Core::Mesh>(36, false);
    Core::Real vertexPositions[] = {
        // back
        -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0,
        // left
        -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0,
        -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0,
        // right
        1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0,
        1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        // top
        -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0,
        -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        // bottom
        -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
        -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0, -1.0, 1.0,
        // front
        1.0, 1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0
    };

    skyboxMesh->enableAttribute(Core::StandardAttributes::Position);
    Core::Bool positionInited = skyboxMesh->initVertexPositions(36);
    ASSERT(positionInited, "Unable to initialize skybox mesh vertex positions.");
    skyboxMesh->getVertexPositions()->store(vertexPositions);

    this->skyboxMaterial = std::make_shared<Core::BasicMaterial>();
    this->skyboxMaterial->build();

    std::shared_ptr<Core::Object3D> skyboxObj = std::make_shared<Core::Object3D>();
    std::shared_ptr<Core::MeshRenderer> skyboxRenderer = std::make_shared<Core::MeshRenderer>(this->skyboxMaterial, skyboxObj);
    skyboxObj->addRenderable<Core::Mesh>(skyboxMesh);
    skyboxObj->setCustomRenderer<Core::Mesh>(skyboxRenderer);
    scene->getRoot()->addObject(skyboxObj);



   /* std::shared_ptr<Core::Scene> scene = std::make_shared<Core::Scene>();
    engine.setScene(scene);

    if (demoMode == DemoMode::Panorama) {
      std::shared_ptr<Core::Mesh> skyboxMesh = std::make_shared<Core::Mesh>(36, false);
      Core::Real vertexPositions[] = {
          // back
          -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0,
          -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0,
          // left
          -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0,
          -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0,
          // right
          1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0,
          1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
          // top
          -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0,
          -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
          // bottom
          -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
          -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0, -1.0, 1.0,
          // front
          1.0, 1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0,
          1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0
      };
      skyboxMesh->enableAttribute(Core::StandardAttributes::Position);
      Core::Bool positionInited = skyboxMesh->initVertexPositions(36);
      ASSERT(positionInited, "Unable to initialize skybox mesh vertex positions.");
      skyboxMesh->getVertexPositions()->store(vertexPositions);

      this->skyboxImages.push_back(imageLoader->loadJPEG("Pano/skybox1.jpg", false));
      this->skyboxImages.push_back(imageLoader->loadJPEG("Pano/skybox3.jpg", false));
      this->skyboxImages.push_back(imageLoader->loadJPEG("Pano/skybox0.jpg", false));
      this->skyboxImages.push_back(imageLoader->loadJPEG("Pano/skybox5.jpg", false));
      this->skyboxImages.push_back(imageLoader->loadJPEG("Pano/skybox4.jpg", false));
      this->skyboxImages.push_back(imageLoader->loadJPEG("Pano/skybox2.jpg", false));

      this->skyboxTexture = Core::CubeTexture::createCubeTexture(this->skyboxImages[0].get(), this->skyboxImages[1].get(),
                                                                this->skyboxImages[2].get(), this->skyboxImages[3].get(),
                                                                this->skyboxImages[4].get(), this->skyboxImages[5].get());

      this->skyboxMaterial = std::make_shared<Core::BasicCubeMaterial>();
      skyboxMaterial->build();
      skyboxMaterial->setSkyboxTexture(skyboxTexture);

      std::shared_ptr<Core::Object3D> skyboxObj = std::make_shared<Core::Object3D>();
      std::shared_ptr<Core::MeshRenderer> skyboxRenderer = std::make_shared<Core::MeshRenderer>(
          skyboxMaterial, skyboxObj);
      skyboxObj->addRenderable<Core::Mesh>(skyboxMesh);
      skyboxObj->setCustomRenderer<Core::Mesh>(skyboxRenderer);
      scene->getRoot()->addObject(skyboxObj);
    }
    else {
      Core::DAMLoader damLoader(assetLoader, imageLoader);
      auto meshObj = damLoader.load("hT3RtfKyx9B","3f13ceb2d2494214ad7cce632c20b770", true, true);
      scene->getRoot()->addObject(meshObj);
    }

    std::shared_ptr<Core::Camera> camera = std::make_shared<Core::Camera>();
    engine.setCamera(camera);*/
  }

}
