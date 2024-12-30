#include "Fireworks.h"
#include "ParticleRenderer.h"
#include <algorithm>
#include <GLFW/glfw3.h>

void Fireworks::initialize() {
  renderer.initialize();
}

void Fireworks::launch() {
  Particle test;
  test.initialize(
    glm::vec3(0.0f, -5.0f, -30.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 12.0f, 0.0f),
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

void Fireworks::switchFirework(int key) {
  switch(key) {
    case GLFW_KEY_1:
      pattern = 1;
      break;
    case GLFW_KEY_0:
      pattern = 0;
      break;
  }
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
    light.intensity = std::max(0.0f, light.intensity - deltaTime * 7.5f); // 强度衰减速度
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