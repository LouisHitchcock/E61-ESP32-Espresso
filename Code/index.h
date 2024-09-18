const char INDEX_HTML[] PROGMEM = R"=====(

<!DOCTYPE html>
<html>
<head>
    <title>E61 ESP Espresso</title>
    
<style>
        body {
          background-color:#121212;
          font-family: arial;

        }
        .sensor-container {
            display: flex;
            justify-content: center;
            margin: 10px auto 10px auto;
            width: 90%;
        }
        .sensor {
            height: 150px;
            width: 30%;
            margin: 0 10px;
            border-radius: 10px;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            color: white;
            font-size: 16px;
            padding: 10px;
        }

        #interpolated {
            background-color: #1e1e1e;
            width: 30%
            margin: 0 10px;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            border-radius: 10px;
            color: white;
            font-size: 16px;
            padding: 10px;
        }
        #temp1 { background-color: #cb4848; }
        
        #temp2 { background-color: #cb4848; }
        .switch-container {
            display: flex;
            justify-content: center;
            margin: 0px auto 10px auto;
            width: 90%;
        }


        .switch {
            padding:10px;
            flex-grow: 1; /* Allows the switch to grow and fill the container */
            margin: 0 5px;
            text-align: center;
            cursor: pointer;
            transition: transform 0.5s;
            border-radius: 10px;
        }

        .toggle-switch {
          position: relative;
          display: inline-block;
          width: 90px;
          height: 34px;
          margin-top: 10px;
          border-radius: 17px;
        }

        .toggle-switch input {
          opacity: 0;
          width: 0;
          height: 0;
        }

        .toggle-slider {
          position: absolute;
          cursor: pointer;
          top: 0;
          left: 0;
          right: 0;
          bottom: 0;
          background-color: #121212;
          transition: .4s;
          border-radius: 17px;
        }

        .toggle-slider:before {
          position: absolute;
          content: "";
          height: 26px;
          width: 26px;
          left: 4px;
          bottom: 4px;
          background-color: white;
          transition: .4s;
          border-radius: 50%;
        }

        input:checked + .toggle-slider {
          background-color: #4894cb;
        }

        input:checked + .toggle-slider:before {
          transform: translateX(56px);
        }

        .toggle-label {
          color: #fff;
          position: absolute;
          top: 50%;
          transform: translateY(-50%);
          transition: opacity 0.4s;
        }

        #toggle-label-off {
          left: 10px;
        }

        #toggle-label-on {
          right: 10px;
          opacity: 0;
        }

        .EnableAll {
          width:90%;
          margin: 0px auto;
          text-align: center;
          cursor: pointer;
          transition: transform 0.5s;
          border-radius: 10px;
          padding: 10px 0;
        }
        .EnableAll:hover {
          transform: scale(0.95);
        }

        .pumpToggle {
          width:90%;
          margin: 10px auto 0px auto;
          text-align: center;
          cursor: pointer;
          transition: transform 0.5s;
          border-radius: 10px;
          padding: 10px 0;
          background-color: #cb4848;
          color: #fff;
        }
        .pumpToggle:hover {
          transform: scale(0.95);
        }
        .switch:hover {
          transform: scale(0.95);
        }
        .sensor-value {
            font-size: 30px; /* Adjust the size as needed */
        }
        
        .green-button {
            background-color: #48cb55;
            color: white;
        }

        .red-button {
            background-color: #cb4848;
            color: white;
        }


        #play-control {
            display: flex;
         align-items: center;
         margin-bottom: 10px;
        }


        .main-container {
            display: flex;
            max-width: 90%;
            justify-content: center;
            align-items: center;
            margin: auto;
            background-color:#121212;
        }

        .presets-container {
            display: flex;
            flex-wrap: wrap;
            align-items: flex-start;
            height: auto;
        }

        .presets-column {
            display: flex;
            flex-direction: column;
            margin-right: 20px;
            justify-content: space-around;
            flex: 1;
            
        }

        .presets-column button {
            flex: auto;
            margin-bottom: 10px;
            font-size: larger;
            padding: 10px 10px;
            /* Add additional button styling if needed */
        }

        .chart-container {
            flex-grow: 1;
            max-width: 66%;
            background-color:#1e1e1e;
            margin-top: 20px;
            border-radius: 10px;
        }

        .colunm1 {
            margin-left:0px 20px 0px 00px;
        }

        /* Add this rule to align items in the second row with the first row */
        .presets-column:nth-child(n+2) {
            align-self: flex-start; /* Align items in the second row to the top */
        }

        .current-value {
            font-weight: bold;
            margin-left: 5px;
            color: white;
            background-color: #404040 !important; /* Add !important */
            padding: 10px;
            font-size: 16px;
            border-radius: 50%;
        }

        #targetSteamTemp, #targetBrewTemp {
          background-color: rgba(0, 0, 0, 0);
          border: none;
          color: #fff;
          font-size: 16px;
          width: 40px;
          text-align: center;
        }

        input[type=number] {
          -moz-appearance: textfield;
        }

        /* Style for the top header */
        .top-header {
            background-color: #272727;
            color: #fff; /* White text color */
            display: flex;
            justify-content: space-between; /* Space items out */
            position: fixed; /* Set the navbar to fixed position */
            top: 0; /* Position the navbar at the top of the page */
            width: 100%; /* Full width */
            height: 55px;
            padding: 20px 0px 20px 0px;
        }

        .top-header .date-time {
            font-size: 1rem; /* Adjust size as needed */
            text-align: center;
            margin: auto 0px auto 40px;
        }

        .top-header .emergency-stop {
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer; /* Make it look clickable */
            font-weight: bold;
            margin-right:40px;
            transition: transform 0.5s;
        }
        .top-header .emergency-stop:hover {
          transform: scale(0.95);
        }

        .top-header .title {
          font-size: 2rem;
          font-weight: bold;
          text-align: center;
          flex-grow: 1;
          margin-top: auto;
          margin-bottom: auto;
        }

        .top-header .tank {
            width: 3%; 
            height: 95%;
            border: 2px solid #007bff; 
            border-radius: 5px;
            position: relative;
            margin-right: 10px;
            transition: transform 0.5s;
        }

      .top-header .fill {
          background-color: #007bff;
          width: 100%;
          position: absolute;
          bottom: 0;
          border-radius: 0px 0px 3px 3px;
          transition: height 0.2s ease-out, transform 0.2s;
      }

        .spacer {
          padding-top: 90px;
        }

        #eStopOverlay {
          position: fixed;
          top: 0;
          left: 0;
          height: 100vh;
          width: 100vw;
          background: rgba(0, 0, 0, 0.5);
          display: flex;
          flex-direction: column;
          align-items: center;
          justify-content: center;
        }

        .eStopTxt {
          font-size: 6em;
          color: #fff;
          text-align: center;
        }

        .eStopHidden {
          display: none;
          opacity: 0;
          transition: opacity 0.5s;
          z-index: -500;
        }
        .eStopVisible {
          display: flex;
          opacity: 1;
          transition: opacity 0.5s;
          z-index: 500;
        }

        .firmwareRestart {
          margin-top: 20px;
          border: 3px solid #fff;
          background: rgba(0, 0, 0, 0.5);
          color: #fff;
          margin-top: 20px;
          padding: 15px;
          transition: transform 0.2s;
        }

        .firmwareRestart:hover {
          background: #fff;
          color: #000;
        }
</style>

</head>

<body>

<div id="eStopOverlay" class="eStopHidden" >
   <div class="eStopTxt">EMERGENCY STOP ACTIVATED</div>
   <button id="firmwareRestartButton" class="firmwareRestart" onclick="toggleSwitchState(6)">Firmware Restart</button>
 </div>

<div class="top-header">
        <div class="date-time" id="currentDateTime"></div>
        <div class="title">The Valkyrie</div>
        <div class="tank">
            <div class="fill" id="fillLevel"></div>
        </div>
        <button class="emergency-stop switch3" id="switch3" onclick="toggleSwitchState(2)">Emergency Stop</button>
</div>

<div class="spacer"></div>

        <div class="sensor-container">
            <div class="sensor" id="temp1">
                <div>Brew Boiler Temp</div>
                <div class="sensor-value" id="currentBrewTemp">0</div>
                <input type="number" id="targetBrewTemp" placeholder="94" min="50" max="98"> 
            </div>
            <div class="sensor" id="interpolated">
                <div>Flow Value</div>
                <div class="sensor-value" id="flowValue">0</div>
                <label class="toggle-switch">
                  <span id="toggle-label-off" class="toggle-label">Manual</span>
                  <input type="checkbox" id="manualAuto" onclick="toggleSwitchState(7)">
                  <span class="toggle-slider"></span>
                  <span id="toggle-label-on" class="toggle-label" style="display:none;">Auto</span>
                </label>
            </div>
            <div class="sensor" id="temp2">
                <div>Steam Boiler Temp</div>
                <div class="sensor-value" id="currentSteamTemp">0</div>
                <input type="number" id="targetSteamTemp" placeholder="125" min="101" max="140"> 
            </div>
        </div>
</div>


    <div class="switch-container">
        <div id="logicState0" class="switch" onclick="toggleSwitchState(0)"><span>Brew Boiler</span></div>
        <div id="logicState1" class="switch" onclick="toggleSwitchState(1)"><span>Steam Boiler</span></div>
    </div>
    <div id="logicState2" class="EnableAll" onclick="toggleSwitchState(3)"><span>Enable All</span></div>
    <div class="pumpToggle" onclick="toggleSwitchState(4)"><span>Fill Boiler</span></div>

      <div id="control-panel">
<div class="main-container">
    <div class="presets-container">
        <div class="presets-column colunm1">
            <button onclick="playGraph()">Play</button>
            <button id="preset1-button" onclick="updateChartWithPreset('preset1', 45)">Preset 1</button>
            <button id="preset2-button" onclick="updateChartWithPreset('preset2', 45)">Preset 2</button>
            <button id="preset3-button" onclick="updateChartWithPreset('preset3', 45)">Preset 3</button>
            <input style="font-size: larger; padding: 20px 10px;" type="number" id="secondsInput" value="45" min="1" max="100">
        </div>
        

        <div class="presets-column">
            <button><span id="currentValue">0</span></button>
            <button id="preset4-button" onclick="updateChartWithPreset('preset4', 45)">Preset 4</button>
            <button id="preset5-button" onclick="updateChartWithPreset('preset5', 45)">Preset 5</button>
            <button id="preset6-button" onclick="updateChartWithPreset('preset6', 45)">Preset 6</button>
            <button onclick="updateChartTime()">Update Time</button>
        </div>
    </div>




    <div class="chart-container">
        <canvas id="myChart"></canvas>
    </div>
</div>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

<script src="https://cdn.jsdelivr.net/npm/chartjs-plugin-dragdata"></script>


<script>
function updateDateTime() {
    const now = new Date();
    const dateTimeString = now.toLocaleString();
    document.getElementById('currentDateTime').innerText = dateTimeString;
}

// Update date and time every second
setInterval(updateDateTime, 1000);

// Initial update
updateDateTime();

</script>

<script>
    // Global variable for the current preset
    let currentPreset = 'preset1';

    // Define the presets functions
    function generatePreset1Data(seconds) {
        return Array.from({length: seconds + 1}, (_, i) => {
            if (i < 2) {
                // Linear ramp up to 100 at second 2
                return 50 * i;
            } else if (i >= 2 && i <= seconds - 2) {
                // Non-linear decline to 50
                const decline = 50 * (1 - Math.exp(-(i - 2) / 20));
                return 100 - decline;
            } else {
                // Linear decline to 0 in the last 2 seconds
                return 50 - 25 * (i - (seconds - 2));
            }
        });
    }

    function generatePreset2Data(seconds) {
        return Array.from({length: seconds + 1}, (_, i) => {
            if (i < 2) {
                // Linear ramp up to 100 in 2 seconds
                return 50 * i;
            } else if (i >= 2 && i < 10) {
                // Hold at 100 for 8 seconds
                return 100;
            } else if (i < seconds) {
                // Non-linear decline to 0, but not reaching 0 yet
                const decayFactor = 2; // Adjust this for a steeper or shallower decline
                const declineRate = (seconds - 10); // Total time for decline
                return 100 * Math.exp(-decayFactor * (i - 10) / declineRate);
            } else {
                // Ensure the final value is exactly 0
                return 0;
            }
        });
    }




    function generatePreset3Data(seconds) {
        return Array.from({length: seconds + 1}, (_, i) => {
            if (i < 2) {
                // Linear ramp up to 100 in 2 seconds
                return 50 * i;
            } else {
                // Linear decline back to 0
                const declineRate = 100 / (seconds - 2);
                return Math.max(100 - declineRate * (i - 2), 0); // Ensures value does not go below 0
            }
        });
    }

    function generatePreset4Data(seconds) {
        const rampDuration = 1; // Duration of the initial ramp from 0 to 100
        const holdDuration1 = 5; // Duration of the first hold at 100
        const cutDuration = 5; // Duration of cutting to 0
        const rampDuration2 = 1; // Duration of the second ramp from 0 to 100
        const linearDeclineDuration = seconds - (rampDuration + holdDuration1 + cutDuration + rampDuration2);

        return Array.from({ length: seconds + 1 }, (_, i) => {
            if (i < rampDuration) {
                return (100 / rampDuration) * i; // Sudden ramp from 0 to 100 in 1 second
            } else if (i < rampDuration + holdDuration1) {
                return 100; // Hold at 100 for 5 seconds
            } else if (i < rampDuration + holdDuration1 + cutDuration) {
                return 0; // Cut to 0 and hold for 5 seconds
            } else if (i < rampDuration + holdDuration1 + cutDuration + rampDuration2) {
                return (100 / rampDuration2) * (i - (rampDuration + holdDuration1 + cutDuration)); // Ramp from 0 to 100 in 1 second
            } else if (i < seconds) {
                return 100 - (50 / linearDeclineDuration) * (i - (rampDuration + holdDuration1 + cutDuration + rampDuration2)); // Linear decline to 50
            } else {
                return 0; // Set to 0 for the remaining time
            }
        });
    }

    function generatePreset5Data(seconds) {
        // Define the logic for preset 5
    }

    function generatePreset6Data(seconds) {
        // Define the logic for preset 6
    }

    const presets = {
        preset1: generatePreset1Data,
        preset2: generatePreset2Data,
        preset3: generatePreset3Data,
        preset4: generatePreset4Data,
        preset5: generatePreset5Data,
        preset6: generatePreset6Data
    };

    let myChart;
    const ctx = document.getElementById('myChart').getContext('2d');
    let playInterval;
    const updateInterval = 1000;

    function createChart(data) {
        const labels = Array.from({length: data.length}, (_, i) => i.toString());
        
        if (myChart) {
            myChart.destroy();
        }

        myChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: labels, 
                datasets: [{
                    label: 'Flow Value',
                    data: data,
                    borderColor: '#cb4848',
                    borderWidth: 2
                }]
            },
            options: {
                scales: {
                    y: {
                        beginAtZero: true,
                        max: 100
                    }
                },
                plugins: {
                    dragData: true
                }
            }
        });
    }

    function playGraph() {
        let currentIndex = 0;
        const dataset = myChart.data.datasets[0];

        if (playInterval) clearInterval(playInterval);

        playInterval = setInterval(() => {
            if (currentIndex < dataset.data.length) {
                dataset.pointBackgroundColor = dataset.data.map((_, i) => i === currentIndex ? 'rgba(75, 192, 192, 1)' : '#cb4848');
                dataset.pointRadius = dataset.data.map((_, i) => i === currentIndex ? 5 : 3);

                myChart.update();

                document.getElementById('currentValue').textContent = dataset.data[currentIndex].toFixed(2);
                currentIndex++;
            } else {
                clearInterval(playInterval);
            }
        }, updateInterval);
    }

    window.updateChartTime = function() {
        const seconds = parseInt(document.getElementById('secondsInput').value, 10);
        updateChartWithPreset(currentPreset, seconds);
    }

    window.updateChartWithPreset = function(preset, seconds) {
        currentPreset = preset;
        let data = presets[preset](seconds);
        createChart(data);
    }

    window.updateChartWithPreset = function(preset) {
        const seconds = parseInt(document.getElementById('secondsInput').value, 10);  // Get the current value from the input
        currentPreset = preset;
        let data = presets[preset](seconds);
        createChart(data);
    };

    window.playGraph = playGraph;

    window.onload = function() {
        updateChartWithPreset('preset1', 45);
    };
</script>


<script>
    function getTemperatureColor(temp) {
        const minTemp = 30;
        const maxTemp = 70;
        // Normalize temperature value between 0 and 1
        const normalized = (temp - minTemp) / (maxTemp - minTemp);
        const red = Math.min(255, Math.max(0, 255 * (1 - normalized)));
        const green = Math.min(255, Math.max(0, 255 * normalized));
        return `rgb(${red}, ${green}, 0)`; // RGB color
    }

    function requestSensorData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                processSensorData(this.responseText);
            }
        };
        xhttp.open("GET", "http://espespresso.local/getData", true);
        xhttp.send();
    }

    function processSensorData(xmlString) {
        var parser = new DOMParser();
        var xmlDoc = parser.parseFromString(xmlString, "text/xml");

        function getElementValueByTagName(parent, tagName) {
            var element = parent.getElementsByTagName(tagName)[0];
            if (!element || !element.childNodes[0]) return null;
            return element.childNodes[0].nodeValue;
        }

        var eStopOverlay = document.getElementById('eStopOverlay');
        var eStopElement = xmlDoc.getElementsByTagName("isEstopTriggered")[0];
        if (eStopElement) {
            var eStopValueElement = eStopElement.getElementsByTagName("value")[0];
            if (eStopValueElement) {
                var eStopValue = eStopValueElement.childNodes[0].nodeValue;
                if (eStopValue === "1") {
                    eStopOverlay.classList.add('eStopVisible');
                    eStopOverlay.classList.remove('eStopHidden');
                } else {
                    eStopOverlay.classList.add('eStopHidden');
                    eStopOverlay.classList.remove('eStopVisible');
                }
            }
        }

        var logicStates = xmlDoc.getElementsByTagName("logicStates")[0];
        if (logicStates) {
            var states = logicStates.getElementsByTagName("state");
            for (var i = 0; i < states.length; i++) {
                var stateId = states[i].getAttribute("id");
                var stateValue = states[i].getAttribute("value");

                var element = document.getElementById("logicState" + stateId);
                if (element) {
                    element.classList.remove("green-button", "red-button");
                    if (stateValue === "1") {
                        element.classList.add("green-button");
                    } else {
                        element.classList.add("red-button");
                    }
                }
            }
        }

        var steamBoilerTargetElement = xmlDoc.getElementsByTagName("SteamBoilerTarget")[0];
        if (steamBoilerTargetElement) {
            var steamBoilerTargetValue = steamBoilerTargetElement.getElementsByTagName("value")[0].childNodes[0].nodeValue;
            document.getElementById("targetSteamTemp").placeholder = steamBoilerTargetValue;
        }

        var brewBoilerTargetElement = xmlDoc.getElementsByTagName("BrewBoilerTarget")[0];
        if (brewBoilerTargetElement) {
            var brewBoilerTargetValue = brewBoilerTargetElement.getElementsByTagName("value")[0].childNodes[0].nodeValue;
            document.getElementById("targetBrewTemp").placeholder = brewBoilerTargetValue;
        }

        var brewBoilerCalc = xmlDoc.getElementsByTagName("BrewBoilerCalc")[0];
        if (brewBoilerCalc) {
            var brewBoilerValue = brewBoilerCalc.getElementsByTagName("value")[0].childNodes[0].nodeValue;
            document.getElementById("currentBrewTemp").innerHTML = brewBoilerValue;
            document.getElementById("temp1").style.backgroundColor = getTemperatureColor(parseFloat(brewBoilerValue));
        }

        var steamBoilerCalc = xmlDoc.getElementsByTagName("SteamBoilerCalc")[0];
        if (steamBoilerCalc) {
            var steamBoilerValue = steamBoilerCalc.getElementsByTagName("value")[0].childNodes[0].nodeValue;
            document.getElementById("currentSteamTemp").innerHTML = steamBoilerValue;
            document.getElementById("temp2").style.backgroundColor = getTemperatureColor(parseFloat(steamBoilerValue));
        }

        var potPercentValue = xmlDoc.querySelector("potPercent value") ? xmlDoc.querySelector("potPercent value").textContent : null;
        var isAutoEnabled = xmlDoc.querySelector("autoEnabled value") ? xmlDoc.querySelector("autoEnabled value").textContent : null;

        var currentValueElement = document.getElementById("currentValue");
        var autoPercentValue = currentValueElement ? currentValueElement.textContent : "N/A";

        // Adjusted the logic here
        if (isAutoEnabled === "1") {
            // Auto mode: Use the automatic function's value
            document.getElementById("flowValue").textContent = autoPercentValue + "%";
        } else {
            // Manual mode: Use the manual function's value
            document.getElementById("flowValue").textContent = potPercentValue + "%";
        }



        var boilerFilledElements = xmlDoc.getElementsByTagName("boilerFilled");
        if (boilerFilledElements.length > 0) {
            var boilerFilledValueElement = boilerFilledElements[0].getElementsByTagName("value")[0];
            if (boilerFilledValueElement) {
                var boilerFilledValue = boilerFilledValueElement.textContent || boilerFilledValueElement.innerText;
                if (boilerFilledValue === "1") {
                    setFillLevel(80);
                } else {
                    setFillLevel(0);
                }
            }
        }

        var outputStates = xmlDoc.getElementsByTagName("outputStates")[0];
        if (outputStates) {
            var states = outputStates.children;
            for (var i = 0; i < states.length; i++) {
                var stateId = states[i].getAttribute("id");
                var stateValue = states[i].getAttribute("value");
                // Update UI as needed for outputStates
            }
        }
    }

    function setFillLevel(percentage) {
        document.getElementById('fillLevel').style.height = percentage + '%';
    }

    function toggleSwitchState(switchNum) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                console.log('Switch state toggled for switch number: ' + switchNum);
            }
        };
        xhttp.open("GET", "http://espespresso.local/toggleSoftwareSwitch" + switchNum, true);
        xhttp.send();
    }

    setInterval(requestSensorData, 250);
</script>

<script>
    function toggleSoftwareSwitch(switchNum) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("SoftwareSwitch" + switchNum).className = this.responseText == 'on' ? 'enabled' : 'disabled';
            }
        };
        xhttp.open("GET", "toggleSoftwareSwitch" + switchNum, true);
        xhttp.send();
    }
</script>

<script>
    document.getElementById('targetBrewTemp').addEventListener('change', function() {
        var newTemp = this.value;

        // Check if the new temperature is within the specified range
        if(newTemp < 50 || newTemp > 98) {
            alert("Temperature must be between 50 and 98");
            return;
        }

        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                console.log('Brew Boiler Temperature updated to: ' + newTemp);
            }
        };
        xhttp.open("GET", "http://espespresso.local/setBrewTemp?value=" + newTemp, true);
        xhttp.send();h
    });

    document.getElementById('targetSteamTemp').addEventListener('change', function() {
        var newTemp = this.value;

        // Check if the new temperature is within the specified range
        if(newTemp < 101 || newTemp > 140) {
            alert("Temperature must be between 101 and 140");
            return;
        }

        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                console.log('Steam Boiler Temperature updated to: ' + newTemp);
            }
        };
        xhttp.open("GET", "http://espespresso.local/setSteamTemp?value=" + newTemp, true);
        xhttp.send();
    });
</script>

</body>
</html>


)=====";