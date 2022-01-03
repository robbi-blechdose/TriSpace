#include "effects.h"
#include "image.h"

Effect effects[MAX_EFFECTS];
GLuint effectTexture;

void initEffects()
{
    effectTexture = loadRGBTexture("res/tex/Effects.png");
    for(uint8_t i = 0; i < MAX_EFFECTS; i++)
    {
        effects[i].life = 0;
    }
}

//TODO: Since the data here is game-dependent, we should move it out of the engine code
uint8_t createEffect(vec3 position, EffectType type)
{
    for(uint8_t i = 0; i < MAX_EFFECTS; i++)
    {
        if(!effects[i].life)
        {
            if(type == EXPLOSION)
            {
                effects[i].startFrame = 0;
                effects[i].frameIndex = 0;
                effects[i].frames = 8;
                effects[i].frameTime = 100;
                effects[i].life = 800;
                effects[i].position = position;
                effects[i].size = 1.5f;
            }
            else if(type == SPARKS)
            {
                effects[i].startFrame = 8;
                effects[i].frameIndex = 8;
                effects[i].frames = 8;
                effects[i].frameTime = 50;
                effects[i].life = 400;
                effects[i].position = position;
                effects[i].size = 0.75f;
            }
            return 1;
        }
    }
    return 0;
}

void calcEffects(uint32_t ticks)
{
    for(uint8_t i = 0; i < MAX_EFFECTS; i++)
    {
        if(effects[i].life)
        {
            effects[i].life -= ticks;
            effects[i].frameIndex = effects[i].startFrame + effects[i].frames - 1 - effects[i].life / effects[i].frameTime;
            //Time up? Clear effect
            if(effects[i].life < 0)
            {
                effects[i].life = 0;
            }
        }
    }
}

void drawEffects()
{
    glBindTexture(GL_TEXTURE_2D, effectTexture);

    for(uint8_t i = 0; i < MAX_EFFECTS; i++)
    {
        if(effects[i].life)
        {
            uint8_t texX = (effects[i].frameIndex % 8) * 32;
            uint8_t texY = (effects[i].frameIndex / 8) * 32;

            glPushMatrix();
            glTranslatef(effects[i].position.x, effects[i].position.y, effects[i].position.z);
            GLfloat matrix[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
            matrix[0] = 1;
            matrix[1] = 0;
            matrix[2] = 0;
            matrix[4] = 0;
            matrix[5] = 1;
            matrix[6] = 0;
            matrix[8] = 0;
            matrix[9] = 0;
            matrix[10] = 1;
            glLoadMatrixf(matrix);

            float size = effects[i].size;
            glBegin(GL_QUADS);
            glTexCoord2f(PTC(texX), PTC(texY + 32));
            glVertex3f(-size, -size, 0);
            glTexCoord2f(PTC(texX + 32), PTC(texY + 32));
            glVertex3f(size, -size, 0);
            glTexCoord2f(PTC(texX + 32), PTC(texY));
            glVertex3f(size, size, 0);
            glTexCoord2f(PTC(texX), PTC(texY));
            glVertex3f(-size, size, 0);
            glEnd();
            glPopMatrix();
        }
    }
}