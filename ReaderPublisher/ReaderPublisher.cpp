// Using SDL and standard IO
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <mosquitto.h>
#include <fstream>

#define JS_RECONNECT
 
#define DEFAULT_ADDRESS "127.0.0.1" //MQTT broker address
#define DEFAULT_PORT 1883           //MQTT broker port
#define TOPIC "/moses/cmd_vel"      //MQTT topic
#define QOS 0                       //MQTT QoS
#define TIMEOUT 10000               //MQTT timeout in milliseconds
#define JOY_READ_INTERVAL 10        //Joystick read interval in milliseconds
#define JOY_RETRY_INTERVAL 1000     //Joystick retry interval in milliseconds
#define V_MAX 1                     //Maximum linear velocity value
#define VN_MAX 1                    //Maximum normal linear velocity value
#define W_MAX 1                     //Maximum angular velocity value

struct mosquitto* mosq = NULL;

// Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;
SDL_Joystick* js = NULL;
SDL_TimerID readID = NULL, retryID = NULL;
SDL_Event event;
bool change_timer = false, prev_connection_state = true, connection_state = true, retry_connection = false;
int num_retries = 0;

int js_num_axes = 0;
int js_num_buttons = 0;
int js_num_hats = 0;

int v_axis, vn_axis, w_axis;

bool sdl_init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }

    SDL_JoystickEventState(SDL_ENABLE);

    if (SDL_NumJoysticks() < 1)
    {
        printf("Warning: No joysticks connected!\n");
        success = false;
    }
    else
    {
        // Load joystick
        js = SDL_JoystickOpen(0);
        if (js == NULL)
        {
            printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            printf("Joystick connected\n");
            printf("Name: %s\n", SDL_JoystickNameForIndex(0));
            js_num_axes = SDL_JoystickNumAxes(js);
            printf("Num axes: %d\n", js_num_axes);
            js_num_buttons = SDL_JoystickNumButtons(js);
            printf("Num buttons: %d\n", js_num_buttons);
            js_num_hats = SDL_JoystickNumHats(js);
            printf("Num hats: %d\n", js_num_hats);
        }
    }

    return success;
}

void sdl_close()
{
    // Close game controller
    SDL_JoystickClose(js);
    js = NULL;
    SDL_Quit();
}

/* bool read_ip_config(std::string& address, int& port)
{
    std::ifstream config_file("ipconf");
    if (!config_file.is_open())
    {
        fprintf(stderr, "Error: Unable to open ipconf file.\n");
        return false;
    }

    std::string line;
    if (std::getline(config_file, line))
    {
        std::istringstream iss(line);
        if (!(iss >> address >> port))
        {
            fprintf(stderr, "Error: Invalid format in ipconf file.\n");
            return false;
        }
    }
    else
    {
        fprintf(stderr, "Error: ipconf file is empty.\n");
        return false;
    }

    config_file.close();
    return true;
} */

bool mosquitto_init(const std::string& address, int port)
{
    int rc;
    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq)
    {
        fprintf(stderr, "Error: Out of memory when creating Mosquitto instance.\n");
        return false;
    }

    rc = mosquitto_connect(mosq, address.c_str(), port, TIMEOUT);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Unable to connect to the broker. Return code: %d\n", rc);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return false;
    }
    return true;
}

void mosquitto_close()
{
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

Uint32 RetryCallback(Uint32 interval, void* param)
{
    retry_connection = true;
    return interval;
}

Uint32 ReadCallback(Uint32 interval, void* param)
{
    std::stringstream json;

    bool d_switch = SDL_JoystickGetButton(js, 4);
    const char* led = SDL_JoystickGetButton(js, 0) ? "true" : "false";

    json << "{" << std::endl;
    json << "\"v\":" << d_switch * -1 * SDL_JoystickGetAxis(js, 1) * V_MAX / 32765 << "," << std::endl;
    json << "\"vn\":" << d_switch * SDL_JoystickGetAxis(js, 0) * VN_MAX / 32765 << "," << std::endl;
    json << "\"w\":" << d_switch * -1 * SDL_JoystickGetAxis(js, 2) * W_MAX / 32765 << std::endl;
    json << "}" << std::endl;

    std::string aux;
    const char* message;
    aux = json.str();
    message = aux.c_str();
    std::cout << message << std::endl;
    int rc = mosquitto_publish(mosq, NULL, TOPIC, strlen(message), message, QOS, false);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Failed to publish message. Return code: %d\n", rc);
    }
    else
    {
        printf("Message published successfully.\n");
    }

    return interval;
}

int main(int argc, char* argv[])
{
    // Game Controller 1 handler
    int rc;
    std::cout << "Initializing SDL\n";
    if (!sdl_init())
    {
        return 0;
    }

    if (SDL_Init(SDL_INIT_TIMER) < 0)
    {
        printf("SDL timer could not initialize! SDL Error: %s\n", SDL_GetError());
        return -1;
    }

    std::cout << "SDL successfully initialized\n";

    std::string address = DEFAULT_ADDRESS;
    int port = DEFAULT_PORT;

    /* if (!read_ip_config(address, port))
    {
        return EXIT_FAILURE;
    } */

    std::cout << "Initializing mosquitto\n";
    if (!mosquitto_init(address, port))
    {
        return EXIT_FAILURE;
    }
    std::cout << "Mosquitto successfully initialized\n";

    std::string aux;
    const char* message;

    readID = SDL_AddTimer(JOY_READ_INTERVAL, ReadCallback, js);
    while (1)
    {
        connection_state = SDL_JoystickGetAttached(js);
        change_timer = (prev_connection_state != connection_state);
        prev_connection_state = connection_state;

        if (!connection_state) // Disconnected
        {
            if (change_timer)
            {
                std::cout << "Change Timer!\n";
                SDL_RemoveTimer(readID);
                sdl_close();
                retryID = SDL_AddTimer(JOY_RETRY_INTERVAL, RetryCallback, js);
            }
            if (retry_connection)
            {
                std::cout << "Retry connection!\n";
                while (SDL_PollEvent(&event))
                {
                }
                sdl_init();
                retry_connection = false;
            }
        }
        else // Connected
        {
            if (change_timer)
            {
                std::cout << "Joystick reconnected!\n";
                SDL_RemoveTimer(retryID);
                readID = SDL_AddTimer(JOY_READ_INTERVAL, ReadCallback, js);
            }
            while (SDL_PollEvent(&event))
            {
            }
        }
        SDL_Delay(10);
    }

    mosquitto_close();
    return EXIT_SUCCESS;
}
