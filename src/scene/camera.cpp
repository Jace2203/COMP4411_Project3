#include "camera.h"

#include "../ui/TraceUI.h"

extern TraceUI *traceUI;

#define PI 3.14159265359
#define SHOW(x) (cerr << #x << " = " << (x) << "\n")

Camera::Camera()
{
    aspectRatio = 1;
    normalizedHeight = 1;
    
    eye = vec3f(0,0,0);
    u = vec3f( 1,0,0 );
    v = vec3f( 0,1,0 );
    look = vec3f( 0,0,-1 );
}

void
Camera::rayThrough( double x, double y, ray &r )
// Ray through normalized window point x,y.  In normalized coordinates
// the camera's x and y vary both vary from 0 to 1.
{
    x -= 0.5;
    y -= 0.5;
    vec3f dir = (look + x * u + y * v).normalize();

    if (traceUI->getDOF())
    {
        vec3f ux = dir.cross(vec3f(1, 0, 0)).normalize();
        if (ux.iszero()) ux = dir.cross(vec3f(0, 1, 0)).normalize();
        vec3f vx = dir.cross(ux).normalize();

        int part = 50;
        vec3f focus_pt = eye + dir.normalize() * traceUI->getFocalLength();
        // vec3f new_eye = eye + prod(vec3f(rand() % (part + 1) * 1.0/part - 0.5, rand() % (part + 1) * 1.0/part - 0.5, 0), (u + v).normalize() ) * traceUI->getApertureSize() * 0.1;
        
        vec3f new_eye = eye + prod(vec3f(rand() % (part + 1) * 2.0/part - 1.0, rand() % (part + 1) * 2.0/part - 1.0, 0), (ux + vx).normalize() ) * traceUI->getApertureSize() * 0.1;
        r = ray( new_eye, (focus_pt - new_eye).normalize() );
    }
    else
        r = ray( eye, dir.normalize() );

    // std::cout << eye << endl;
    // std::cout << new_eye << endl;

}

void
Camera::setEye( const vec3f &eye )
{
    this->eye = eye;
}

void
Camera::setLook( double r, double i, double j, double k )
// Set the direction for the camera to look using a quaternion.  The
// default camera looks down the neg z axis with the pos y axis as up.
// We derive the new look direction by rotating the camera by the
// quaternion rijk.
{
                                // set look matrix
    m[0][0] = 1.0 - 2.0 * (i * i + j * j);
    m[0][1] = 2.0 * (r * i - j * k);
    m[0][2] = 2.0 * (j * r + i * k);
    
    m[1][0] = 2.0 * (r * i + j * k);
    m[1][1]= 1.0 - 2.0 * (j * j + r * r);
    m[1][2] = 2.0 * (i * j - r * k);
    
    m[2][0] = 2.0 * (j * r - i * k);
    m[2][1] = 2.0 * (i * j + r * k);
    m[2][2] = 1.0 - 2.0 * (i * i + r * r);

    update();
}

void
Camera::setLook( const vec3f &viewDir, const vec3f &upDir )
{
    vec3f z = -viewDir;          // this is where the z axis should end up
    const vec3f &y = upDir;      // where the y axis should end up
    vec3f x = y.cross(z);               // lah,

    m = mat3f( x,y,z ).transpose();

    update();
}

void
Camera::setFOV( double fov )
// fov - field of view (height) in degrees    
{
    fov /= (180.0 / PI);      // convert to radians
    normalizedHeight = 2 * tan( fov / 2 );
    update();
}

void
Camera::setAspectRatio( double ar )
// ar - ratio of width to height
{
    aspectRatio = ar;
    update();
}

void
Camera::update()
{
    u = m * vec3f( 1,0,0 ) * normalizedHeight*aspectRatio;
    v = m * vec3f( 0,1,0 ) * normalizedHeight;
    look = m * vec3f( 0,0,-1 );
}




