#ifndef VISUALISATION_H
#define VISUALISATION_H

#include "KinematicModel.h"
#include "GLUtils.h"

#include <GL/glu.h>


template <typename JointParam>
void drawModel(const KinematicModel<JointParam> &model, const Vector3D& colour, double model_size, const bool drawJoints, const bool drawAxis)
{
    for (int i = 0; i < model.njoints(); i++)
        drawJoint(model, i, colour, model_size, drawJoints, drawAxis);
}

template <typename JointParam>
void drawJoint(const KinematicModel<JointParam> &model, int ind, const Vector3D& colour, double model_size, const bool drawJoints, const bool drawAxis)
{
    if (drawAxis)
    {
        glPushMatrix();
        glTranslate(model.joints(ind).global_position());
        glRotate(model.joints(ind).global_orientation());

        glBegin( GL_LINES );
        glColor3f( 1.0, 0.0, 0.0 );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 1.0, 0.0, 0.0 );

        glColor3f( 0.0, 1.0, 0.0 );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.0, 1.0, 0.0 );

        glColor3f( 0.0, 0.0, 1.0 );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.0, 0.0, 1.0 );
        glEnd();

        glPopMatrix();
    }

    glColor(colour);

    GLUquadricObj *sphere;

    if (drawJoints)
    {
        sphere = gluNewQuadric();
        glPushMatrix();
            glTranslate(model.joints(ind).global_position());
            glColor3f(1.0-colour[0], 1.0-colour[1], 1.0-colour[2]);
            gluQuadricDrawStyle(sphere, GLU_FILL);
            gluSphere(sphere, model_size/60.0, 20, 20 );
        glPopMatrix();
        gluDeleteQuadric(sphere);
    }

    if (drawJoints)
    {
        glPushMatrix();
            glBegin(GL_LINES);
//                 glColor(Vector3D(0.0, 0.0, 1.0));
//                 glVertex(model.joints(ind).global_endeffector());
//                 glVertex(model.joints(ind).global_position());

                glColor(Vector3D(1.0, 1.0, 1.0));
                if(model.joints(ind).parent() >= 0)
                {
                    glVertex(model.getParent(model.joints(ind)).global_position());
                    glVertex(model.joints(ind).global_position());
                }
            glEnd();
        glPopMatrix();
    }

    // draw endeffector
    sphere = gluNewQuadric();
    glPushMatrix();
        glTranslate(model.joints(ind).global_endeffector());
        glColor(Vector3D(colour[0], colour[1], 1.0-colour[2]));
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluSphere(sphere, model_size/60.0, 20, 20 );
    glPopMatrix();
    gluDeleteQuadric(sphere);
}

#endif
