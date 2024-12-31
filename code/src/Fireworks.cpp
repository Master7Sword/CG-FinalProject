#include "Fireworks.h"
#include "ParticleRenderer.h"
#include <algorithm>
#include <GLFW/glfw3.h>
#include <random>

void Fireworks::initialize() {
  renderer.initialize();
}

void Fireworks::launch() {
  Particle test;
  float vMin = 10.0f;
  float vMax = 15.0f;
  float xMin = -20.0f;
  float xMax = 20.0f;
  float zMin = -40.0f;
  float zMax = -20.0f;
  double random_fraction_x = static_cast<double>(std::rand()) / RAND_MAX;
  double random_fraction_z = static_cast<double>(std::rand()) / RAND_MAX;
  double random_fraction_v = static_cast<double>(std::rand()) / RAND_MAX;
  float xCoord = xMin + random_fraction_x * (xMax - xMin);
  float zCoord = zMin + random_fraction_z * (zMax - zMin);
  float velocity = vMin + random_fraction_v * (vMax - vMin);
  test.initialize(
    glm::vec3(xCoord, -5.0f, zCoord),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, velocity, 0.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    1.0f,
    100.0f,
    false,
    false,
    glm::vec3(0.0f, -0.981f, 0.0f),
    pattern
  );
  particles.push_back(test);
}

void Fireworks::switchFirework() {
  pattern = (pattern + 1) % 5;
}

void Fireworks::update(float deltaTime, std::vector<Light> &lights) {
  std::vector<Particle> newParticles; // 用来存储新生成的粒子

  // 更新每个粒子的状态
  for (auto &particle : particles) {
    particle.update(
      deltaTime,
      newParticles,
      lights
    ); // 新生成的粒子存入 newParticles，后续拷回 particles
  }
  particles.insert(particles.end(), newParticles.begin(), newParticles.end());
  particles.erase(
    std::remove_if(
      particles.begin(),
      particles.end(),
      [](const Particle &p) { return p.check_recycle(); }
    ),
    particles.end()
  );
  // 更新光源的状态
  for (auto &light : lights) {
    light.ttl -= deltaTime;
    if(light.ttl < FLT_MAX / 2)
      light.intensity = std::max(0.0f, light.intensity - deltaTime * 300.0f); // 强度衰减速度
  }
  lights.erase(
    std::remove_if(
      lights.begin(),
      lights.end(),
      [](const Light &light) {
        return light.intensity <= 0.0f || light.ttl <= 0.0f;
      }
    ),
    lights.end()
  );
}

void Fireworks::render(const glm::mat4& view, const glm::mat4& projection){
  renderer.render(particles, view, projection);
}