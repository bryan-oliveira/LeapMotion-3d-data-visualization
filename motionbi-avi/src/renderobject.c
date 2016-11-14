#include "render.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * *  SETTINGS   * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
renderObject * renderNewObj(int nplots, int plotsize, GLfloat maxsize, GLfloat maxheight, GLfloat width) {
    renderObject * res = (renderObject *) calloc(1, sizeof(renderObject));
    
    res->tmatrix.scale.x = 1.0;
    res->tmatrix.scale.y = 1.0;
    res->tmatrix.scale.z = 1.0;
    
    res->tmatrix.rotate.x = 0.0;
    res->tmatrix.rotate.y = 1.0;
    res->tmatrix.rotate.z = 0.0;
    
    /* If it is to draw the floor */
    res->drawfloor = 1;
    /* Default how highligh and transparency */
    res->highlight = 1.50;
    res->nadir = 0.15;
    
    /* New Plots */
    res->znum = nplots;
    res->xnum = plotsize;
    
    /* Allocate memory to values */
    res->values = (GLfloat **) calloc(res->znum, sizeof(GLfloat *));
    int i;
    for (i = 0; i < res->znum; i++)
        res->values[i] = (GLfloat *) calloc(res->xnum, sizeof(GLfloat));
    
    /* And allocate memory to states */
    res->states = (renderState **) calloc(res->znum, sizeof(renderState *));
    for (i = 0; i < res->znum; i++)
        res->states[i] = (renderState *) calloc(res->xnum, sizeof(renderState));
    
    /* And allocate memory to legends */
    res->titles.xlabels = (char **) calloc(res->xnum, sizeof(char *));
    res->titles.zlabels = (char **) calloc(res->znum, sizeof(char *));
    /* And for textures */
    res->titles.xtextures = (GLuint *) calloc(res->xnum, sizeof(GLuint));
    res->titles.ztextures = (GLuint *) calloc(res->znum, sizeof(GLuint));
    /* And for ratios */
    res->titles.xratios = (double *) calloc(res->xnum, sizeof(double));
    res->titles.zratios = (double *) calloc(res->znum, sizeof(double));

    /* Min/max height */
    res->miny = 0;
    res->maxy = maxheight;
    
    /* Default higher */
    res->higher = maxheight;
    
    /* Calc init, inc, gap and x/z min/max values */
    if (res->xnum > res->znum) {
        res->width = width = (maxsize / res->xnum) * width;
        
        GLfloat gap = (maxsize - res->xnum * width) / (res->xnum + 1.0);
        
        res->gap    = gap;
        res->inc    = gap + width;
        res->xinit  = -(maxsize/2.0) + gap + width / 2.0;
        res->zinit  = -(res->inc * (res->znum - 1)) / 2.0;

        res->maxx   = maxsize / 2.0;
        res->minx   = -res->maxx;
        res->maxz   = (res->znum * width + (res->znum + 1) * gap) / 2.0;
        res->minz   = -res->maxz;
        
    } else {
        res->width = width = (maxsize / res->znum) * width;
        
        GLfloat gap = (maxsize - res->znum * width) / (res->znum + 1.0);
        
        res->gap    = gap;
        res->inc    = gap + width;
        res->xinit  = -(res->inc * (res->xnum - 1)) / 2.0;
        res->zinit  = -(maxsize/2.0) + gap + width / 2.0;
        
        res->maxx   = (res->xnum * width + (res->xnum + 1) * gap) / 2.0;
        res->minx   = -res->maxx;
        res->maxz   = maxsize / 2.0;
        res->minz   = -res->maxz;
    }

    return res;
}

void renderAddSetToObj(renderObject * obj, GLfloat **values) {
    int i, j;
    int set = obj->nsets++;

    obj->sets[set].values = (GLfloat **) calloc(obj->znum, sizeof(GLfloat *));

    for (i = 0; i < obj->znum; i++) {
        obj->sets[set].values[i] = (GLfloat *) calloc(obj->xnum, sizeof(GLfloat));
        for (j = 0; j < obj->xnum; j++) {
            /* Add to new set */
            obj->sets[set].values[i][j] = values[i][j];
            /* Add to sum */
            obj->values[i][j] += values[i][j];
            /* The biggest one */
            obj->higher = obj->values[i][j] > obj->higher ? obj->values[i][j] : obj->higher;
        }
    }

    obj->sets[set].show = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * *  RENDER   * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void renderObj(renderObject * obj, int alphaing) {
    
    if(obj->hide)
        return;

    int i, j, k;
    double hw, hh, lx, ly, lz;
    glPushMatrix();
        glTranslatef(obj->tmatrix.translate.x, obj->tmatrix.translate.y,
            obj->tmatrix.translate.z);
        glRotatef(obj->tmatrix.angle * (180/M_PI),
            obj->tmatrix.rotate.x, obj->tmatrix.rotate.y, obj->tmatrix.rotate.z);
        glScalef(obj->tmatrix.scale.x, obj->tmatrix.scale.y, obj->tmatrix.scale.z);
        
        /* Drawing the floor */
        if ((obj->drawfloor) && (alphaing == renderMaterialHasAlpha(obj->matfloor))) {
            glPushAttrib(GL_POLYGON_BIT);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, -1.0);
            
            renderSetMaterial(obj->matfloor, 1.0, 1.0);
            glBegin(GL_QUADS);
                glNormal3f  (0, 1, 0);
                glVertex3f(obj->minx, obj->miny, obj->minz);
                glVertex3f(obj->minx, obj->miny, obj->maxz);
                glVertex3f(obj->maxx, obj->miny, obj->maxz);
                glVertex3f(obj->maxx, obj->miny, obj->minz);
            glEnd();
            glPopAttrib();
        }
        
        /* Draw x,y,z axis */
        if (!alphaing) {
            glDisable (GL_LIGHTING);
            glLineWidth(2);
            glBegin(GL_LINES);
                glColor4f(1.0, 1.0, 1.0, 1.0);
                glVertex3f(obj->minx, obj->miny, obj->minz);
                glVertex3f(obj->maxx, obj->miny, obj->minz);

                glVertex3f(obj->minx, obj->miny, obj->minz);
                glVertex3f(obj->minx, obj->maxy, obj->minz);

                glVertex3f(obj->minx, obj->miny, obj->minz);
                glVertex3f(obj->minx, obj->miny, obj->maxz);
            glEnd();
            glEnable (GL_LIGHTING);
            glLineWidth(1);
        }
        /* Drawing bars */
        GLfloat os;
        for (i = 0; i < obj->znum; i++) {
            for (j = 0; j < obj->xnum; j++) {
                if (alphaing != (obj->states[i][j] == R_UNSELECTED))
                    continue;
                switch (obj->states[i][j]) {
                    case R_DEFAULT:     renderSetMaterial(obj->matdefault, 1.0, 1.0);               break;
                    case R_SELECTED:    renderSetMaterial(obj->matdefault, obj->highlight, 1.0);    break;
                    case R_NADIR:       renderSetMaterial(obj->matdefault, obj->nadir, 1.0);        break;
                    case R_UNSELECTED:  renderSetMaterial(obj->matdefault, obj->nadir, obj->nadir); break;
                }
                
                glPushAttrib(GL_POLYGON_BIT);
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1.0, -1.0);
                
                /* And draw the bar */
                GLfloat maxh = obj->values[i][j] / obj->higher * obj->maxy;
                renderBar(obj->width,
                    maxh,
                    obj->xinit + obj->inc*j,
                    obj->zinit + obj->inc*(obj->znum -i -1),
                    0.0);
                glPopAttrib();

                /* Draw the sub sets */
                for (k = obj->nsets - 1, os = 0; k >= 0; k--) {
                    if (!obj->sets[k].show)
                        continue;
                    switch (obj->states[i][j]) {
                        case R_DEFAULT:     renderSetMaterial(obj->sets[k].mat, 1.0, 1.0);               break;
                        case R_SELECTED:    renderSetMaterial(obj->sets[k].mat, obj->highlight, 1.0);    break;
                        case R_NADIR:       renderSetMaterial(obj->sets[k].mat, obj->nadir, 1.0);        break;
                        case R_UNSELECTED:  renderSetMaterial(obj->sets[k].mat, obj->nadir, obj->nadir); break;
                    }

                    /* And draw the subset bar */
                    GLfloat maxsf = obj->sets[k].values[i][j] / obj->higher * obj->maxy;
                    renderBar(obj->width,
                        (maxh - maxsf) + maxsf - os,
                        obj->xinit + obj->inc*j,
                        obj->zinit + obj->inc*(obj->znum -i -1),
                        maxh - maxsf - os);
                    os += maxsf;
                }
            }
        }
        GLfloat lw = 0.2, lh = obj->width;

    /* Draw labels */
    if (alphaing == 1) {
        double sx, sz, w, h;
        glEnable(GL_TEXTURE_2D);
        renderSetMaterial(obj->titles.matlabels, 1.0, 1.0);
        glNormal3f  (0, 1, 0);

        /* Draw z labels */
        sx = obj->minx - obj->gap;
        sz = obj->minz + obj->gap;
        h  = obj->inc - obj->gap;
        for(i = 0; i < obj->znum; i++) {
            glBindTexture(GL_TEXTURE_2D, obj->titles.ztextures[i]);
            glBegin(GL_QUADS);
                w  = h * obj->titles.zratios[i];

                glTexCoord2i(1,0); glVertex3f(sx,     0.0, sz + obj->inc * i);
                glTexCoord2i(0,0); glVertex3f(sx - w, 0.0, sz + obj->inc * i);
                glTexCoord2i(0,1); glVertex3f(sx - w, 0.0, sz + obj->inc * i + h);
                glTexCoord2i(1,1); glVertex3f(sx,     0.0, sz + obj->inc * i + h);

                glTexCoord2i(0,0); glVertex3f(-sx,     0.0, sz + obj->inc * i);
                glTexCoord2i(0,1); glVertex3f(-sx,     0.0, sz + obj->inc * i + h);
                glTexCoord2i(1,1); glVertex3f(-sx + w, 0.0, sz + obj->inc * i + h);
                glTexCoord2i(1,0); glVertex3f(-sx + w, 0.0, sz + obj->inc * i);
            glEnd();
        }
        
        /* Draw x labels */
        sx = obj->minx + obj->gap;
        sz = obj->minz - obj->gap;
        h  = obj->inc - obj->gap;
        for(i = 0; i < obj->xnum; i++) {
            glBindTexture(GL_TEXTURE_2D, obj->titles.xtextures[i]);
            glBegin(GL_QUADS);
                w  = h * obj->titles.xratios[i];

                glTexCoord2i(0,0); glVertex3f(sx + obj->inc * i,     0.0, sz);
                glTexCoord2i(0,1); glVertex3f(sx + obj->inc * i + h, 0.0, sz);
                glTexCoord2i(1,1); glVertex3f(sx + obj->inc * i + h, 0.0, sz - w);
                glTexCoord2i(1,0); glVertex3f(sx + obj->inc * i,     0.0, sz - w);

                glTexCoord2i(0,0); glVertex3f(sx + obj->inc * i,     0.0, -sz + w);
                glTexCoord2i(0,1); glVertex3f(sx + obj->inc * i + h, 0.0, -sz + w);
                glTexCoord2i(1,1); glVertex3f(sx + obj->inc * i + h, 0.0, -sz);
                glTexCoord2i(1,0); glVertex3f(sx + obj->inc * i,     0.0, -sz);
            glEnd();
        }


        /*Draw axis labels */
        h = 0.2;
        renderSetMaterial(obj->titles.matlabels, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, obj->titles.xaxistexture);
        hw = h * obj->titles.xaxisratio, hh = h,
        lx = obj->maxx + obj->gap, ly = obj->miny, lz = obj->minz - hh/2.0;
        glBegin(GL_QUADS);
            glTexCoord2i(0,0); glVertex3f(lx,       ly, lz);
            glTexCoord2i(0,1); glVertex3f(lx,       ly, lz + hh);
            glTexCoord2i(1,1); glVertex3f(lx + hw,  ly, lz + hh);
            glTexCoord2i(1,0); glVertex3f(lx + hw,  ly, lz);
        glEnd();
        hw = h * obj->titles.yaxisratio, hh = h;
        lx = obj->minx - hw / 2.0, ly = obj->maxy + obj->gap, lz = obj->minz - hh / 2.0;
        glBindTexture(GL_TEXTURE_2D, obj->titles.yaxistexture);
        glBegin(GL_QUADS);
            glTexCoord2i(0,0); glVertex3f(lx,       ly, lz);
            glTexCoord2i(0,1); glVertex3f(lx,       ly, lz + hh);
            glTexCoord2i(1,1); glVertex3f(lx + hw,  ly, lz + hh);
            glTexCoord2i(1,0); glVertex3f(lx + hw,  ly, lz);
        glEnd();
        hw = h * obj->titles.zaxisratio, hh = h;
        lx = obj->minx - hw / 2.0, ly = obj->miny, lz = obj->maxz + obj->gap;
        glBindTexture(GL_TEXTURE_2D, obj->titles.zaxistexture);
        glBegin(GL_QUADS);
            glTexCoord2i(0,0); glVertex3f(lx,       ly, lz);
            glTexCoord2i(0,1); glVertex3f(lx,       ly, lz + hh);
            glTexCoord2i(1,1); glVertex3f(lx + hw,  ly, lz + hh);
            glTexCoord2i(1,0); glVertex3f(lx + hw,  ly, lz);
        glEnd();
        glDisable(GL_TEXTURE_2D);

    }

    glPopMatrix();
}

void renderBar(GLfloat size, GLfloat h, GLfloat x, GLfloat z, GLfloat f) {
    GLfloat inc = size / 2.0;

    glBegin(GL_QUADS);
        /* Face Lf */
        glNormal3f  (0, 0, 1);
        glVertex3f(x - inc, f,  z + inc);
        glVertex3f(x + inc, f,  z + inc);
        glVertex3f(x + inc, h,  z + inc);
        glVertex3f(x - inc, h,  z + inc);
        
        /* Face Lr */
        glNormal3f  (1, 0, 0);
        glVertex3f(x + inc, f,  z + inc);
        glVertex3f(x + inc, f,  z - inc);
        glVertex3f(x + inc, h,  z - inc);
        glVertex3f(x + inc, h,  z + inc);
        
        /* Face Lb */
        glNormal3f  (0, 0, -1);
        glVertex3f(x + inc, f,  z - inc);
        glVertex3f(x - inc, f,  z - inc);
        glVertex3f(x - inc, h,  z - inc);
        glVertex3f(x + inc, h,  z - inc);
        
        /* Face Ll */
        glNormal3f  (-1, 0, 0);
        glVertex3f(x - inc, f,  z - inc);
        glVertex3f(x - inc, f,  z + inc);
        glVertex3f(x - inc, h,  z + inc);
        glVertex3f(x - inc, h,  z - inc);
        
        /* Face Top */
        glNormal3f  (0, 1, 0);
        glVertex3f(x - inc, h,  z + inc);
        glVertex3f(x + inc, h,  z + inc);
        glVertex3f(x + inc, h,  z - inc);
        glVertex3f(x - inc, h,  z - inc);

        /* Face Buttom */
        glNormal3f  (0, -1, 0);
        glVertex3f(x - inc, f,  z + inc);
        glVertex3f(x + inc, f,  z + inc);
        glVertex3f(x + inc, f,  z - inc);
        glVertex3f(x - inc, f,  z - inc);
    glEnd();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * *  TOOLS  * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
renderObject * renderGenObject(int sizez, int sizex, int shape, int values, int colour, int labels) {
    renderObject * newobj = renderGenObjectShape(shape, sizez, sizex);
    
    renderGenObjectValues(newobj, values);
    renderGenObjectColour(newobj, colour);
    renderGenObjectLabels(newobj, labels);
    
    return newobj;
}

renderObject * renderGenObjectShape(int shape, int sizez, int sizex) {
    renderObject * newobj;
    float size, height, fattiness;

    switch (shape) {
        case 1: /* Thin */
            size      = 2;
            height    = 1;
            fattiness = 0.4;
            break;
        case 2: /* Fat */
            size      = 2;
            height    = 1;
            fattiness = 0.925;
            break;
        case 0:     /* Default */
        default:
            size      = 2;
            height    = 1;
            fattiness = 0.75;
            break;
    }
    return renderNewObj(sizez, sizex, size, height, fattiness);
}

renderObject * renderGenObjectMerge(renderObject *first, renderObject *second) {

    renderObject *newObj = renderGenObjectShape(0, first->znum, first->xnum);

    renderGenObjectColour(newObj, 0);

    renderAddSetToObj(newObj, first->values);
    renderAddSetToObj(newObj, second->values);

    newObj->titles            = first->titles;
    newObj->xlegends          = first->xlegends;
    newObj->zlegends          = first->zlegends;
    newObj->xlegendtexture    = first->xlegendtexture;
    newObj->zlegendtexture    = first->zlegendtexture;
    newObj->xaxislabel        = first->xaxislabel;
    newObj->yaxislabel        = first->yaxislabel;
    newObj->zaxislabel        = first->zaxislabel;
    newObj->xaxislabeltexture = first->xaxislabeltexture;
    newObj->yaxislabeltexture = first->yaxislabeltexture;
    newObj->zaxislabeltexture = first->zaxislabeltexture;

    newObj->sets[0].show = 1;
    newObj->sets[1].show = 1;

    newObj->hide = 1;

    return newObj;

}

renderObject * renderGenObjectValues(renderObject * obj, int values) {
    GLfloat ** set;
    int i, j;

    switch (values) {
        case 1: /* noise/random */
            set = (GLfloat **) calloc(obj->znum, sizeof(GLfloat *));
            for (i = 0; i < obj->znum; i++) {
                set[i] = (GLfloat *) calloc(obj->xnum, sizeof(GLfloat));
                for (j = 0; j < obj->xnum; j++) {
                    set[i][j] = (((rand() % 100) / 100.0)) / 7.0;
                }
            }
            break;
        case 2: /* constant */
            set = (GLfloat **) calloc(obj->znum, sizeof(GLfloat *));
            for (i = 0; i < obj->znum; i++) {
                set[i] = (GLfloat *) calloc(obj->xnum, sizeof(GLfloat));
                for (j = 0; j < obj->xnum; j++) {
                    set[i][j] = 0.25;
                }
            }
            break;
        case 0:     /* Default */
        default:    /* 3D sin */
            set = (GLfloat **) calloc(obj->znum, sizeof(GLfloat *));
            for (i = 0; i < obj->znum; i++) {
                set[i] = (GLfloat *) calloc(obj->xnum, sizeof(GLfloat));
                GLfloat a = sin((i+1) * (3/(float)obj->znum));
                for (j = 0; j < obj->xnum; j++) {
                    set[i][j] = sin((j+1) * (3/(float)obj->xnum)) * a;
                }
            }
            break;
    }

    renderAddSetToObj(obj, set);
    for (i = 0; i < obj->znum; i++)
        free(set[i]);
    free(set);

    return obj;
}

renderObject * renderGenObjectColour(renderObject * obj, int colour) {
    int i;
    renderMaterial rm;
    rm.ambient.r = rm.ambient.g = rm.ambient.b = 0;
    rm.ambient.a = 1.0;
    rm.emission = rm.specular = rm.ambient;
    rm.shininess = 1;
    switch (colour) {
        case 1: /*  */

            break;
        case 2: /*  */

            break;
        case 0:     /* Default */
        default:    /* Default blue : random subsets */
            rm.diffuse = renderHSVtoRGB(6 * 36, 0.9, 0.9, 1.0);
            obj->matdefault = rm;
            rm.diffuse = renderHSVtoRGB(6 * 36, 0.4, 0.2, 1.0);
            obj->matfloor = rm;
            for (i = 0; i < R_DEFmaxsets; i++) {
                rm.diffuse = renderHSVtoRGB((randInitColor++ % 10) * 36, 0.9, 0.9, 1.0);
                obj->sets[i].mat = rm;
            }
            /* For the titles */
            rm.diffuse = renderHSVtoRGB(0, 0.0, 1.0, 1.0);
            obj->titles.mataxis =
                obj->titles.matlabels =
                    obj->titles.matbars = rm;
            break;
    }
    return obj;
}

renderObject * renderGenObjectLabels(renderObject * obj, int labels) {
    int i, j;

    switch (labels) {
        case 1: /*  */
            
            break;
        case 2: /*  */
            
            break;
        case 0:     /* Default */
        default:    /* Batatas, vendas, meses */
            obj->titles.xaxislabel = "Tipos de batata";
            obj->titles.yaxislabel = "Vendas";
            obj->titles.zaxislabel = "Mes";

            for (i = 0; i < obj->xnum; i++) {
                obj->titles.xlabels[i] = (char *)calloc(10, sizeof(char));
                sprintf(obj->titles.xlabels[i], "Batata %c", 'A' + i);
            }
            char months[12][14] = { "Janeiro", "Fevereiro", "Marco",
                                "Abril", "Maio", "Junho",
                                "Julho", "Agosto", "Setembro",
                                "Outubro", "Novembro", "Dezembro"};
            for (i = 0; i < obj->znum; i++) {
                obj->titles.zlabels[i] = (char *)calloc(15, sizeof(char));
                sprintf(obj->titles.zlabels[i], "%s", months[i % 12]);
            }
            break;
    }

    obj->titles.xaxistexture = renderTextTexture(obj->titles.xaxislabel, &obj->titles.xaxisratio, 0);
    obj->titles.yaxistexture = renderTextTexture(obj->titles.yaxislabel, &obj->titles.yaxisratio, 0);
    obj->titles.zaxistexture = renderTextTexture(obj->titles.zaxislabel, &obj->titles.zaxisratio, 0);

    for (i = 0; i < obj->xnum; i++)
        obj->titles.xtextures[i] = renderTextTexture(obj->titles.xlabels[i], &obj->titles.xratios[i], 0);

    for (i = 0; i < obj->znum; i++)
        obj->titles.ztextures[i] = renderTextTexture(obj->titles.zlabels[i], &obj->titles.zratios[i], 0);

    return obj;
}
