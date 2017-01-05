
![logo](https://static.creatordev.io/logo-md-s.svg)

# The Lumpy Ci40 Application  

The Lumpy Ci40 is part of bigger project called "Weather Station". Using code from this repository you will be able to handle various sensor clicks inserted into your Ci40 board. Values measured by those clicks will be sent to Creator Device Server.

This fork reads data from another device that is connected through AwaLWM2M instead of reading directly from click board connected to Creator. That way you can put your sensor for example on 6lowpan clicker board.

---

## Environment for Weather Station project  
The complete IoT Environment is builded with following components:
* LWM2M cloud server  
* Ci40 application which allows usage of clicks in mirkroBUS sockets.
* Contiki based applications build for clicker platform:
  *  [Temperature sensor](https://github.com/CreatorKit/temperature-sensor)
* a mobile application to present Weather measurements.  

## Ci40 Application Specific Dependencies
This application uses the [Awa LightweightM2M](https://github.com/FlowM2M/AwaLWM2M) implementation of the OMA Lightweight M2M protocol to provide a secure and standards compliant device management solution without the need for an intimate knowledge of M2M protocols. Additionnaly MikroE Clicks support is done through [LetMeCreate library](https://github.com/CreatorDev/LetMeCreate).

## Integration With OpenWrt
It's assumed that you have build envriroment for ci40 openWrt described [here](https://github.com/CreatorKit/build) and this is located in folder `work`. So structure inside will be:

    work/
      build/  
      constrained-os/  
      dist/
      packages/

Clone this repository into `work/packages`, after this operation structure will look:

    work/
      build/  
      constrained-os/  
      dist/
      packages/
        weather-station-gateway

Now copy folder from `packages/weather-station-gateway/feeds` into `work/dist/openwrt-ckt-feeds`.
Then execute commands:

    cd work/dist/openwrt
    ./scripts/feeds update
    ./scripts/feeds update weather-station-gateway
    ./scripts/feeds install weather-station-gateway
    make menuconfig

in menuconfig please press `/` and type `weather-station-gateway` one occurrence will appear. Mark it with `<*>` and do save of config.
In terminal type `make` to build openwrt image with this application. After uploading to ci40 edit file located in `/etc/init.d/weather_station_initd` and put proper switch arguments related to clicks which you put into mikroBus port.

## Setup For Execution - Development

Lumpy uses an AWA LwM2M Client to communicate the with the Creator Device server It requires few steps to be done before running the project.   
First of all you need to go to **Creator Developer Console** and create certificate which will be used to make secure connection. 
If you havent done this earlier, you will find usefull informations on [ Creator Device Server](https://docs.creatordev.io/deviceserver/guides/iot-framework/) page. The execute the daemon run this command:

```bash
 $ awa_clientd --bootstrap coaps://deviceserver.flowcloud.systems:15684 --endPointName WeatherStationDevice --certificate=/root/certificate.crt --ipcPort 12345 -p7000 -d
```

Then you have to provide IPSO object definitions, to do so please run `clientObjectsDefine.sh` script from `scripts` folder (make sure the AWA LwM2M Client Daemon is working!). 

```bash
 $ ./clientObjectsDefine.sh
```
And here you go!
Wait! No! Put some clicks into Ci40 MikroBUS, and then you can execute Lumpy with one of following options:

| Switch        | Description |
|---------------|----------|
|-1, --click1   | Type of click installed in microBUS slot 1 (default:none)|
|-2, --click2   | Type of click installed in microBUS slot 2 (default:none)|
|-s, --sleep    | Delay between measurements in seconds. (default: 60s)|
|-v, --logLevel | Debug level from 1 to 5 (default:info): fatal(1), error(2), warning(3), info(4), debug(5) and max(>5)|
|-h, --help     | prints help|

Please refer to section 'Supported Clicks' to obtain argument values for switch --click1 and --click2. If one of slots is empty you can skip proper switch or set it's value to `none`.  

For example, connect a Thermo 3 Click (temperature sensor), to **MikroBus 1**. Then execute the following command:

```bash
$ ./weatherStation -1 thermo3
```
Finally, you can check the updated temperature values on the **Creator Developer Console**. 

## Supported Clicks

From wide range of [MikroE clicks](http://www.mikroe.com/index.php?url=store/click/) in this project you can use:

| Click                                                   | weatherStation argument name |
|-------------------------------------------------------- | ---------------------------- | 
| [Air Quality](http://www.mikroe.com/click/air-quality/) | air                          |
| [Carbon Monoxide](http://www.mikroe.com/click/co/)      | co                           |
| [Thermo3](http://www.mikroe.com/click/thermo3/)         | thermo3                      |
| [Thunder](http://www.mikroe.com/click/thunder/)         | thunder                      |
| [Weather](http://www.mikroe.com/click/weather/)         | weather                      |

----
