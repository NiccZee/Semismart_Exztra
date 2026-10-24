#ifndef ROOTPAGE_H
#define ROOTPAGE_H
#include "../../user_config.h"
#if BLUETOOTH_WIFI_ENABLED

#include <pgmspace.h>

const char rootPageHtml[] PROGMEM = R"HTML(
<!doctype html>
<html>
<head>
<meta charset='utf-8'/>
<title>SemiSmart</title>
<style>

body, .container {
    font-size: 150%;
}

body {
    background-color:#f0f0f0;
    font-family:Arial,Helvetica,Sans-Serif;
    color:#000088;
    margin:0;
    padding:20px;
}

.container {
    max-width:750px;
    font-size:20px;
    margin:20px auto;
    text-align:center;
}

.power-label {
    font-weight:800;
    font-size: 60px;
    color:#222;
    margin-bottom:2px;
}

.frame {
    display:inline-block;
    width:600px;
    padding:30px;
    border-width:12px;
    border-radius:90px;
    border:12px solid #e07902;
    box-sizing:border-box;
    background:transparent;
}

.slider-wrap {
    width:300px;
    margin:1px auto;
    display:flex;
    align-items:center;
    gap:12px;
    justify-content:center;
}

.power-slider-wrap {
    width:500px;
}

input[type='range'] {
    -webkit-appearance:none;
    appearance:none;
    width:480px;
    height:30px;
    background:transparent;
}

input[type='range']::-webkit-slider-runnable-track {
    height:12px;
    background:#ddd;
    border-radius:6px;
}
input[type='range']::-moz-range-track {
    height:12px;
    background:#ddd;
    border-radius:6px;
}

input[type='range']::-webkit-slider-thumb {
    -webkit-appearance:none;
    width:60px;
    height:60px;
    margin-top:-25px;
    border-radius:50%;
    background:var(--thumb-color, #ff4d4d) !important;
    box-shadow:4px 4px 6px rgba(0,0,0,0.22);
    cursor:pointer;
}
input[type='range']::-moz-range-thumb {
    width:60px;
    height:60px;
    border-radius:50%;
    background:var(--thumb-color, #ff4d4d) !important;
    box-shadow:4px 4px 6px rgba(0,0,0,0.22);
    cursor:pointer;
}

.indicator {
    width:90px;
    height:90px;
    border-width:9px;
    border-radius:50%;
    border:9px solid #111;
    box-shadow:0 2px 6px rgba(0,0,0,0.18);
    display:inline-block;
    vertical-align:middle;
}

#powerSlider + #powerIndicator {
    margin-left:1px;
}

.status-text {
    margin-top:1px;
    font-size:60px;
    color:#222;
    font-weight:700;
}

.mode-row, .sensor-row {
    margin-top:12px;
    color:#111;
    display:flex;
    flex-direction:column;
    gap:4px;
    width:100%;
}

.sensor-row {
    font-size:54px;
}

.mode-row {
    font-size:42px;
}

.sensor-item {
    display:flex;
    justify-content:space-between;
    align-items:center;
    width:100%;
}

.label {
    text-align:left;
    width:340px;
    display:inline-block;
}

.target-row {
    margin-bottom:22px;
}

.reading {
    text-align:right;
    min-width:237px;
}

.value {
    font-weight:600;
}

.slider-wrap {
    text-align:right;
    justify-content:flex-end;
}

.divider {
    width:calc(100% - 20px);
    height:16px;
    background-color:#e07902;
    margin:10px auto;
}

.small-divider {
    width:80%;
    height:6px;
    background-color:#e07902;
    margin:10px auto;
}

.toggle-switch {
    position: relative;
    width: 121px;
    height: 60px;
    display: inline-block;
}

.toggle-switch input {
    display: none;
}

.slider-toggle {
    position: absolute;
    cursor: pointer;
    top: 0; left: 0;
    right: 0; bottom: 0;
    background-color: #ff4d4d;
    border-radius: 60px;
    transition: 0.3s;
    box-shadow: inset 0 0 8px rgba(0,0,0,0.3);
}

.slider-toggle:before {
    position: absolute;
    content: "";
    height: 52px;
    width: 52px;
    left: 4px;
    bottom: 4px;
    background-color: white;
    border-radius: 50%;
    transition: 0.3s;
    box-shadow: 2px 2px 6px rgba(0,0,0,0.25);
}

.switch-label {
    width: 260px;
    text-align: right;
    display: inline-block;
    overflow: hidden;
    text-overflow: ellipsis;
}

.toggle-switch input:checked + .slider-toggle {
    background-color: #4caf50;
}

.toggle-switch input:checked + .slider-toggle:before {
    transform: translateX(61px);
}

.sensor-temp .slider-toggle {
    background-color: #ff4d4d !important;
}

.sensor-hum .slider-toggle {
    background-color: #4da6ff !important;
}

.powerloss-switch .slider-toggle {
    background-color: #a64dff !important;
}

.powerloss-switch input:checked + .slider-toggle {
    background-color: #c27dff !important;
}

.fullwidth-setting {
    width: 100%;
    display: flex;
    flex-direction: column;
    align-items: center;
    margin: 25px 0;
}

.fullwidth-setting .setting-title {
    font-size: 42px;
    font-weight: 600;
    margin-bottom: 1px;
    margin-top: 10px;
    text-align: center;
    width: 100%;
}

.fullwidth-setting .slider-container {
    width: 98%;
}

.fullwidth-setting input[type="range"] {
    width: 100%;
}

.fullwidth-setting .setting-value {
    margin-top: 0px;
    margin-bottom: 20px;
    font-size: 38px;
    text-align: center;
    width: 100%;
}

@media (max-width:500px){
    .slider-wrap { width:300px; gap:8px; }
    input[type='range'] { width:200px; }
}

.file-upload {
    display:flex;
    gap:12px;
    align-items:center;
    justify-content:center;
    margin-top:20px;
}

.file-input {
    display:none;
}

.file-btn {
    background:#e07902;
    color:#111;
    font-size:38px;
    font-weight:700;
    padding:12px 30px;
    border-radius:12px;
    border: none;
    cursor:pointer;
    box-shadow:0 4px 10px rgba(0,0,0,0.18);
    text-transform:none;
}

.file-btn:hover { 
    filter:brightness(0.98); 
}

.file-name {
    font-size:22px;
    color:#222;
    max-width:320px;
    overflow:hidden;
    text-overflow:ellipsis;
    white-space:nowrap;
    display:inline-block;
    text-align:left;
}

.progress-wrap {
    width: 100%;
    display: flex;
    justify-content: center;
    margin-top: 12px;
}
.progress-bar-outer {
    width: 80%;
    max-width: 760px;
    background: #eee;
    height: 36px;
    border-radius: 12px;
    box-shadow: inset 0 1px 3px rgba(0,0,0,0.08);
    overflow: hidden;
}
.progress-bar-inner {
    width: 0%;
    height: 100%;
    background: #4da6ff;
    border-radius: 12px;
    transition: width 200ms ease, background-color 200ms ease;
}

</style>
</head>

<body>
<div class='container'>
    <h1 style="margin:0 0 4px 0; font-size:86px; color:#e07902; background-color:#111;">SemiSmart Exztra</h1>

    <div class='power-label'>POWER</div>
    <div class='frame'>
        <div class='slider-wrap power-slider-wrap'>
            <input id='powerSlider' type='range' min='0' max='2' step='1' value='%d'/>
            <span id='powerIndicator' class='indicator'></span>
        </div>
    </div>

    <div class='status-text' id='powerStatus'>Initializing...</div>
    <div class="divider"></div>

    <div class="sensor-row">
        <div class="sensor-item"><span class="label">Temperature:</span><span class="reading"><span id="tempValue" class="value">--</span><b>°C</b></span></div>
        <div class="sensor-item"><span class="label">Humidity:</span><span class="reading"><span id="humValue" class="value">--</span><b>%</b></span></div>
        <div class="sensor-item"><span class="label">Fan Speed:</span><span class="reading"><span id="fanSpeedValue" class="value">--</span></span></div>
        <div class="sensor-item"><span class="label">Heater power:</span><span class="reading"><span id="heatPowerValue" class="value">--</span></span></div>
        <div class="sensor-item"><span class="label">Heater Temp:</span><span class="reading"><span id="heaterTempValue" class="value">--</span><b>°C</b></span></div>
        <div class="divider"></div>
    </div>

    <div class="mode-row">
        <div id="etaRow" class="sensor-item" style="display:none;">
            <span class="label">Drying stops in:</span>
            <span class="reading"><span id="etaValue" class="value">--:--:--</span></span>
        </div>
        <div id="etaDivider" class="divider" style="display:none;"></div>

        <div id="startRow" class="sensor-item" style="display:none;">
            <span class="label">Drying starts in:</span>
            <span class="reading"><span id="startValue" class="value">--:--:--</span></span>
        </div>
        <div id="startDivider" class="divider" style="display:none;"></div>

        <div class="sensor-item"><span class="label">Operating mode:</span><span class="reading"><span id="operatingMode" class="value">--</span></span></div>
        <div class="sensor-item"><span class="label">Controling mode:</span><span class="reading"><span id="controlMode" class="value">--</span></span></div>
        <div class="divider"></div>

        <div style="width:100%; text-align:center; font-size:48px; font-weight:700; margin-top:20px; margin-bottom:10px;">
            Target Settings
        </div>

        <div class="fullwidth-setting">
            <span class="setting-title">Target Temperature:</span>
            <div class="slider-container">
                <input id="targetTempSlider" type="range" min="15" max="45" step="1">
            </div>
            <span id="targetTempValue" class="setting-value">25°C</span>

            <span class="setting-title">Target Humidity:</span>
            <div class="slider-container">
                <input id="targetHumSlider" type="range" min="5" max="90" step="1">
            </div>
            <span id="targetHumValue" class="setting-value">50%</span>
        </div>

        <div class="divider"></div>

        <div style="width:100%; text-align:center; font-size:48px; font-weight:700; margin-top:20px; margin-bottom:10px;">
            Mode Settings
        </div>

        <div class="sensor-item target-row">
            <span class="label">Operating Mode:</span>
            <label class="toggle-switch">
                <input type="checkbox" id="modeToggle">
                <span class="slider-toggle"></span>
            </label>
            <span id="modeToggleLabel" class="value switch-label">Drymode from Humidity</span>
        </div>

        <div class="sensor-item target-row" id="useModeBlock">
            <span class="label">Cycling use mode:</span>
            <label class="toggle-switch">
                <input type="checkbox" id="useModeToggle">
                <span class="slider-toggle"></span>
            </label>
            <span id="useModeLabel" class="value switch-label">Auto</span>
        </div>

        <div class="sensor-item target-row">
            <span class="label">Cycling sensor mode:</span>
            <label class="toggle-switch" id="sensorModeSwitch">
                <input type="checkbox" id="sensorModeToggle">
                <span class="slider-toggle"></span>
            </label>
            <span id="sensorModeLabel" class="value switch-label">Temperature</span>
        </div>

        <div class="divider"></div>

        <div style="width:100%; text-align:center; font-size:48px; font-weight:700; margin-top:20px; margin-bottom:10px;">
            System Settings
        </div>

        <div class="fullwidth-setting">
            <span class="setting-title">Dry Timer Duration:</span>
            <div class="slider-container">
                <input id="dryTimerSlider" type="range" min="10" max="720" step="10" value="60">
            </div>
            <span id="dryTimerValue" class="setting-value">01h 00m</span>

			<div class="small-divider"></div>

            <span class="setting-title">Idle Time Between Cycles:</span>

            <div class="slider-container">
                <input id="idleHoursSlider" type="range" min="0" max="23" step="1" value="0">
            </div>
            <span id="idleHoursValue" class="setting-value">0 hours</span>

            <div class="slider-container">
                <input id="idleMinutesSlider" type="range" min="0" max="59" step="1" value="0">
            </div>
            <span id="idleMinutesValue" class="setting-value">0 minutes</span>

            <span id="idleTimerValue" class="setting-value">No repetition of the cycles</span>

			<div class="small-divider"></div>

            <span class="setting-title">Screen Saver Start Time:</span>
            <div class="slider-container">
                <input id="screenSaverSlider" type="range" min="0" max="60" step="1" value="0">
            </div>
            <span id="screenSaverValue" class="setting-value">Screen saver is disabled</span>
        </div>

		<div class="small-divider"></div>

        <div class="sensor-item target-row">
            <span class="label">Power Loss Memory:</span>
            <label class="toggle-switch powerloss-switch" id="powerLossSwitch">
                <input type="checkbox" id="powerLossToggle">
                <span class="slider-toggle"></span>
            </label>
            <span id="powerLossLabel" class="value switch-label">Power off after power loss</span>
        </div>

        <div class="divider"></div>
        <div id="ledSection" style="display:none;">
            <div style="width:100%; text-align:center; font-size:48px; font-weight:700; margin-top:20px; margin-bottom:10px;">
                LED Settings
            </div>

            <div class="sensor-item target-row">
                <span class="label">LED Pattern:</span>
                <select id="ledPatternSelect" style="font-size:36px; padding:8px; width:380px;">
                    <option value="0">Solid White</option>
                    <option value="1">Pulsing White</option>
                    <option value="2">Rainbow</option>
                    <option value="3">Rainbow Chase</option>
                    <option value="4">Fire Flicker</option>
                    <option value="5">Plasma</option>
                    <option value="6">Sparkle</option>
                    <option value="7">Sparkle Colorful</option>
                    <option value="8">Aurora Fade</option>
                    <option value="9">Color Drift</option>
                    <option value="10">Spooky Flicker</option>
                    <option value="11">Ghost Pulse</option>
                    <option value="12">Lightning Storm</option>
                    <option value="13">Creepy Crawl</option>
                    <option value="14">Zombie March</option>
                    <option value="15">Eyes in the Dark</option>
                </select>
                <span id="ledPatternLabel" class="value switch-label" style="width:260px; margin-left:8px;"></span>
            </div>

            <div class="fullwidth-setting" id="ledBrightnessBlock" style="display:none;">
                <span class="setting-title">LED Brightness:</span>

                <div class="slider-container">
                    <input id="ledBrightnessSlider" type="range" min="0" max="4" step="1" value="2">
                </div>

                <span id="ledBrightnessValue" class="setting-value">Mid</span>
            </div>

        <div class="divider"></div>
        </div>

        <div style="width:100%; text-align:center; font-size:48px; font-weight:700; margin-top:20px; margin-bottom:10px;">
            Firmware Upload
        </div>

        <div class="file-upload" style="width:100%;">
            <form id="otaForm" method="POST" action="/update" enctype="multipart/form-data" style="display:flex; gap:12px; align-items:center; width:100%; justify-content:center;">
                <input id="updateFile" class="file-input" type="file" name="update" accept=".bin" />
                <label for="updateFile" class="file-btn">Select Firmware</label>
                <span id="updateFileName" class="file-name">No file selected</span>
                <input type="submit" id="uploadSubmit" class="file-btn" value="Upload Firmware" />
            </form>
        </div>

        <div style="text-align:center; margin-top:12px;">
            <div id="uploadStatus" class="setting-value" style="font-size:22px; color:#222;">No upload in progress</div>

            <div class="progress-wrap" aria-hidden="false">
              <div class="progress-bar-outer" role="progressbar" aria-valuemin="0" aria-valuemax="100" aria-valuenow="0">
                <div id="uploadProgressBar" class="progress-bar-inner"></div>
              </div>
            </div>
        </div>
    
        <div class="divider"></div>

    </div>

<script>
(function() {

    var slider           = document.getElementById('powerSlider');
    var indicator        = document.getElementById('powerIndicator');
    var status           = document.getElementById('powerStatus');
    var tempEl           = document.getElementById('tempValue');
    var humEl            = document.getElementById('humValue');
    var heaterTempEl     = document.getElementById("heaterTempValue");
    var fanSpeedEl       = document.getElementById('fanSpeedValue');
    var heatPowerEl      = document.getElementById('heatPowerValue');
    var oprModeEl        = document.getElementById('operatingMode');
    var ctrlModeEl       = document.getElementById('controlMode');
    var etaDivider       = document.getElementById("etaDivider");
    var etaTimeRow       = document.getElementById("etaRow");
    var etaTimeValue     = document.getElementById("etaValue");
    var startDivider     = document.getElementById("startDivider");
    var startTimeRow     = document.getElementById("startRow");
    var startTimeValue   = document.getElementById("startValue");

    var targetTempSlider = document.getElementById("targetTempSlider");
    var targetTempValue  = document.getElementById("targetTempValue");
    var targetHumSlider  = document.getElementById("targetHumSlider");
    var targetHumValue   = document.getElementById("targetHumValue");
    var dryTimerSlider   = document.getElementById("dryTimerSlider");
    var dryTimerValue    = document.getElementById("dryTimerValue");
    var idleTimerValue   = document.getElementById("idleTimerValue");
    var screenSaverSlider = document.getElementById("screenSaverSlider");
    var screenSaverValue  = document.getElementById("screenSaverValue");

    var useModeBlock     = document.getElementById("useModeBlock");
    var modeToggle       = document.getElementById("modeToggle");
    var modeToggleLabel  = document.getElementById("modeToggleLabel");
    var modeJustChanged  = false;
    var useModeToggle    = document.getElementById("useModeToggle");
    var useModeLabel     = document.getElementById("useModeLabel");
    var sensorModeToggle = document.getElementById("sensorModeToggle");
    var sensorModeLabel  = document.getElementById("sensorModeLabel");
    var powerLossToggle = document.getElementById("powerLossToggle");
    var powerLossLabel  = document.getElementById("powerLossLabel");
    var sensorModeSwitch = document.getElementById("sensorModeSwitch");

    var idleHoursSlider   = document.getElementById("idleHoursSlider");
    var idleHoursValue    = document.getElementById("idleHoursValue");
    var idleMinutesSlider = document.getElementById("idleMinutesSlider");
    var idleMinutesValue  = document.getElementById("idleMinutesValue");

    var controlModeJustChanged = false;

    var updateFileInput = document.getElementById('updateFile');
    var updateFileName = document.getElementById('updateFileName');
    var otaForm = document.getElementById('otaForm');
    var uploadStatus = document.getElementById('uploadStatus');
    var progressBar = document.getElementById('uploadProgressBar');

    var brightnessJustChanged = false;
    var ledPatternSelect = document.getElementById("ledPatternSelect");
    var ledPatternLabel  = document.getElementById("ledPatternLabel");
    var ledBrightnessSlider = document.getElementById("ledBrightnessSlider");
    var ledBrightnessValue  = document.getElementById("ledBrightnessValue");

    const LED_BRIGHTNESS_LABELS = [
        "Lowest",
        "Low",
        "Mid",
        "High",
        "Highest"
    ];
 


     if (ledPatternSelect) {
         ledPatternSelect.addEventListener("change", function () {
             var v = parseInt(ledPatternSelect.value, 10);
             fetch("/setledpattern?value=" + encodeURIComponent(v)).catch(()=>{});
             // reflect change immediately
             var selectedText = ledPatternSelect.options[ledPatternSelect.selectedIndex].text;
             if (ledPatternLabel) ledPatternLabel.textContent = selectedText;
         });
     }

    function lerpColor(c1, c2, t) {
        return 'rgb(' +
            Math.round(c1[0] + (c2[0] - c1[0]) * t) + ',' +
            Math.round(c1[1] + (c2[1] - c1[1]) * t) + ',' +
            Math.round(c1[2] + (c2[2] - c1[2]) * t) + ')';
    }

    function tempToColor(v, min, max) {
        const t = (v - min) / (max - min || 1);
        return lerpColor([0,128,255], [255,0,0], t);
    }

    function humToColor(v, min, max) {
        const t = (v - min) / (max - min || 1);
        return lerpColor([0,200,0], [0,120,255], t);
    }

    function computeIdleMinutes() {
        return parseInt(idleHoursSlider.value, 10) * 60 +
               parseInt(idleMinutesSlider.value, 10);
    }

    function setThumbColor(sl, color) {
        if (!sl) return;
        try { sl.style.setProperty('--thumb-color', color); } catch (e) {}
    }

    function stateInfo(v) {
        if (v === 0) return { text:'OFF',     color:'#ff4d4d', param:'off' };
        if (v === 1) return { text:'CYCLING', color:'#4caf50', param:'cycling' };
        return          { text:'ACTIVE',  color:'#4d79ff', param:'on' };
    }

    function minutesToHHMM(mins) {
        const h = Math.floor(mins / 60);
        const m = mins % 60;
        return `${String(h).padStart(2,'0')}h ${String(m).padStart(2,'0')}m`;
    }

    function computeControlMode() {
        const useAuto = useModeToggle.checked;
        const sensorTemp = sensorModeToggle.checked;

        if (useAuto && sensorTemp)  return "CONTROL_AUTO_TEMP";
        if (useAuto && !sensorTemp) return "CONTROL_AUTO_HUM";
        if (!useAuto && sensorTemp) return "CONTROL_USER_TEMP";
        return "CONTROL_USER_HUM";
    }

    function sendControlMode() {
        const mode = computeControlMode();
        controlModeJustChanged = true;
        fetch("/setcontrolmode?value=" + encodeURIComponent(mode)).catch(()=>{});
        setTimeout(function(){ controlModeJustChanged = false; }, 1500);
    }

    function updateUI() {
        var v = parseInt(slider.value,10);
        var info = stateInfo(v);
        status.textContent = info.text;
        indicator.style.background = info.color;
        setThumbColor(slider, info.color);
        slider.style.background = (v===1)?'#e8f7e8':(v===2?'#e8f0ff':'#fff0f0');
    }

    function sendState(v) {
        var info = stateInfo(v);
        fetch('/setpower?state=' + encodeURIComponent(info.param)).catch(()=>{});
    }

    slider.addEventListener('input', updateUI);
    slider.addEventListener('change', function(){ sendState(parseInt(slider.value,10)); });

    targetTempSlider.addEventListener("input", function () {
        const v = parseInt(targetTempSlider.value, 10);
        const min = parseInt(targetTempSlider.min, 10);
        const max = parseInt(targetTempSlider.max, 10);

        targetTempValue.textContent = v + "°C";
        setThumbColor(targetTempSlider, tempToColor(v, min, max));
    });

    targetHumSlider.addEventListener("input", function () {
        const v = parseInt(targetHumSlider.value, 10);
        const min = parseInt(targetHumSlider.min, 10);
        const max = parseInt(targetHumSlider.max, 10);

        targetHumValue.textContent = v + "%";
        setThumbColor(targetHumSlider, humToColor(v, min, max));
    });

    targetTempSlider.addEventListener("change", function () {
        var v = parseInt(targetTempSlider.value, 10);
        fetch("/settargettemp?value=" + encodeURIComponent(v)).catch(()=>{});
    });

    targetHumSlider.addEventListener("change", function () {
        var v = parseInt(targetHumSlider.value, 10);
        fetch("/settargethumidity?value=" + encodeURIComponent(v)).catch(()=>{});
    });

	dryTimerSlider.addEventListener("input", function () {
		const v = parseInt(dryTimerSlider.value, 10);
		dryTimerValue.textContent = minutesToHHMM(v);
	});

    dryTimerSlider.addEventListener("change", function () {
        const v = parseInt(dryTimerSlider.value, 10);
        fetch("/setdryduration?value=" + encodeURIComponent(v)).catch(()=>{});
        setThumbColor(dryTimerSlider, "#4da6ff");
    });

    idleHoursSlider.addEventListener("input", function () {
        const h = parseInt(idleHoursSlider.value, 10);
        idleHoursValue.textContent = h === 1 ? h + " hour" : h + " hours";

        const total = computeIdleMinutes();
        idleTimerValue.textContent = total === 0 ? 
            "No repetition of the cycles" : minutesToHHMM(total);
    });

    idleMinutesSlider.addEventListener("input", function () {
        const m = parseInt(idleMinutesSlider.value, 10);
        idleMinutesValue.textContent = m === 1 ? m + " minute" : m + " minutes";

        const total = computeIdleMinutes();
        idleTimerValue.textContent = total === 0 ? 
            "No repetition of the cycles" : minutesToHHMM(total);
    });

    function sendIdleTimer() {
        const total = computeIdleMinutes();
        fetch("/setidlestart?value=" + encodeURIComponent(total)).catch(()=>{});
    }

    idleHoursSlider.addEventListener("change", sendIdleTimer);
    idleMinutesSlider.addEventListener("change", sendIdleTimer);

    screenSaverSlider.addEventListener("input", function () {
        const v = parseInt(screenSaverSlider.value, 10);

        if (v === 0)
            screenSaverValue.textContent = "Screen saver is disabled";
        else
            screenSaverValue.textContent = v + " min";

        setThumbColor(screenSaverSlider, "#4da6ff");
    });

    screenSaverSlider.addEventListener("change", function () {
        const v = parseInt(screenSaverSlider.value, 10);
        fetch("/setscreensaver?value=" + encodeURIComponent(v)).catch(()=>{});
    });

    powerLossToggle.addEventListener("change", function () {
        const v = powerLossToggle.checked ? 1 : 0;

        powerLossLabel.textContent =
            v === 1
            ? "Remember last state after power loss"
            : "Power off after power loss";

        fetch("/setpowerlossmemory?value=" + encodeURIComponent(v)).catch(()=>{});
    });

    modeToggle.addEventListener("change", function () {
        var v = modeToggle.checked ? 1 : 0;

        modeToggleLabel.textContent = v === 1
            ? "Drying on Timer"
            : "Drymode from Humidity";

        modeJustChanged = true;
        fetch("/setmode?value=" + encodeURIComponent(v)).catch(()=>{});
        setTimeout(function(){ modeJustChanged = false; }, 1500);
    });

    useModeToggle.addEventListener("change", function () {
        useModeLabel.textContent = useModeToggle.checked ? "Auto" : "User";
        sendControlMode();
    });

    sensorModeToggle.addEventListener("change", function () {
        const isTemp = sensorModeToggle.checked;

        sensorModeLabel.textContent = isTemp ? "Temperature" : "Humidity";

        sensorModeSwitch.classList.toggle("sensor-temp", isTemp);
        sensorModeSwitch.classList.toggle("sensor-hum", !isTemp);

        sendControlMode();
    });

    ledBrightnessSlider.addEventListener("change", function () {
        const v = parseInt(ledBrightnessSlider.value, 10);

        brightnessJustChanged = true;
        fetch("/setledbrightness?value=" + encodeURIComponent(v)).catch(()=>{});

        setTimeout(() => brightnessJustChanged = false, 1200);
    });

    if (updateFileInput && updateFileName) {
        updateFileInput.addEventListener('change', function () {
            if (updateFileInput.files && updateFileInput.files.length > 0) {
                updateFileName.textContent = updateFileInput.files[0].name;
            } else {
                updateFileName.textContent = 'No file selected';
            }
        });
    }

    function setUploadProgress(percent) {
        if (!progressBar) return;
        var p = Math.max(0, Math.min(100, Math.round(percent)));
        progressBar.style.width = p + '%';
        // color mapping: in-progress blue, success green, error red (set by other functions)
        if (p < 100) progressBar.style.background = '#4da6ff';
        else progressBar.style.background = '#4caf50';
    }

    function setUploadStatus(msg, type) {
        if (!uploadStatus) return;
        uploadStatus.textContent = msg;
        if (type === 'success') {
            uploadStatus.style.color = '#4caf50';
            setUploadProgress(100);
        } else if (type === 'error') {
            uploadStatus.style.color = '#e04d4d';
            if (progressBar) progressBar.style.background = '#e04d4d';
        } else {
            uploadStatus.style.color = '#222';
        }
    }

    function uploadFirmware() {
        if (!updateFileInput || !updateFileInput.files || updateFileInput.files.length === 0) {
            setUploadStatus('No file selected', 'error');
            return;
        }

        var file = updateFileInput.files[0];
        var formData = new FormData();
        formData.append('update', file);

        var xhr = new XMLHttpRequest();
        xhr.open('POST', '/update', true);

        xhr.upload.onprogress = function (e) {
            if (e.lengthComputable) {
                var percent = (e.loaded / e.total) * 100;
                setUploadStatus('Uploading: ' + Math.round(percent) + '%', 'progress');
                setUploadProgress(percent);
            } else {
                setUploadStatus('Uploading...', 'progress');
            }
        };

        xhr.onload = function () {
            if (xhr.status === 200) {
                var resp = (xhr.responseText || '').toUpperCase();
                if (resp.indexOf('OK') !== -1) {
                    setUploadStatus('Upload complete — device rebooting. Waiting for root...', 'success');
                    // ensure progress shows 100%
                    setUploadProgress(100);
                    waitForRootAndReload();
                } else {
                    setUploadStatus('Upload failed: ' + xhr.responseText, 'error');
                    setUploadProgress(0);
                }
            } else {
                setUploadStatus('Upload failed, HTTP ' + xhr.status, 'error');
                setUploadProgress(0);
            }
        };

        xhr.onerror = function () {
            setUploadStatus('Upload error (network)', 'error');
            setUploadProgress(0);
        };

        xhr.send(formData);
        setUploadStatus('Starting upload...', 'progress');
        setUploadProgress(0);
    }

    if (otaForm) {
        otaForm.addEventListener('submit', function (ev) {
            ev.preventDefault();
            uploadFirmware();
        });
    }

    function waitForRootAndReload() {
        var attempt = function () {
            // Use fetch to check availability; don't cache result
            fetch('/', { method: 'GET', cache: 'no-store' })
                .then(function (r) {
                    if (r.ok) {
                        // Root available — navigate to it (reload)
                        window.location.href = '/';
                    } else {
                        setUploadStatus('Rebooting... retrying in 10s', 'progress');
                    }
                })
                .catch(function () {
                    setUploadStatus('Device offline, retrying in 10s...', 'progress');
                });
        };

        attempt();
        var pollInterval = setInterval(attempt, 10000);

        window.addEventListener('beforeunload', function () {
            clearInterval(pollInterval);
        });
    }

    function fetchSensors() {
        fetch('/sensors').then(function(resp){
            if (!resp.ok) throw new Error('network');
            return resp.json();
        }).then(function(o){
            try {
                if (o.temp !== undefined) tempEl.textContent = parseFloat(o.temp).toFixed(2);
                if (o.hum  !== undefined) humEl.textContent  = parseFloat(o.hum).toFixed(2);

                if (o.heatPower !== undefined) {
                    const hp = parseInt(o.heatPower,10);
                    heatPowerEl.textContent = (hp>0)? hp+'%' : 'Heater is Off';
                }

                if (o.heaterTemp !== undefined) {
                    heaterTempEl.textContent = parseFloat(o.heaterTemp).toFixed(1);
                }

                if (o.fanSpeed !== undefined) {
                    const fs = parseInt(o.fanSpeed,10);
                    const hp = parseInt(o.heatPower,10);
                    fanSpeedEl.textContent =
                        (hp===0 && fs>0) ? 'Cooling' :
                        (fs>0) ? fs+'%' : 'Fans Off';
                }

                if (o.oprMode !== undefined) {
                    const opm = parseInt(o.oprMode, 10);
                    const apm = parseInt(o.autoPrintMode, 10);
                    const ctrlm = parseInt(o.ctrlMode, 10);

                    if (!modeJustChanged) {
                        modeToggle.checked = (opm === 1);
                        modeToggleLabel.textContent = (opm === 1)
                            ? "Drying on Timer"
                            : "Drymode from Humidity";
                    }

                    if (opm===0 && apm===0) oprModeEl.textContent='Drymode from Humidity';
                    else if (opm===1 && apm===0) oprModeEl.textContent='Drying on Timer';
                    else if (apm===1 && (ctrlm===1 || ctrlm===3)) oprModeEl.textContent='Printing';

                    if (apm===1 && (ctrlm===1 || ctrlm===3))
                        status.textContent="PRINTING";
                    else
                        updateUI();
                }

                if (o.gpioInterfaceEnabled !== undefined) {
                    const gpioEnabled = parseInt(o.gpioInterfaceEnabled, 10);

                    if (useModeBlock) {
                        useModeBlock.style.display = (gpioEnabled === 1) ? "flex" : "none";
                    }

                    if (gpioEnabled === 0) {
                        useModeToggle.checked = false;
                        useModeLabel.textContent = "User";
                    }
                }

                if (o.ctrlMode !== undefined) {
                    const ctrlm = parseInt(o.ctrlMode, 10);

                    if (!controlModeJustChanged) {
                        if (ctrlm === 0) { // CONTROL_USER_TEMP
                            useModeToggle.checked = false;
                            sensorModeToggle.checked = true;
                            useModeLabel.textContent = "User";
                            sensorModeLabel.textContent = "Temperature";
                        }
                        else if (ctrlm === 1) { // CONTROL_AUTO_TEMP
                            useModeToggle.checked = true;
                            sensorModeToggle.checked = true;
                            useModeLabel.textContent = "Auto";
                            sensorModeLabel.textContent = "Temperature";
                        }
                        else if (ctrlm === 2) { // CONTROL_USER_HUM
                            useModeToggle.checked = false;
                            sensorModeToggle.checked = false;
                            useModeLabel.textContent = "User";
                            sensorModeLabel.textContent = "Humidity";
                        }
                        else if (ctrlm === 3) { // CONTROL_AUTO_HUM
                            useModeToggle.checked = true;
                            sensorModeToggle.checked = false;
                            useModeLabel.textContent = "Auto";
                            sensorModeLabel.textContent = "Humidity";
                        }
                    }

                    ctrlModeEl.textContent =
                        (ctrlm === 0) ? "User Mode Temperature" :
                        (ctrlm === 1) ? "Auto Mode Temperature" :
                        (ctrlm === 2) ? "User Mode Humidity" :
                                        "Auto Mode Humidity";

                    if (ctrlm===1 || ctrlm===3) {
                        slider.max = 1;
                        if (!controlModeJustChanged && parseInt(slider.value,10)===2) {
                            slider.value = 1;
                            updateUI();
                        }
                    } else {
                        slider.max = 2;
                    }
                }

                if (o.ETAhours !== undefined) {
                    const ETAH=parseInt(o.ETAhours,10);
                    const ETAM=parseInt(o.ETAminutes,10);
                    const ETAS=parseInt(o.ETAseconds,10);

                    etaTimeValue.textContent =
                        `${String(ETAH).padStart(2,'0')}:${String(ETAM).padStart(2,'0')}:${String(ETAS).padStart(2,'0')}`;

                    const shouldShowETA =
                        parseInt(o.oprMode,10)===1 &&
                        parseInt(o.autoPrintMode,10)===0 &&
                        parseInt(slider.value,10)===2;

                    etaTimeRow.style.display = shouldShowETA ? "flex" : "none";
                    etaDivider.style.display = shouldShowETA ? "block" : "none";
                }

                if (o.dryTimerDuration !== undefined) {
                    const dd = parseInt(o.dryTimerDuration, 10);
                    if (!isNaN(dd)) {
                        dryTimerSlider.value = dd;
                        dryTimerValue.textContent = minutesToHHMM(dd);
                        setThumbColor(dryTimerSlider, "#4da6ff");
                    }
                }

                if (o.targetTemp !== undefined) {
                    const tt=parseInt(o.targetTemp,10);

                    if (o.tempMin !== undefined) targetTempSlider.min = parseInt(o.tempMin,10);
                    if (o.tempMax !== undefined) targetTempSlider.max = parseInt(o.tempMax,10);

                    targetTempSlider.value = tt;
                    targetTempValue.textContent = tt + "°C";
                    setThumbColor(targetTempSlider, tempToColor(tt, targetTempSlider.min, targetTempSlider.max));
                }

                if (o.targetHumidity !== undefined) {
                    const th=parseInt(o.targetHumidity,10);

                    if (o.humMin !== undefined) targetHumSlider.min = parseInt(o.humMin,10);
                    if (o.humMax !== undefined) targetHumSlider.max = parseInt(o.humMax,10);

                    targetHumSlider.value = th;
                    targetHumValue.textContent = th + "%";
                    setThumbColor(targetHumSlider, humToColor(th, targetHumSlider.min, targetHumSlider.max));
                }

                if (o.idleStartTimer !== undefined) {
                    const ist = parseInt(o.idleStartTimer, 10);
                    const h = Math.floor(ist / 60);
                    const m = ist % 60;

                    idleHoursSlider.value = h;
                    idleMinutesSlider.value = m;

                    idleHoursValue.textContent = h === 1 ? h + " hour" : h + " hours";
                    idleMinutesValue.textContent = m === 1 ? m + " minute" : m + " minutes";

                    idleTimerValue.textContent =
                        ist === 0
                        ? "No repetition of the cycles"
                        : minutesToHHMM(ist);

                    setThumbColor(idleHoursSlider, "#4da6ff");
                    setThumbColor(idleMinutesSlider, "#4da6ff");
                }

                if (o.screenSaverStartTime !== undefined) {
                    const st = parseInt(o.screenSaverStartTime, 10);

                    screenSaverSlider.value = st;

                    if (st === 0)
                        screenSaverValue.textContent = "Screen saver is disabled";
                    else
                        screenSaverValue.textContent = st + " min";

                    setThumbColor(screenSaverSlider, "#4da6ff");
                }

                if (o.powerOutageMemoryMode !== undefined) {
                    const pm = parseInt(o.powerOutageMemoryMode, 10);

                    powerLossToggle.checked = (pm === 1);

                    powerLossLabel.textContent =
                        (pm === 1)
                        ? "Remember last state after power loss"
                        : "Power off after power loss";
                }

                var ledSection = document.getElementById("ledSection");
                const hasLedInfo = (o.ledPatternIndex !== undefined) || (o.ledBrightnessLevel !== undefined);
                if (ledSection) ledSection.style.display = hasLedInfo ? "block" : "none";
                var ledBrightnessBlock = document.getElementById("ledBrightnessBlock");
                if (ledBrightnessBlock) ledBrightnessBlock.style.display = hasLedInfo ? "block" : "none";

                 if (o.ledPatternIndex !== undefined && ledPatternSelect) {
                     const idx = parseInt(o.ledPatternIndex, 10);
                     if (!isNaN(idx)) {
                         if (String(ledPatternSelect.value) !== String(idx)) ledPatternSelect.value = idx;
                         if (ledPatternLabel) ledPatternLabel.textContent = (o.ledPatternName || ledPatternSelect.options[ledPatternSelect.selectedIndex].text);
                     }
                 }

                if (!brightnessJustChanged &&
                    o.ledBrightnessLevel !== undefined && ledBrightnessSlider) {
                    const b = parseInt(o.ledBrightnessLevel, 10);
                    if (!isNaN(b)) {
                        ledBrightnessSlider.value = b;
                        ledBrightnessValue.textContent = LED_BRIGHTNESS_LABELS[b];
                        setThumbColor(ledBrightnessSlider, "#4da6ff");
                    }
                }

               if (o.startHours !== undefined) {
                    const SH=parseInt(o.startHours,10);
                    const SM=parseInt(o.startMinutes,10);
                    const SS=parseInt(o.startSeconds,10);
                    const mto=parseInt(o.manualTurnOff,10);

                    startTimeValue.textContent =
                        `${String(SH).padStart(2,'0')}:${String(SM).padStart(2,'0')}:${String(SS).padStart(2,'0')}`;

                    const shouldShowSTART =
                        parseInt(o.oprMode,10)===1 &&
                        parseInt(o.autoPrintMode,10)===0 &&
                        mto===0 &&
                        parseInt(slider.value,10)===0;

                    startTimeRow.style.display = shouldShowSTART ? "flex" : "none";
                    startDivider.style.display = shouldShowSTART ? "block" : "none";
                }

                if (o.state !== undefined) {
                    var v=parseInt(o.state,10);
                    if (parseInt(slider.max,10)===1 && v===2) v=1;
                    if (slider.value != v) {
                        slider.value=v;
                        updateUI();
                    }
                }

            } catch(e){}
        }).catch(function(){ /* ignore transient errors */ });
    }

    updateUI();
    fetchSensors();
    setInterval(fetchSensors,1000);

})();
</script>

</body>
</html>
)HTML";

#endif // BLUETOOTH_WIFI_ENABLED
#endif // ROOTPAGE_H
