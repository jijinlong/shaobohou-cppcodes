#include "CrossPlatform.h"

#include "AcclaimDisplay.h"

#include "special.h"
#include "GLUtils.h"


void AcclaimDisplay::draw(const KinematicModel<AcclaimJoint> &model, const Vector3D &colour)
{
    for(int i = 0; i < model.njoints(); i++)
    {
        const AcclaimJoint &joint = model.joints(i);

        glPushMatrix();
            glTranslate(joint.global_position());
            glRotate(joint.global_orientation());
            drawAxis();
        glPopMatrix();

        if(joint.parent() >= 0)
        {
            glBegin(GL_LINES);
                glVertex(joint.global_position());
                glVertex(model.getParent(joint).global_position());
            glEnd();
        }

        Vector3D global_direction = joint.global_endeffector_displacement().direction();
        double length = joint.local_endeffector().magnitude();

        glColor3d(colour[0], colour[1], colour[2]);
        glPushMatrix();
            glTranslate(joint.global_position());
            glTranslate(global_direction* joint.local_endeffector().magnitude() * 0.5);

            //align bone
            Vector3D zAxis = Vector3D(0.0, 0.0, 1.0);
            Vector3D newAxis = zAxis ^ global_direction;
            double newAngle = acos(zAxis *global_direction);
            glRotated(rad2deg(newAngle), newAxis[0], newAxis[1], newAxis[2]);

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

void AcclaimDisplay::drawAxis()
{
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(2.0, 0., 0.0);

    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 2.0, 0.0);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 2.0);
    glEnd();
}
