#definition of Temperature sensor
awa-client-define -o 3303 -j "Temperature" -y multiple \
    -r 5700 -n "Sensor Value" -t float -u single -q mandatory -k r \
    -r 5701 -n "Units" -t string -u single -q optional -k r \
    -r 5601 -n "Min Measured Value" -t float -u single -q optional -k r \
    -r 5602 -n "Max Measured Value" -t float -u single -q optional -k r 

#define of Humiditi sensor
awa-client-define -o 3304 -j "Humodity" -y multiple \
    -r 5700 -n "Sensor Value" -t float -u single -q mandatory -k r \
    -r 5701 -n "Units" -t string -u single -q optional -k r \
    -r 5601 -n "Min Measured Value" -t float -u single -q optional -k r \
    -r 5602 -n "Max Measured Value" -t float -u single -q optional -k r 

#define of Barometer sensor                                                           
awa-client-define -o 3315 -j "Barometer" -y multiple \
    -r 5700 -n "Sensor Value" -t float -u single -q mandatory -k r \
    -r 5701 -n "Units" -t string -u single -q optional -k r \
    -r 5601 -n "Min Measured Value" -t float -u single -q optional -k r \
    -r 5602 -n "Max Measured Value" -t float -u single -q optional -k r

#define of Concentration sensor                                                          
awa-client-define -o 3325 -j "Concentration" -y multiple \
    -r 5700 -n "Sensor Value" -t float -u single -q mandatory -k r \
    -r 5701 -n "Units" -t string -u single -q optional -k r \
    -r 5601 -n "Min Measured Value" -t float -u single -q optional -k r \
    -r 5602 -n "Max Measured Value" -t float -u single -q optional -k r \
    -r 5750 -n "Application Type" -t string -u single -q optional -k r 

#define of Distance sensor
awa-client-define -o 3330 -j "Distance" -y multiple \
    -r 5700 -n "Sensor Value" -t float -u single -q mandatory -k r \
    -r 5701 -n "Units" -t string -u single -q optional -k r \
    -r 5601 -n "Min Measured Value" -t float -u single -q optional -k r \
    -r 5602 -n "Max Measured Value" -t float -u single -q optional -k r \
    -r 5750 -n "Application Type" -t string -u single -q optional -k r

#define of Power sensor     
awa-client-define -o 3328 -j "Power" -y multiple \
    -r 5700 -n "Sensor Value" -t float -u single -q mandatory -k r \
    -r 5701 -n "Units" -t string -u single -q optional -k r \
    -r 5601 -n "Min Measured Value" -t float -u single -q optional -k r \
    -r 5602 -n "Max Measured Value" -t float -u single -q optional -k r \
    -r 5750 -n "Application Type" -t string -u single -q optional -k r

#define Device information object
awa-client-define -o 3 -j "Device" -y single -m \
  -r 2 -n "Serial Number" -t string -u single -q mandatory -k r
        
MAC="$(cat /sys/class/net/eth0/address)"                                 
awa-client-set --create /3/0
awa-client-set /3/0/2=${MAC} 
