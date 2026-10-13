#ifndef GRAU_A_SPRITE_H
#define GRAU_A_SPRITE_H

struct SpriteAnimation
{
    int frameCount;
    double frameDuration;
    bool loop;
    int currentFrame = 0;
    double timer = 0.0;
    bool finished = false;

    void reset()
    {
        currentFrame = 0;
        timer = 0.0;
        finished = false;
    }

    void update(double deltaTime)
    {
        if (finished || deltaTime <= 0.0)
            return;
        timer += deltaTime;
        // Preserva o tempo excedente; mais de um quadro pode passar por update.
        // A tolerancia evita atrasos de um quadro por arredondamento de doubles.
        while (timer + 1e-9 >= frameDuration && !finished)
        {
            timer -= frameDuration;
            if (timer < 0.0) timer = 0.0;
            ++currentFrame;
            if (currentFrame == frameCount)
            {
                currentFrame = loop ? 0 : frameCount - 1;
                finished = !loop;
            }
        }
    }
};

#endif
