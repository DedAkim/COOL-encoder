#include <iostream>
#include <fstream>
#include <string.h>
#include <random>
#include <ctime>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void xor_crypt(char* data, const char* key, unsigned int l_data, unsigned int l_key)
{
    if (l_key == 0) return;
    
    for (size_t i = 0; i < l_data; ++i)
    {
        data[i] ^= key[i % l_key];
    }
}
void generate_random_key(char* key_buffer, size_t key_length)
{
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(33, 126);
    
    for (size_t i = 0; i < key_length - 1; ++i)
    {
        key_buffer[i] = static_cast<char>(dist(rng));
    }
    key_buffer[key_length - 1] = '\0';
}

int main()
{
    char filename[256] = "encode.bin";
    char filename2[256] = "encode.bin";
    char output_filename[256] = "output.txt";
    
    char text1[256] = "";
    char _text1[256] = "";
    char text2[256] = "";
    char _text2[256] = "";
    char key1[32] = "";
    char key2[32] = "";
    char status_msg[256] = "";
    bool key_generated = false;
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow((int)(600 * main_scale), (int)(400 * main_scale), "encoder/decoder", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    static const ImWchar ranges[] = 
    {
        0x0020, 0x00FF,
        0x0400, 0x052F,
        0,
    };
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 18.0f, nullptr, ranges);

    ImGui::StyleColorsClassic();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) 
    {
        std::cout << "Failed to initialize ImGui GLFW backend\n";
    }
    
    if (!ImGui_ImplOpenGL3_Init("#version 150")) 
    {
        std::cout << "Failed to initialize ImGui OpenGL backend\n";
    }

    bool main_window = true;
    bool window_1 = false;
    bool window_2 = false;

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto inputWidth = 250.0f;
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui::SetNextWindowSize(ImVec2(display_w, display_h));

        ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus;
        
        if(main_window)
        {
            ImGui::Begin("Main Panel", nullptr, window_flags);

            ImGui::TextColored(ImVec4(0, 1, 1, 1), "XOR Шифрование/Дешифрование");
            ImGui::Separator();
            ImGui::Spacing();

            if(ImGui::Button("Зашифровать"))
            {
                window_1 = true;
                main_window = false;
                generate_random_key(key1, sizeof(key1));
                key_generated = true;
            }

            if(ImGui::Button("Расшифровать"))
            {
                window_2 = true;
                main_window = false;
                key2[0] = '\0';
            }

            ImGui::End();
        }
        if(window_1)
        {
            ImGui::Begin("window1", nullptr, window_flags);

            ImGui::Text("Текст для шифровки: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(inputWidth);
            ImGui::InputText("##text", text1, IM_ARRAYSIZE(text1));

            ImGui::Text("Имя bin файла: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(inputWidth);
            ImGui::InputText("##filename", filename, IM_ARRAYSIZE(filename));

            ImGui::Text("Замена ключа: ");
            if (ImGui::Button("Сгенерировать новый ключ"))
            {
                generate_random_key(key1, sizeof(key1));
                snprintf(status_msg, sizeof(status_msg), "Сгенерирован новый ключ");
            }

            ImGui::Separator();

            ImGui::Text("Зашифровать текст: ");
            ImGui::SameLine();
            if(ImGui::Button("Зашифровать текст в файл"))
            {
                if (strlen(text1) == 0)
                {
                    snprintf(status_msg, sizeof(status_msg), "Ошибка: Введите текст для шифрования");
                }
                else if (strlen(filename) == 0)
                {
                    snprintf(status_msg, sizeof(status_msg), "Ошибка: Укажите имя файла");
                }
                else if (!strstr(filename, ".bin"))
                {
                    snprintf(status_msg, sizeof(status_msg), "Ошибка: Файл должен иметь расширение .bin");
                }
                else if (strlen(key1) == 0)
                {
                    snprintf(status_msg, sizeof(status_msg), "Ошибка: Ключ не сгенерирован");
                }
                else
                {
                    size_t text_len = strlen(text1);
                    
                    strncpy(_text1, text1, sizeof(_text1) - 1);
                    _text1[sizeof(_text1) - 1] = '\0';
                    
                    xor_crypt(_text1, key1, text_len, strlen(key1));
                    
                    if (strlen(filename) >= 256)
                    {
                        snprintf(status_msg, sizeof(status_msg), "Ошибка: Имя файла слишком длинное");
                    }
                    else
                    {
                        FILE * fp1 = fopen(filename, "wb");
                        if (fp1)
                        {
                            size_t written = fwrite(_text1, 1, text_len, fp1);
                            fclose(fp1);
                            
                            if (written == text_len)
                            {
                                std::cerr << "Encryption completed. Original length: " << text_len << std::endl;
                                std::cerr << "Encrypted data (hex): ";
                                for (size_t i = 0; i < text_len; ++i)
                                {
                                    printf("%02X ", (unsigned char)_text1[i]);
                                }
                                std::cerr << std::endl;
                                
                                snprintf(status_msg, sizeof(status_msg), 
                                        "Текст зашифрован и сохранен в %s (размер: %zu байт)", 
                                        filename, text_len);
                            }
                            else
                            {
                                snprintf(status_msg, sizeof(status_msg), 
                                        "Ошибка записи: записано %zu из %zu байт", 
                                        written, text_len);
                            }
                        }
                        else
                        {
                            snprintf(status_msg, sizeof(status_msg), 
                                    "Ошибка открытия файла '%s' для записи", 
                                    filename);
                        }
                    }
                }
            }

            ImGui::Text("Ключ для расшифровки данных: ");
            ImGui::SameLine();
            if (ImGui::Button("Скопировать ключ"))
            {
                if (strlen(key1) > 0)
                {
                    ImGui::SetClipboardText(key1);
                    snprintf(status_msg, sizeof(status_msg), "Ключ скопирован в буфер обмена");
                }
                else
                {
                    snprintf(status_msg, sizeof(status_msg), "Ключ не сгенерирован");
                }
            }

            if (strlen(status_msg) > 0)
            {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Статус: %s", status_msg);
            }

            ImGui::Separator();
            if(ImGui::Button("Выйти в меню"))
            {
                window_1 = false;
                main_window = true;
                status_msg[0] = '\0';
                text1[0] = '\0';
            }
            ImGui::End();
        }
        if(window_2)
        {
            ImGui::Begin("window2", nullptr, window_flags);
            
            ImGui::Text("Имя входного файла: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(inputWidth);
            ImGui::InputText("##filename", filename2, IM_ARRAYSIZE(filename2));
            
            ImGui::Text("Имя выходного файла: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(inputWidth);
            ImGui::InputText("##output", output_filename, IM_ARRAYSIZE(output_filename));

            ImGui::Text("Ключ: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(inputWidth);
            ImGui::InputText("##key2", key2, IM_ARRAYSIZE(key2));

            ImGui::Text("Расшифровать текст: ");
            ImGui::SameLine();
            if(ImGui::Button("Расшифровать файл"))
            {
                if (strlen(filename2) == 0)
                {
                    snprintf(status_msg, sizeof(status_msg), "Укажите имя файла");
                }
                else if (strlen(key2) == 0)
                {
                    snprintf(status_msg, sizeof(status_msg), "Введите ключ");
                }
                else
                {
                    FILE * fp_in = fopen(filename2, "rb");
                    if (fp_in)
                    {
                        fseek(fp_in, 0, SEEK_END);
                        long file_size = ftell(fp_in);
                        fseek(fp_in, 0, SEEK_SET);
                        
                        if (file_size > 0 && file_size < sizeof(text2))
                        {
                            size_t bytes_read = fread(text2, 1, file_size, fp_in);
                            fclose(fp_in);
                            
                            if (bytes_read == (size_t)file_size)
                            {
                                memcpy(_text2, text2, bytes_read);
                                xor_crypt(_text2, key2, bytes_read, strlen(key2));
                                _text2[bytes_read] = '\0';
                                FILE * fp_out = fopen(output_filename, "wb");
                                if (fp_out)
                                {
                                    fwrite(_text2, 1, bytes_read, fp_out);
                                    fclose(fp_out);
                                    std::cerr << "Decryption completed. File size: " << file_size << std::endl;
                                    std::cerr << "Decrypted text: " << _text2 << std::endl;
                                    
                                    snprintf(status_msg, sizeof(status_msg), 
                                             "Файл расшифрован. Результат сохранен в %s", output_filename);
                                }
                                else
                                {
                                    snprintf(status_msg, sizeof(status_msg), 
                                             "Ошибка создания выходного файла");
                                }
                            }
                            else
                            {
                                snprintf(status_msg, sizeof(status_msg), 
                                         "Ошибка чтения файла");
                            }
                        }
                        else
                        {
                            fclose(fp_in);
                            snprintf(status_msg, sizeof(status_msg), 
                                     "Некорректный размер файла (должен быть от 1 до %zu байт)", 
                                     sizeof(text2) - 1);
                        }
                    }
                    else
                    {
                        snprintf(status_msg, sizeof(status_msg), 
                                 "Не удалось открыть файл: %s", filename2);
                    }
                }
            }
            if (strlen(_text2) > 0)
            {
                ImGui::Separator();
                ImGui::Text("Расшифрованный текст:");
                ImGui::TextWrapped("%s", _text2);
            }
            if (strlen(status_msg) > 0)
            {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "Статус: %s", status_msg);
            }

            ImGui::Separator();
            if(ImGui::Button("Выйти в меню"))
            {
                window_2 = false;
                main_window = true;
                status_msg[0] = '\0';
                _text2[0] = '\0';
            }
            ImGui::End();
        }

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}