#include "Particle.h"
#include "utils.h"

void Particle::initialize(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& velocity, 
                          const glm::vec3& color, float transparency, float ttl, bool is_boomed, bool is_tail,
                          const glm::vec3& acceleration = glm::vec3(0.0f, -0.981f, 0.0f)) {
    this->loc = position;
    this->dir = direction;
    this->v = velocity;
    this->a = acceleration;
    this->color = color;
    this->transparency = transparency;
    this->ttl = ttl;
    this->is_boomed = is_boomed;
    this->is_tail = is_tail;
    this->recycle = false;
}

void Particle::update(float deltaTime, std::vector<Particle>& newParticles) {
    // 在原地添加一个粒子实现拖尾
    if(!is_tail){
        Particle tail;
        tail.initialize(loc, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), color, transparency-0.05f, 1.0f, true, true, glm::vec3(0.0f, 0.0f, 0.0f));
        newParticles.push_back(tail);

        // 更新当前粒子属性
        float k = -0.005; // 空气阻力系数
        a = v * v * k + glm::vec3(0.0f,-1.981f, 0.0f);
        v += a * deltaTime;
        loc += v * deltaTime;

        if (!is_boomed) {
            if (v.y < 0.5f) {
                is_boomed = true;
                this->recycle = true;
                for (int i = 0; i < 200; ++i) {
                    glm::vec3 dir = glm::normalize(glm::vec3(
                        (rand() % 200 - 100) / 100.0f, 
                        (rand() % 200 - 100) / 100.0f,
                        (rand() % 200 - 100) / 100.0f
                    ));
                    glm::vec3 velocity = dir * 4.0f;
                    glm::vec3 color = getRandomColor();
                    Particle p;
                    p.initialize(loc, dir, velocity, color, 1.0f, 5.0f, true, false, glm::vec3(0.0f,-0.981f, 0.0f));
                    newParticles.push_back(p);
                }
            }
        } else {
            ttl -= deltaTime;
            transparency = std::max(0.0f, transparency - 0.2f * deltaTime);
        }
    }
    else{ 
        // 拖尾粒子更新
        ttl -= deltaTime;
        transparency = std::max(0.0f, transparency - 3.0f * deltaTime);
    }
}


bool Particle::check_recycle() const {
    return ttl <= 0.0f || recycle == true || transparency <= 0.0f;
}
