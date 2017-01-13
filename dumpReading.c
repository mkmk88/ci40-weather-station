/************************************************************************************************************************
 Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group companies.
 All rights reserved.
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
        following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
        following disclaimer in the documentation and/or other materials provided with the distribution.
     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
        products derived from this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************/


#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <letmecreate/letmecreate.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <awa/common.h>
#include <awa/client.h>
#include <awa/types.h>
#include <awa/server.h>
#include "log.h"

#define OPERATION_PERFORM_TIMEOUT 1000
#define EXTENDED_OPERATION_PERFORM_TIMEOUT 5000
#define CLIENT_ID "MK_NODE1"

typedef float (*SensorReadFunc)(uint8_t);

typedef enum {
    ClickType_None = 0,
    ClickType_Thermo3,
    ClickType_Weather,
    ClickType_Thunder,
    ClickType_AirQuality,
    ClickType_CODetector
} ClickType;

typedef enum {
    IfaceType_microBus = 0,
    IfaceType_AwaLWM2M,
    IfaceType_Unknown = 99
} IfaceType;

ClickType g_Click1Type = ClickType_None;
ClickType g_Click2Type = ClickType_None;
IfaceType g_IfaceType = IfaceType_microBus;
AwaServerSession *g_server_session;

AwaClientSession* g_ClientSession;
int g_LogLevel = LOG_INFO;
FILE* g_DebugStream;
int g_SleepTime = 60;   //default 1 minute

ClickType configDecodeClickType(char* type) {
    static struct element {
        char* name;
        ClickType mapsTo;
    };
    static struct element types[] = {
            {"air", ClickType_AirQuality},
            {"co", ClickType_CODetector},
            {"thermo3", ClickType_Thermo3},
            {"thunder", ClickType_Thunder},
            {"weather", ClickType_Weather},
            {NULL, -1}
    };

    struct element* iter = &types[0];
    while (iter->name != NULL) {
        if (strcasecmp(iter->name, type) == 0) {
            return iter->mapsTo;
        }
        iter++;
    }

    return ClickType_None;
}

static IfaceType configDecodeIfaceType(char *type)
{
    if (!strcmp(type, "microBus")) {
        return IfaceType_microBus;
    }
    if (!strcmp(type, "AwaLWM2M")) {
        return IfaceType_AwaLWM2M;
    }

    LOG(LOG_ERROR, "Unknown interface\n");
    return IfaceType_Unknown;
}

static void printUsage(const char *program)
{
    printf("Usage: %s [options]\n\n"
        " -1, --click1   : Type of click installed in microBus slot 1 (default:none)\n"
        "                  air, co, none, thermo3, thunder, weather\n"
        " -2, --click2   : Type of click installed in microBus slot 2 (default:none)\n"
        "                  air, co, none, thermo3, thunder, weather\n"
        " -s, --sleep    : delay between measurements in seconds. (default: 60s)\n"
        " -v, --logLevel : Debug level from 1 to 5\n"
        "                   fatal(1), error(2), warning(3), info(4), debug(5) and max(>5)\n"
        "                   default is info.\n"
        " -i, --iface    : Interface on which sensor is available (default:microBus)\n"
        "                  microBus, AwaLWM2M\n"
        " -h, --help     : prints this help\n",
        program);
}

bool loadConfiguration(int argc, char **argv) {
    int c;
    bool success = true;

    while (true) {
        static struct option long_options[] = {
        { "click1", required_argument, 0, '1' },
        { "click2", required_argument, 0, '2' },
        { "iface", required_argument, 0, 'i'},
        { "logLevel", required_argument, 0, 'v'},
        { "help", no_argument, 0, 'h'},
        { "sleep", required_argument, 0, 's'},
        { 0, 0, 0, 0 } };

        int option_index = 0;
        c = getopt_long(argc, argv, "s:1:2:c:i:hv:", long_options, &option_index);

        if (c == -1) break;

        switch (c) {
            case '1':
                g_Click1Type = configDecodeClickType(optarg);
                break;

            case '2':
                g_Click2Type = configDecodeClickType(optarg);
                break;

            case 's':
                g_SleepTime = atoi(optarg);
                break;

            case 'v':
                g_LogLevel = atoi(optarg);
                break;

            case 'h':
                printUsage(argv[0]);
                success = false;
                break;

            case 'i':
                g_IfaceType = configDecodeIfaceType(optarg);
                if (g_IfaceType == IfaceType_Unknown) {
                    printUsage(argv[0]);
                    success = false;
                }
                break;

            case '?':
                /* getopt_long already printed an error message. */
                success = false;
                break;

            default:
                abort();
        }
    }

    return success;
}

float readThermo3(uint8_t busIndex) {
    LOG(LOG_DEBUG, "Reading thermo3 on bus#%d", busIndex);
    float temperature = 0.f;
    const AwaServerReadResponse *response;
    const AwaFloat *value = NULL;

    if (g_IfaceType == IfaceType_AwaLWM2M) {
        AwaServerReadOperation * operation = AwaServerReadOperation_New(g_server_session);
        AwaServerReadOperation_AddPath(operation, CLIENT_ID, "/3303/0/5700");
        AwaServerReadOperation_Perform(operation, EXTENDED_OPERATION_PERFORM_TIMEOUT);
        response = AwaServerReadOperation_GetResponse(operation, CLIENT_ID);
        AwaServerReadResponse_GetValueAsFloatPointer(response, "/3303/0/5700", &value);
        temperature = (float)*value;
        AwaServerReadOperation_Free(&operation);
    } else {
        i2c_select_bus(busIndex);

        thermo3_click_enable(0);
        thermo3_click_get_temperature(&temperature);
        thermo3_click_disable();
    }

    return temperature;
}

float readCO(uint8_t busIndex) {
    LOG(LOG_DEBUG, "Reading CO on bus#%d", busIndex);
    uint16_t value = 0;

    co_click_get_measure(busIndex, &value);

    return value;
}

float readAirQuality(uint8_t busIndex) {
    LOG(LOG_DEBUG, "Reading air quality on bus#%d", busIndex);
    uint16_t value = 0;

    air_quality_click_get_measure(busIndex, &value);

    return value;
}

uint8_t readWeather(uint8_t busIndex, double* data) {
	LOG(LOG_DEBUG, "Reading weather on bus#%d", busIndex);

	i2c_select_bus(busIndex);
	if (weather_click_read_measurements(&data[0], &data[1], &data[2]) < 0) {
		LOG(LOG_ERROR, "Reading weather measurements failed!");
		return -1;
	}

	return 0;
}

bool connectToAwa() {
    g_ClientSession = AwaClientSession_New();

    if (g_ClientSession != NULL) {
        if (AwaClientSession_SetIPCAsUDP(g_ClientSession, "127.0.0.1", 12345) == AwaError_Success) {
            if (AwaClientSession_Connect(g_ClientSession) == AwaError_Success) {
                LOG(LOG_INFO, "Client Session Established: 127.0.0.1:12345\n");
            } else {
                LOG(LOG_ERROR, "AwaClientSession_Connect() failed\n");
                AwaClientSession_Free(&g_ClientSession);
                g_ClientSession = NULL;
            }
        } else {
            LOG(LOG_ERROR, "AwaClientSession_SetIPCAsUDP() failed\n");
            AwaClientSession_Free(&g_ClientSession);
            g_ClientSession = NULL;
        }
    } else {
        LOG(LOG_ERROR, "AwaClientSession_New() failed\n");
    }
    return g_ClientSession != NULL;
}

void disconnectAwa() {
    if (g_ClientSession == NULL) {
        return;
    }
    AwaClientSession_Disconnect(g_ClientSession);
    AwaClientSession_Free(&g_ClientSession);
}

void createIPSO(int objectId, int instance, int resourceId) {
    AwaClientSetOperation * operation = AwaClientSetOperation_New(g_ClientSession);

    char buf[40];
    if (resourceId == -1) {
        sprintf(&buf[0], "/%d/%d", objectId, instance);
        AwaClientSetOperation_CreateObjectInstance(operation, &buf[0]);

    } else {
        sprintf(&buf[0], "/%d/%d/%d", objectId, instance, resourceId);
        LOG(LOG_INFO, "Creating instance of resource %s", &buf[0]);
        AwaClientSetOperation_CreateOptionalResource(operation, &buf[0]);
    }

    AwaError result = AwaClientSetOperation_Perform(operation, OPERATION_PERFORM_TIMEOUT);
    LOG(LOG_DEBUG, "Awa create response: %d", result);
    AwaClientSetOperation_Free(&operation);
}

void setIPSO(int objectId, int instance, int resourceId, float value, bool shouldRetry) {
    char buf[40];
    sprintf(&buf[0], "/%d/%d/%d", objectId, instance, resourceId);
    LOG(LOG_INFO, "Storing value %0.3f into %s", value, &buf[0]);
    AwaClientSetOperation* operation = AwaClientSetOperation_New(g_ClientSession);
    AwaClientSetOperation_AddValueAsFloat(operation, &buf[0], value);
    AwaError result = AwaClientSetOperation_Perform(operation, OPERATION_PERFORM_TIMEOUT);
    LOG(LOG_DEBUG, "Awa set response: %d", result);
    if (result == AwaError_Response || result == AwaError_PathInvalid || result == AwaError_PathNotFound) {
        LOG(LOG_DEBUG, "Looks like instance of %s not exists, try to create one", &buf[0]);
        if (shouldRetry == true) {
            createIPSO(objectId, instance, -1);
            createIPSO(objectId, instance, resourceId);
            setIPSO(objectId, instance, resourceId, value, false);
        }
    }
}

float getIPSO(int objectId, int instance, int resourceId, float defaultValue) {
    char buf[40];
    sprintf(&buf[0], "/%d/%d/%d", objectId, instance, resourceId);
    LOG(LOG_DEBUG, "Getting value of %s", &buf[0]);
    AwaClientGetOperation * operation = AwaClientGetOperation_New(g_ClientSession);

    AwaClientGetOperation_AddPath(operation, &buf[0]);
    AwaError result = AwaClientGetOperation_Perform(operation, OPERATION_PERFORM_TIMEOUT);
    if (result != AwaError_Success) {
        AwaClientGetOperation_Free(&operation);
        return defaultValue;
    }

    const AwaFloat* value;
    float resultValue;

    const AwaClientGetResponse* response = AwaClientGetOperation_GetResponse(operation);
    AwaClientGetResponse_GetValueAsFloatPointer(response, (const char*)&buf[0], &value);
    resultValue = (float)*value;
    LOG(LOG_DEBUG, "Got value %f\n", (float)resultValue);

    AwaClientGetOperation_Free(&operation);
    return resultValue;
}

uint8_t setMeasurement(int objId, int instance, double value) {

	float minValue = getIPSO(objId, instance, 5601, 1000);
	float maxValue = getIPSO(objId, instance, 5602, -1000);

	setIPSO(objId, instance, 5700, value, true);
	if (minValue > value) {
		setIPSO(objId, instance, 5601, value, true);
	}
	if (maxValue < value) {
		setIPSO(objId, instance, 5602, value, true);
	}
	return 0;
}

void handleMeasurements(uint8_t bus, int objId, int instance, SensorReadFunc sensorFunc) {
    float value = sensorFunc(bus);
    setMeasurement(objId, instance, value);
}


void handleWeatherMeasurements(uint8_t busIndex,
		int temperatureInstance, int pressureInstance, int humidityInstance) {

	double data[] = {0,0,0};
    if (readWeather(busIndex, data) < 0) {
    	LOG(LOG_ERROR, "Reading weather on bus#%d failed!", busIndex);
    	return;
    }
    LOG(LOG_INFO, "Reading weather measurements: temp = %f, pressure = %f, humidity = %f",
    			data[0], data[1], data[2]);
    setMeasurement(3303, temperatureInstance, data[0]);
    setMeasurement(3315, pressureInstance, data[1]);
    setMeasurement(3304, humidityInstance, data[2]);
}

void performMeasurements() {
    if (connectToAwa() == false) {
        return;
    }

    int index;
    int instanceIndex[] = {0,		//3303 - temperature
						   1, 		//3304 - humidity
						   2,		//3315 - barometer
						   3,		//3325 - concentration
						   4,		//3330 - distance
						   5};		//3328 - power

    //contains last used instance ids for all registered sensors
    int instances[] = {0,	//3303
					   0,	//3304
					   0,	//3315
					   0,	//3325
					   0,	//3330
					   0};	//3328

    for (index = 0; index < 2; index++) {
        uint8_t bus = index == 0 ? MIKROBUS_1 : MIKROBUS_2;

        switch (index == 0 ? g_Click1Type : g_Click2Type) {
            case ClickType_Thermo3:
            	handleMeasurements(bus, 3303, instances[instanceIndex[0]]++, &readThermo3);

                break;

            case ClickType_Weather:
            	handleWeatherMeasurements(bus,
            			instances[instanceIndex[0]]++,
						instances[instanceIndex[1]]++,
						instances[instanceIndex[2]]++);

            	break;
            case ClickType_Thunder:
            	break;
            case ClickType_AirQuality:
            	handleMeasurements(bus, 3325, instances[instanceIndex[3]]++, &readAirQuality);
            	break;
            case ClickType_CODetector:
            	handleMeasurements(bus, 3325, instances[instanceIndex[3]]++, &readCO);
                break;
            default:
                break;
        }
    }

    disconnectAwa();
}

static void disconnectExtendedAwa()
{
    AwaServerSession_Disconnect(g_server_session);
    AwaServerSession_Free(&g_server_session);
}

void cleanupOnExit() {
    i2c_release();
    disconnectAwa();
    disconnectExtendedAwa();
}

void initialize() {
	int index;
	for (index = 0; index < 2; index++) {
		uint8_t bus = index == 0 ? MIKROBUS_1 : MIKROBUS_2;

		switch (index == 0 ? g_Click1Type : g_Click2Type) {
		case ClickType_Thermo3:
			break;
		case ClickType_Weather:
			i2c_select_bus(index);
			if (weather_click_enable() < 0) {
				LOG(LOG_ERROR, "Failed to enable weather click on bus#%d\n", index);
			}
			break;

			//TODO: add rest if needed
		default:
			break;
		}
	}
}

bool initialize_extended_awa()
{
    g_server_session = AwaServerSession_New();
    if (g_server_session) {
        if (AwaServerSession_Connect(g_server_session) != AwaError_Success) {
            return false;
        }
        return true;
    }

    return false;
}

int main(int argc, char **argv) {
    if (loadConfiguration(argc, argv) == false) {
        return -1;
    }

    signal(SIGINT, &cleanupOnExit);
    atexit(&cleanupOnExit);

    switch (g_IfaceType) {
        case IfaceType_microBus:            
            i2c_init();
            initialize();
            break;
        case IfaceType_AwaLWM2M:
            if (!initialize_extended_awa()) {
                    return 1;
            }
            break;
        default:
            return 1;
    }

    while(true) {
        performMeasurements();
        sleep(g_SleepTime);
    }

    return 0;
}
