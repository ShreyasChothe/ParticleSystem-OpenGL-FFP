#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/GL.h>
#include <gl/glu.h>

#define TWO_PI 6.28318530718f      // 360 degrees in radians (2 * pi)

struct Particle
{
	float position[3];
	float randomDir[3];
	float velocity[3];
	float sizeRandom;
	float life;
	float maxLife;
	float phase;
	float brightness;
};

struct ParticleSystem
{
	Particle* particles = NULL;
	int count;
	float time;

	float emitterPosition[3] = {0.0f, 0.0f, 0.0f};
	float emitterSize[3] = {5.0f, 3.0f, 3.0f};

	float baseVelocity[3] = {0.0f, 0.15f, 0.0f};
	float velocityRandomness = 0.10f;
	float swayStrength = 0.3f;
	float swaySpeed = 0.8f;

	float lifeMin = 12.0f;
	float lifeMax = 18.0f;
	float sizeMin = 0.04f;
	float sizeMax = 0.12f;
	float brightness = 1.0f;
	float haloAlpha = 0.5f;
	float coreSize = 0.35f;
	float coreWhite = 1.0f;
	int segments = 12;
	float color[3];
};

float particleRandom(float min, float max);
void spawnParticle(ParticleSystem* ps, Particle* p, bool randomAge);
void initializeParticleSystem(ParticleSystem* ps, int count);
void updateParticleSystem(ParticleSystem* ps, float dt);
void renderParticleSystem(ParticleSystem* ps);

void uninitializeParticleSystem(ParticleSystem* ps);
