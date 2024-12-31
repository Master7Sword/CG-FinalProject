#include "Particle.h"
#include "ParticleRenderer.h"

class Fireworks {
public:
  Fireworks() = default;
  ~Fireworks() = default;

  void initialize();
  void render(const std::vector<Particle>& particles, const glm::mat4& view, const glm::mat4& projection);
  long numParticles() const { return particles.size(); };
  void launch();
  void switchFirework();
  void update(float deltaTime, std::vector<Light>& lights);
  void render(const glm::mat4& view, const glm::mat4& projection);

private:
  std::vector<Particle> particles; // 存储所有粒子
  ParticleRenderer renderer;
  int pattern = 0;                 // 接下来发射的烟花的爆炸图案,已发射的烟花不受影响, 0:无图案, 1:MSC
};