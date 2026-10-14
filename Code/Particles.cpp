#include "Particles.h"

extern FILE* gpFile;

static float gCos[33], gSin[33];

static void drawFan(float x, float y, float z, float radius,
                    float cr, float cg, float cb, float ca,
                    float er, float eg, float eb, float ea, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(cr, cg, cb, ca);
    glVertex3f(x, y, z);

    glColor4f(er, eg, eb, ea);
    for(int i = 0; i <= segments; ++i)
        glVertex3f(x + gCos[i] * radius, y + gSin[i] * radius, z);

    glEnd();
}

float particleRandom(float min, float max)
{
    float t = (float)rand() / (float)RAND_MAX;
    return min + (max - min) * t;
}

void spawnParticle(ParticleSystem* ps, Particle* p, bool randomAge)
{
    for(int i = 0; i < 3; ++i)
    {
        p->position[i] = ps->emitterPosition[i] + particleRandom(-ps->emitterSize[i], ps->emitterSize[i]);
    }

    float z = particleRandom(-1.0f, 1.0f);
    float angle = particleRandom(0.0f, TWO_PI);
    float ring = sqrtf(1.0f - z * z);
    float length = particleRandom(0.0f, 1.0f);

    p->randomDir[0] = ring * cosf(angle) * length;
    p->randomDir[1] = ring * sinf(angle) * length;
    p->randomDir[2] = z * length;

    p->maxLife = particleRandom(ps->lifeMin, ps->lifeMax);
    p->life = p->maxLife;
    p->phase = particleRandom(0.0f, 6.283185f);
    p->brightness = particleRandom(0.4f, 1.0f);
    p->sizeRandom = particleRandom(0.0f, 1.0f);

    if(randomAge)
    {
        float age = particleRandom(0.0f, p->maxLife);
        p->life -= age;
        for(int i = 0; i < 3; ++i)
            p->position[i] += (ps->baseVelocity[i] + p->randomDir[i] * ps->velocityRandomness) * age;
    }

}

void initializeParticleSystem(ParticleSystem* ps, int count)
{
    void uninitialize(void);

	ps->count     = count;
	ps->time      = 0.0f;
	ps->particles = (Particle*)malloc(sizeof(Particle) * count);
    if(ps->particles == NULL)
    {
        fprintf(gpFile, "Error in allocating memory for particles with count %d\n", count);
        uninitialize();
        
    }

	for (int i = 0; i < count; i++)
		spawnParticle(ps, &ps->particles[i], true);
 
	//ps->texture = createGlowTexture();
}

void updateParticleSystem(ParticleSystem* ps, float dt)
{
	if (ps->particles == NULL)
		return;
 
	ps->time += dt;
 
	for (int i = 0; i < ps->count; i++)
	{
		Particle* p = &ps->particles[i];
 
		// age
		p->life -= dt;
		if (p->life <= 0.0f)
		{
			spawnParticle(ps, p, false);	// dead -> reborn at the emitter
			continue;
		}
		float t = ps->time * ps->swaySpeed + p->phase;
 
		float swayX = sinf(t) + 0.5f * sinf(t * 2.3f + 1.7f);
		float swayY = 0.3f * sinf(t * 1.3f + 0.5f);
		float swayZ = cosf(t * 0.8f)   + 0.5f * sinf(t * 1.9f + 0.4f);
 
		float vx = ps->baseVelocity[0] + p->randomDir[0] * ps->velocityRandomness;
		float vy = ps->baseVelocity[1] + p->randomDir[1] * ps->velocityRandomness;
		float vz = ps->baseVelocity[2] + p->randomDir[2] * ps->velocityRandomness;

		p->position[0] += (vx + swayX * ps->swayStrength) * dt;
		p->position[1] += (vy + swayY * ps->swayStrength) * dt;
		p->position[2] += (vz + swayZ * ps->swayStrength) * dt;
	}
}

void renderParticleSystem(ParticleSystem* ps)
{
    if(ps->particles == NULL)   return;

    int seg = ps->segments;
    if (seg < 3) seg = 3;
    if (seg > 32) seg = 32;
    for(int i = 0; i <= seg; ++i)
    {
        float angle = 6.283185f * (float)i / (float)seg;
        gCos[i] = cosf(angle);
        gSin[i] = sinf(angle);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    float cr = ps->color[0];
    float cg = ps->color[1];
    float cb = ps->color[2];

    for(int i = 0; i < ps->count; ++i)
    {
        Particle* p = &ps->particles[i];

		float ratio   = p->life / p->maxLife;		// 1 -> 0
		float age     = 1.0f - ratio;				// 0 -> 1
		float fadeIn  = age / 0.15f;   if (fadeIn  > 1.0f) fadeIn  = 1.0f;
		float fadeOut = ratio / 0.25f; if (fadeOut > 1.0f) fadeOut = 1.0f;

        float alpha = fadeIn * fadeOut * p->brightness * ps->brightness;
        float radius = ps->sizeMin + (ps->sizeMax - ps->sizeMin) * p->sizeRandom;

        float x = p->position[0], y = p->position[1], z = p->position[2];

        drawFan(x, y, z, radius,
                cr, cg, cb, alpha * ps->haloAlpha,
                cr, cg, cb, 0.0f, seg);

        float w = ps->coreWhite * p->brightness;
        float wr = cr + (1.0f - cr) * w;
        float wg = cg + (1.0f - cg) * w;
        float wb = cb + (1.0f - cb) * w;

        float edgeAlpha = alpha * ps->haloAlpha * (1.0f - ps->coreSize);

        drawFan(x, y, z, radius * ps->coreSize, 
                wr, wg, wb, alpha,
                cr, cg, cb, edgeAlpha, seg);
    }

    glDepthMask(GL_TRUE);
}
 
void uninitializeParticleSystem(ParticleSystem* ps)
{
    if(ps->particles != NULL)
    {
        free(ps->particles);
        ps->particles = NULL;
    }

    ps->count = 0;
}