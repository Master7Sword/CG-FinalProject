#include "Particle.h"
#include "utils.h"

void Particle::initialize(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& velocity, 
                          const glm::vec3& color, float transparency, float ttl, bool is_boomed, bool is_tail,
                          const glm::vec3& acceleration) {
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


void Particle::update(float deltaTime, std::vector<Particle>& newParticles, std::vector<Light>& lights) {
    // 在原地添加一个粒子实现拖尾
    if(!is_tail){
        Particle tail;
        tail.initialize(loc, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), color, transparency-0.05f, 3.0f, true, true, glm::vec3(0.0f, 0.0f, 0.0f));
        newParticles.push_back(tail);

        // 更新当前粒子属性
        float k = -0.02; // 空气阻力系数
        a = v * v * k + glm::vec3(0.0f,-2.0f, 0.0f);
        v += a * deltaTime;
        loc += v * deltaTime;

        if (!is_boomed) {
            if (v.y < 0.5f) {
                explosionSound[explosion_index].play();
                explosion_index = (explosion_index + 1) % MAX_SOUNDS;

                is_boomed = true;
                this->recycle = true;

                Light explosionLight;
                explosionLight.position = loc;
                explosionLight.color = color;
                explosionLight.intensity = 0.2f; // 初始强度
                explosionLight.ttl = 2.0f;       // 持续时间（秒）
                lights.push_back(explosionLight);

                for (int i = 0; i < 200; ++i) {
                    glm::vec3 dir = glm::normalize(glm::vec3(
                        (rand() % 200 - 100) / 100.0f, 
                        (rand() % 200 - 100) / 100.0f,
                        (rand() % 200 - 100) / 100.0f
                    ));
                    glm::vec3 velocity = dir * 4.0f;
                    glm::vec3 color = getRandomColor();
                    // glm::vec3 color = glm::vec3(255.0f/255.0f, 99.0f/255.0f, 71.0f/255.0f);
                    Particle p;
                    p.initialize(loc, dir, velocity, color, 1.0f, 5.0f, true, false, glm::vec3(0.0f,-0.981f, 0.0f));
                    newParticles.push_back(p);
                }
            }
            transparency = std::max(0.0f, transparency - 0.2f * deltaTime);
        } else {
            ttl -= deltaTime;
            transparency = std::max(0.0f, transparency - 0.3f * deltaTime);
        }
    }
    else{ 
        // 拖尾粒子更新
        ttl -= deltaTime;
        transparency = std::max(0.0f, transparency - 1.5f * deltaTime);
    }
}


bool Particle::check_recycle() const {
    return ttl <= 0.0f || recycle == true || transparency <= 0.0f;
}

void updateParticles(float deltaTime, std::vector<Particle>& particles, std::vector<Light>& lights) {
    std::vector<Particle> newParticles;  // 用来存储新生成的粒子

    // 更新每个粒子的状态
    for (auto& particle : particles) {
        particle.update(deltaTime, newParticles, lights);  // 新生成的粒子存入 newParticles，后续拷回 particles
    }
    particles.insert(particles.end(), newParticles.begin(), newParticles.end());
    particles.erase(std::remove_if(particles.begin(), particles.end(), 
                                   [](const Particle& p) { return p.check_recycle(); }), 
                    particles.end());

    // 更新光源的状态
    for (auto& light : lights) {
        light.ttl -= deltaTime;
        light.intensity = std::max(0.0f, light.intensity - deltaTime * 0.1f); // 强度衰减速度
    }
    lights.erase(std::remove_if(lights.begin(), lights.end(),
                                [](const Light& light) { return light.intensity <= 0.0f || light.ttl <= 0.0f; }),
                 lights.end());
}