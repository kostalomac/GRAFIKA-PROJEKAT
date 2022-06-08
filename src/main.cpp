    #pragma region INCLUDES
    #include <stb_image.h>
    #include "imgui.h"
    #include "imgui_impl_glfw.h"
    #include "imgui_impl_opengl3.h"

    #include <glad/glad.h>
    #include <GLFW/glfw3.h>

    #include <../glm/glm/glm.hpp>
    #include <../glm/glm/gtc/matrix_transform.hpp>
    #include <../glm/glm/gtc/type_ptr.hpp>

    #include <learnopengl/filesystem.h>
    #include <learnopengl/shader.h>
    #include <learnopengl/camera.h>
    #include <learnopengl/model.h>

    #include <iostream>

    #pragma endregion INCLUDES





    #pragma region FUNCTION-DECLARATIONS


    void framebuffer_size_callback(GLFWwindow *window, int width, int height);

    void mouse_callback(GLFWwindow *window, double xpos, double ypos);

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

    void processInput(GLFWwindow *window);

    void processInputScreenShejder(GLFWwindow  *window,Shader *nasShejder ) ;

        void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

    #pragma endregion FUNCTION-DECLARATIONS

    #pragma region GLOBAL-VARIABLES
    // settings
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    // camera

    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;
    //KERNEL EFEKAT
    bool ulkjuciKernel = false;
    bool ulkjuciBLUR = false;
    bool hdr = false;
    float exposure ;


    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    struct PointLight {
        glm::vec3 position;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };


    struct DirectionalLight {
        glm::vec3 direction;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };

    struct ProgramState {
        glm::vec3 clearColor = glm::vec3(0);
        bool ImGuiEnabled = false;
        Camera camera;
        bool CameraMouseMovementUpdateEnabled = true;
        glm::vec3 backpackPosition = glm::vec3(0.0f);
        float backpackScale = 1.0f;
        PointLight pointLight;
        ProgramState()
                : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

        void SaveToFile(std::string filename);

        void LoadFromFile(std::string filename);
    };

    #pragma endregion GLOBAL-VARIABLES

    #pragma region PROGRAM-STATE-FUNCTIONS


    void ProgramState::SaveToFile(std::string filename) {
        std::ofstream out(filename);
        out << clearColor.r << '\n'
            << clearColor.g << '\n'
            << clearColor.b << '\n'
            << ImGuiEnabled << '\n'
            << camera.Position.x << '\n'
            << camera.Position.y << '\n'
            << camera.Position.z << '\n'
            << camera.Front.x << '\n'
            << camera.Front.y << '\n'
            << camera.Front.z << '\n';
    }

    void ProgramState::LoadFromFile(std::string filename) {
        std::ifstream in(filename);
        if (in) {
            in >> clearColor.r
               >> clearColor.g
               >> clearColor.b
               >> ImGuiEnabled
               >> camera.Position.x
               >> camera.Position.y
               >> camera.Position.z
               >> camera.Front.x
               >> camera.Front.y
               >> camera.Front.z;
        }
    }

    ProgramState *programState;

    #pragma endregion PROGRAM-STATE-FUNCTIONS

void DrawImGui(ProgramState *programState);



    glm::vec3 ColorOfCube =glm::vec3(1.0, 1.0, 1.0) ;

int main() {
    exposure = 1.0f;
#pragma region SETUP

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;



    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
#pragma endregion SETUP

    #pragma region SHADER-MODEL-LOAD

    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader cubeShader("resources/shaders/cubeShader.vs", "resources/shaders/cubeShader.fs");
    Shader svetloShader("resources/shaders/cubeShader.vs", "resources/shaders/cubeShaderSmallBox.fs");
    Shader ScreenShacer("resources/shaders/framebuffershader.vs", "resources/shaders/framebuffershader.fs");
    Shader BlurShader("resources/shaders/BlurShader.vs", "resources/shaders/BlurShader.fs");
    Shader IzvodjSvetla("resources/shaders/BlurShader.vs", "resources/shaders/IzdvojSvetla.fs");
    Shader BloomFinal("resources/shaders/BlurShader.vs","resources/shaders/BloomFinal.fs");
    // load models
    // -----------
    Model ourModel("resources/objects/male/Male.obj");
    ourModel.SetShaderTextureNamePrefix("material.");

    #pragma endregion SHADER-MODEL-LOAD

#pragma region BLUR-SETUP


#pragma endregion BLUR-SETUP

    #pragma region KOCKA-SETUP

    float quadVertices[] = {
            -1.0f,1.0f,0.0f,1.0f,
            -1.0f,-1.0f,0.0f,0.0f,
            1.0f,-1.0f,1.0f,0.0f,

            -1.0f,1.0f,0.0f,1.0f,
            1.0f,-1.0f,1.0f,0.0f,
            1.0f,1.0f,1.0f,1.0f
    };

    float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f , 0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,1.0f , 0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,1.0f , 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f , 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,0.0f , 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,0.0f , 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,0.0f , 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,1.0f , 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,1.0f , 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,1.0f , 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,0.0f , 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,0.0f , 0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,1.0f , 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,1.0f , 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,0.0f , 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,0.0f , 1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,0.0f , 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,1.0f , 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,1.0f , 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,1.0f , 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,1.0f , 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,1.0f , 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,0.0f , 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,1.0f , 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,0.0f , 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,1.0f , 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,1.0f , 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,1.0f , 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,0.0f , 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,0.0f , 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,0.0f , 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f , 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,1.0f , 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,1.0f , 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,0.0f , 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,0.0f , 1.0f,
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);




    unsigned int VBOquadVertices, VAOquadVertices;
    glGenVertexArrays(1, &VAOquadVertices);
    glGenBuffers(1, &VBOquadVertices);

    glBindVertexArray(VAOquadVertices);

    glBindBuffer(GL_ARRAY_BUFFER, VBOquadVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);





#pragma endregion KOCKA-SETUP

    #pragma region LIGHT-DECLARATION

    PointLight& pointLight = programState->pointLight;
    pointLight.position = glm::vec3(4.0f, 4.0, 0.0);
    pointLight.ambient = glm::vec3(0.1, 0.1, 0.1);
    pointLight.diffuse = glm::vec3(1.0, 1.0, 1.0);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

    #pragma endregion LIGHT-DECLARATION

    #pragma region TEXTURA-SETUP
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/box.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);



    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data2 = stbi_load(FileSystem::getPath("resources/textures/adventure.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data2);

    ourShader.setInt("texture1", 0);

    ourShader.setInt("texture2", 1);


#pragma endregion TEXTURA-SETUP



    #pragma region SCREENSHADER-SETUP

    ScreenShacer.use();
    ScreenShacer.setInt("screenTexture",0);




    unsigned int fbo;
    glGenFramebuffers(1,&fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,fbo);

    unsigned int textureFramebuffer[2];
    glGenTextures(2,textureFramebuffer);

    for (unsigned int i = 0 ; i < 2 ; ++i) {
        glBindTexture(GL_TEXTURE_2D,textureFramebuffer[i]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,SCR_WIDTH,SCR_HEIGHT,0,GL_RGBA,GL_FLOAT,NULL);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0 + i,GL_TEXTURE_2D,textureFramebuffer[i],0);
    }


    unsigned int rbo;
    glGenRenderbuffers(1,&rbo);
    glBindRenderbuffer(GL_RENDERBUFFER,rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,SCR_WIDTH,SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo);

    unsigned int attachment[2] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2,attachment);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE){
        std::cout << "YAYYYYY";
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);




    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2,pingpongFBO);
    glGenTextures(2,pingpongColorbuffers);
    for(unsigned int i = 0; i < 2; ++i){
        glBindFramebuffer(GL_FRAMEBUFFER,pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D,pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,SCR_WIDTH,SCR_HEIGHT,0,GL_RGBA,GL_FLOAT,NULL);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0 ,GL_TEXTURE_2D,pingpongColorbuffers[i],0);
    }


#pragma endregion SCREENSHADER-SETUP

    #pragma region MAIN-LOOP
    BlurShader.use();
    BlurShader.setInt("image",0);
    BloomFinal.use();
    BloomFinal.setInt("scene",0);
    BloomFinal.setInt("bloomBlur",1);


    while (!glfwWindowShouldClose(window)) {


            #pragma region LOOP-SETUP

            // per-frame time logic
            // --------------------
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // input
            // -----
            processInput(window);
            processInputScreenShejder(window,&BloomFinal );
            glBindFramebuffer(GL_FRAMEBUFFER,fbo);
            glEnable(GL_DEPTH_TEST);

            // render
            // ------
            glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            #pragma endregion LOOP-SETUP



            #pragma region OUR-SHADER-SETUP

            // don't forget to enable shader before setting uniforms
            ourShader.use();
            ourShader.setInt("material.texture_diffuse1",0);
            ourShader.setInt("material.texture_specular1",0);
            ourShader.setFloat("material.shininess",1);
            ourShader.setVec3("viewPosition",programState->camera.Position);
            ourShader.setVec3("pointLight.position", glm::vec3(1.0,0.0,0.0));
            ourShader.setVec3("pointLight.ambient", glm::vec3(1.0,1.0,1.0));
            ourShader.setVec3("pointLight.diffuse", glm::vec3(1.0,1.0,1.0));
            ourShader.setVec3("pointLight.specular", glm::vec3(1.0,1.0,1.0));
            ourShader.setFloat("pointLight.constant",  0.05f);
            ourShader.setFloat("pointLight.linear",  0.05f);
            ourShader.setFloat("pointLight.quadratic", 0.05f);
            ourShader.setFloat("material.shininess", 1024.0f);



            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                    (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = programState->camera.GetViewMatrix();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);

            #pragma endregion OUR-SHADER-SETUP

            #pragma region MODEL-PRIZIVANJE

            // render the loaded model
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model,
                                   programState->backpackPosition); // translate it down so it's at the center of the scene
            model = glm::scale(model, glm::vec3(programState->backpackScale));    // it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);
            ourShader.setFloat("time",glfwGetTime());
            ourModel.Draw(ourShader);

            #pragma endregion MODEL-PRIZIVANJE

            #pragma region PRIZIVANJE-KOCKE

                #pragma region SPAWN-MALEKOCKE
                    cubeShader.use();

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texture);

                    glBindVertexArray(VAO);

                    cubeShader.setVec3("viewPosition",programState->camera.Position);
                    cubeShader.setInt("material.texture_diffuse1",0);
                    cubeShader.setInt("material.texture_specular1",0);
                    cubeShader.setFloat("material.shininess",1);
                    cubeShader.setVec3("directionalLight.direction", glm::vec3(0.0,-1.0,0.0));
                    cubeShader.setVec3("directionalLight.ambient", glm::vec3(0.1,0.1 ,0.1));
                    cubeShader.setVec3("directionalLight.diffuse", glm::vec3(0.5,0.5 ,0.5));
                    cubeShader.setVec3("directionalLight.specular", glm::vec3(0.3,0.3 ,0.3));
                    cubeShader.setFloat("directionalLight.constant",  0.2f);
                    cubeShader.setFloat("directionalLight.linear",  0.2f);
                    cubeShader.setFloat("directionalLight.quadratic", 0.2f);
                    cubeShader.setFloat("material.shininess", 1000.0f);



                    cubeShader.setVec3("pointLight.position", pointLight.position);
                    cubeShader.setVec3("pointLight.ambient", pointLight.ambient);
                    cubeShader.setVec3("pointLight.diffuse", pointLight.diffuse);
                    cubeShader.setVec3("pointLight.specular", pointLight.specular);
                    cubeShader.setFloat("pointLight.constant",  0.1f);
                    cubeShader.setFloat("pointLight.linear",  0.1f);
                    cubeShader.setFloat("pointLight.quadratic", 0.05f);
                    cubeShader.setFloat("material.shininess", 30.0f);


                    cubeShader.setVec3("spotlight.position",programState->camera.Position );
                    cubeShader.setVec3("spotlight.lightDir",programState->camera.Front );
                    cubeShader.setVec3("spotlight.ambient", glm::vec3(0.1,0.1 ,0.1));
                    cubeShader.setVec3("spotlight.diffuse", glm::vec3(0.4,0.4 ,0.4));
                    cubeShader.setVec3("spotlight.specular", glm::vec3(1.0,1.0 ,1.0));
                    cubeShader.setFloat("spotlight.constant",  0.1f);
                    cubeShader.setFloat("spotlight.linear",  0.1f);
                    cubeShader.setFloat("spotlight.quadratic", 0.05f);
                    cubeShader.setFloat("material.shininess", 30.0f);


                    cubeShader.setMat4("projection", projection);
                    cubeShader.setMat4("view", view);
                    cubeShader.setMat4("model", glm::mat4(1.0f));



                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texture2);



                    int temp = 1;
                    for (int i = 3; i < 9; ++i) {
                        temp = temp * (-1);
                        model = glm::scale(glm::mat4 (1.0f), glm::vec3 (1.0,1.0,1.0));
                        model = glm::translate(model, glm::vec3 (1.0 + i * temp * cos(temp*i*70.2),1.0 - i* temp * cos(temp*i*11.2),i * sin(temp*i*30.2)));
                        cubeShader.setMat4("model", model);
                        glDrawArrays(GL_TRIANGLES, 0, 36);

                    }
                #pragma endregion SPAWN-MALEKOCKE

                #pragma region SPAWN-VELIKA-KUTIJA

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texture);

                    model = glm::scale(glm::mat4 (1.0f), glm::vec3 (20.0,20.0,20.0));
                    model = glm::translate(model, glm::vec3 (0.0f,0.0f,0.0f));
                    cubeShader.setMat4("model", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);



                #pragma endregion SPAWN-VELIKA-KUTIJA

                #pragma region SPAWN-SVETLO

                                svetloShader.use();


                                svetloShader.setMat4("projection", projection);
                                svetloShader.setMat4("view", view);
                                svetloShader.setMat4("model", glm::mat4(1.0f));

                                model = glm::mat4(1.0);
                                model = glm::scale(glm::mat4 (1.0f), glm::vec3 (0.4,0.4,0.4));
                                pointLight.position = glm::vec3(sin((glm::mediump_float) glfwGetTime()) * 3 ,cos((glm::mediump_float) glfwGetTime()) * 3,0);
                                model = glm::translate(model,glm::vec3(sin((glm::mediump_float) glfwGetTime()) * 3 ,cos((glm::mediump_float) glfwGetTime()) * 3,0));
                                model = glm::translate(model, pointLight.position);
                                model =  glm::rotate(model ,(glm::mediump_float) glfwGetTime() * 3 , glm::vec3(1.0, 1.0, 1.0));

                                //ColorOfCube = glm::vec3(sin((glm::mediump_float) glfwGetTime()),cos((glm::mediump_float) glfwGetTime()*2),1.0);

                                pointLight.diffuse = ColorOfCube ;
                                pointLight.specular = ColorOfCube;
                                pointLight.ambient = ColorOfCube;
                                svetloShader.setVec3("ColorOfCube",ColorOfCube);

                               svetloShader.setMat4("model", model);
                                glDrawArrays(GL_TRIANGLES, 0, 36);
                #pragma endregion SPAWN-SVETLO

            #pragma endregion PRIZIVANJE-KOCKE







            glBindFramebuffer(GL_FRAMEBUFFER,0);

            //BLOOM


            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        bool horizontal = true;
        bool firstIteration = true;
        BlurShader.use();
        unsigned int amount = 10;

        for(unsigned int i = 0 ; i < amount;++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER,pingpongFBO[horizontal]);
            BlurShader.setInt("horizontal",horizontal);
            if(firstIteration)
            {
                glBindTexture(GL_TEXTURE_2D,textureFramebuffer[1]);

            }else{
                glBindTexture(GL_TEXTURE_2D,pingpongColorbuffers[!horizontal]);
            }
            glBindVertexArray(VAOquadVertices);
            glDrawArrays(GL_TRIANGLES,0,6);
            horizontal = !horizontal;
            if(firstIteration){
                firstIteration = false;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER,0);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        BloomFinal.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,textureFramebuffer[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,pingpongColorbuffers[0]);
        glBindVertexArray(VAOquadVertices);
        glDrawArrays(GL_TRIANGLES,0,6);



             #pragma region NEBITAN-KRAJ-LOOP

                    if (programState->ImGuiEnabled)
                        DrawImGui(programState);



                    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
                    // -------------------------------------------------------------------------------
                    glfwSwapBuffers(window);
                    glfwPollEvents();
            #pragma endregion NEBITAN-KRAJ-LOOP

    }
#pragma endregion MAIN-LOOP

    #pragma region END-CLEAN-UP

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
#pragma endregion END-CLEAN-UP

    return 0;
}



#pragma region INPUT-FUNCTIONS

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

void processInputScreenShejder(GLFWwindow  *window,Shader *nasShejder ) {
    static float time = 0;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        if(glfwGetTime()-time > 0.2)
        {

            ulkjuciBLUR = !ulkjuciBLUR;
            nasShejder->setBool("bloom",ulkjuciBLUR);

            time = glfwGetTime();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        if(glfwGetTime()-time > 0.2)
        {

            ulkjuciKernel = !ulkjuciKernel;
            nasShejder->setBool("ukljuciEDGEDETECTION",ulkjuciKernel);

            time = glfwGetTime();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if(glfwGetTime()-time > 0.2)
        {

            hdr = !hdr;
            nasShejder->setBool("hdr",hdr);
            exposure = 1;
            nasShejder->setFloat("exposure",exposure );
            time = glfwGetTime();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if(glfwGetTime()-time > 0.2)
        {

            nasShejder->setFloat("exposure",exposure );
            exposure = exposure + 0.1;
            cout<<exposure<<endl;

            time = glfwGetTime();

        }
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if(glfwGetTime()-time > 0.2)
        {

            nasShejder->setFloat("exposure",exposure );
            exposure = exposure - 0.1;
            cout<<exposure<<endl;

            time = glfwGetTime();

        }
    }

}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
         ImGui::Begin("Prozor za stvari");
        ImGui::Text("Slajderi i sve ostalo");
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
        ImGui::DragFloat3("Pozicija Coveka", (float*)&programState->backpackPosition);
        ImGui::DragFloat("Velichina Coveka", &programState->backpackScale, 0.05, 0.1, 4.0);

        ImGui::DragFloat("Boja crvena",&ColorOfCube.x, 0.05, 0.0, 1.0);
        ImGui::DragFloat("Boja plava", &ColorOfCube.y, 0.05, 0.0, 1.0);
        ImGui::DragFloat("Boja zelena", &ColorOfCube.z, 0.05, 0.0, 1.0);
        ImGui::End();
    }


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            programState->CameraMouseMovementUpdateEnabled = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

#pragma endregion INPUT-FUNCTIONS