#include "Particle.h"
#include "utils.h"
#include "msc.h"
#include <glm/geometric.hpp>

void Particle::initialize(
    const glm::vec3& position,
    const glm::vec3& direction,
    const glm::vec3& velocity,
    const glm::vec3& color,
    float transparency,
    float ttl,
    bool is_boomed,
    bool is_tail,
    const glm::vec3& acceleration,
    int pattern
) {
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
    this->pattern = pattern;
}


void Particle::update(float deltaTime, std::vector<Particle>& newParticles, std::vector<Light>& lights) {
    if(!is_tail){  // 在原地添加一个粒子实现拖尾
        Particle tail;
        tail.initialize(loc, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), color, transparency-0.2f, 3.0f, true, true, glm::vec3(0.0f, 0.0f, 0.0f));
        newParticles.push_back(tail);

        // 更新当前粒子属性
        float k; // 空气阻力系数
        if(!is_boomed){
            k = -0.2;
            a = v * k + glm::vec3(0.0f,-2.0f, 0.0f);
        }   
        else{
            k = -0.6;
            a = v * k + glm::vec3(0.0f,-0.5f, 0.0f);
        } 
            
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
                explosionLight.intensity = 600.0f; // 初始强度
                explosionLight.ttl = 5.0f;       // 持续时间（秒）
                lights.push_back(explosionLight);

                switch (pattern) {
                case 1: {  // MSC
                    std::vector<glm::vec3> s_locations = msc_positions(loc, 0.1);
                    std::vector<glm::vec3> d_locations = msc_positions(loc, 1.0);
                    std::vector<glm::vec3> rgbs = msc_rgbs();
                    for (int i = 0; i < s_locations.size(); ++i) {
                        glm::vec3 dir = glm::normalize(glm::vec3(
                            d_locations[i].x - s_locations[i].x, 
                            d_locations[i].y - s_locations[i].y,
                            d_locations[i].z - s_locations[i].z
                        ));
                        glm::vec3 velocity = dir * (glm::distance(d_locations[i], s_locations[i]) * 0.01f);
                        Particle p;
                        p.initialize(
                            loc,
                            dir,
                            velocity,
                            rgbs[i],
                            1.0f,
                            5.0f,
                            true,
                            false,
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            pattern
                        );
                        newParticles.push_back(p);
                    }
                } break;
                default:  // 无图案
                    glm::vec3 color1 = getRandomColor();
                    glm::vec3 color2 = getRandomColor();
                    for (int i = 0; i < 200; ++i) {
                        glm::vec3 dir = glm::normalize(glm::vec3(
                            (rand() % 200 - 100) / 100.0f, 
                            (rand() % 200 - 100) / 100.0f,
                            (rand() % 200 - 100) / 100.0f
                        ));
                        glm::vec3 velocity = dir * 5.0f;
                        // glm::vec3 color = getRandomColor();
                        Particle p;
                        p.initialize(loc, dir, velocity, color1, 1.0f, 5.0f, true, false, glm::vec3(0.0f,-0.981f, 0.0f));
                        newParticles.push_back(p);
                    }
                    for (int i = 0; i < 200; ++i) {
                        glm::vec3 dir = glm::normalize(glm::vec3(
                            (rand() % 200 - 100) / 100.0f, 
                            (rand() % 200 - 100) / 100.0f,
                            (rand() % 200 - 100) / 100.0f
                        ));
                        glm::vec3 velocity = dir * 3.0f;
                        // glm::vec3 color = getRandomColor();
                        Particle p;
                        p.initialize(loc, dir, velocity, color2, 1.0f, 5.0f, true, false, glm::vec3(0.0f,-0.981f, 0.0f));
                        newParticles.push_back(p);
                    }
                    Light explosionLight1;
                    explosionLight1.position = loc;
                    explosionLight1.color = color1;
                    explosionLight1.intensity = 500.0f; // 初始强度
                    explosionLight1.ttl = 5.0f;       // 持续时间（秒）

                    Light explosionLight2;
                    explosionLight2.position = loc;
                    explosionLight2.color = color2;
                    explosionLight2.intensity = 500.0f; // 初始强度
                    explosionLight2.ttl = 5.0f;       // 持续时间（秒）

                    lights.push_back(explosionLight1);
                    lights.push_back(explosionLight2);
                }
            }
            transparency = std::max(0.0f, transparency - 0.2f * deltaTime);
        } else {
            ttl -= deltaTime;
            transparency = std::max(0.0f, transparency - 0.3f * deltaTime);
        }
    }
    else{  // 拖尾粒子更新
        ttl -= deltaTime;
        transparency = std::max(0.0f, transparency - 1.5f * deltaTime);
    }
}


bool Particle::check_recycle() const {
    return ttl <= 0.0f || recycle == true || transparency <= 0.0f;
}
