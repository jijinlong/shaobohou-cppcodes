#include "CrossPlatform.h"

#include "BiovisionDisplay.h"

#include "special.h"
#include "GLUtils.h"

#include <iostream>


void BiovisionDisplay::draw(const KinematicModel<BiovisionJoint> &model, const Vector3D &colour, bool drawAxis)
{
    for(int i = 0; i < model.njoints(); i++)
    {
        const BiovisionJoint &joint = model.joints(i);

        if(drawAxis)
        {
            glPushMatrix();
                glTranslate(joint.global_position());
                glRotate(joint.global_orientation());

                glBegin(GL_LINES);
                    glColor3f(1.0, 0.0, 0.0);
                    glVertex3f(0.0, 0.0, 0.0);
                    glVertex3f(20.0, 0., 0.0);

                    glColor3f(0.0, 1.0, 0.0);
                    glVertex3f(0.0, 0.0, 0.0);
                    glVertex3f(0.0, 20.0, 0.0);

                    glColor3f(0.0, 0.0, 1.0);
                    glVertex3f(0.0, 0.0, 0.0);
                    glVertex3f(0.0, 0.0, 20.0);
                glEnd();
            glPopMatrix();
        }

        if(joint.parent() >= 0)
        {
            glBegin(GL_LINES);
                glVertex(joint.global_position());
                glVertex(model.getParent(joint).global_position());
            glEnd();

            Vector3D global_direction = (joint.global_position()-model.getParent(joint).global_position()).direction();
            double length = joint.local_position().magnitude();

            glColor(colour);
            glPushMatrix();
                glTranslate(joint.global_position());
                glTranslate(-global_direction*length*0.5);

                //align bone
                Vector3D zAxis = Vector3D(0.0, 0.0, 1.0);
                Vector3D newAxis = zAxis ^ global_direction;
                double newAngle = acos(zAxis *global_direction);
                glRotateRad(newAngle, newAxis);

                //draw bone
                GLUquadricObj *qobj = gluNewQuadric();
                gluQuadricDrawStyle(qobj, GLU_FILL);
                gluQuadricNormals(qobj, GLU_SMOOTH);
                glScalef(0.25, 0.25, 1.0);
                gluSphere(qobj, length*0.5, 20, 20);
                gluDeleteQuadric(qobj);
            glPopMatrix();
        }
    }
}
