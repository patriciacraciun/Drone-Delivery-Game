#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "lab_m1/Tema2/transform3D.h"

using namespace std;
using namespace m1;

Tema2::Tema2() : terrainMesh(nullptr) {}

Tema2::~Tema2() {
    if (terrainMesh) {
        delete terrainMesh;
    }
}


void Tema2::Init() {
    polygonMode = GL_FILL;
    renderCameraTarget = false;

    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 2, 5), dronePosition, glm::vec3(0, 1, 0));
    camera->distanceToTarget = 5.0f;

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, zNear, zFar);

    minimapCamera = new implemented::Camera();

    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(resolution.x - resolution.x / 5.f - 20, 20, resolution.x / 5.f, resolution.y / 5.f);

    {
        Shader* shader = new Shader("TerrainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    meshes["droneArm"] = CreateParallelepiped("droneArm", glm::vec3(0.5f, 0.5f, 0.5f), 1.8f, 0.1f, 0.25f);
    meshes["droneCube"] = CreateParallelepiped("droneCube", glm::vec3(0.5f, 0.5f, 0.5f), 0.15f, 0.15f, 0.15f);
    meshes["droneRotor"] = CreateParallelepiped("droneRotor", glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, 0.04f, 0.04f);
    meshes["treeTrunk"] = CreateCylinder("treeTrunk", glm::vec3(0.55f, 0.27f, 0.07f), 0.1f, 1.0f, 32);
    meshes["treeLeaves"] = CreateCone("treeLeaves", glm::vec3(0.0f, 0.5f, 0.0f), 0.5f, 1.0f, 32);
    meshes["treeLeavesSnow"] = CreateCone("treeLeaves", glm::vec3(1.0f, 1.0f, 1.0f), 0.6f, 1.1f, 32);
    meshes["houseBase"] = CreateParallelepiped("houseBase", glm::vec3(0.7f, 0.7f, 0.7f), 1.0f, 1.5f, 1.0f);
    meshes["houseRoof"] = CreateCone("houseRoof", glm::vec3(1.0f, 0.0f, 0.0f), 0.7f, 1.5f, 32);
    meshes["package"] = CreateGift("package", glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(1.0f, 0.0f, 1.0f), 1.0f, 1.0f, 1.0f);
    meshes["rectangle"] = CreateRectangle("rectangle", glm::vec3(1.0f, 0.0f, 0.0f), 0.3f, 0.5f);
    meshes["triangle"] = CreateTriangle("triangle", glm::vec3(1.0f, 0.0f, 0.0f), 0.5f, 0.3f);
    meshes["circle"] = CreateCircle("circle", glm::vec3(0.5f, 0.0f, 0.5f), 4.0f, 32);
    meshes["destinationRectangle"] = CreateRectangle("dest_rectangle", glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f);
    meshes["treeCircle"] = CreateCircle("circle", glm::vec3(0.0f, 0.5f, 0.0f), 2.0f, 32);
    meshes["houseCircle"] = CreateCircle("circle", glm::vec3(0.7f, 0.7f, 0.7f), 2.0f, 32);
    meshes["number0"] = CreateNumber0("number0", glm::vec3(1.0f, 1.0f, 1.0f), 0.05f);
    meshes["number1"] = CreateNumber1("number1", glm::vec3(1, 1, 1), 0.05f);
    meshes["number2"] = CreateNumber2("number2", glm::vec3(1, 1, 1), 0.05f);
    meshes["number3"] = CreateNumber3("number3", glm::vec3(1, 1, 1), 0.05f);
    meshes["slash"] = CreateSlash("slash", glm::vec3(1.0f, 1.0f, 1.0f), 0.05f);

    GenerateTerrain(200, 200);
    GenerateObstacles(500);
    GenerateHouses(7);

    dronePosition = glm::vec3(0, 2.0f, 0);
    droneForward = glm::vec3(0, 0, -1);
    GenerateNewPackage();

    previousDronePosition = dronePosition;

    float rotationAngle = glm::radians(45.0f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0, 1, 0));
    droneForward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(droneForward, 0.0f)));
}


Mesh* Tema2::CreateParallelepiped(const std::string& name, glm::vec3 color, float width, float height, float depth) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-width / 2, -height / 2, -depth / 2), color),
        VertexFormat(glm::vec3(width / 2, -height / 2, -depth / 2), color),
        VertexFormat(glm::vec3(width / 2, -height / 2, depth / 2), color),
        VertexFormat(glm::vec3(-width / 2, -height / 2, depth / 2), color),

        VertexFormat(glm::vec3(-width / 2, height / 2, -depth / 2), color),
        VertexFormat(glm::vec3(width / 2, height / 2, -depth / 2), color),
        VertexFormat(glm::vec3(width / 2, height / 2, depth / 2), color),
        VertexFormat(glm::vec3(-width / 2, height / 2, depth / 2), color)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        0, 1, 5, 0, 5, 4,
        3, 2, 6, 3, 6, 7,
        0, 3, 7, 0, 7, 4,
        1, 2, 6, 1, 6, 5
    };

    Mesh* parallelepiped = new Mesh(name);
    parallelepiped->InitFromData(vertices, indices);
    return parallelepiped;
}


Mesh* Tema2::CreateCylinder(const std::string& name, glm::vec3 color, float radius, float height, int numSegments) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));
    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / numSegments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, 0, z), color));
    }

    vertices.push_back(VertexFormat(glm::vec3(0, height, 0), color));
    int baseOffset = numSegments + 1;
    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / numSegments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, height, z), color));
    }

    for (int i = 1; i <= numSegments; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back((i % numSegments) + 1);
    }

    for (int i = 1; i <= numSegments; ++i) {
        indices.push_back(baseOffset);
        indices.push_back(baseOffset + (i % numSegments) + 1);
        indices.push_back(baseOffset + i);
    }

    for (int i = 1; i <= numSegments; ++i) {
        int next = (i % numSegments) + 1;
        indices.push_back(i);
        indices.push_back(next);
        indices.push_back(baseOffset + i);

        indices.push_back(baseOffset + i);
        indices.push_back(next);
        indices.push_back(baseOffset + next);
    }

    Mesh* cylinder = new Mesh(name);
    cylinder->InitFromData(vertices, indices);
    return cylinder;
}


Mesh* Tema2::CreateCone(const std::string& name, glm::vec3 color, float radius, float height, int numSegments) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));
    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / numSegments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, 0, z), color));
    }

    vertices.push_back(VertexFormat(glm::vec3(0, height, 0), color));
    int topVertexIndex = numSegments + 1;

    for (int i = 1; i <= numSegments; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back((i % numSegments) + 1);
    }

    for (int i = 1; i <= numSegments; ++i) {
        indices.push_back(topVertexIndex);
        indices.push_back(i);
        indices.push_back((i % numSegments) + 1);
    }

    Mesh* cone = new Mesh(name);
    cone->InitFromData(vertices, indices);
    return cone;
}


Mesh* Tema2::CreateGift(const std::string& name, glm::vec3 boxColor, glm::vec3 ribbonColor, float width, float height, float depth) {
    Mesh* giftBox = CreateParallelepiped(name, boxColor, width, height, depth);

    float ribbonThickness = 0.15f;
    Mesh* ribbonVertical = CreateParallelepiped(name + "_ribbonVertical", ribbonColor, ribbonThickness, height + 0.02f, depth + 0.02f);

    Mesh* ribbonHorizontal = CreateParallelepiped(name + "_ribbonHorizontal", ribbonColor, width + 0.02f, ribbonThickness, depth + 0.02f);

    meshes[name] = giftBox;
    meshes[name + "_ribbonVertical"] = ribbonVertical;
    meshes[name + "_ribbonHorizontal"] = ribbonHorizontal;

    return giftBox;
}


Mesh* Tema2::CreateRectangle(const std::string& name, const glm::vec3& color, float width, float height) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-width / 2, 0, -height / 2), color),
        VertexFormat(glm::vec3(width / 2, 0, -height / 2), color),
        VertexFormat(glm::vec3(width / 2, 0, height / 2), color),
        VertexFormat(glm::vec3(-width / 2, 0, height / 2), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

    Mesh* rectangle = new Mesh(name);
    rectangle->InitFromData(vertices, indices);
    return rectangle;
}


Mesh* Tema2::CreateTriangle(const std::string& name, const glm::vec3& color, float base, float height) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0, 0, height / 2), color),
        VertexFormat(glm::vec3(base / 2, 0, -height / 2), color),
        VertexFormat(glm::vec3(-base / 2, 0, -height / 2), color)
    };

    std::vector<unsigned int> indices = { 0, 1, 2 };

    Mesh* triangle = new Mesh(name);
    triangle->InitFromData(vertices, indices);
    return triangle;
}

Mesh* Tema2::CreateCircle(const std::string& name, const glm::vec3& color, float radius, int numSegments) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));

    for (int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / numSegments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, 0, z), color));
    }

    for (int i = 1; i <= numSegments; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back((i % numSegments) + 1);
    }

    Mesh* circle = new Mesh(name);
    circle->InitFromData(vertices, indices);
    return circle;
}


Mesh* Tema2::CreateSegment(const glm::vec3& start, const glm::vec3& end, float width, const glm::vec3& color) {
    glm::vec3 direction = glm::normalize(end - start);
    glm::vec3 perpendicular = glm::vec3(-direction.y, direction.x, 0) * width / 2.0f;

    std::vector<VertexFormat> vertices = {
        VertexFormat(start - perpendicular, color),
        VertexFormat(start + perpendicular, color),
        VertexFormat(end - perpendicular, color),
        VertexFormat(end + perpendicular, color)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 1, 2, 3
    };

    Mesh* segment = new Mesh("segment");
    segment->InitFromData(vertices, indices);
    return segment;
}


Mesh* Tema2::CreateNumber0(const std::string& name, glm::vec3 color, float width) {
    std::vector<Mesh*> segments;
    float height = 1.0f;

    segments.push_back(CreateSegment(glm::vec3(-0.1f, 0.1f, 0), glm::vec3(0.1f, 0.1f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(-0.1f, 0.1f, 0), glm::vec3(-0.1f, -0.1f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(0.1f, 0.1f, 0), glm::vec3(0.1f, -0.1f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(-0.1f, -0.1f, 0), glm::vec3(0.1f, -0.1f, 0), width, color));

    return CombineSegments(name, segments);
}


Mesh* Tema2::CreateNumber1(const std::string& name, glm::vec3 color, float width) {
    return CreateSegment(glm::vec3(0.0f, 0.12f, 0), glm::vec3(0.0f, -0.12f, 0), width, color);
}


Mesh* Tema2::CreateNumber2(const std::string& name, glm::vec3 color, float width) {
    std::vector<Mesh*> segments;

    segments.push_back(CreateSegment(glm::vec3(-0.1f, 0.1f, 0), glm::vec3(0.1f, 0.1f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(0.1f, 0.1f, 0), glm::vec3(0.1f, 0.0f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(-0.1f, 0.0f, 0), glm::vec3(0.1f, 0.0f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(-0.1f, 0.0f, 0), glm::vec3(-0.1f, -0.1f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(-0.1f, -0.1f, 0), glm::vec3(0.1f, -0.1f, 0), width, color));

    return CombineSegments(name, segments);
}


Mesh* Tema2::CreateNumber3(const std::string& name, glm::vec3 color, float width) {
    std::vector<Mesh*> segments;

    segments.push_back(CreateSegment(glm::vec3(-0.1f, 0.1f, 0), glm::vec3(0.1f, 0.1f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(0.1f, 0.1f, 0), glm::vec3(0.1f, 0.0f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(-0.1f, 0.0f, 0), glm::vec3(0.1f, 0.0f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(0.1f, 0.0f, 0), glm::vec3(0.1f, -0.1f, 0), width, color));
    segments.push_back(CreateSegment(glm::vec3(-0.1f, -0.1f, 0), glm::vec3(0.1f, -0.1f, 0), width, color));

    return CombineSegments(name, segments);
}


Mesh* Tema2::CreateSlash(const std::string& name, glm::vec3 color, float width) {
    return CreateSegment(glm::vec3(-0.1f, -0.1f, 0), glm::vec3(0.1f, 0.1, 0), width, color);
}


Mesh* Tema2::CombineSegments(const std::string& name, const std::vector<Mesh*>& segments) {
    std::vector<VertexFormat> combinedVertices;
    std::vector<unsigned int> combinedIndices;

    unsigned int offset = 0;
    for (const auto& segment : segments) {
        combinedVertices.insert(combinedVertices.end(), segment->vertices.begin(), segment->vertices.end());
        for (auto index : segment->indices) {
            combinedIndices.push_back(index + offset);
        }
        offset += segment->vertices.size();
    }

    Mesh* combinedMesh = new Mesh(name);
    combinedMesh->InitFromData(combinedVertices, combinedIndices);
    return combinedMesh;
}


Mesh* Tema2::CreateSphere(const std::string& name, const glm::vec3& color, float radius, int sectors, int stacks) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * (glm::pi<float>() / stacks);
        float xy = radius * cos(stackAngle);
        float z = radius * sin(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectors;

            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);

            vertices.push_back(VertexFormat(glm::vec3(x, y, z), color));
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    Mesh* sphere = new Mesh(name);
    sphere->InitFromData(vertices, indices);
    return sphere;

}


void Tema2::RenderDrone(glm::vec3 position) {
    glm::mat4 modelMatrix;

    glm::vec3 up(0, 1, 0);
    glm::vec3 right = glm::cross(up, droneForward);
    glm::mat4 rotationMatrix = glm::mat4(glm::mat3(right, up, -droneForward));

    // bratele dronei
    modelMatrix = glm::translate(glm::mat4(1), position) * rotationMatrix;
    modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
    RenderMesh(meshes["droneArm"], shaders["VertexColor"], modelMatrix, 0, camera);

    modelMatrix = glm::translate(glm::mat4(1), position) * rotationMatrix;
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-45.0f), glm::vec3(0, 1, 0));
    RenderMesh(meshes["droneArm"], shaders["VertexColor"], modelMatrix, 0, camera);

    // cuburile si elicele
    float armLength = 0.6f;
    std::vector<glm::vec3> rotorPositions = {
        glm::vec3(armLength, 0.1f, armLength),
        glm::vec3(-armLength, 0.1f, armLength),
        glm::vec3(armLength, 0.1f, -armLength),
        glm::vec3(-armLength, 0.1f, -armLength)
    };

    for (const auto& rotorPos : rotorPositions) {
        modelMatrix = glm::translate(glm::mat4(1), position) * rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, rotorPos);
        RenderMesh(meshes["droneCube"], shaders["VertexColor"], modelMatrix, 0, camera);

        float bladeRotation = Engine::GetElapsedTime() * 8.5f;
        glm::vec3 rotorOffset = glm::vec3(0, 0.15f, 0);
        modelMatrix = glm::translate(glm::mat4(1), position) * rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, rotorPos + rotorOffset);
        modelMatrix = glm::rotate(modelMatrix, bladeRotation, glm::vec3(0, 1, 0));
        RenderMesh(meshes["droneRotor"], shaders["VertexColor"], modelMatrix, 0, camera);
    }
}


void Tema2::GenerateTerrain(int rows, int cols) {
    float cellSize = 1.0f;
    float halfWidth = cols * cellSize / 2.0f;
    float halfHeight = rows * cellSize / 2.0f;

    vertices.clear();
    indices.clear();

    for (int i = 0; i <= rows; ++i) {
        for (int j = 0; j <= cols; ++j) {
            float x = j * cellSize - halfWidth;
            float z = i * cellSize - halfHeight;
            // inaltimea initiala
            float y = 0.0f;
            vertices.emplace_back(glm::vec3(x, y, z), glm::vec3(0.2f, 0.4f, 0.1f));
        }
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int topLeft = i * (cols + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (cols + 1);
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    if (terrainMesh) {
        delete terrainMesh;
    }

    terrainMesh = new Mesh("terrain");
    terrainMesh->InitFromData(vertices, indices);
    meshes["terrain"] = terrainMesh;
}


void Tema2::GenerateObstacles(int count) {
    trees.clear();
    std::srand(std::time(nullptr));

    for (int i = 0; i < count; ++i) {
        float x = static_cast<float>(rand() % 200 - 100);
        float z = static_cast<float>(rand() % 200 - 100);
        float y = 1.0f;

        glm::vec3 potentialPosition = glm::vec3(x, y, z);

        bool overlapping = false;
        for (const auto& tree : trees) {
            if (glm::distance(potentialPosition, tree.position) < 2.0f) {
                overlapping = true;
                break;
            }
        }

        if (glm::distance(potentialPosition, dronePosition) < 3.0f) {
            overlapping = true;
        }

        if (!overlapping) {
            Tree tree;
            tree.position = potentialPosition;
            tree.trunkHeight = 1.0f + static_cast<float>(rand() % 50) / 100.0f;
            tree.trunkRadius = 1.8f;
            tree.leafHeight = 3.0f * tree.trunkHeight;
            tree.leafRadius = 2.0f;

            trees.push_back(tree);
        }
        else {
            --i;
        }
    }
}


bool Tema2::CheckCollisionWithTerrain() {
    if (dronePosition.y <= 1.0f) {
        return true;
    }
    return false;
}


bool Tema2::CheckCollisionWithObstacles() {
    float droneRadius = 0.3f;

    for (const auto& tree : trees) {
        float distanceXZ = glm::length(glm::vec2(dronePosition.x - tree.position.x, dronePosition.z - tree.position.z));
        if (distanceXZ <= tree.trunkRadius + droneRadius &&
            dronePosition.y <= tree.trunkHeight + droneRadius) {
            return true;
        }

        for (int j = 0; j < 4; ++j) {
            float leafHeight = tree.leafHeight * std::pow(0.8f, j);
            float leafRadius = tree.leafRadius * std::pow(0.8f, j);
            glm::vec3 conePosition = tree.position + glm::vec3(0.0f, tree.trunkHeight + j * (leafHeight / 2), 0.0f);

            if (CheckSphereConeCollision(dronePosition, droneRadius, conePosition, leafRadius, leafHeight)) {
                return true;
            }
        }
    }

    for (const auto& house : houses) {
        glm::vec3 minBound = house.position - house.size / 2.0f;
        glm::vec3 maxBound = house.position + house.size / 2.0f;
        maxBound.y += house.roofHeight;

        if (dronePosition.x >= minBound.x && dronePosition.x <= maxBound.x &&
            dronePosition.y >= minBound.y && dronePosition.y <= maxBound.y &&
            dronePosition.z >= minBound.z && dronePosition.z <= maxBound.z) {
            return true;
        }
    }

    return false;
}


bool Tema2::CheckSphereConeCollision(glm::vec3 sphereCenter, float sphereRadius, glm::vec3 coneBase, float coneRadius, float coneHeight) {
    glm::vec3 coneTop = coneBase + glm::vec3(0.0f, coneHeight, 0.0f);

    glm::vec3 coneAxis = glm::normalize(coneTop - coneBase);
    glm::vec3 sphereToBase = sphereCenter - coneBase;
    float projectionLength = glm::dot(sphereToBase, coneAxis);

    if (projectionLength < 0 || projectionLength > coneHeight) {
        return false;
    }

    float radiusAtProjection = coneRadius * (1.0f - projectionLength / coneHeight);
    glm::vec3 projectionPoint = coneBase + projectionLength * coneAxis;
    float distanceToAxis = glm::length(sphereCenter - projectionPoint);

    return distanceToAxis <= sphereRadius + radiusAtProjection;
}


void Tema2::GenerateHouses(int count) {
    houses.clear();
    std::srand(std::time(nullptr));

    float houseWidth = 5.0f;
    float houseDepth = 5.0f;
    float houseHeight = 6.5f;
    float roofHeight = 2.0f;

    for (int i = 0; i < count; ++i) {
        bool validPosition = false;

        while (!validPosition) {
            float x = static_cast<float>(rand() % 200 - 100);
            float z = static_cast<float>(rand() % 200 - 100);
            float y = -0.5f;

            glm::vec3 position = glm::vec3(x, y, z);
            glm::vec3 size = glm::vec3(houseWidth, houseHeight, houseDepth);

            validPosition = true;

            for (const auto& tree : trees) {
                float distanceXZ = glm::length(glm::vec2(position.x - tree.position.x, position.z - tree.position.z));
                float houseRadius = glm::max(houseWidth, houseDepth) / 2.0f;

                if (distanceXZ <= tree.trunkRadius + houseRadius) {
                    validPosition = false;
                    break;
                }
            }

            for (const auto& existingHouse : houses) {
                glm::vec3 min1 = position - size / 2.0f;
                glm::vec3 max1 = position + size / 2.0f;
                glm::vec3 min2 = existingHouse.position - existingHouse.size / 2.0f;
                glm::vec3 max2 = existingHouse.position + existingHouse.size / 2.0f;

                if (min1.x < max2.x && max1.x > min2.x &&
                    min1.y < max2.y && max1.y > min2.y &&
                    min1.z < max2.z && max1.z > min2.z) {
                    validPosition = false;
                    break;
                }
            }

            if (validPosition) {
                House house;
                house.position = position; 
                house.size = size;
                house.roofHeight = roofHeight;
                houses.push_back(house);
            }
        }
    }
}


void Tema2::RenderHouses() {
    glm::mat4 modelMatrix;

    for (const auto& house : houses) {
        modelMatrix = glm::translate(glm::mat4(1), house.position);
        modelMatrix = glm::scale(modelMatrix, house.size);
        RenderMesh(meshes["houseBase"], shaders["VertexColor"], modelMatrix, 0, camera);

        modelMatrix = glm::translate(glm::mat4(1), house.position + glm::vec3(0.0f, house.size.y - 1.6f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(house.size.x, house.roofHeight, house.size.z));
        RenderMesh(meshes["houseRoof"], shaders["VertexColor"], modelMatrix, 0, camera);

    }
}


void Tema2::RenderScene() {
    glm::mat4 modelMatrix;

    if (terrainMesh) {
        modelMatrix = glm::mat4(1);
        RenderMesh(terrainMesh, shaders["TerrainShader"], modelMatrix, 1, camera);
    }

    RenderDrone(dronePosition);

    for (const auto& tree : trees) {
        modelMatrix = glm::translate(glm::mat4(1), tree.position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(tree.trunkRadius, tree.trunkHeight, tree.trunkRadius));
        RenderMesh(meshes["treeTrunk"], shaders["VertexColor"], modelMatrix, 0, camera);

        float currentLeafHeight = tree.leafHeight;
        float currentLeafRadius = tree.leafRadius;
        for (int j = 0; j < 4; ++j) {
            glm::mat4 leavesMatrix = glm::translate(glm::mat4(1), tree.position + glm::vec3(0.0f, tree.trunkHeight + j * (currentLeafHeight / 2), 0.0f));
            leavesMatrix = glm::scale(leavesMatrix, glm::vec3(currentLeafRadius, currentLeafHeight, currentLeafRadius));
            if (j == 3 && isChristmasTheme) {
                RenderMesh(meshes["treeLeavesSnow"], shaders["VertexColor"], leavesMatrix, 0, camera);
            }
            else {
                RenderMesh(meshes["treeLeaves"], shaders["VertexColor"], leavesMatrix, 0, camera);
            }
            currentLeafHeight *= 0.8f;
            currentLeafRadius *= 0.8f;
        }
    }

    RenderHouses();

    glm::vec3 arrowTarget = currentPackage.isCarried ? deliveryDestination : currentPackage.position;
    float droneRotation = atan2(droneForward.z, droneForward.x);
    RenderArrowInFrontOfDrone(dronePosition + glm::vec3(0, 1, 0), arrowTarget);

    for (const auto& globe : globes) {
        modelMatrix = glm::translate(glm::mat4(1), globe.position);
        RenderMesh(meshes[globe.meshName], shaders["VertexColor"], modelMatrix, 0, camera);
    }
}


void Tema2::RenderMinimap() {
    RenderMesh(terrainMesh, shaders["TerrainShader"], glm::mat4(1), 1, minimapCamera);

    glm::vec3 arrowColor = glm::vec3(1.0f, 0.0f, 0.0f);
    float droneYaw = atan2(droneForward.x, droneForward.z);
    RenderArrow(dronePosition, droneYaw, arrowColor, true);

    glm::vec3 target = currentPackage.isCarried ? (deliveryDestination + glm::vec3(0, 2, 0)) : currentPackage.position;

    glm::mat4 targetMatrix = glm::translate(glm::mat4(1), target);
    targetMatrix = glm::scale(targetMatrix, glm::vec3(1.5f));
    RenderMesh(meshes["circle"], shaders["VertexColor"], targetMatrix, 0, minimapCamera);

    for (const auto& tree : trees) {
        modelMatrix = glm::translate(glm::mat4(1), tree.position);
        RenderMesh(meshes["treeCircle"], shaders["VertexColor"], modelMatrix, 0, minimapCamera);
    }
    for (const auto& house : houses) {
        glm::vec3 position = house.position;
        position.y += 2.0f;
        modelMatrix = glm::translate(glm::mat4(1), position);
        RenderMesh(meshes["houseCircle"], shaders["VertexColor"], modelMatrix, 0, minimapCamera);
    }
}


void Tema2::GenerateNewPackage() {
    currentPackage.position = glm::vec3(rand() % 200 - 100, 3.5f, rand() % 200 - 100);
    currentPackage.isCarried = false;

    if (!houses.empty()) {
        int randomIndex = rand() % houses.size();
        deliveryDestination = houses[randomIndex].position;
    }
}


void Tema2::CheckPackageCollision() {
    if (!currentPackage.isCarried &&
        abs(dronePosition.z - currentPackage.position.z) < 1.5f &&
        abs(dronePosition.x - currentPackage.position.x) < 1.5f) {
        currentPackage.isCarried = true;
    }
}


void Tema2::CheckDelivery() {
    if (currentPackage.isCarried &&
        abs(dronePosition.x - deliveryDestination.x) < 2.5f &&
        abs(dronePosition.z - deliveryDestination.z) < 2.5f) {
        deliveredPackages++;
        GenerateNewPackage();

        if (deliveredPackages == 3) {
            isChristmasTheme = true;
            ApplyChristmasTheme();
        }
    }
}


void Tema2::RenderPackage() {
    glm::mat4 modelMatrix;

    if (currentPackage.isCarried) {
        modelMatrix = glm::translate(glm::mat4(1), dronePosition - glm::vec3(0.0f, 0.8f, 0.0f));
    }
    else {
        modelMatrix = glm::translate(glm::mat4(1), currentPackage.position);
    }

    RenderMesh(meshes["package"], shaders["VertexColor"], modelMatrix, 0, camera);

    glm::mat4 ribbonVerticalMatrix = modelMatrix;
    RenderMesh(meshes["package_ribbonVertical"], shaders["VertexColor"], ribbonVerticalMatrix, 0, camera);

    glm::mat4 ribbonHorizontalMatrix = modelMatrix;
    RenderMesh(meshes["package_ribbonHorizontal"], shaders["VertexColor"], ribbonHorizontalMatrix, 0, camera);
}


void Tema2::RenderArrow(const glm::vec3& position, float yaw, const glm::vec3& color, bool isMinimap) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, yaw, glm::vec3(0, 1, 0));

    glm::vec3 rectScale = isMinimap ? glm::vec3(10.0f, 7.0f, 10.0f) : glm::vec3(1.0f, 0.7f, 1.0f);
    glm::vec3 triScale = isMinimap ? glm::vec3(10.0f, 8.0f, 16.0f) : glm::vec3(1.0f, 0.8f, 1.6f);

    glm::mat4 rectMatrix = modelMatrix;
    rectMatrix = glm::scale(rectMatrix, rectScale);
    RenderMesh(meshes["rectangle"], shaders["VertexColor"], rectMatrix, 0, isMinimap ? minimapCamera : camera);

    glm::mat4 triMatrix = modelMatrix;
    triMatrix = glm::translate(triMatrix, glm::vec3(0.0f, 0.0f, isMinimap ? 4.0f : 0.4f));
    triMatrix = glm::scale(triMatrix, triScale);
    RenderMesh(meshes["triangle"], shaders["VertexColor"], triMatrix, 0, isMinimap ? minimapCamera : camera);
}


void Tema2::RenderArrowInFrontOfDrone(glm::vec3 startPosition, glm::vec3 targetPosition) {
    glm::vec3 arrowPosition = dronePosition + droneForward;
    glm::vec3 directionToTarget = glm::normalize(glm::vec3(targetPosition.x - arrowPosition.x, 0.0f, targetPosition.z - arrowPosition.z));
    float arrowYaw = atan2(directionToTarget.x, directionToTarget.z);
    RenderArrow(arrowPosition, arrowYaw, glm::vec3(0.0f, 0.0f, 1.0f), false);
}


void Tema2::RenderDeliveredPackages(int deliveredPackages, int totalPackages) {
    glm::mat4 modelMatrix;

    glm::vec3 startPosition = dronePosition + glm::vec3(0.0f, 2.0f, 0.0f);

    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glm::mat4 billboardMatrix = glm::inverse(glm::mat4(glm::mat3(viewMatrix)));

    glm::mat4 groupMatrix = glm::translate(glm::mat4(1.0f), startPosition) * billboardMatrix;

    if (deliveredPackages < 3) {
        std::string deliveredMesh = "number" + std::to_string(deliveredPackages);
        modelMatrix = groupMatrix * glm::translate(glm::mat4(1), glm::vec3(-0.3f, 0.0f, 0.0f));
        RenderMesh(meshes[deliveredMesh], shaders["VertexColor"], modelMatrix, 0, camera);

        modelMatrix = groupMatrix * glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, 0.0f));
        RenderMesh(meshes["slash"], shaders["VertexColor"], modelMatrix, 0, camera);

        std::string totalMesh = "number" + std::to_string(totalPackages);
        modelMatrix = groupMatrix * glm::translate(glm::mat4(1), glm::vec3(0.3f, 0.0f, 0.0f));
        RenderMesh(meshes[totalMesh], shaders["VertexColor"], modelMatrix, 0, camera);
    }
}


void Tema2::ApplyChristmasTheme() {
    for (auto& tree : trees) {
        glm::vec3 treeTop = tree.position + glm::vec3(0.0f, tree.trunkHeight, 0.0f);

        for (int i = 0; i < 10; ++i) {
            float height = static_cast<float>(rand()) / RAND_MAX * tree.leafHeight;

            float angle = static_cast<float>(rand()) / RAND_MAX * glm::pi<float>() * 2.0f;
            float radius = tree.leafRadius * 0.35f;

            glm::vec3 globPosition = tree.position + glm::vec3(
                radius * cos(angle),
                tree.trunkHeight + height,
                radius * sin(angle)
            );

            std::string globName = "glob" + std::to_string(i) + "_" + std::to_string(rand());
            glm::vec3 globColor = glm::vec3(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX
            );

            meshes[globName] = CreateSphere(globName, globColor, 0.2f, 16, 16);
            globes.push_back({ globName, globPosition });
        }
    }
}


void Tema2::FrameStart() {
    glClearColor(0, 0.72f, 0.91f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::UpdateMinimapCamera() {
    minimapCamera->distanceToTarget = 100;
    glm::vec3 minimapCameraPosition = dronePosition + glm::vec3(0, minimapCamera->distanceToTarget, 0);
    glm::vec3 minimapCameraTarget = dronePosition;
    minimapCamera->Set(minimapCameraPosition, minimapCameraTarget, glm::vec3(0, 0, -1));

}


void Tema2::Update(float deltaTimeSeconds) {
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    if (CheckCollisionWithTerrain()) {
        dronePosition.y = 1.5f;
    }
    if (CheckCollisionWithObstacles()) {
        dronePosition = previousDronePosition;
    }

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    glm::vec3 cameraOffset = glm::normalize(camera->forward) * (-camera->distanceToTarget);
    glm::vec3 cameraPosition = dronePosition + cameraOffset;
    glm::vec3 cameraTarget = dronePosition;
    camera->Set(cameraPosition, cameraTarget, glm::vec3(0, 1, 0));

    CheckPackageCollision();
    CheckDelivery();
    RenderScene();
    RenderPackage();
    RenderDeliveredPackages(deliveredPackages, 3);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);
    UpdateMinimapCamera();
    RenderMinimap();

    glViewport(0, 0, resolution.x, resolution.y);

}


void Tema2::FrameEnd(){
    
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int isTerrain, implemented::Camera* activeCamera) {
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(activeCamera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int locIsTerrain = glGetUniformLocation(shader->program, "isTerrain");
    glUniform1i(locIsTerrain, isTerrain);

    int locIsChristmas = glGetUniformLocation(shader->program, "isChristmas");
    glUniform1i(locIsChristmas, isChristmasTheme ? 1 : 0);


    mesh->Render();
}


void Tema2::OnInputUpdate(float deltaTime, int mods) {
    float cubeSpeed = 10.0f * deltaTime;
    glm::vec3 right = glm::cross(droneForward, glm::vec3(0, 1, 0));
    previousDronePosition = dronePosition;

    if (window->KeyHold(GLFW_KEY_W)) {
        dronePosition += droneForward * cubeSpeed;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        dronePosition -= droneForward * cubeSpeed;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        dronePosition -= right * cubeSpeed;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        dronePosition += right * cubeSpeed;
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        dronePosition.y -= cubeSpeed;
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        dronePosition.y += cubeSpeed;
    }
    if (window->KeyHold(GLFW_KEY_Z)) {
        float rotationSpeed = glm::radians(90.0f) * deltaTime;
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationSpeed, glm::vec3(0, 1, 0));
        droneForward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(droneForward, 0.0f)));
        camera->RotateThirdPerson_OY(rotationSpeed);
    }

    if (window->KeyHold(GLFW_KEY_X)) {
        float rotationSpeed = glm::radians(90.0f) * deltaTime;
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -rotationSpeed, glm::vec3(0, 1, 0));
        droneForward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(droneForward, 0.0f)));
        camera->RotateThirdPerson_OY(-rotationSpeed);
    }

    glm::vec3 cameraOffset = glm::normalize(camera->forward) * (-camera->distanceToTarget);
    glm::vec3 cameraPosition = dronePosition + cameraOffset;
    glm::vec3 cameraTarget = dronePosition;
    camera->Set(cameraPosition, cameraTarget, glm::vec3(0, 1, 0));
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    float sensitivityOX = 0.002f;
    float sensitivityOY = 0.002f;

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        camera->RotateThirdPerson_OX(-sensitivityOX * deltaY);
        camera->RotateThirdPerson_OY(-sensitivityOY * deltaX);
    }
}


void Tema2::OnKeyPress(int key, int mods)
{

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}