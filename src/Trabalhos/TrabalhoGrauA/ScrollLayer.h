#ifndef GRAU_A_SCROLL_LAYER_H
#define GRAU_A_SCROLL_LAYER_H

#include <cmath>

struct ScrollLayer
{
    double period;
    double speedFactor;
    double offset = 0.0;

    void update(double deltaTime, double worldSpeed)
    {
        if (deltaTime <= 0.0 || worldSpeed <= 0.0) return;
        offset = std::fmod(offset + deltaTime * worldSpeed * speedFactor, period);
        // Normalização de arredondamento
        if (period - offset < 1e-8 || offset < 1e-8) offset = 0.0;
    }

    float copyX(int copy) const
    {
        return static_cast<float>(copy * period - std::floor(offset));
    }
};

#endif
