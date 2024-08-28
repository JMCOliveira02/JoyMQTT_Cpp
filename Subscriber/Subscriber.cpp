#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <mosquitto.h>
#include <json.hpp>

#define ADDRESS "127.0.0.1"//"10.122.241.72" // Replace with your MQTT broker address
#define PORT 1883     // Replace with your MQTT broker port
#define TOPIC "/moses/cmd_vel"         // Replace with the topic you want to subscribe to

void on_message_callback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message)
{
    /* printf("Received message on topic %s: %s\n", message->topic, (char *)message->payload);
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count(); */
    try
    {
        nlohmann::json json = nlohmann::json::parse((char*)message->payload);
        /* long long timestamp_message = json["timestamp"];
        printf("Latency: %lld\n", timestamp - timestamp_message);
        moving_avg(timestamp - timestamp_message);
        std::cout << "Average latency (last 10 messages): " << avg_latency << std::endl; */
        std::cout << "v: " << json["v"] << std::endl;
        std::cout << "vn: " << json["vn"] << std::endl;
        std::cout << "w: " << json["w"] << std::endl;
        std::cout << std::endl;
        return;
    }
    catch (const std::exception& e)
    {
        printf("Couldn't parse json\n");
        return;
    }
}

int main()
{
    struct mosquitto* mosq = NULL;

    mosquitto_lib_init(); // Initialize the Mosquitto library

    mosq = mosquitto_new("mqtt_c_subscriber", true, NULL);
    if (!mosq)
    {
        fprintf(stderr, "Error: Unable to create Mosquitto instance.\n");
        return 1;
    }

    mosquitto_message_callback_set(mosq, on_message_callback);

    printf("connecting...\n");

    if (mosquitto_connect(mosq, ADDRESS, PORT, 60) != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Error: Unable to connect to the broker.\n");
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    if (mosquitto_subscribe(mosq, NULL, TOPIC, 0) != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Error: Unable to subscribe to the topic.\n");
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    fprintf(stderr, "CONNECTION SUCCESSFULL!.\n");
    while (1)
    {
        if (mosquitto_loop(mosq, -1, 1) != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "Error: Mosquitto loop failed.\n");
            break;
        }
    }

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
