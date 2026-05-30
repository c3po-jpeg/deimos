#include <iostream>
#include "headers/application.hxx"
#include "headers/scene.hxx"

int main(int argc, char *argv[])
{
    try
    {
        const int WINDOW_WIDTH  = 800;
        const int WINDOW_HEIGHT = 600;

        // Initialize application (Vulkan, SDL, rendering pipeline)
        Application app("Enceladus - Vulkan | SDL2", WINDOW_WIDTH, WINDOW_HEIGHT);

        {
            Scene scene(app.getCore(), WINDOW_WIDTH, WINDOW_HEIGHT);
            // Add game objects
            // Large static floor
            scene.addCubeSphere(0.0f, -1002.0f, 0.0f, 1000.0f, 300, 
                Material::checker({1.0,1.0,1.0}, {0.4,0.4,0.4}, 300.0), 
                1.0f, true); 

            scene.addCubeSphere(-0.5f, 3.0f, -0.5f, 0.5f, 40);
            scene.addCubeSphere( 2.0f, 2.0f, -1.0f, 0.5f, 40);
            // app.toggleWireframe(); // Start in wireframe mode for debugging
            app.run(scene);
        
        }
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
}
