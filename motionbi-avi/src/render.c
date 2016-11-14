#include "render.h"


renderEnv *renderInit() {
    /* * * * Init SDL-GL context * * * */
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Log("Successfully initialized SDL");

    /* Ativar double buffering*/
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    //SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    /* Anti-aliasing */
    /*SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);/**/

    // Janela SDL
    SDL_Window *window = SDL_CreateWindow(R_DEFwintitle,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        R_WWIDTH, R_WHEIGHT ,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    /*init sdl ttf for text rendering*/
    TTF_Init();

    /* * * * Init GLGlew * * * */
    GLenum result = glewInit();
    if (result != GLEW_OK) {
        SDL_Log("Glew error: %s\n", glewGetErrorString(result));
        exit(-1);
    }
    SDL_Log("%s: %s\n", "OpenGL Version ", glGetString(GL_VERSION));
    
    
    /* * * * Init OpenGL * * * */
    glEnable(GL_DEPTH_TEST | GL_LINE_SMOOTH | GL_POLYGON_SMOOTH | GL_LIGHTING);
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    
    /* Init render environment */
    renderEnv * env = (renderEnv *) calloc(1, sizeof(renderEnv));
    
    env->objects = (renderObject **) calloc(R_DEFobjs, sizeof(renderObject *));
    env->nobj    = 0;
    
    env->window = window;
    env->glcontext = glcontext;
    
    /* LookAt */
    GLdouble lookat[] = R_DEFlookat;
    env->lookat.eyex    = lookat[0];
    env->lookat.eyey    = lookat[1];
    env->lookat.eyez    = lookat[2];
    env->lookat.centerx = lookat[3];
    env->lookat.centery = lookat[4];
    env->lookat.centerz = lookat[5];
    env->lookat.upx     = lookat[6];
    env->lookat.upy     = lookat[7];
    env->lookat.upz     = lookat[8];
    
    /* Window */
    env->winw = R_WWIDTH;
    env->winh = R_WHEIGHT;
    
    /* Ambient Light */
    GLfloat ambient[]   = R_DEFambient;
    env->ambientlight.r = ambient[0];
    env->ambientlight.g = ambient[1];
    env->ambientlight.b = ambient[2];
    env->ambientlight.a = ambient[3];
    
    /* Clear Colour */
    GLfloat clearcolour[] = R_DEFclear;
    env->clearcolour.r = clearcolour[0];
    env->clearcolour.g = clearcolour[1];
    env->clearcolour.b = clearcolour[2];
    env->clearcolour.a = clearcolour[3];
    
    /* Config */
    env->drawxyz    = R_DEFdrawxyz;
    env->fps        = R_DEFfps;
    env->fullfps    = R_DEFfullfps;
    env->wireframe  = R_DEFwireframe;
    env->linewidth  = R_DEFlinew;
    
    srand(time(NULL));
    randInitColor = rand() % 10;
    
    return env;
}

void renderStop(renderEnv * env) {
    // Cleaning tasks
    SDL_GL_DeleteContext(env->glcontext);
    SDL_DestroyWindow(env->window);
    SDL_Quit();
}

void renderLoop(renderEnv * env) {
    int i;
    /* Before drawing */ 
    glClearColor(env->clearcolour.r, env->clearcolour.g, env->clearcolour.b,
        env->clearcolour.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, env->winw, env->winh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, env->winw / (GLfloat) env->winh, 0.1, 1<<16);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLineWidth(env->linewidth);

    /* If draw lines or polygs */
    if (env->wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    /* Camera Position */
    gluLookAt(env->lookat.eyex,    env->lookat.eyey,    env->lookat.eyez,
              env->lookat.centerx, env->lookat.centery, env->lookat.centerz,
              env->lookat.upx,     env->lookat.upy,     env->lookat.upz);

    /* If draw XYZ axis at (0,0,0) */
    if (env->drawxyz)
        render_drawXYZ(10.0);
    
    /* The lights */
    renderLightning(env);

    /* Draw with no alpha channel*/
    for (i = 0; i < env->nobj; i++) {
        if (env->objects[i])
            renderObj(env->objects[i], 0);
    }

    glDepthMask(GL_FALSE);
    glEnable   (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    /* Draw with alpha channel */
    for (i = 0; i < env->nobj; i++) {
        if (env->objects[i])
            renderObj(env->objects[i], 1);
    }

    glDisable  (GL_BLEND);
    glDepthMask(GL_TRUE);

    SDL_GL_SwapWindow(env->window);
}

void renderLightning(renderEnv * env) {
    glEnable (GL_LIGHTING);
    /* Ambient Light */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,  (GLfloat *)&env->ambientlight);
    
    /* Directional Lights */
    glEnable(GL_LIGHT0);
    GLfloat lp0[] = {-1, 1, 1.5, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, renderNormalizevf(lp0));
    glLight4f(GL_LIGHT0, GL_AMBIENT,  0.0, 0.0, 0.0, 1.0f);
    glLight4f(GL_LIGHT0, GL_DIFFUSE,  1.0f, 1.0f, 1.0f, 1.0f);

    glEnable( GL_LIGHT1);
    GLfloat lp1[] = {0.5, 1, -1, 0.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, renderNormalizevf(lp1));
    glLight4f(GL_LIGHT1, GL_AMBIENT,  0.0, 0.0, 0.0, 1.0f);
    glLight4f(GL_LIGHT1, GL_DIFFUSE,  0.6f, 0.6f, 0.6f, 1.0f);
    
    /* Default material */
    //renderSetMaterial(renderMaterialPreset(0));
}

void renderSetMaterial(renderMaterial m, GLfloat highlight, GLfloat alpha) {
    glMaterial4f(GL_FRONT, GL_AMBIENT,
        m.ambient.r * highlight, m.ambient.g * highlight, m.ambient.b * highlight, alpha);
    glMaterial4f(GL_FRONT, GL_DIFFUSE,
        m.diffuse.r * highlight, m.diffuse.g * highlight, m.diffuse.b * highlight, alpha);
    glMaterial4f(GL_FRONT, GL_SPECULAR,
        m.specular.r * highlight, m.specular.g * highlight, m.specular.b * highlight, alpha);
    glMaterialf (GL_FRONT, GL_SHININESS, m.shininess);
}

void renderAddObject(renderEnv * env, renderObject * obj) {
    if (env->nobj >= R_DEFobjs)
        return;
    env->objects[env->nobj++] = obj;
    obj->id = env->nobj - 1;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * *  TOOLS  * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void render_drawXYZ(GLfloat size) {
    glDisable (GL_LIGHTING);
    glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0); 
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(size, 0.0, 0.0);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, size, 0.0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, size);
    glEnd();
    glEnable (GL_LIGHTING);
}



int renderMaterialHasAlpha(renderMaterial mat) {
    if (mat.ambient.a  < 1.0 ||
        mat.emission.a < 1.0 ||
        mat.diffuse.a  < 1.0 ||
        mat.specular.a < 1.0)
        return 1;
    return 0;
}

void glLight3f(GLenum light, GLenum pname, GLfloat a, GLfloat b, GLfloat c) {
    GLfloat vector[] = {a, b, c};
    glLightfv(light, pname, vector);
}

void glLight4f(GLenum light, GLenum pname, GLfloat a, GLfloat b, GLfloat c, GLfloat d) {
    GLfloat vector[] = {a, b, c, d};
    glLightfv(light, pname, vector);
}

void glMaterial3f(GLenum light, GLenum pname, GLfloat a, GLfloat b, GLfloat c) {
    GLfloat vector[] = {a, b, c};
    glMaterialfv(light, pname, vector);
}

void glMaterial4f(GLenum light, GLenum pname, GLfloat a, GLfloat b, GLfloat c, GLfloat d) {
    GLfloat vector[] = {a, b, c, d};
    glMaterialfv(light, pname, vector);
}

GLfloat * renderNormalizevf(GLfloat * v) {
    GLfloat n = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[0]);
    v[0] /= n; v[1] /= n; v[2] /= n;
    return v;
}


GLuint renderTextTexture(char *text, double * ratio, int mipmaps) {
    TTF_Font * fontmono = TTF_OpenFont("DejaVuSansMono.ttf", 128);
    TTF_Font * font = TTF_OpenFont("DejaVuSans.ttf", 64);
    SDL_Color color = {255, 255, 255, 0};
    
    SDL_Surface *ts = TTF_RenderText_Blended(font, text, color);
    *ratio = ts->w / (float) ts->h;
    
    /*create the openGL texture from text bitmap*/
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (mipmaps)
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (mipmaps)
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, ts->w, ts->h,
                        GL_RGBA, GL_UNSIGNED_BYTE, ts->pixels);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ts->w, ts->h, 0,
                        GL_RGBA, GL_UNSIGNED_BYTE, ts->pixels);

    TTF_CloseFont(font);
    SDL_FreeSurface(ts);

    return texture;
}

renderColour renderHSVtoRGB(double h, double s, double v, double alpha) {
    double c = 0.0, m = 0.0, x = 0.0;
    renderColour color;
    color.a = alpha;
    h = fmod(h, 360.0);
    c = v * s;
    x = c * (1.0 - fabs(fmod(h / 60.0, 2) - 1.0));
    m = v - c;
    if (h >= 0.0 && h < 60.0) {
        color.r = c + m;    color.g = x + m;    color.b = m;
    } else if (h >= 60.0 && h < 120.0) {
        color.r = c + m;    color.g = c + m;    color.b = m;
    } else if (h >= 120.0 && h < 180.0) {
        color.r = m;        color.g = c + m;    color.b = x + m;
    } else if (h >= 180.0 && h < 240.0) {
        color.r = m;        color.g = x + m;    color.b = c + m;
    } else if (h >= 240.0 && h < 300.0) {
        color.r = x + m;    color.g = m;        color.b = c + m;
    } else if (h >= 300.0 && h < 360.0) {
        color.r = c + m;    color.g = m;        color.b = x + m;
    } else {
        color.r = m;        color.g = m;        color.b = m;
    }
    return color;
}
