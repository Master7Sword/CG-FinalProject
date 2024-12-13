#include "Particle.h"
#include "utils.h"

void Particle::initialize(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& velocity, 
                          const glm::vec3& color, float luminance, float ttl, bool is_boomed,
                          const glm::vec3& acceleration = glm::vec3(0.0f, -0.981f, 0.0f)) {
    this->loc = position;
    this->dir = direction;
    this->v = velocity;
    this->a = acceleration;
    this->color = color;
    this->luminance = luminance;
    this->ttl = ttl;
    this->is_boomed = is_boomed;
    this->recycle = false;
}

void Particle::update(float deltaTime, std::vector<Particle>& newParticles) {
    v += a * deltaTime;
    loc += v * deltaTime;

    if (!is_boomed) {
        // 当垂直速度低于某个阈值时爆炸
        if (v.y < 0.1f) {
            is_boomed = true;
            this->recycle = true; // 炸了之后当前这个粒子就可以回收了
            std::cout << "Boom!! New particles are generated!" << std::endl;
            for (int i = 0; i < 1000; ++i) {  // 预设爆炸发射1000个粒子
                glm::vec3 dir = glm::normalize(glm::vec3(
                    (rand() % 200 - 100) / 100.0f, 
                    (rand() % 200 - 100) / 100.0f,
                    (rand() % 200 - 100) / 100.0f
                )); // 随机方向
                glm::vec3 velocity = dir * 5.0f; // 速度跟方向一样
                // glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f); // 红色
                glm::vec3 color = getRandomColor();
                Particle p;
                p.initialize(this->loc, dir, velocity, color, 1.0f, 2.0f, true, glm::vec3(0.0f,-0.981f, 0.0f));
                newParticles.push_back(p);  // 新粒子存入 newParticles
            }
        }
    } else {
        ttl -= deltaTime; // update中只用更新ttl就行，回收Particle的逻辑在main中

        // 粒子亮度随时间衰减
        luminance = std::max(0.0f, luminance - 0.2f * deltaTime);
    }
}

bool Particle::check_recycle() const {
    return ttl <= 0.0f || recycle == true;
}
