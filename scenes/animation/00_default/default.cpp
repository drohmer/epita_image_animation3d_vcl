
#include "default.hpp"

#include <random>

#ifdef SCENE_DEFAULT_3D_GRAPHICS

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;


/** This function is called before the beginning of the animation loop
    It is used to declare and initialize data that will be used later during display */
void scene_model::setup_data(std::map<std::string, GLuint>& shaders, scene_structure&, gui_structure& gui)
{
    // Create a mesh structure containing a quadrangle defined by four corner-vertex positions.
    const mesh mesh_plane = mesh_primitive_quad({ -2,-1,-2 }, { -2,-1,2 }, { 2,-1,2 }, { 2,-1,-2 });
    // Convert the mesh structure into object that can be displayed (mesh_drawable)
    plane = mesh_drawable(mesh_plane); // note that plane is an attribute of the class (declared in .hpp file)
    plane.shader = shaders["mesh"];    // A default shader can be set as an attribute of the object (used when calling the draw function)

    // Create similarily a cylinder
    cylinder = mesh_drawable(mesh_primitive_cylinder(0.2f, { 0,-1,0 }, { 0,1,0 }, 20, 20)); // Note that a mesh_drawable can be directly constructed from a mesh
    cylinder.uniform.color = { 0.8f,0.8f,1 }; // can set the color (R,G,B) used in the shader
    cylinder.shader = shaders["mesh"];      // Default shader for the cylinder

    // Create similarily a cube
    cube = mesh_drawable(mesh_primitive_parallelepiped());
    cube.uniform.color = { 1,1,0 };
    cube.shader = shaders["mesh"];


    // Create a curve
    // **************************************** //
    std::vector<vec3> curve_cpu;    // the basic structure of a curve is a vector of vec3


    const size_t N_curve = 150;     // Number of samples of the curve
    for (size_t k = 0; k < N_curve; ++k)
    {
        const float u = k / (N_curve - 1.0f); // u \in [0,1]

        // curve oscillating as a cosine
        const float x = 0;
        const float y = 0.1f * std::cos(u * 16 * 3.14f);
        const float z = 4.0f * (u - 0.5f);

        curve_cpu.push_back({ x,y,z });
    }
    // send data to GPU and store it into a curve_drawable structure
    curve = curve_drawable(curve_cpu);
    curve.uniform.color = { 0,1,0 };
    curve.shader = shaders["curve"];
    // **************************************** //




    // allow by default the display of helper visual frames
    gui.show_frame_worldspace = true;
    gui.show_frame_camera = true;

}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{

    // ********************************************* //
    // Update timer and GUI
    // ********************************************* //

    // Update the GUI with the timer passed as parameter
    set_gui();

    // Update timer to get current time in the animation loop
    timer.update();
    const float time = timer.t;




    // ********************************************* //
    // Display objects
    // ********************************************* //


    // Display plane
    // ********************************************* //

    // the general syntax to display a mesh is:
    //   draw(objectName, camera [,optional: shaderID ]);
    draw(plane, scene.camera);



    // Display cylinder
    // ********************************************* //

    // The cylinder is rotated around the axis (1,0,0), by an angle = time/2
    const vec3 axis_of_rotation = { 1,0,0 };
    const float angle_of_rotation = time / 2.0f;
    // Creation of the 3x3 rotation matrix
    const mat3 rotation = rotation_from_axis_angle_mat3(axis_of_rotation, angle_of_rotation);

    // Set translation and rotation parameters (send and used in shaders using uniform variables)
    cylinder.uniform.transform.translation = { 1.5,0,0 };
    cylinder.uniform.transform.rotation = rotation;

    // Display of the cylinder
    draw(cylinder, scene.camera);

    // Meshes can also be displayed as wireframe using the specific shader
    draw(cylinder, scene.camera, shaders["wireframe"]);


    // Display cube
    // ********************************************* //
    cube.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0,1,0 }, std::sin(3 * time));
    cube.uniform.transform.translation = { -1,0,0 };
    draw(cube, scene.camera);


    // Display curve
    // ********************************************* //
    curve.uniform.transform.translation = { 1.9f,0,0 };
    curve.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, time);
    draw(curve, scene.camera);





}

/** Update the visual GUI */
void scene_model::set_gui()
{
    // Slider to set time scaling factor
    ImGui::SliderFloat("Time scale", &timer.scale, 0.05f /*min value*/, 4.0f /*max value*/);

    // Stop/Start time
    bool const stop = ImGui::Button("Stop"); ImGui::SameLine();
    bool const start = ImGui::Button("Start");
    if (stop)  timer.stop();
    if (start) timer.start();


}

#endif

