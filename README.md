
![](img.png)

---

## The Lumpy Ci40 application  
The Lumpy Ci40 is part of bigger project called "Weather Station". Using code from this repository you will be able to handle various sensor clicks inserted into your Ci40 board. Values measured by those clicks will be sent to Creator Device Server. 

## Environment for Weather Station project  
The complete IoT Environment is builded with following components:
* LWM2M cloud server  
* Ci40 application which allows usage of clicks in mirkroBUS sockets.
* Contiki based applications build for clicker platform:
  *  [Temperature sensor](https://github.com/CreatorKit/temperature-sensor)
* a mobile application to present Weather measurements.  

## Ci40 application specific dependencies
This application uses the [Awa LightweightM2M](https://github.com/FlowM2M/AwaLWM2M) implementation of the OMA Lightweight M2M protocol to provide a secure and standards compliant device management solution without the need for an intimate knowledge of M2M protocols. Additionnaly MikroE Clicks support is done through [LetMeCreate library](https://github.com/CreatorDev/LetMeCreate).

## Integration with openWrt
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

Now copy folder from `packages/weather-station-gateway/feeds` into `work/dist/openwrt/openwrt-ckt-feeds`.
Then execute commands:

    cd work/dist/openwrt
    ./scripts/feeds update
    ./script/feeds update weather-station-gateway
    ./script/feeds install weather-station-gateway
    make menuconfig

in menuconfig please press `/` and type `weather-station-gateway` one occurrence will appear. Mark it with `<*>` and do save of config.
In terminal type `make` to build openwrt image with this application. After uploading to ci40 edit file located in `/etc/init.d/weather_station_initd` and put proper switch arguments related to clicks which you put into mikroBus port.

## Setup for execution - development
While Lumpy uses Awa Client Deamon to communicate with Cloud LWM2M server it requires few things to be done before project will run. First of all you need to go to CreatorKit console and create certificate which will be used to make secure connection. If you havent done this earlier, you will find usefull informations on [ Creator Device Server](https://docs.creatordev.io/deviceserver/) page. Then you have to provide IPSO object definitions, to do so please run `clientObjectsDefine.sh` shript from `scripts` folder (make sure Awa client deamon is working!). And here you go!
Wait! No! Put some clicks into Ci40 mikroBUS, and then you can execute Lumpy with one of following options:

| Switch        | Description |
|---------------|----------|
|-1, --click1   | Type of click installed in microBUS slot 1 (default:none)|
|-2, --click2   | Type of click installed in microBUS slot 2 (default:none)|
|-s, --sleep    | Delay between measurements in seconds. (default: 60s)|
|-v, --logLevel | Debug level from 1 to 5 (default:info): fatal(1), error(2), warning(3), info(4), debug(5) and max(>5)|
|-h, --help     | prints help|

Please refer to section 'Supprted clicks' to obtain argument values for switch --click1 and --click2. If one of slots is empty you can skip proper switch or set it's value to `none`.

## Supported clicks
From wide range of [MikroE clicks](http://www.mikroe.com/index.php?url=store/click/) in this project you can use:

| Click                | Argument |
|--------------------- |----------|
| [Air Quality](http://www.mikroe.com/click/air-quality/) | air |
| [Carbon monoxide](http://www.mikroe.com/click/co/) | co  |
| [Thermo3](http://www.mikroe.com/click/thermo3/) | thermo3 |
| [Thunder](http://www.mikroe.com/click/thunder/) | thunder |
| [Weather](http://www.mikroe.com/click/weather/) | weather |

----
