#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include "Light.h"


class Particle {
public:
    void initialize(
        const glm::vec3& position,
        const glm::vec3& direction,
        const glm::vec3& velocity,
        const glm::vec3& color,
        float transparency,
        float ttl,
        bool is_boomed,
        bool is_tail,
        const glm::vec3& acceleration,
        int pattern = 0
    );
    
    void update(float deltaTime, std::vector<Particle>& particles, std::vector<Light>& lights);
    
    bool check_recycle() const;
    
    glm::vec3 getPosition() const { return loc; }
    glm::vec3 getColor() const { return color; }
    
    float getTransparency() const {return transparency; }
    float getTTL() const {return ttl; }

private:
    glm::vec3 loc;       // 位置
    glm::vec3 dir;       // 方向
    glm::vec3 v;         // 速度
    glm::vec3 a;         // 加速度
    glm::vec3 color;     // 颜色
    float transparency;  // 透明度
    float ttl;           // 生存时间
    bool is_boomed;      // 是否已爆炸
    bool recycle;        // 是否已可回收
    bool is_tail;        // 是否为拖尾粒子
    int pattern;         // 爆炸图案, 0:无图案, 1:MSC
};


void updateParticles(float deltaTime, std::vector<Particle>& particles, std::vector<Light>& lights);

#endif
