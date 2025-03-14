#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/tema2_camera.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };
        struct Tree {
            glm::vec3 position;
            float trunkHeight;
            float trunkRadius;
            float leafHeight;
            float leafRadius;
        };
        struct House {
            glm::vec3 position;
            glm::vec3 size;
            float roofHeight;
        };
        struct Package {
            glm::vec3 position;
            bool isCarried;
        };
        struct Globe {
            std::string meshName;
            glm::vec3 position;
        };


        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int isTerrain, implemented::Camera* activeCamera);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void RenderScene();
        void GenerateTerrain(int rows, int cols);
        void GenerateObstacles(int numObstacles);
        bool CheckSphereConeCollision(glm::vec3 sphereCenter, float sphereRadius, glm::vec3 coneBase, float coneRadius, float coneHeight);
        bool CheckCollisionWithObstacles();
        bool CheckCollisionWithTerrain();
        Mesh* CreateCylinder(const std::string& name, glm::vec3 color, float radius, float height, int numSegments);
        Mesh* CreateCone(const std::string& name, glm::vec3 color, float radius, float height, int numSegments);
        Mesh* CreateParallelepiped(const std::string& name, glm::vec3 color, float width, float height, float depth);
        void GenerateHouses(int count);
        void RenderHouses();
        void RenderDrone(glm::vec3 position);
        void GenerateNewPackage();
        void CheckPackageCollision();
        void CheckDelivery();
        void RenderPackage();
        void RenderArrowInFrontOfDrone(glm::vec3 startPosition, glm::vec3 targetPosition);
        Mesh* CreateGift(const std::string& name, glm::vec3 boxColor, glm::vec3 ribbonColor, float width, float height, float depth);
        void UpdateMinimapCamera();
        void RenderMinimap();
        void RenderArrow(const glm::vec3& position, float yaw, const glm::vec3& color, bool isMinimap);
        Mesh* CreateRectangle(const std::string& name, const glm::vec3& color, float width, float height);
        Mesh* CreateTriangle(const std::string& name, const glm::vec3& color, float base, float height);
        Mesh* CreateCircle(const std::string& name, const glm::vec3& color, float radius, int numSegments);
        Mesh* CreateNumber0(const std::string& name, glm::vec3 color, float width);
        Mesh* CreateNumber1(const std::string& name, glm::vec3 color, float width);
        Mesh* CreateNumber2(const std::string& name, glm::vec3 color, float width);
        Mesh* CreateNumber3(const std::string& name, glm::vec3 color, float width);
        Mesh* CreateSlash(const std::string& name, glm::vec3 color, float width);
        void RenderDeliveredPackages(int deliveredPackages, int totalPackages);
        Mesh* CreateSegment(const glm::vec3& start, const glm::vec3& end, float width, const glm::vec3& color);
        Mesh* CombineSegments(const std::string& name, const std::vector<Mesh*>& segments);
        void ApplyChristmasTheme();
        Mesh* CreateSphere(const std::string& name, const glm::vec3& color, float radius, int sectors, int stacks);




        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;
        Mesh* terrainMesh;

    protected:
        implemented::Camera* camera;
        implemented::Camera* minimapCamera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        float zNear = 0.01f;
        float zFar = 200.0f;
        GLenum polygonMode;
        ViewportArea miniViewportArea;
        glm::mat4 modelMatrix;
        glm::vec3 dronePosition;
        glm::vec3 droneForward = glm::vec3(0, 0, -1);
        float cameraAngleAroundDrone = 0.0f;
        std::vector<Mesh*> obstacleMeshes;
        std::vector<glm::vec3> obstaclePositions;
        const int terrainRows = 50;
        const int terrainCols = 50;
        std::vector<glm::vec3> obstacles;
        std::vector<Tree> trees;
        glm::vec3 previousDronePosition;
        std::vector<House> houses;
        Package currentPackage;
        glm::vec3 deliveryDestination;
        int deliveredPackages = 2;
        bool isChristmasTheme = false;
        std::vector<Globe> globes;


    };
}
