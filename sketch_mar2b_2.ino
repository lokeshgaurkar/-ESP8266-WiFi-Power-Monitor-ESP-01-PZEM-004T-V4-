#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <PZEM004Tv30.h>
#include <EEPROM.h>

const char* ap_ssid="PowerMonitor";
const char* ap_password="11223344";
const char* hostname="powermonitor";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

PZEM004Tv30 pzem(Serial);

float voltage,current,power,pf,freq;
double energy_kwh=0;

unsigned long lastSample=0;
unsigned long lastSave=0;

#define EEPROM_SIZE 512
#define ENERGY_ADDR 0
#define TARIFF_ADDR 32

struct Tariff{
float meter;
float ship;
float tax;
float r1;
float r2;
float r3;
};

Tariff tariff;

const char webpage[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en">

<head>

<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">

<title>PowerMonitor</title>

<style>

body{
font-family:Arial;
background:#0f172a;
color:white;
margin:0;
text-align:center;
}

.header{
font-size:26px;
padding:18px;
background:#1e293b;
}

.grid{
display:grid;
grid-template-columns:repeat(auto-fit,minmax(140px,1fr));
gap:15px;
padding:20px;
}

.card{
background:#1e293b;
padding:18px;
border-radius:12px;
}

.value{
font-size:24px;
margin-top:8px;
}

.unit{
font-size:13px;
color:#94a3b8;
}

button{
cursor:pointer;
-webkit-tap-highlight-color:transparent;
}

.resetBtn{
margin:20px;
padding:12px 20px;
border:none;
border-radius:8px;
background:#ef4444;
color:white;
font-size:16px;
}

.billbox{
background:#1e293b;
margin:20px;
padding:22px;
border-radius:14px;
max-width:420px;
margin-left:auto;
margin-right:auto;
}

.inputgroup{
display:flex;
flex-direction:column;
text-align:left;
margin-bottom:14px;
}

.inputgroup label{
font-size:13px;
color:#94a3b8;
margin-bottom:4px;
}

.inputgroup input{
padding:10px;
border-radius:8px;
border:none;
background:#0f172a;
color:white;
font-size:15px;
}

.saveBtn{
width:100%;
padding:12px;
border:none;
border-radius:8px;
background:#22c55e;
color:white;
font-size:16px;
margin-top:10px;
}

.otaBtn{
padding:10px 20px;
border:none;
border-radius:8px;
background:#3b82f6;
color:white;
}

.billtable{
width:100%;
margin-top:20px;
border-collapse:collapse;
}

.billtable td{
padding:10px;
border-bottom:1px solid #334155;
}

.billtable td:last-child{
text-align:right;
}

.totalrow{
font-size:18px;
font-weight:bold;
color:#22c55e;
}

#Volt{
    color: #ef4444;
}
#Amp{
    color: #22c55e;
}
#Watt{
    color: yellow;
}
#kWh{
    color: cyan;
}
#Hz{
    color:rgb(72, 78, 255);
}
#PF{
    color: magenta;
}

.footer{
margin-top:20px;
margin-bottom:20px;
font-size:13px;
color:#94a3b8;
}

</style>

</head>

<body>

<div class="header">Power ⚡︎ Monitor</div>

<div class="grid">

<div class="card">Voltage<div class="value" id="v">--</div><div class="unit" id="Volt">Volt</div></div>
<div class="card">Current<div class="value" id="c">--</div><div class="unit"id="Amp">Amp</div></div>
<div class="card">Power<div class="value" id="p">--</div><div class="unit"id="Watt">Watt</div></div>
<div class="card">Energy<div class="value" id="e">--</div><div class="unit"id="kWh">kWh</div></div>
<div class="card">Frequency<div class="value" id="f">--</div><div class="unit"id="Hz">Hz</div></div>
<div class="card">Power Factor<div class="value" id="pf">--</div><div class="unit"id="PF">PF</div></div>


</div>

<button id="resetBtn" class="resetBtn">Reset Energy</button>

<div class="billbox">

<h2>Estimate Electricity Bill</h2>

<div class="inputgroup">
<label>Meter Charge</label>
<input id="meter" type="number">
</div>

<div class="inputgroup">
<label>Shipping / Unit</label>
<input id="ship" type="number">
</div>

<div class="inputgroup">
<label>Tax % (on total bill)</label>
<input id="tax" type="number">
</div>

<div class="inputgroup">
<label>0-100 Unit Price</label>
<input id="rate1" type="number">
</div>

<div class="inputgroup">
<label>101-300 Unit Price</label>
<input id="rate2" type="number">
</div>

<div class="inputgroup">
<label>300+ Unit Price</label>
<input id="rate3" type="number">
</div>

<button id="saveBtn" class="saveBtn">Save Calculator Settings</button>

<table class="billtable">

<tr><td>Energy Charges</td><td id="energyCost">0</td></tr>
<tr><td>Meter Charges</td><td id="meterCost">0</td></tr>
<tr><td>Shipping Charges</td><td id="shipCost">0</td></tr>
<tr><td>Electricity Tax</td><td id="taxCost">0</td></tr>
<tr class="totalrow"><td>Total Bill</td><td id="bill">0</td></tr>

</table>

</div>

<a href="/update">
<button id="otaBtn" class="otaBtn">Firmware Update</button>
</a>

<div class="footer">Made with ❤️ by Lokesh</div>

<script>

function vibrate(){
if(navigator.vibrate){
navigator.vibrate(40);
}
}

/* Notification permission */

function requestNotificationPermission(){
if("Notification" in window){
if(Notification.permission!=="granted"){
Notification.requestPermission();
}
}
}

let lastNotify=0

function powerNotification(data){

if(Notification.permission==="granted"){

let now=Date.now()

if(now-lastNotify>60000){

new Notification("⚡ Power Monitor",{
body:"Voltage: "+data.v+" V\nPower: "+data.p+" W\nEnergy: "+data.e+" kWh"
})

lastNotify=now

}

}

}

let confirmMode=false
let countdown=5
let timer

function resetStep(){

let btn=document.getElementById("resetBtn")

if(!confirmMode){

confirmMode=true
countdown=5
btn.innerHTML="Confirm Reset (5)"

timer=setInterval(()=>{

countdown--
btn.innerHTML="Confirm Reset ("+countdown+")"

if(countdown==0){
clearInterval(timer)
confirmMode=false
btn.innerHTML="Reset Energy"
}

},1000)

}else{

fetch("/reset")
clearInterval(timer)
confirmMode=false
btn.innerHTML="Reset Energy"

}

}

function calcBill(){

let units=parseFloat(e.innerText)

let meterCharge=parseFloat(meter.value)
let shipCharge=parseFloat(ship.value)
let taxPercent=parseFloat(tax.value)

let r1=parseFloat(rate1.value)
let r2=parseFloat(rate2.value)
let r3=parseFloat(rate3.value)

let energyCharge=0

if(units<=100)
energyCharge=units*r1
else if(units<=300)
energyCharge=(100*r1)+((units-100)*r2)
else
energyCharge=(100*r1)+(200*r2)+((units-300)*r3)

let shippingCost=units*shipCharge

let subtotal=energyCharge+meterCharge+shippingCost
let taxAmount=subtotal*(taxPercent/100)

let total=subtotal+taxAmount

energyCost.innerHTML=energyCharge.toFixed(2)+" ₹"
meterCost.innerHTML=meterCharge.toFixed(2)+" ₹"
shipCost.innerHTML=shippingCost.toFixed(2)+" ₹"
taxCost.innerHTML=taxAmount.toFixed(2)+" ₹"
bill.innerHTML=total.toFixed(2)+" ₹"

}

function saveTariff(){

fetch(`/tariff?meter=${meter.value}&ship=${ship.value}&tax=${tax.value}&r1=${rate1.value}&r2=${rate2.value}&r3=${rate3.value}`)
.then(()=>alert("Calculator values saved"))

}

function update(){

fetch("/data")
.then(r=>r.json())
.then(data=>{

v.innerHTML=data.v
c.innerHTML=data.c
p.innerHTML=data.p
e.innerHTML=data.e
f.innerHTML=data.f
pf.innerHTML=data.pf

if(!meter.value){
meter.value=data.meter
ship.value=data.ship
tax.value=data.tax
rate1.value=data.r1
rate2.value=data.r2
rate3.value=data.r3
}

calcBill()

if(data.p>5){
powerNotification(data)
}

})

}

document.getElementById("resetBtn").addEventListener("click",()=>{vibrate();resetStep();});
document.getElementById("saveBtn").addEventListener("click",()=>{vibrate();saveTariff();});
document.getElementById("otaBtn").addEventListener("click",()=>{vibrate();});

requestNotificationPermission()

setInterval(update,1500)
update()

</script>

</body>
</html>

)=====";


void handleRoot(){ server.send_P(200,"text/html",webpage); }

void handleReset(){
energy_kwh=0;
EEPROM.put(ENERGY_ADDR,energy_kwh);
EEPROM.commit();
server.send(200,"text/plain","reset");
}

void handleTariff(){

tariff.meter=server.arg("meter").toFloat();
tariff.ship=server.arg("ship").toFloat();
tariff.tax=server.arg("tax").toFloat();
tariff.r1=server.arg("r1").toFloat();
tariff.r2=server.arg("r2").toFloat();
tariff.r3=server.arg("r3").toFloat();

EEPROM.put(TARIFF_ADDR,tariff);
EEPROM.commit();

server.send(200,"text/plain","saved");
}

void handleData(){

voltage=pzem.voltage();
current=pzem.current();
power=pzem.power();
freq=pzem.frequency();
pf=pzem.pf();

if(isnan(voltage)) voltage=0;
if(isnan(current)) current=0;
if(isnan(power)) power=0;
if(isnan(freq)) freq=0;
if(isnan(pf)) pf=0;

unsigned long now=millis();
double hours=(now-lastSample)/3600000.0;

energy_kwh+=(power*hours)/1000.0;
lastSample=now;

if(millis()-lastSave>300000){
EEPROM.put(ENERGY_ADDR,energy_kwh);
EEPROM.commit();
lastSave=millis();
}

String json="{";

json+="\"v\":"+String(voltage,1)+",";
json+="\"c\":"+String(current,3)+",";
json+="\"p\":"+String(power,1)+",";
json+="\"e\":"+String(energy_kwh,5)+",";
json+="\"f\":"+String(freq,1)+",";
json+="\"pf\":"+String(pf,2)+",";

json+="\"meter\":"+String(tariff.meter)+",";
json+="\"ship\":"+String(tariff.ship)+",";
json+="\"tax\":"+String(tariff.tax)+",";
json+="\"r1\":"+String(tariff.r1)+",";
json+="\"r2\":"+String(tariff.r2)+",";
json+="\"r3\":"+String(tariff.r3);

json+="}";

server.send(200,"application/json",json);
}

void setup(){

Serial.begin(9600);

EEPROM.begin(EEPROM_SIZE);

EEPROM.get(ENERGY_ADDR,energy_kwh);
EEPROM.get(TARIFF_ADDR,tariff);

WiFi.mode(WIFI_AP);
WiFi.softAP(ap_ssid,ap_password);

MDNS.begin(hostname);

server.on("/",handleRoot);
server.on("/data",handleData);
server.on("/reset",handleReset);
server.on("/tariff",handleTariff);

httpUpdater.setup(&server);

server.begin();

lastSample=millis();
}

void loop(){

server.handleClient();
MDNS.update();
yield();

}
